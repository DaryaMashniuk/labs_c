﻿#include <iostream>
#include <cmath>
#include <cassert>

using namespace std;

class Point {
private:
    double x; // Координата X
    double y; // Координата Y
    double z; // Координата Z
    double vx; // Скорость по оси X
    double vy; // Скорость по оси Y
    double vz; // Скорость по оси Z
    double ax; // Ускорение по оси X
    double ay; // Ускорение по оси Y
    double az; // Ускорение по оси Z
    double time; // Время

public:
    // Конструктор
    Point(double x = 0, double y = 0, double z = 0,
        double vx = 0, double vy = 0, double vz = 0,
        double ax = 0, double ay = 0, double az = 0,
        double time = 0)
        : x(x), y(y), z(z), vx(vx), vy(vy), vz(vz),
        ax(ax), ay(ay), az(az), time(time) {}

    // Арифметические операции
    Point operator+(const Point& other) const {
        return Point(x + other.x, y + other.y, z + other.z);
    }

    Point operator-(const Point& other) const {
        return Point(x - other.x, y - other.y, z - other.z);
    }

    Point operator*(double scalar) const {
        return Point(x * scalar, y * scalar, z * scalar);
    }

    Point operator/(double scalar) const {
        assert(scalar != 0); // Проверка деления на ноль
        return Point(x / scalar, y / scalar, z / scalar);
    }

    // Методы для определения скорости и ускорения
    void update(double dt) {
        time += dt;
        vx += ax * dt;
        vy += ay * dt;
        vz += az * dt;
        x += vx * dt;
        y += vy * dt;
        z += vz * dt;
    }

    double getSpeed() const {
        return sqrt(vx * vx + vy * vy + vz * vz);
    }

    double getAcceleration() const {
        return sqrt(ax * ax + ay * ay + az * az);
    }

    // Метод для проверки пересечения траекторий
    bool intersects(const Point& other) const {
        if (vx == other.vx && vy == other.vy && vz == other.vz) {
            double t = (x - other.x) / vx;
            return !((y - other.y) / vy == t && (z - other.z) / vz == t);
        }

        double tX = (other.x - x) / (vx - other.vx);
        double tY = (other.y - y) / (vy - other.vy);
        double tZ = (other.z - z) / (vz - other.vz);

        return (tX >= 0 && tY >= 0 && tZ >= 0) &&
            (fabs(tX - tY) < 1e-6) && (fabs(tY - tZ) < 1e-6);
    }

    // Метод для вычисления расстояния между двумя точками
    double distance(const Point& other) const {
        return sqrt(pow(x - other.x, 2) + pow(y - other.y, 2) + pow(z - other.z, 2));
    }

    // Метод для вывода информации о точке
    void print() const {
        cout << "Point(" << x << ", " << y << ", " << z << ") "
            << "with velocity(" << vx << ", " << vy << ", " << vz << "), "
            << "time: " << time << endl;
    }

    // Геттеры для тестирования
    double getX() const { return x; }
    double getY() const { return y; }
    double getZ() const { return z; }
    double getVx() const { return vx; }
    double getVy() const { return vy; }
    double getVz() const { return vz; }
};

// Функции для тестирования с тем же подходом:

void testAdditionOfPoints() {
    Point p1(1.0, 2.0, 3.0);
    Point p2(4.0, 5.0, 6.0);
    Point p3 = p1 + p2;

    assert(abs(p3.getX() - 5.0) < 1e-6);
    assert(abs(p3.getY() - 7.0) < 1e-6);
    assert(abs(p3.getZ() - 9.0) < 1e-6);
    cout << "Test Addition passed." << endl;
}

void testUpdatePosition() {
    Point p(0.0, 0.0, 0.0, 1.0, 1.0, 1.0);
    p.update(1.0);

    assert(abs(p.getX() - 1.0) < 1e-6);
    assert(abs(p.getY() - 1.0) < 1e-6);
    assert(abs(p.getZ() - 1.0) < 1e-6);
    cout << "Test Update Position passed." << endl;
}

void testIntersection() {
    Point p1(0.0, 0.0, 0.0, 1.0, 1.0, 1.0);
    Point p2(1.0, 1.0, 1.0, 1.0, 1.0, 1.0);

    assert(p1.intersects(p2) == false);
    cout << "Test Intersection passed." << endl;
}

void testDistanceBetweenPoints() {
    Point p1(0.0, 0.0, 0.0);
    Point p2(3.0, 4.0, 0.0);

    double dist = p1.distance(p2);
    assert(abs(dist - 5.0) < 1e-6);
    cout << "Test Distance passed." << endl;
}

void testIntersectionDifferentSpeeds() {
    Point p1(0.0, 0.0, 0.0, 1.0, 1.0, 1.0);
    Point p2(3.0, 3.0, 3.0, 0.0, 0.0, 0.0);

    assert(p1.intersects(p2) == true);
    cout << "Test Intersection Different Speeds passed." << endl;
}

// Основная функция для запуска тестов
int main() {
    testAdditionOfPoints();
    testUpdatePosition();
    testIntersection();
    testDistanceBetweenPoints();
    testIntersectionDifferentSpeeds();

    cout << "All tests passed successfully!" << endl;
    return 0;
}
