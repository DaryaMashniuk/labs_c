#include "pch.h"
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <map>
#include <set>
#include <memory>
#include <fstream>
#include "gtest/gtest.h"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"

// Класс для обработки символов
class Character {
private:
    char value;
public:
    Character(char val) : value(val) {}
    char getValue() const { return value; }
    bool isPunctuation() const { return std::ispunct(value); }
    bool isSpace() const { return std::isspace(value); }
    bool isAlphabetic() const { return std::isalpha(value); }
};

// Класс для обработки слов
class Word {
private:
    std::string word;
public:
    Word(const std::string& w) : word(w) {}
    std::string getWord() const { return word; }
    char getFirstLetter() const { return std::tolower(word[0]); }
};

// Класс для предложения
class Sentence {
private:
    std::vector<Word> words;
public:
    void addWord(const Word& word) {
        words.push_back(word);
    }
    std::vector<Word> getWords() const {
        return words;
    }
};

// Класс для абзаца
class Paragraph {
private:
    std::vector<Sentence> sentences;
public:
    void addSentence(const Sentence& sentence) {
        sentences.push_back(sentence);
    }
    std::vector<Sentence> getSentences() const {
        return sentences;
    }
};

// Основной класс для работы с текстом
class TextProcessor {
private:
    std::vector<Paragraph> paragraphs;
    std::shared_ptr<spdlog::logger> logger;

public:
    TextProcessor(std::shared_ptr<spdlog::logger> log) : logger(log) {}

    // Проверка существования файла
    bool fileExists(const std::string& filename) const {
        std::ifstream file(filename);
        return file.good();
    }

    // Чтение текста из файла и возврат его как строки
    std::string readFileContent(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            logger->error("Could not open file {}", filename);
            return "";
        }

        std::stringstream ss;
        ss << file.rdbuf();  // Чтение содержимого файла в строковой поток
        file.close();
        return ss.str();
    }

    // Новый метод для вывода предупреждения о пустом файле
    void warnIfEmptyFile(const std::string& filename) {
        logger->warn("File {} is empty.", filename);
    }

    // Основной метод обработки текста из файла
    bool processTextFromFile(const std::string& filename) {
        if (!fileExists(filename)) {
            logger->error("File {} does not exist.", filename);
            return false;
        }

        std::string rawText = readFileContent(filename);

        // Проверка на пустоту файла, выводим предупреждение и возвращаем true
        if (rawText.empty()) {
            warnIfEmptyFile(filename);
            return true;
        }

        Paragraph paragraph = processRawText(rawText);
        if (!paragraph.getSentences().empty()) {
            paragraphs.push_back(paragraph);
        }

        logger->info("Text from file {} processed successfully.", filename);
        return true;
    }

    // Обработка текста на слова и предложение, возвращает обработанный абзац
    Paragraph processRawText(const std::string& rawText) {
        std::string word;
        Paragraph currentParagraph;
        Sentence currentSentence;
        std::istringstream stream(rawText);

        while (stream >> word) {
            word = cleanWord(word);  // Убираем пунктуацию
            if (!word.empty()) {
                currentSentence.addWord(Word(word));  // Добавляем слово
            }

            // Проверяем конец предложения (на примере знаков окончания)
            if (isEndOfSentence(word)) {
                currentParagraph.addSentence(currentSentence);  // Добавляем предложение в абзац
                currentSentence = Sentence();  // Начинаем новое предложение
            }
        }

        // Если есть непустое предложение, добавляем его в абзац
        if (!currentSentence.getWords().empty()) {
            currentParagraph.addSentence(currentSentence);
        }

        return currentParagraph;
    }

    // Обработка и очистка слова от пунктуации
    std::string cleanWord(const std::string& word) {
        std::string cleanedWord = word;
        cleanedWord.erase(std::remove_if(cleanedWord.begin(), cleanedWord.end(), ::ispunct), cleanedWord.end());
        return cleanedWord;
    }

    // Проверка, является ли слово концом предложения
    bool isEndOfSentence(const std::string& word) const {
        if (word.empty()) return false;
        char lastChar = word.back();
        return lastChar == '.' || lastChar == '!' || lastChar == '?';
    }

    // Печать слов по алфавиту
    void printWordsAlphabetically() const {
        std::map<char, std::set<std::string>> wordMap;

        // Собираем все слова по первой букве
        for (const auto& paragraph : paragraphs) {
            for (const auto& sentence : paragraph.getSentences()) {
                for (const auto& word : sentence.getWords()) {
                    wordMap[word.getFirstLetter()].insert(word.getWord());
                }
            }
        }

        // Печатаем слова по алфавиту
        for (const auto& entry : wordMap) {
            std::string line = std::string(1, entry.first) + ": ";
            for (const auto& word : entry.second) {
                line += word + " ";
            }
            logger->info(line);
        }
    }

    // Другие методы для работы с текстом
    void clearText() {
        paragraphs.clear();
        logger->info("Text cleared.");
    }

    // Метод для подсчета количества слов в тексте
    int countWords() const {
        int count = 0;
        for (const auto& paragraph : paragraphs) {
            for (const auto& sentence : paragraph.getSentences()) {
                count += sentence.getWords().size();
            }
        }
        logger->info("Total word count: {}", count);
        return count;
    }


};

// Тесты с использованием Google Test
class TextProcessorTest : public ::testing::Test {
protected:
    std::shared_ptr<spdlog::logger> logger;
    TextProcessor* textProcessor;

    void SetUp() override {
        logger = spdlog::get("test_logger");
        if (!logger) {
            logger = spdlog::stdout_color_mt("test_logger");
        }
        spdlog::set_level(spdlog::level::info);

        textProcessor = new TextProcessor(logger);
    }

    void TearDown() override {
        delete textProcessor;
        textProcessor = nullptr;
    }
};

// Тесты для проверки обработки текста из файла
TEST_F(TextProcessorTest, TestProcessAndPrintFromFile) {
    std::string fileName = "test.txt";
    std::ofstream outFile(fileName);
    outFile << "Hello world! This is a test. Anna went to the zoo.";
    outFile.close();

    ASSERT_TRUE(textProcessor->processTextFromFile(fileName));
    textProcessor->printWordsAlphabetically();
}

TEST_F(TextProcessorTest, TestNonExistentFile) {
    std::string fileName = "non_existing_file.txt";
    ASSERT_FALSE(textProcessor->processTextFromFile(fileName));
}

TEST_F(TextProcessorTest, TestEmptyFile) {
    std::string fileName = "empty_file.txt";
    std::ofstream outFile(fileName);
    outFile.close();

    ASSERT_TRUE(textProcessor->processTextFromFile(fileName));
    textProcessor->printWordsAlphabetically();
}

TEST_F(TextProcessorTest, TestWithPunctuationAndSpaces) {
    std::string fileName = "punctuation_test.txt";
    std::ofstream outFile(fileName);
    outFile << "   Zebra...!! and  Antelope?   Anna...!!! went to the zoo. ";
    outFile.close();

    ASSERT_TRUE(textProcessor->processTextFromFile(fileName));
    textProcessor->printWordsAlphabetically();
}

// Основная функция
int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
