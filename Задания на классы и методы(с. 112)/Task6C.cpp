#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <string>
#include <cassert>

#define _USE_MATH_DEFINES
#include <math.h>

using namespace std;

// Класс для представления рациональной дроби
class Rational {
private:
    int numerator;   // Числитель
    int denominator; // Знаменатель

    // Метод для нахождения наибольшего общего делителя (НОД)
    int gcd(int a, int b) const {
        while (b != 0) {
            int temp = b;
            b = a % b;
            a = temp;
        }
        return abs(a);
    }

public:
    // Конструктор
    Rational(int num = 0, int denom = 1) : numerator(num), denominator(denom) {
        if (denom == 0) {
            throw invalid_argument("Denominator cannot be zero");
        }
        simplify();
    }

    // Метод для упрощения дроби
    void simplify() {
        int gcdValue = gcd(numerator, denominator);
        numerator /= gcdValue;
        denominator /= gcdValue;
        if (denominator < 0) { // Знак дроби
            numerator = -numerator;
            denominator = -denominator;
        }
    }

    // Получение значения дроби в виде double
    double value() const {
        return static_cast<double>(numerator) / denominator;
    }

    // Метод для вывода значения дроби
    void display() const {
        cout << numerator << "/" << denominator;
    }
};

// Класс для окружности
class Circle {
private:
    Rational centerX; // Координата X центра
    Rational centerY; // Координата Y центра
    double radius;    // Радиус окружности

public:
    // Конструктор
    Circle(Rational x, Rational y, double r) : centerX(x), centerY(y), radius(r) {}

    // Метод для вычисления площади
    double area() const {
        return M_PI * radius * radius;
    }

    // Метод для вычисления периметра (длины окружности)
    double perimeter() const {
        return 2 * M_PI * radius;
    }

    // Метод для получения центра
    Rational getCenterX() const { return centerX; }
    Rational getCenterY() const { return centerY; }

    // Метод для вывода информации об окружности
    void display() const {
        cout << "Circle(center: (";
        centerX.display();
        cout << ", ";
        centerY.display();
        cout << "), radius: " << radius << ")" << endl;
    }
};

// Функция для проверки, лежат ли три точки на одной прямой
bool areCollinear(const Rational& x1, const Rational& y1, const Rational& x2, const Rational& y2, const Rational& x3, const Rational& y3) {
    // Используем детерминант для проверки коллинеарности
    return (y2.value() - y1.value()) * (x3.value() - x2.value()) == (y3.value() - y2.value()) * (x2.value() - x1.value());
}

// Функция для группировки окружностей по центрам, лежащим на одной прямой
void groupCirclesByLine(const vector<Circle>& circles) {
    map<string, vector<Circle>> groups;

    // Перебираем все окружности
    for (size_t i = 0; i < circles.size(); ++i) {
        for (size_t j = i + 1; j < circles.size(); ++j) {
            Rational x1 = circles[i].getCenterX();
            Rational y1 = circles[i].getCenterY();
            Rational x2 = circles[j].getCenterX();
            Rational y2 = circles[j].getCenterY();

            vector<Circle> collinearCircles = { circles[i], circles[j] };

            for (size_t k = 0; k < circles.size(); ++k) {
                if (k != i && k != j) {
                    Rational x3 = circles[k].getCenterX();
                    Rational y3 = circles[k].getCenterY();

                    // Проверяем, лежат ли три точки на одной прямой
                    if (areCollinear(x1, y1, x2, y2, x3, y3)) {
                        collinearCircles.push_back(circles[k]);
                    }
                }
            }

            // Если найдены коллинеарные окружности, добавляем их в группу
            if (collinearCircles.size() > 2) {
                string key = to_string(x1.value()) + "," + to_string(y1.value()) + ";" +
                    to_string(x2.value()) + "," + to_string(y2.value());
                groups[key] = collinearCircles;
            }
        }
    }

    // Удаляем дубликаты и выводим группы
    set<string> printedGroups;
    for (const auto& group : groups) {
        if (printedGroups.find(group.first) == printedGroups.end()) {
            printedGroups.insert(group.first);
            cout << "Group centered at line formed by points: " << group.first << endl;
            for (const auto& circle : group.second) {
                circle.display();
            }
        }
    }
}

// Функция для нахождения окружностей с наибольшей и наименьшей площадью
void findMinMaxArea(const vector<Circle>& circles) {
    if (circles.empty()) return;

    const Circle* minCircle = &circles[0];
    const Circle* maxCircle = &circles[0];

    for (const auto& circle : circles) {
        if (circle.area() < minCircle->area()) {
            minCircle = &circle;
        }
        if (circle.area() > maxCircle->area()) {
            maxCircle = &circle;
        }
    }

    cout << "Circle with minimum area: ";
    minCircle->display();
    cout << "Circle with maximum area: ";
    maxCircle->display();
}

int main() {
    // Test Rational class
    Rational r1(1, 2);
    assert(r1.value() == 0.5); // Test value method
    r1.simplify();
    assert(r1.value() == 0.5); // Simplify method should not change the value

    // Test Circle class
    Circle c1(Rational(1, 2), Rational(3, 4), 5.0);
    assert(abs(c1.area() - 78.5398) < 0.0001); // Test area method
    assert(abs(c1.perimeter() - 31.4159) < 0.0001); // Test perimeter method

    // Test areCollinear function
    bool collinear = areCollinear(Rational(1, 2), Rational(1, 2), Rational(2, 3), Rational(2, 3), Rational(3, 4), Rational(3, 4));
    assert(collinear == true); // Testing for collinearity

    vector<Circle> circles;

    // Создаем окружности
    circles.emplace_back(Rational(1, 2), Rational(3, 4), 5.0);
    circles.emplace_back(Rational(2, 3), Rational(6, 8), 3.0);
    circles.emplace_back(Rational(1, 2), Rational(3, 4), 2.0);
    circles.emplace_back(Rational(5, 6), Rational(7, 8), 4.0);

    // Группировка окружностей по центрам на одной прямой
    groupCirclesByLine(circles);

    // Нахождение окружностей с наибольшей и наименьшей площадью
    findMinMaxArea(circles);

    cout << "All tests passed successfully!" << endl;

    return 0;
}
