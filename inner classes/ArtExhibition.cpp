#include "pch.h"

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include "gtest/gtest.h"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"

class ArtExhibitionService {
private:
    std::string exhibitionName;
    std::shared_ptr<spdlog::logger> logger;

    class Painting {
    private:
        std::string title;
        std::string author;
        std::string date;

    public:
        Painting(const std::string& title, const std::string& author, const std::string& date)
            : title(title), author(author), date(date) {}

        const std::string& getTitle() const {
            return title;
        }

        const std::string& getAuthor() const {
            return author;
        }

        const std::string& getDate() const {
            return date;
        }
    };

    std::vector<Painting> paintings;

public:
    ArtExhibitionService(const std::string& name, std::shared_ptr<spdlog::logger> logger) : exhibitionName(name), logger(logger) {}

    void addPainting(const std::string& title, const std::string& author, const std::string& date) {
        paintings.emplace_back(title, author, date);
        logger->info("Added painting: Title={}, Author={}, Date={}", title, author, date);
    }

    void sortPaintingsByTitle() {
        std::sort(paintings.begin(), paintings.end(), [](const Painting& p1, const Painting& p2) {
            return p1.getTitle() < p2.getTitle();
            });
        logger->info("Paintings sorted by title");
    }

    void sortPaintingsByAuthor() {
        std::sort(paintings.begin(), paintings.end(), [](const Painting& p1, const Painting& p2) {
            return p1.getAuthor() < p2.getAuthor();
            });
        logger->info("Paintings sorted by author");
    }

    void sortPaintingsByDate() {
        std::sort(paintings.begin(), paintings.end(), [](const Painting& p1, const Painting& p2) {
            return p1.getDate() < p2.getDate();
            });
        logger->info("Paintings sorted by date");
    }

    std::vector<std::string> getPaintingTitles() const {
        std::vector<std::string> titles;
        for (const auto& painting : paintings) {
            titles.push_back(painting.getTitle());
        }
        return titles;
    }
};

// Google Test Fixture
class ArtExhibitionTest : public ::testing::Test {
protected:
    std::shared_ptr<spdlog::logger> logger;
    ArtExhibitionService exhibition{ "Test Art Exhibition", logger };

    void SetUp() override {
        // Используем существующий логгер или создаем новый
        logger = spdlog::get("test_logger");
        if (!logger) {
            logger = spdlog::stdout_color_mt("test_logger");
        }
        exhibition = ArtExhibitionService("Test Art Exhibition", logger);
    }
};

TEST_F(ArtExhibitionTest, AddPaintingTest) {
    exhibition.addPainting("Test Painting", "Test Author", "2024");
    auto titles = exhibition.getPaintingTitles();
    EXPECT_EQ(titles.size(), 1);
    EXPECT_EQ(titles[0], "Test Painting");
}

TEST_F(ArtExhibitionTest, SortByTitleTest) {
    exhibition.addPainting("B", "Author B", "2024");
    exhibition.addPainting("A", "Author A", "2024");
    exhibition.sortPaintingsByTitle();
    auto titles = exhibition.getPaintingTitles();
    EXPECT_EQ(titles[0], "A");
    EXPECT_EQ(titles[1], "B");
}

TEST_F(ArtExhibitionTest, SortByAuthorTest) {
    exhibition.addPainting("Title A", "B", "2024");
    exhibition.addPainting("Title B", "A", "2024");
    exhibition.sortPaintingsByAuthor();
    auto titles = exhibition.getPaintingTitles();
    EXPECT_EQ(titles[0], "Title B");
    EXPECT_EQ(titles[1], "Title A");
}

TEST_F(ArtExhibitionTest, SortByDateTest) {
    exhibition.addPainting("Title A", "Author A", "2023");
    exhibition.addPainting("Title B", "Author B", "2022");
    exhibition.sortPaintingsByDate();
    auto titles = exhibition.getPaintingTitles();
    EXPECT_EQ(titles[0], "Title B");
    EXPECT_EQ(titles[1], "Title A");
}

TEST_F(ArtExhibitionTest, AddMultiplePaintingsTest) {
    exhibition.addPainting("Painting 1", "Author 1", "2025");
    exhibition.addPainting("Painting 2", "Author 2", "2024");
    exhibition.addPainting("Painting 3", "Author 3", "2023");
    auto titles = exhibition.getPaintingTitles();
    EXPECT_EQ(titles.size(), 3);
}

TEST_F(ArtExhibitionTest, SortByDateWithMultiplePaintingsTest) {
    exhibition.addPainting("Title C", "Author C", "2023");
    exhibition.addPainting("Title A", "Author A", "2025");
    exhibition.addPainting("Title B", "Author B", "2024");
    exhibition.sortPaintingsByDate();
    auto titles = exhibition.getPaintingTitles();
    EXPECT_EQ(titles[0], "Title C");
    EXPECT_EQ(titles[1], "Title B");
    EXPECT_EQ(titles[2], "Title A");
}

int main(int argc, char** argv) {
    // Создаем логгер
    auto logger = spdlog::stdout_color_mt("console");

    // Передаем логгер в ArtExhibitionService
    ArtExhibitionService artExhibition("Art Exhibition", logger);

    // Запускаем тесты
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

