#include <iostream>
#include <vector>
#include <algorithm>
#include <cassert>
#define _USE_MATH_DEFINES
#include <cmath>
#include <math.h>

// Класс для работы с рациональными дробями
class Rational {
private:
    int numerator;
    int denominator;

    // Нахождение НОД для сокращения дробей
    int gcd(int a, int b) {
        while (b != 0) {
            int temp = b;
            b = a % b;
            a = temp;
        }
        return a;
    }

public:
    Rational(int num = 0, int denom = 1) : numerator(num), denominator(denom) {
        if (denominator == 0) throw std::invalid_argument("Denominator cannot be zero.");
        simplify();
    }

    void simplify() {
        int g = gcd(numerator, denominator);
        numerator /= g;
        denominator /= g;
        if (denominator < 0) {
            numerator = -numerator;
            denominator = -denominator;
        }
    }

    double toDouble() const {
        return static_cast<double>(numerator) / denominator;
    }

    friend std::ostream& operator<<(std::ostream& os, const Rational& r) {
        os << r.numerator << "/" << r.denominator;
        return os;
    }

    bool operator==(const Rational& other) const {
        return numerator == other.numerator && denominator == other.denominator;
    }
};

class Circle {
private:
    Rational x, y;
    double radius;

public:
    Circle(const Rational& x, const Rational& y, double r) : x(x), y(y), radius(r) {}

    double area() const {
        return M_PI * radius * radius;
    }

    double perimeter() const {
        return 2 * M_PI * radius;
    }

    const Rational& getX() const { return x; }
    const Rational& getY() const { return y; }

    void print() const {
        std::cout << "Circle: Center(" << x << ", " << y << "), Radius: " << radius
            << ", Area: " << area() << ", Perimeter: " << perimeter() << std::endl;
    }

    static bool areCollinear(const Circle& c1, const Circle& c2, const Circle& c3) {
        double x1 = c1.getX().toDouble(), y1 = c1.getY().toDouble();
        double x2 = c2.getX().toDouble(), y2 = c2.getY().toDouble();
        double x3 = c3.getX().toDouble(), y3 = c3.getY().toDouble();

        return (x1 * (y2 - y3) + x2 * (y3 - y1) + x3 * (y1 - y2)) == 0;
    }

    static std::vector<std::tuple<int, int, int>> findCollinearGroups(const std::vector<Circle>& circles) {
        int n = circles.size();
        std::vector<std::tuple<int, int, int>> collinearGroups;

        for (int i = 0; i < n - 2; ++i) {
            for (int j = i + 1; j < n - 1; ++j) {
                for (int k = j + 1; k < n; ++k) {
                    if (Circle::areCollinear(circles[i], circles[j], circles[k])) {
                        collinearGroups.emplace_back(i + 1, j + 1, k + 1);  // Индексы окружностей, начиная с 1
                    }
                }
            }
        }

        return collinearGroups;
    }

    static Circle findMinArea(const std::vector<Circle>& circles) {
        return *std::min_element(circles.begin(), circles.end(), [](const Circle& a, const Circle& b) {
            return a.area() < b.area();
            });
    }

    static Circle findMaxArea(const std::vector<Circle>& circles) {
        return *std::max_element(circles.begin(), circles.end(), [](const Circle& a, const Circle& b) {
            return a.area() < b.area();
            });
    }

    static Circle findMinPerimeter(const std::vector<Circle>& circles) {
        return *std::min_element(circles.begin(), circles.end(), [](const Circle& a, const Circle& b) {
            return a.perimeter() < b.perimeter();
            });
    }

    static Circle findMaxPerimeter(const std::vector<Circle>& circles) {
        return *std::max_element(circles.begin(), circles.end(), [](const Circle& a, const Circle& b) {
            return a.perimeter() < b.perimeter();
            });
    }
};

// Тесты для методов класса Circle
void testRationalDenominator() {

    // Тест на случай с нулевым знаменателем
    try {
        Rational r1(1, 0); // Должен выбросить исключение
        std::cout << "Rational created: " << r1 << std::endl;
    }
    catch (const std::invalid_argument& e) {
        std::cout << "Caught an exception for zero denominator: " << e.what() << std::endl;
    }

    std::cout << "Test for Rational denominator completed.\n";
}

void testMinMaxArea() {
    std::vector<Circle> circles = {
        Circle(Rational(1, 2), Rational(3, 4), 2.5),
        Circle(Rational(0, 1), Rational(0, 1), 1.5),
        Circle(Rational(2, 1), Rational(3, 1), 3.0)
    };

    Circle minCircle = Circle::findMinArea(circles);
    Circle maxCircle = Circle::findMaxArea(circles);

    // Ожидаемые значения площадей
    double expectedMinArea = M_PI * 1.5 * 1.5;
    double expectedMaxArea = M_PI * 3.0 * 3.0;

    assert(std::abs(minCircle.area() - expectedMinArea) < 0.001);
    assert(std::abs(maxCircle.area() - expectedMaxArea) < 0.001);

    std::cout << "Test findMinMaxArea passed successfully.\n";
}

void testMinMaxPerimeter() {
    std::vector<Circle> circles = {
        Circle(Rational(1, 2), Rational(3, 4), 2.5),
        Circle(Rational(0, 1), Rational(0, 1), 1.5),
        Circle(Rational(2, 1), Rational(3, 1), 3.0)
    };

    Circle minCircle = Circle::findMinPerimeter(circles);
    Circle maxCircle = Circle::findMaxPerimeter(circles);

    // Ожидаемые значения периметров
    double expectedMinPerimeter = 2 * M_PI * 1.5;
    double expectedMaxPerimeter = 2 * M_PI * 3.0;

    assert(std::abs(minCircle.perimeter() - expectedMinPerimeter) < 0.001);
    assert(std::abs(maxCircle.perimeter() - expectedMaxPerimeter) < 0.001);

    std::cout << "Test findMinMaxPerimeter passed successfully.\n";
}

void testCollinearGroups() {
    std::vector<Circle> circles = {
        Circle(Rational(1, 1), Rational(1, 1), 2.5),
        Circle(Rational(2, 1), Rational(2, 1), 1.5),
        Circle(Rational(3, 1), Rational(3, 1), 3.0),
        Circle(Rational(2, 1), Rational(4, 1), 1.2),
        Circle(Rational(4, 1), Rational(2, 1), 1.2)
    };

    std::vector<std::tuple<int, int, int>> expectedGroups = {
        {1, 2, 3},
        {3, 4, 5} // Ожидаем, что окружности с центрами (1/2, 1), (1, 2) и (3/2, 3) коллинеарны
    };

    std::vector<std::tuple<int, int, int>> actualGroups = Circle::findCollinearGroups(circles);

    // Сравнение ожидаемых и фактических групп
    assert(expectedGroups == actualGroups);

    std::cout << "Test findCollinearGroups passed successfully.\n";
}

void testCollinearCheck() {
    Circle c1(Rational(1, 2), Rational(1, 1), 2.5);
    Circle c2(Rational(1, 1), Rational(2, 1), 1.5);
    Circle c3(Rational(3, 2), Rational(3, 1), 3.0);

    bool areCollinear = Circle::areCollinear(c1, c2, c3);
    assert(areCollinear == true);  // Ожидаем, что центры окружностей коллинеарны

    std::cout << "Test areCollinear passed successfully.\n";
}

int main() {
    testRationalDenominator(); // Запуск теста на знаменатель
    testMinMaxArea();
    testMinMaxPerimeter();
    testCollinearGroups();
    testCollinearCheck();
    std::cout << "All tests passed successfully.\n";
    return 0;
}