#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <stdexcept>
#include <cassert>

using namespace std;
const int CURRENT_YEAR = 2024;

class Car {
private:
    int id;
    string brand;
    string model;
    int year;
    string color;
    double price;
    string regNumber;

public:
    // Конструктор
    Car(int id, const string& brand, const string& model, int year, const string& color, double price, const string& regNumber)
        : id(id), brand(brand), model(model), year(year), color(color), price(price), regNumber(regNumber) {
        if (year > CURRENT_YEAR) throw invalid_argument("Year cannot be greater than current year.");
        if (brand.empty()) throw invalid_argument("Brand cannot be empty.");
        if (model.empty()) throw invalid_argument("Model cannot be empty.");
        if (id < 0) throw invalid_argument("ID cannot be negative.");
        if (price < 0) throw invalid_argument("Price cannot be negative.");
    }

    // Методы доступа
    string getBrand() const { return brand; }
    string getModel() const { return model; }
    int getYear() const { return year; }
    double getPrice() const { return price; }
    string getRegNumber() const { return regNumber; }

    // Метод для представления объекта в виде строки
    string toString() const {
        return "ID: " + to_string(id) + ", Brand: " + brand + ", Model: " + model +
            ", Year: " + to_string(year) + ", Color: " + color +
            ", Price: " + to_string(price) + ", Reg Number: " + regNumber;
    }

    // Оператор сравнения для тестов
    bool operator==(const Car& other) const {
        return id == other.id && brand == other.brand && model == other.model &&
            year == other.year && color == other.color && price == other.price &&
            regNumber == other.regNumber;
    }
};

class CarManager {
private:
    vector<Car> cars;

public:
    // Метод для добавления автомобиля
    void addCar(const Car& car) {
        for (const auto& existingCar : cars) {
            if (existingCar.getRegNumber() == car.getRegNumber()) {
                throw invalid_argument("Registration number must be unique.");
            }
        }
        cars.push_back(car);
    }

    // a) Список автомобилей заданной марки
    vector<Car> listCarsByBrand(const string& brand) const {
        vector<Car> result;
        for (const auto& car : cars) {
            if (car.getBrand() == brand) {
                result.push_back(car);
            }
        }
        return result;
    }

    // b) Список автомобилей заданной модели, которые эксплуатируются больше n лет
    vector<Car> listCarsByModelAndAge(const string& model, int n) const {
        vector<Car> result;
        for (const auto& car : cars) {
            if (car.getModel() == model && (CURRENT_YEAR - car.getYear() > n)) {
                result.push_back(car);
            }
        }
        return result;
    }

    // c) Список автомобилей заданного года выпуска, цена которых больше указанной
    vector<Car> listCarsByYearAndPrice(int year, double minPrice) const {
        vector<Car> result;
        for (const auto& car : cars) {
            if (car.getYear() == year && car.getPrice() > minPrice) {
                result.push_back(car);
            }
        }
        return result;
    }
};

// Тестирование методов CarManager

void testCarConstructor() {
    // Тест на корректный конструктор
    try {
        Car car1(1, "Toyota", "Corolla", 2015, "Red", 15000, "ABC123");
        cout << "Car created: " << car1.toString() << endl; // Вывод созданного автомобиля
    }
    catch (const invalid_argument& e) {
        cout << "Error: " << e.what() << endl;
    }

    // Тест на некорректные данные
    try {
        Car car2(2, "", "Civic", 2023, "Blue", 20000, "XYZ456"); // Пустая марка
    }
    catch (const invalid_argument& e) {
        cout << "Caught an exception for empty brand: " << e.what() << endl;
    }

    try {
        Car car3(3, "Honda", "", 2023, "Blue", 20000, "XYZ456"); // Пустая модель
    }
    catch (const invalid_argument& e) {
        cout << "Caught an exception for empty model: " << e.what() << endl;
    }

    try {
        Car car4(4, "Ford", "Focus", 2025, "Black", 18000, "LMN789"); // Год больше текущего
    }
    catch (const invalid_argument& e) {
        cout << "Caught an exception for year greater than current year: " << e.what() << endl;
    }

    try {
        Car car5(-1, "Toyota", "Corolla", 2020, "Red", 15000, "ABC123"); // Отрицательный ID
    }
    catch (const invalid_argument& e) {
        cout << "Caught an exception for negative ID: " << e.what() << endl;
    }

    cout << "Test for Car constructor completed." << endl;
}

