#include "pch.h"
#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include <gtest/gtest.h>
#include <sstream>

using namespace std;

const string configFilePath = "db_config.ini";

// ShopDatabase class definition
class ShopDatabase {
    private:
    struct Config {
        string server;
        string username;
        string password;
        string database;
    } config;

    sql::mysql::MySQL_Driver* driver;
    sql::Connection* con;
    sql::Statement* stmt = nullptr;
    sql::PreparedStatement* pstmt = nullptr;
    static shared_ptr<spdlog::logger> logger;

    void loadConfig() {
        auto configMap = parseIniFile(configFilePath);

        config.server = configMap["database.server"];
        config.username = configMap["database.username"];
        config.password = configMap["database.password"];
        config.database = configMap["database.database"];
    }

    map<string, string> parseIniFile(const string& filename) {
        ifstream file(filename);
        if (!file.is_open()) {
            throw runtime_error("Unable to open configuration file: " + filename);
        }

        map<string, string> config;
        string line, section;

        while (getline(file, line)) {
            line.erase(0, line.find_first_not_of(" \t"));
            if (line.empty() || line[0] == ';' || line[0] == '#') continue;

            if (line[0] == '[') {
                section = line.substr(1, line.find(']') - 1);
            }
            else {
                size_t delimPos = line.find('=');
                if (delimPos != string::npos) {
                    string key = line.substr(0, delimPos);
                    string value = line.substr(delimPos + 1);

                    key.erase(key.find_last_not_of(" \t") + 1);
                    value.erase(0, value.find_first_not_of(" \t"));

                    config[section + "." + key] = value;
                }
            }
        }

        file.close();
        return config;
    }

    int getLastInsertId() {
        sql::ResultSet* res = stmt->executeQuery("SELECT LAST_INSERT_ID()");
        res->next();
        int id = res->getInt(1);
        delete res;
        return id;
    }
public:
    ShopDatabase() {
        loadConfig();  // Load configuration from .ini file
        driver = sql::mysql::get_mysql_driver_instance();
        con = driver->connect(config.server, config.username, config.password);
        con->setSchema(config.database);

        if (!logger) {
            logger = spdlog::stdout_color_mt("shop_logger");
            logger->set_level(spdlog::level::info);
            logger->flush_on(spdlog::level::info);
        }
    }

    virtual ~ShopDatabase() {
        delete pstmt;
        delete stmt;
        delete con;
    }

    virtual void initializeDatabase() {
        logger->info("Initializing database...");
        stmt = con->createStatement();

        stmt->execute("DROP TABLE IF EXISTS order_items");
        stmt->execute("DROP TABLE IF EXISTS orders");
        stmt->execute("DROP TABLE IF EXISTS products");

        stmt->execute(R"(
        CREATE TABLE products (
            id INT AUTO_INCREMENT PRIMARY KEY,
            name VARCHAR(100) NOT NULL,
            description TEXT,
            price DECIMAL(10, 2) NOT NULL
        )
    )");

        stmt->execute(R"(
        CREATE TABLE orders (
            id INT AUTO_INCREMENT PRIMARY KEY,
            order_date DATE NOT NULL
        )
    )");

        stmt->execute(R"(
        CREATE TABLE order_items (
            id INT AUTO_INCREMENT PRIMARY KEY,
            order_id INT NOT NULL,
            product_id INT NOT NULL,
            quantity INT NOT NULL,
            FOREIGN KEY (order_id) REFERENCES orders(id) ON DELETE CASCADE,
            FOREIGN KEY (product_id) REFERENCES products(id)
        )
    )");

        logger->info("Database initialized.");
    }

    virtual void addProduct(const string& name, const string& description, double price) {
        pstmt = con->prepareStatement("INSERT INTO products (name, description, price) VALUES (?, ?, ?)");
        pstmt->setString(1, name);
        pstmt->setString(2, description);
        pstmt->setDouble(3, price);
        pstmt->execute();
        logger->info("Added product: {}", name);
    }

    virtual void addOrder(const string& date, const vector<pair<int, int>>& items) {
        pstmt = con->prepareStatement("INSERT INTO orders (order_date) VALUES (?)");
        pstmt->setString(1, date);
        pstmt->execute();

        int orderId = getLastInsertId();

        pstmt = con->prepareStatement("INSERT INTO order_items (order_id, product_id, quantity) VALUES (?, ?, ?)");
        for (const auto& item : items) {
            pstmt->setInt(1, orderId);
            pstmt->setInt(2, item.first);
            pstmt->setInt(3, item.second);
            pstmt->execute();
        }

        logger->info("Added order with ID: {}", orderId);
    }

