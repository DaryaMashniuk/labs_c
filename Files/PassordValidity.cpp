#include "pch.h"
#include "gtest/gtest.h"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include <cctype>
#include <string>
#include <fstream>
#include <iostream>

class PasswordProcessor {
private:
    std::shared_ptr<spdlog::logger> logger;

public:
    PasswordProcessor() {
        // Проверяем, существует ли уже логгер, чтобы избежать дублирования
        logger = spdlog::get("password_logger");
        if (!logger) {
            // Если логгер не существует, создаем новый
            logger = spdlog::stdout_color_mt("password_logger");
        }
        spdlog::set_level(spdlog::level::info);
    }

    // Метод для проверки, достаточно ли длинный пароль
    bool isValidLength(const std::string& password) {
        if (password.length() < 10) {
            logger->warn("Password is too short: {} characters.", password.length());
            return false;
        }
        return true;
    }

    // Метод для проверки, содержит ли пароль заглавные буквы
    bool containsUpperCase(const std::string& password) {
        for (char ch : password) {
            if (std::isupper(ch)) {
                return true;
            }
        }
        logger->warn("Password does not contain an uppercase letter.");
        return false;
    }

    // Метод для проверки, содержит ли пароль строчные буквы
    bool containsLowerCase(const std::string& password) {
        for (char ch : password) {
            if (std::islower(ch)) {
                return true;
            }
        }
        logger->warn("Password does not contain a lowercase letter.");
        return false;
    }

    // Метод для проверки, содержит ли пароль цифры
    bool containsDigit(const std::string& password) {
        for (char ch : password) {
            if (std::isdigit(ch)) {
                return true;
            }
        }
        logger->warn("Password does not contain a digit.");
        return false;
    }

    // Метод для проверки на недопустимые символы
    bool containsInvalidCharacters(const std::string& password) {
        for (char ch : password) {
            if (!std::isalnum(ch) && ch != '_') {
                logger->warn("Password contains invalid character: '{}'", ch);
                return true;
            }
        }
        return false;
    }

    // Метод для проверки, является ли пароль сильным
    bool isStrongPassword(const std::string& password) {
        return isValidLength(password) &&
            !containsInvalidCharacters(password) &&
            containsUpperCase(password) &&
            containsLowerCase(password) &&
            containsDigit(password);
    }

    // Метод для проверки существования файла
    bool fileExists(const std::string& fileName) {
        std::ifstream file(fileName);
        return file.good();
    }

    // Метод для чтения пароля из файла
    std::string readPasswordFromFile(const std::string& fileName) {
        if (!fileExists(fileName)) {
            logger->error("File '{}' does not exist.", fileName);
            return "";
        }

        std::ifstream file(fileName);
        if (!file.is_open()) {
            logger->error("Could not open file '{}'.", fileName);
            return "";
        }

        std::string password;
        std::getline(file, password);

        if (password.empty()) {
            logger->warn("File '{}' is empty or contains no valid password.", fileName);
        }
        else {
            logger->info("Password read from file '{}'.", fileName);
        }

        file.close();
        return password;
    }
};

class PasswordTest : public ::testing::Test {
protected:
    PasswordProcessor* passwordProcessor;

    void SetUp() override {
        passwordProcessor = new PasswordProcessor(); // Создаем экземпляр PasswordProcessor
        spdlog::set_level(spdlog::level::info);      // Устанавливаем уровень логирования
    }

    void TearDown() override {
        delete passwordProcessor; // Освобождаем память
        passwordProcessor = nullptr;
    }
};

// Тесты для проверки различных сценариев
TEST_F(PasswordTest, TestStrongPassword) {
    std::string password = "Strong_Pass1";
    ASSERT_TRUE(passwordProcessor->isStrongPassword(password)) << "Expected password to be strong.";
}

TEST_F(PasswordTest, TestShortPassword) {
    std::string password = "Short1";
    ASSERT_FALSE(passwordProcessor->isStrongPassword(password)) << "Expected password to be too short.";
}

TEST_F(PasswordTest, TestNoUpperCase) {
    std::string password = "lowercase1_";
    ASSERT_FALSE(passwordProcessor->isStrongPassword(password)) << "Expected password to lack uppercase letters.";
}

TEST_F(PasswordTest, TestNoLowerCase) {
    std::string password = "UPPERCASE1_";
    ASSERT_FALSE(passwordProcessor->isStrongPassword(password)) << "Expected password to lack lowercase letters.";
}

TEST_F(PasswordTest, TestNoDigit) {
    std::string password = "NoDigits__";
    ASSERT_FALSE(passwordProcessor->isStrongPassword(password)) << "Expected password to lack digits.";
}

TEST_F(PasswordTest, TestInvalidCharacter) {
    std::string password = "Invalid@Pass1";
    ASSERT_FALSE(passwordProcessor->isStrongPassword(password)) << "Expected password to contain invalid characters.";
}

// Новый тест для проверки чтения пароля из файла
TEST_F(PasswordTest, TestReadPasswordFromFile) {
    std::string fileName = "test_password.txt";

    // Создаем тестовый файл с паролем
    std::ofstream outFile(fileName);
    outFile << "Valid_Pass1";
    outFile.close();

    // Читаем пароль из файла и проверяем его
    std::string password = passwordProcessor->readPasswordFromFile(fileName);
    ASSERT_FALSE(password.empty()) << "Expected password to be read from file.";
    ASSERT_TRUE(passwordProcessor->isStrongPassword(password)) << "Expected password to be strong.";
}

// Основная функция
int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);

    // Выполнение всех тестов
    int testResult = RUN_ALL_TESTS();

    // Пример использования чтения пароля из файла после тестов
    PasswordProcessor pp;

    std::string fileName = "password.txt";
    std::string userPassword = pp.readPasswordFromFile(fileName);

    if (!userPassword.empty()) {
        if (pp.isStrongPassword(userPassword)) {
            std::cout << "Password is strong!" << std::endl;
        }
        else {
            std::cout << "Password is not strong enough!" << std::endl;
        }
    }

    return testResult;
}
