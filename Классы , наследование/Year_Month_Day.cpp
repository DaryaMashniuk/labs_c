#include "pch.h"
#include <iostream>
#include <string>
#include <ctime>
#include <sstream>
#include <functional>
#include <gtest/gtest.h>
#include <spdlog/spdlog.h>

// Base class for date components
class DateComponent {
public:
    virtual std::string toString() const = 0;
    virtual int hashCode() const = 0;
    virtual bool equals(const DateComponent& other) const = 0;
};

// Day class
class Day : public DateComponent {
private:
    int day;

public:
    Day(int day) : day(day) {}

    int getValue() const { return day; }
    void setValue(int value) { day = value; }

    bool equals(const DateComponent& other) const override {
        const Day* otherDay = dynamic_cast<const Day*>(&other);
        return otherDay && day == otherDay->day;
    }

    int hashCode() const override {
        return day;
    }

    std::string toString() const override {
        return std::to_string(day);
    }


};

// Month class
class Month : public DateComponent {
private:
    int month;

public:
    Month(int month) : month(month) {}

    int getValue() const { return month; }
    void setValue(int value) { month = value; }

    bool equals(const DateComponent& other) const override {
        const Month* otherMonth = dynamic_cast<const Month*>(&other);
        return otherMonth && month == otherMonth->month;
    }

    int hashCode() const override {
        return month;
    }

    std::string toString() const override {
        return std::to_string(month);
    }


};

// Year class
class Year : public DateComponent {
private:
    int day;
    int month;
    int year;

public:
    Year(int year) : year(year), day(1), month(1) {}

    void setDate(int day, int month, int year) {
        this->day = day;
        this->month = month;
        this->year = year;
    }

    std::string getWeekday() const {
        std::tm time = {};
        time.tm_year = year - 1900; // tm_year is year since 1900
        time.tm_mon = month - 1;     // tm_mon is 0-11
        time.tm_mday = day;

        std::mktime(&time);
        const char* days[] = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };
        return days[time.tm_wday];
    }

    int daysInMonth(int month) const {
        if (month < 1 || month > 12) return 0;
        static const int days[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
        int leapYear = (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
        return (month == 2 && leapYear) ? 29 : days[month - 1];
    }

    int calculateDays(int startDay, int startMonth, int startYear, int endDay, int endMonth, int endYear) const {
        std::tm start = {};
        start.tm_year = startYear - 1900;
        start.tm_mon = startMonth - 1;
        start.tm_mday = startDay;

        std::tm end = {};
        end.tm_year = endYear - 1900;
        end.tm_mon = endMonth - 1;
        end.tm_mday = endDay;

        std::time_t startTime = std::mktime(&start);
        std::time_t endTime = std::mktime(&end);

        return static_cast<int>(std::difftime(endTime, startTime) / (60 * 60 * 24));
    }

    bool equals(const DateComponent& other) const override {
        const Year* otherYear = dynamic_cast<const Year*>(&other);
        return otherYear && day == otherYear->day && month == otherYear->month && year == otherYear->year;
    }

    int hashCode() const override {
        return day * 10000 + month * 100 + year;
    }

    std::string toString() const override {
        std::stringstream ss;
        ss << day << "-" << month << "-" << year;
        return ss.str();
    }


};

int main(int argc, char** argv) {
    Year year(2023);
    year.setDate(29, 9, 2023);

    std::cout << "Day of the week: " << year.getWeekday() << std::endl;
    std::cout << "The amount of days in September 2023: " << year.daysInMonth(9) << std::endl;

    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

// Google Test для методов
class DayTest : public ::testing::Test {
protected:
    Day day{ 15 };
};

TEST_F(DayTest, TestGetValue) {
    EXPECT_EQ(day.getValue(), 15);
}

TEST_F(DayTest, TestEquals) {
    Day otherDay(15);
    Day differentDay(16);
    EXPECT_TRUE(day.equals(otherDay));
    EXPECT_FALSE(day.equals(differentDay));
}

TEST_F(DayTest, TestHashCode) {
    EXPECT_EQ(day.hashCode(), 15);
}

TEST_F(DayTest, TestToString) {
    EXPECT_EQ(day.toString(), "15");
}

class MonthTest : public ::testing::Test {
protected:
    Month month{ 9 };
};

TEST_F(MonthTest, TestGetValue) {
    EXPECT_EQ(month.getValue(), 9);
}

TEST_F(MonthTest, TestEquals) {
    Month otherMonth(9);
    Month differentMonth(10);
    EXPECT_TRUE(month.equals(otherMonth));
    EXPECT_FALSE(month.equals(differentMonth));
}

TEST_F(MonthTest, TestHashCode) {
    EXPECT_EQ(month.hashCode(), 9);
}

TEST_F(MonthTest, TestToString) {
    EXPECT_EQ(month.toString(), "9");
}

class YearTest : public ::testing::Test {
protected:
    Year year{ 2023 };

    void SetUp() override {
        year.setDate(29, 9, 2023);
    }
};

TEST_F(YearTest, TestGetWeekday) {
    EXPECT_EQ(year.getWeekday(), "Friday");
}

TEST_F(YearTest, TestDaysInMonth) {
    EXPECT_EQ(year.daysInMonth(2), 28); // Невисокосный год
    year.setDate(29, 2, 2024); // Установка даты на високосный год
    EXPECT_EQ(year.daysInMonth(2), 29);
}

TEST_F(YearTest, TestCalculateDays) {
    EXPECT_EQ(year.calculateDays(1, 1, 2023, 1, 1, 2024), 365);
    EXPECT_EQ(year.calculateDays(29, 9, 2023, 29, 9, 2024), 366); // Високосный год
}

TEST_F(YearTest, TestEquals) {
    Year otherYear(2023);
    otherYear.setDate(29, 9, 2023);
    Year differentYear(2024);
    EXPECT_TRUE(year.equals(otherYear));
    EXPECT_FALSE(year.equals(differentYear));
}

TEST_F(YearTest, TestHashCode) {
    EXPECT_EQ(year.hashCode(), 29 * 10000 + 9 * 100 + 2023);
}

TEST_F(YearTest, TestToString) {
    EXPECT_EQ(year.toString(), "29-9-2023");
}