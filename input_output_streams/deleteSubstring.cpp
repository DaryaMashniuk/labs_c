#include "pch.h"
#include "gtest/gtest.h"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include <fstream>
#include <string>
#include <iostream>
#include <algorithm>
#include <stdexcept>

class TextProcessor {
private:
    std::shared_ptr<spdlog::logger> logger;

public:
    TextProcessor() {
        logger = spdlog::get("text_processor_logger");
        if (!logger) {
            logger = spdlog::stdout_color_mt("text_processor_logger");
        }
        spdlog::set_level(spdlog::level::info);
    }

    // Method to read file content into a string
    std::string readFile(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            logger->error("Failed to open file: {}", filename);
            throw std::runtime_error("File not found or could not be opened.");
        }
        std::string content((std::istreambuf_iterator<char>(file)),
            std::istreambuf_iterator<char>());
        logger->info("File successfully read: {}", filename);
        return content;
    }

    // Method to read text from the terminal
    std::string readFromTerminal() {
        std::string input;
        logger->info("Enter text (type 'END' to finish):");
        std::string line;
        while (std::getline(std::cin, line) && line != "END") {
            input += line + "\n";
        }
        logger->info("Text successfully read from the terminal.");
        return input;
    }

    // Method to remove a substring from a string
    std::string removeSubstring(const std::string& line, const std::string& substring) {
        std::string result = line;
        size_t pos;
        while ((pos = result.find(substring)) != std::string::npos) {
            result.erase(pos, substring.length());
            logger->info("Removed occurrence of substring '{}' from line.", substring);
        }
        return result;
    }

    // Method to write text to a file
    bool writeToFile(const std::string& content, const std::string& filename = "test_output.txt") {
        std::ofstream outFile(filename);
        if (!outFile.is_open()) {
            logger->error("Failed to open file for writing: {}", filename);
            throw std::runtime_error("Could not open output file.");
        }
        outFile << content;
        logger->info("Content successfully written to file: {}", filename);
        return true;
    }

    // Method to interact with the user and execute processing based on input
    void interact() {
        std::string choice;
        std::cout << "Would you like to input text from a file or the terminal? (enter 'file' or 'terminal'): ";
        std::cin >> choice;
        std::cin.ignore();

        std::string text;
        if (choice == "file") {
            std::string inputFile;
            std::cout << "Enter the file name to read from: ";
            std::cin >> inputFile;
            try {
                text = readFile(inputFile);
            }
            catch (const std::runtime_error& e) {
                std::cerr << e.what() << std::endl;
                return;
            }
        }
        else if (choice == "terminal") {
            text = readFromTerminal();
        }
        else {
            std::cerr << "Invalid choice." << std::endl;
            return;
        }

        std::cout << "Enter the substring to remove: ";
        std::string substring;
        std::cin >> substring;

        std::string modifiedText = removeSubstring(text, substring);

        std::string outputFile;
        std::cout << "Enter the output file name (or press enter to use 'test_output.txt'): ";
        std::cin.ignore();
        std::getline(std::cin, outputFile);
        if (outputFile.empty()) {
            outputFile = "test_output.txt";
        }

        try {
            writeToFile(modifiedText, outputFile);
            std::cout << "Result written to " << outputFile << std::endl;
        }
        catch (const std::runtime_error& e) {
            std::cerr << e.what() << std::endl;
        }
    }
};

// Test class
class TextProcessorTest : public ::testing::Test {
protected:
    TextProcessor* processor;

    void SetUp() override {
        processor = new TextProcessor();
    }

    void TearDown() override {
        delete processor;
        processor = nullptr;
    }
};

// Test removing a substring from a line
TEST_F(TextProcessorTest, RemoveSubstringTest) {
    std::string line = "This is a test line.";
    std::string substring = "test";
    std::string expected = "This is a  line.";

    ASSERT_EQ(processor->removeSubstring(line, substring), expected);
}

// Test reading file content
TEST_F(TextProcessorTest, ReadFileTest) {
    std::string testFile = "test_file.txt";
    std::ofstream outFile(testFile);
    outFile << "Test line 1.\nTest line 2.\n";
    outFile.close();

    std::string content = processor->readFile(testFile);
    std::string expectedContent = "Test line 1.\nTest line 2.\n";
    ASSERT_EQ(content, expectedContent);
}

// Test for handling non-existent file and throwing exception
TEST_F(TextProcessorTest, NonExistentFileTest) {
    std::string nonExistentFile = "non_existent_file.txt";
    EXPECT_THROW({
        processor->readFile(nonExistentFile);
        }, std::runtime_error);
}

// Test writing content to a file
TEST_F(TextProcessorTest, WriteToFileTest) {
    std::string testContent = "This is the test output.";
    std::string testFile = "test_output_test.txt";
    ASSERT_NO_THROW({
        processor->writeToFile(testContent, testFile);
        });

    std::ifstream inFile(testFile);
    ASSERT_TRUE(inFile.is_open());
    std::string readContent((std::istreambuf_iterator<char>(inFile)), std::istreambuf_iterator<char>());
    ASSERT_EQ(readContent, testContent);
}

// Main function
int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    int testResult = RUN_ALL_TESTS();

    TextProcessor processor;
    processor.interact();

    return testResult;
}