    virtual void deleteOrdersWithProductQuantity(int productId, int quantity) {
        pstmt = con->prepareStatement(R"(
            DELETE FROM orders 
            WHERE id IN (
                SELECT order_id 
                FROM order_items 
                WHERE product_id = ? AND quantity = ?
            )
        )");
        pstmt->setInt(1, productId);
        pstmt->setInt(2, quantity);
        pstmt->execute();
        logger->info("Deleted orders containing product ID {} with quantity {}", productId, quantity);
    }

    virtual void displayOrderDetails(int orderId) {
        pstmt = con->prepareStatement(R"(
            SELECT o.id, o.order_date, p.name, oi.quantity, p.price 
            FROM orders o
            JOIN order_items oi ON o.id = oi.order_id
            JOIN products p ON p.id = oi.product_id
            WHERE o.id = ? 
        )");
        pstmt->setInt(1, orderId);
        sql::ResultSet* res = pstmt->executeQuery();

        logger->info("Order Details for ID {}:", orderId);
        while (res->next()) {
            cout << "Product: " << res->getString("name")
                << ", Quantity: " << res->getInt("quantity")
                << ", Price: " << res->getDouble("price") << endl;
        }
        delete res;
    }

};

shared_ptr<spdlog::logger> ShopDatabase::logger = nullptr;

// Stub class to simulate ShopDatabase for testing
class StubShopDatabase : public ShopDatabase {
public:
    StubShopDatabase() : ShopDatabase() {}

    void initializeDatabase() override {
        std::cout << "Initializing database (stubbed)" << std::endl;
    }

    void addProduct(const std::string& name, const std::string& description, double price) override {
        std::cout << "Adding product: " << name << " with price " << price << std::endl;
    }

    void addOrder(const std::string& date, const std::vector<std::pair<int, int>>& items) override {
        std::cout << "Adding order on date: " << date << " with " << items.size() << " items." << std::endl;
    }

    void deleteOrdersWithProductQuantity(int productId, int quantity) override {
        std::cout << "Deleting orders with productId " << productId << " and quantity " << quantity << std::endl;
    }

    void displayOrderDetails(int orderId) override {
        std::cout << "Displaying order details for orderId: " << orderId << std::endl;
    }
};

// Test cases
TEST(ShopDatabaseTest, InitializeDatabase) {
    StubShopDatabase db;
    std::ostringstream output;
    std::streambuf* original = std::cout.rdbuf(output.rdbuf());

    db.initializeDatabase();

    std::cout.rdbuf(original);  // Restore original output

    std::string expected_output = "Initializing database (stubbed)\n";
    EXPECT_EQ(output.str(), expected_output);
}

TEST(ShopDatabaseTest, AddProduct) {
    StubShopDatabase db;
    std::ostringstream output;
    std::streambuf* original = std::cout.rdbuf(output.rdbuf());

    db.addProduct("Apple", "Fresh Red Apple", 1.20);

    std::cout.rdbuf(original);  // Restore original output

    std::string expected_output = "Adding product: Apple with price 1.2\n";
    EXPECT_EQ(output.str(), expected_output);
}

TEST(ShopDatabaseTest, AddOrder) {
    StubShopDatabase db;
    std::ostringstream output;
    std::streambuf* original = std::cout.rdbuf(output.rdbuf());

    db.addOrder("2024-11-25", { {1, 10}, {2, 5} });

    std::cout.rdbuf(original);  // Restore original output

    std::string expected_output = "Adding order on date: 2024-11-25 with 2 items.\n";
    EXPECT_EQ(output.str(), expected_output);
}

TEST(ShopDatabaseTest, DeleteOrdersWithProductQuantity) {
    StubShopDatabase db;
    std::ostringstream output;
    std::streambuf* original = std::cout.rdbuf(output.rdbuf());

    db.deleteOrdersWithProductQuantity(1, 10);

    std::cout.rdbuf(original);  // Restore original output

    std::string expected_output = "Deleting orders with productId 1 and quantity 10\n";
    EXPECT_EQ(output.str(), expected_output);
}

TEST(ShopDatabaseTest, DisplayOrderDetails) {
    StubShopDatabase db;
    std::ostringstream output;
    std::streambuf* original = std::cout.rdbuf(output.rdbuf());

    db.displayOrderDetails(1);

    std::cout.rdbuf(original);  // Restore original output

    std::string expected_output = "Displaying order details for orderId: 1\n";
    EXPECT_EQ(output.str(), expected_output);
}

// Entry point for the tests
int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
