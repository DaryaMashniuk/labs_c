#include "pch.h"
/*Кредиты. Сформировать набор предложений клиенту по целевым кредитам различных банков для оптимального выбора. Учитывать возможность
досрочного погашения кредита и/или увеличения кредитной линии.
Реализовать выбор и поиск кредита.*/

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <algorithm>
#include <stdexcept>
#include "gtest/gtest.h"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"

// Credit class to represent credit offers
class Credit {
public:
    std::string bankName;       // Bank name
    double interestRate;        // Interest rate
    double creditLimit;         // Credit limit
    bool earlyRepayment;        // Early repayment option
    bool increaseCreditLine;    // Credit line increase option

    Credit(std::string bankName, double interestRate, double creditLimit, bool earlyRepayment, bool increaseCreditLine)
        : bankName(std::move(bankName)), interestRate(interestRate), creditLimit(creditLimit),
        earlyRepayment(earlyRepayment), increaseCreditLine(increaseCreditLine) {}

    void display() const {
        std::cout << "Bank: " << bankName << "\n"
            << "Interest Rate: " << interestRate << "%\n"
            << "Credit Limit: " << creditLimit << " rubles\n"
            << "Early Repayment: " << (earlyRepayment ? "Yes" : "No") << "\n"
            << "Increase Credit Line: " << (increaseCreditLine ? "Yes" : "No") << "\n";
    }
};

// CreditManager class to manage credits
class CreditManager {
private:
    std::vector<Credit> credits;
    std::shared_ptr<spdlog::logger> logger;

    // Helper method to check if a file can be opened
    void checkFileOpen(std::ifstream& file, const std::string& filename) {
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open file for reading: " + filename);
        }
    }

    void checkFileOpen(std::ofstream& file, const std::string& filename) {
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open file for writing: " + filename);
        }
    }

public:
    CreditManager() {
        // Check if the logger is already created
        logger = spdlog::get("text_processor_logger");
        if (!logger) {
            logger = spdlog::stdout_color_mt("text_processor_logger");
            spdlog::set_level(spdlog::level::info); // Set the global log level
        }
    }

    void addCredit(const Credit& credit) {
        credits.push_back(credit);
        logger->info("Added credit from bank: {}", credit.bankName);
    }

    void displayCredits() const {
        logger->info("Displaying all available credits:");
        for (const auto& credit : credits) {
            credit.display();
            std::cout << "------------------------------\n";
        }
    }

    std::vector<Credit> searchCredits(double maxInterestRate, double minCreditLimit) const {
        logger->info("Searching for credits with max interest rate: {} and min credit limit: {}", maxInterestRate, minCreditLimit);
        std::vector<Credit> result;
        for (const auto& credit : credits) {
            if (credit.interestRate <= maxInterestRate && credit.creditLimit >= minCreditLimit) {
                result.push_back(credit);
                logger->info("Found matching credit: {} with interest rate: {} and credit limit: {}", credit.bankName, credit.interestRate, credit.creditLimit);
            }
        }
        logger->info("Search completed. Found {} matching credits.", result.size());
        return result;
    }

    void serialize(const std::string& filename) {
        logger->info("Serializing credits to file: {}", filename);
        std::ofstream outFile(filename, std::ios::binary);
        checkFileOpen(outFile, filename);  // Check if file opens successfully

        size_t size = credits.size();
        outFile.write(reinterpret_cast<const char*>(&size), sizeof(size));
        for (const auto& credit : credits) {
            size_t length = credit.bankName.size();
            outFile.write(reinterpret_cast<const char*>(&length), sizeof(length));
            outFile.write(credit.bankName.c_str(), length);
            outFile.write(reinterpret_cast<const char*>(&credit.interestRate), sizeof(credit.interestRate));
            outFile.write(reinterpret_cast<const char*>(&credit.creditLimit), sizeof(credit.creditLimit));
            outFile.write(reinterpret_cast<const char*>(&credit.earlyRepayment), sizeof(credit.earlyRepayment));
            outFile.write(reinterpret_cast<const char*>(&credit.increaseCreditLine), sizeof(credit.increaseCreditLine));
            logger->info("Serialized credit: {}", credit.bankName);
        }
        outFile.close();
        logger->info("Successfully serialized {} credits to file: {}", size, filename);
    }

    void deserialize(const std::string& filename) {
        logger->info("Deserializing credits from file: {}", filename);
        std::ifstream inFile(filename, std::ios::binary);
        checkFileOpen(inFile, filename);  // Check if file opens successfully

        size_t size;
        inFile.read(reinterpret_cast<char*>(&size), sizeof(size));
        credits.clear();
        logger->info("Expecting to read {} credits.", size);
        for (size_t i = 0; i < size; ++i) {
            Credit credit("", 0.0, 0.0, false, false);
            size_t length;
            inFile.read(reinterpret_cast<char*>(&length), sizeof(length));
            credit.bankName.resize(length);
            inFile.read(&credit.bankName[0], length);
            inFile.read(reinterpret_cast<char*>(&credit.interestRate), sizeof(credit.interestRate));
            inFile.read(reinterpret_cast<char*>(&credit.creditLimit), sizeof(credit.creditLimit));
            inFile.read(reinterpret_cast<char*>(&credit.earlyRepayment), sizeof(credit.earlyRepayment));
            inFile.read(reinterpret_cast<char*>(&credit.increaseCreditLine), sizeof(credit.increaseCreditLine));
            credits.push_back(credit);
            logger->info("Deserialized credit: {}", credit.bankName);
        }
        inFile.close();
        logger->info("Successfully deserialized {} credits from file: {}", size, filename);
    }
};

