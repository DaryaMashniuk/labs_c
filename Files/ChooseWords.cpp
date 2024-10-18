#include "pch.h"
#include "gtest/gtest.h"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include <unordered_set>
#include <sstream>
#include <string>
#include <algorithm>
#include <cctype>
#include <fstream>

// Class for processing text from a file
class TextProcessor {
private:
    std::shared_ptr<spdlog::logger> logger;

    // Convert string to lowercase
    std::string toLowerCase(const std::string& str) const {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(), ::tolower);
        return result;
    }

    // Check if the first and last letters of a word match
    bool firstAndLastMatch(const std::string& word) const {
        if (word.length() < 2) {
            return false;
        }

        char first = std::tolower(word.front());
        char last = std::tolower(word.back());
        return first == last;
    }

public:
    // Constructor with logger
    TextProcessor(std::shared_ptr<spdlog::logger> logger) : logger(logger) {}

    // Method for checking if file exists
    bool fileExists(const std::string& fileName) const {
        std::ifstream file(fileName);
        return file.good();  // Checks if the file exists and is accessible
    }

    // Method for reading and processing text from a file
    std::unordered_set<std::string> processTextFromFile(const std::string& fileName) {
        std::unordered_set<std::string> result;

        // Check if the file exists using std::ifstream
        if (!fileExists(fileName)) {
            logger->error("Error: File {} does not exist", fileName);
            return result;  // Return empty result
        }

        std::ifstream file(fileName);

        if (!file.is_open()) {
            logger->error("Error: Could not open file {}", fileName);
            return result;  // Return empty result
        }

        logger->info("File {} successfully opened", fileName);
        std::string word;

        while (file >> word) {
            // Remove punctuation from the word
            word.erase(std::remove_if(word.begin(), word.end(), ::ispunct), word.end());

            if (firstAndLastMatch(word)) {
                std::string lowerWord = toLowerCase(word);
                if (result.find(lowerWord) == result.end()) {
                    logger->info("Added word: {}", word);
                    result.insert(lowerWord);
                }
            }
        }

        if (result.empty()) {
            logger->warn("File {} contains no words that match the condition", fileName);
        }

        logger->info("Processing of file {} completed", fileName);
        return result;
    }
};

// Tests using Google Test
class TextProcessorTest : public ::testing::Test {
protected:
    std::shared_ptr<spdlog::logger> logger;
    TextProcessor* processor;

    void SetUp() override {
        logger = spdlog::get("test_logger");
        if (!logger) {
            logger = spdlog::stdout_color_mt("test_logger");
        }
        spdlog::set_level(spdlog::level::info);  // Set logging level

        processor = new TextProcessor(logger);
    }

    void TearDown() override {
        delete processor;
        processor = nullptr;
    }
};

// Test for checking file with words where the first and last letters match
TEST_F(TextProcessorTest, MatchingWordsInFileTest) {
    std::string fileName = "test_matching_words.txt"; // Create a file with test data
    std::ofstream outFile(fileName);
    outFile << "Anna went to the zoo and saw a radar. It was noon!";
    outFile.close();

    auto words = processor->processTextFromFile(fileName);
    EXPECT_EQ(words.size(), 3);
    EXPECT_TRUE(words.find("anna") != words.end());
    EXPECT_TRUE(words.find("radar") != words.end());
    EXPECT_TRUE(words.find("noon") != words.end());
}

// Test for checking the case when the file does not exist
TEST_F(TextProcessorTest, FileNotFoundTest) {
    std::string fileName = "non_existing_file.txt";
    auto words = processor->processTextFromFile(fileName);
    EXPECT_TRUE(words.empty());
}

// Test for checking an empty file
TEST_F(TextProcessorTest, EmptyFileTest) {
    std::string fileName = "empty_file.txt"; // Create an empty file
    std::ofstream outFile(fileName);
    outFile.close();

    auto words = processor->processTextFromFile(fileName);
    EXPECT_TRUE(words.empty());
}

// Test for checking a file with no words that match the condition
TEST_F(TextProcessorTest, NoMatchingWordsInFileTest) {
    std::string fileName = "no_matching_words.txt"; // Create a file with text that doesn't match the condition
    std::ofstream outFile(fileName);
    outFile << "apple, hello, world!";  // Words that don't have matching first and last letters
    outFile.close();

    auto words = processor->processTextFromFile(fileName);
    EXPECT_TRUE(words.empty());
}

// Test for checking a file with repeated matching words
TEST_F(TextProcessorTest, RepeatedWordsInFileTest) {
    std::string fileName = "repeated_words.txt"; // Create a file with repeated words
    std::ofstream outFile(fileName);
    outFile << "Anna went to the zoo and saw Anna again. Noon is here again!";
    outFile.close();

    auto words = processor->processTextFromFile(fileName);
    EXPECT_EQ(words.size(), 2);
    EXPECT_TRUE(words.find("anna") != words.end());
    EXPECT_TRUE(words.find("noon") != words.end());
}

// Main function
int main(int argc, char** argv) {
    spdlog::set_level(spdlog::level::info);  // Set logging level
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}