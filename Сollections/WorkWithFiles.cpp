#include "pch.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include "gtest/gtest.h"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"

class FileProcessor {
private:
    std::shared_ptr<spdlog::logger> logger;

public:
    // Конструктор с инициализацией логгера
    FileProcessor(std::shared_ptr<spdlog::logger> logger) : logger(logger) {}

    // Метод чтения строк из файла
    std::vector<std::string> readFromFile(const std::string& filename) {
        std::ifstream file(filename);  // Открываем файл
        std::vector<std::string> lines;  // Для хранения строк
        std::string line;

        // Проверка на успешное открытие файла
        if (!file.is_open()) {
            logger->error("Failed to open file: {}", filename);  // Логируем ошибку
            return lines;
        }

        // Чтение файла построчно
        while (std::getline(file, line)) {
            lines.push_back(line);
            logger->info("Read line: {}", line);  // Логируем прочитанную строку
        }

        file.close();  // Закрываем файл
        logger->info("File reading completed: {}", filename);
        return lines;  // Возвращаем строки
    }

    // Метод записи строк в обратном порядке
    void writeToFileInReverse(const std::string& filename, const std::vector<std::string>& lines) {
        std::ofstream file(filename);  // Открываем файл для записи

        // Проверка на успешное открытие файла
        if (!file.is_open()) {
            logger->error("Failed to open file for writing: {}", filename);  // Логируем ошибку
            return;
        }

        // Запись строк в обратном порядке
        for (auto it = lines.rbegin(); it != lines.rend(); ++it) {
            file << *it << std::endl;
            logger->info("Written line: {}", *it);  // Логируем запись строки
        }

        file.close();  // Закрываем файл
        logger->info("File writing completed: {}", filename);
    }
};



// Google Test Fixture
class FileProcessorTest : public ::testing::Test {
protected:
    std::shared_ptr<spdlog::logger> logger;
    FileProcessor* processor;

    void SetUp() override {
        logger = spdlog::stdout_color_mt("test_logger");  // Инициализация логгера
        processor = new FileProcessor(logger);  // Создаем экземпляр FileProcessor
    }

    void TearDown() override {
        delete processor;
        spdlog::drop("test_logger");  // Удаляем логгер после тестов
    }
};

TEST_F(FileProcessorTest, ReadFromFileTest) {
    // Создаем тестовый файл для чтения
    std::ofstream file("test_input.txt");
    file << "Line 1\nLine 2\nLine 3\n";
    file.close();

    // Чтение файла с помощью метода readFromFile
    auto lines = processor->readFromFile("test_input.txt");

    // Проверка, что прочитаны 3 строки
    EXPECT_EQ(lines.size(), 3);
    EXPECT_EQ(lines[0], "Line 1");
    EXPECT_EQ(lines[1], "Line 2");
    EXPECT_EQ(lines[2], "Line 3");
}

TEST_F(FileProcessorTest, WriteToFileInReverseTest) {
    std::vector<std::string> lines = { "Line 1", "Line 2", "Line 3" };

    // Записываем строки в обратном порядке
    processor->writeToFileInReverse("test_output.txt", lines);

    // Читаем результат
    std::ifstream file("test_output.txt");
    std::string line;
    std::vector<std::string> outputLines;

    while (std::getline(file, line)) {
        outputLines.push_back(line);
    }
    file.close();

    // Проверяем, что строки записаны в обратном порядке
    EXPECT_EQ(outputLines[0], "Line 3");
    EXPECT_EQ(outputLines[1], "Line 2");
    EXPECT_EQ(outputLines[2], "Line 1");
}

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}