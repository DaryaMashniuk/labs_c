#include "pch.h"

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <stdexcept>
#include <algorithm>
#include "gtest/gtest.h"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"

// Класс FileProcessor для обработки строк в файлах
class FileProcessor {
private:
    static std::shared_ptr<spdlog::logger> logger; // Статический логгер

    // Метод для проверки открытия файла
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
    FileProcessor() {
        // Инициализация статического логгера при первом вызове
        if (!logger) {
            logger = spdlog::stdout_color_mt("file_processor_logger");
            spdlog::set_level(spdlog::level::info); // Устанавливаем глобальный уровень логирования
        }
    }

    // Метод для чтения строк из файла
    std::vector<std::string> readLines(const std::string& filename) {
        logger->info("Reading lines from file: {}", filename);
        std::ifstream inputFile(filename);
        checkFileOpen(inputFile, filename);

        std::vector<std::string> lines;
        std::string line;
        while (std::getline(inputFile, line)) {
            lines.push_back(line);
        }

        inputFile.close();
        logger->info("Read {} lines from file: {}", lines.size(), filename);
        return lines;
    }

    // Метод для записи строк в файл
    void writeLines(const std::vector<std::string>& lines, const std::string& filename) {
        logger->info("Writing lines to file: {}", filename);
        std::ofstream outputFile(filename);
        checkFileOpen(outputFile, filename);

        for (const auto& line : lines) {
            outputFile << line << "\n";
        }

        outputFile.close();
        logger->info("Successfully wrote lines to file: {}", filename);
    }

    // Метод для замены первого и последнего слова в строке
    std::string swapFirstAndLastWords(const std::string& line) {
        std::istringstream iss(line);
        std::vector<std::string> words;
        std::string word;

        // Разбиваем строку на слова
        while (iss >> word) {
            words.push_back(word);
        }

        // Если строка пуста или содержит одно слово, возвращаем её как есть
        if (words.size() < 2) {
            return line;
        }

        // Меняем местами первое и последнее слово
        std::swap(words.front(), words.back());

        // Собираем строку обратно
        std::ostringstream oss;
        for (size_t i = 0; i < words.size(); ++i) {
            oss << words[i];
            if (i < words.size() - 1) {
                oss << " ";
            }
        }

        logger->info("Swapped first and last words in line: {}", line);
        return oss.str();
    }

    // Метод для обработки файла с заменой первого и последнего слов
    void processFile(const std::string& inputFilename, const std::string& outputFilename) {
        auto lines = readLines(inputFilename);
        std::vector<std::string> processedLines;

        for (const auto& line : lines) {
            processedLines.push_back(swapFirstAndLastWords(line));
        }

        writeLines(processedLines, outputFilename);
        logger->info("Processed file with swapped words and saved to output file: {}", outputFilename);
    }
};

// Инициализация статического логгера
std::shared_ptr<spdlog::logger> FileProcessor::logger = nullptr;

// Тесты для FileProcessor
class FileProcessorTest : public ::testing::Test {
protected:
    FileProcessor processor;

    // Вспомогательный метод для тестирования
    std::string swapWordsInLine(const std::string& line) {
        return processor.swapFirstAndLastWords(line);
    }
};

TEST_F(FileProcessorTest, TestSwapFirstAndLastWords) {
    EXPECT_EQ(swapWordsInLine("Hello world"), "world Hello");
    EXPECT_EQ(swapWordsInLine("One two three"), "three two One");
    EXPECT_EQ(swapWordsInLine("first"), "first"); // Одно слово, без изменений
    EXPECT_EQ(swapWordsInLine(""), ""); // Пустая строка, без изменений
}

TEST_F(FileProcessorTest, TestSwapInFile) {
    // Временные файлы для теста
    const std::string inputFilename = "test_input.txt";
    const std::string outputFilename = "test_output.txt";

    // Записываем тестовые данные в файл
    std::ofstream testFile(inputFilename);
    testFile << "Hello world\n";
    testFile << "One two three\n";
    testFile << "singleword\n";
    testFile.close();

    // Обрабатываем файл
    processor.processFile(inputFilename, outputFilename);

    // Проверяем содержимое выходного файла
    std::ifstream resultFile(outputFilename);
    std::string line;
    std::getline(resultFile, line);
    EXPECT_EQ(line, "world Hello"); // Проверяем первую строку
    std::getline(resultFile, line);
    EXPECT_EQ(line, "three two One"); // Проверяем вторую строку
    std::getline(resultFile, line);
    EXPECT_EQ(line, "singleword"); // Проверяем третью строку (без изменений)
    resultFile.close();

    // Удаляем временные файлы
    std::remove(inputFilename.c_str());
    std::remove(outputFilename.c_str());
}

// Основная программа
int main(int argc, char** argv) {
    // Инициализация логгера
    spdlog::set_level(spdlog::level::info);
    auto logger = spdlog::stdout_color_mt("console");

    std::string inputFilename, outputFilename;

    // Запрашиваем у пользователя имена файлов
    std::cout << "Enter input file name: ";
    std::cin >> inputFilename;
    std::cout << "Enter output file name: ";
    std::cin >> outputFilename;

    // Создаем объект FileProcessor и обрабатываем файл
    FileProcessor processor;
    try {
        processor.processFile(inputFilename, outputFilename);
    }
    catch (const std::runtime_error& e) {
        logger->error("Error processing file: {}", e.what());
        return 1;
    }

    // Запуск тестов
    ::testing::InitGoogleTest(&argc, argv);
    logger->info("Running tests...");
    int result = RUN_ALL_TESTS();
    logger->info("Tests completed with result: {}", result);
    return result;
}