// Tests for CreditManager
class CreditManagerTest : public ::testing::Test {
protected:
    CreditManager manager;

    void SetUp() override {
        manager.addCredit(Credit("Sberbank", 9.5, 100000, true, true));
        manager.addCredit(Credit("VTB", 10.0, 200000, true, false));
    }
};

TEST_F(CreditManagerTest, TestSearchCredits) {
    auto results = manager.searchCredits(10.0, 150000);
    EXPECT_EQ(results.size(), 1); // Should return 1 credit
}

TEST_F(CreditManagerTest, TestSearchCreditsWithNoResults) {
    auto results = manager.searchCredits(8.0, 150000); // No credits should meet this criteria
    EXPECT_EQ(results.size(), 0); // Should return 0 credits
}

TEST_F(CreditManagerTest, TestAddCredit) {
    manager.addCredit(Credit("Alfa-Bank", 8.0, 150000, false, true));
    auto results = manager.searchCredits(10.0, 100000);
    EXPECT_EQ(results.size(), 3); // Should return 3 credits now
}

TEST_F(CreditManagerTest, TestSerializeAndDeserialize) {
    manager.serialize("test_credits.dat");
    CreditManager newManager;
    newManager.deserialize("test_credits.dat");

    auto results = newManager.searchCredits(10.0, 150000);
    EXPECT_EQ(results.size(), 1); // Should return 1 credit

    // Verify the details of the deserialized credit
    auto credit = results.front();
    EXPECT_EQ(credit.bankName, "VTB");
    EXPECT_EQ(credit.interestRate, 10.0);
    EXPECT_EQ(credit.creditLimit, 200000);
}

TEST_F(CreditManagerTest, TestEmptySerializeAndDeserialize) {
    CreditManager emptyManager;
    emptyManager.serialize("empty_credits.dat");
    CreditManager newManager;
    newManager.deserialize("empty_credits.dat");

    auto results = newManager.searchCredits(10.0, 150000);
    EXPECT_EQ(results.size(), 0); // Should return 0 credits for an empty manager
}

TEST_F(CreditManagerTest, TestSerializeNonExistentFile) {
    CreditManager emptyManager;
    EXPECT_THROW(emptyManager.deserialize("non_existent_file.dat"), std::runtime_error);
}

// Main program
int main(int argc, char** argv) {
    // Setup logging
    spdlog::set_level(spdlog::level::info);
    auto logger = spdlog::stdout_color_mt("console");
    logger->info("Starting credit management program...");

    CreditManager manager;
    manager.addCredit(Credit("Sberbank", 9.5, 100000, true, true));
    manager.addCredit(Credit("VTB", 10.0, 200000, true, false));
    manager.addCredit(Credit("Alfa-Bank", 8.0, 150000, false, true));

    logger->info("Available credits:");
    manager.displayCredits();

    // Save credits to file
    try {
        manager.serialize("credits.dat");
        logger->info("Credits saved to file credits.dat");
    }
    catch (const std::runtime_error& e) {
        logger->error("Error saving credits: {}", e.what());
        return 1; // Exit on error
    }

    // Clear and load credits from file
    CreditManager newManager;
    try {
        newManager.deserialize("credits.dat");
        logger->info("Credits loaded from file credits.dat:");
        newManager.displayCredits();
    }
    catch (const std::runtime_error& e) {
        logger->error("Error loading credits: {}", e.what());
        return 1; // Exit on error
    }

    // Run tests
    ::testing::InitGoogleTest(&argc, argv);
    logger->info("Running tests...");
    int result = RUN_ALL_TESTS();
    logger->info("Tests completed with result: {}", result);
    return result;
}