void testListCarsByBrand() {
    CarManager manager;
    manager.addCar(Car(1, "Toyota", "Corolla", 2015, "Red", 15000, "ABC123"));
    manager.addCar(Car(2, "Honda", "Civic", 2023, "Blue", 20000, "XYZ456"));
    manager.addCar(Car(3, "Toyota", "Camry", 2020, "Black", 30000, "LMN789"));

    vector<Car> expected = {
        Car(1, "Toyota", "Corolla", 2015, "Red", 15000, "ABC123"),
        Car(3, "Toyota", "Camry", 2020, "Black", 30000, "LMN789")
    };

    vector<Car> result = manager.listCarsByBrand("Toyota");

    assert(result == expected); // Сравниваем списки автомобилей
    cout << "Test for listCarsByBrand passed." << endl;
}

void testCarRegistrationNumberUniqueness() {
    CarManager manager;
    manager.addCar(Car(1, "Toyota", "Corolla", 2015, "Red", 15000, "ABC123"));

    // Тест на уникальность регистрационного номера
    try {
        manager.addCar(Car(2, "Honda", "Civic", 2023, "Blue", 20000, "ABC123")); // Дублирующийся номер
    }
    catch (const invalid_argument& e) {
        cout << "Caught an exception for duplicate registration number: " << e.what() << endl;
    }

    try {
        manager.addCar(Car(3, "Ford", "Focus", 2016, "Black", 18000, "XYZ456")); // Уникальный номер
        cout << "Car created with unique registration number." << endl;
    }
    catch (const invalid_argument& e) {
        cout << "Error: " << e.what() << endl;
    }

    cout << "Test for car registration number uniqueness completed." << endl;
}

void testListCarsByModelAndAge() {
    CarManager manager;
    manager.addCar(Car(1, "Toyota", "Corolla", 2015, "Red", 15000, "ABC123"));
    manager.addCar(Car(2, "Honda", "Civic", 2018, "Blue", 20000, "XYZ456"));
    manager.addCar(Car(3, "Honda", "Civic", 2010, "Black", 18000, "LMN789"));

    vector<Car> expected = {
        Car(3, "Honda", "Civic", 2010, "Black", 18000, "LMN789")
    };

    vector<Car> result = manager.listCarsByModelAndAge("Civic", 10);

    assert(result == expected); // Сравниваем результат с ожидаемым
    cout << "Test for listCarsByModelAndAge passed." << endl;
}

void testListCarsByYearAndPrice() {
    CarManager manager;
    manager.addCar(Car(1, "Toyota", "Corolla", 2015, "Red", 15000, "ABC123"));
    manager.addCar(Car(2, "Honda", "Civic", 2023, "Blue", 20000, "XYZ456"));
    manager.addCar(Car(3, "Ford", "Focus", 2015, "Black", 18000, "LMN789"));

    vector<Car> expected = {
        Car(1, "Toyota", "Corolla", 2015, "Red", 15000, "ABC123"),
        Car(3, "Ford", "Focus", 2015, "Black", 18000, "LMN789")
    };

    vector<Car> result = manager.listCarsByYearAndPrice(2015, 14000);

    assert(result == expected); // Сравниваем результат с ожидаемым
    cout << "Test for listCarsByYearAndPrice passed." << endl;
}

void runAllTests() {
    testCarConstructor(); // Запуск теста конструктора
    testListCarsByBrand();
    testCarRegistrationNumberUniqueness(); // Запуск теста уникальности регистрационного номера
    testListCarsByModelAndAge();
    testListCarsByYearAndPrice();
}

int main() {
    runAllTests(); // Запуск всех тестов
    return 0;
}