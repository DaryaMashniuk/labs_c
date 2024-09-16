#include <iostream>
#include <vector>
#include <string>
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
    // Конструкторы
    Car(int id, string brand, string model, int year, string color, double price, string regNumber)
        : id(id), brand(brand), model(model), year(year), color(color), price(price), regNumber(regNumber) {
        // Проверка на корректность данных
        assert(year <= CURRENT_YEAR); // Год не может быть больше текущего
        assert(!brand.empty()); // Марка не может быть пустой
        assert(!model.empty()); // Модель не может быть пустой
        assert(id >= 0); // Идентификатор не может быть отрицательным
        assert(brand.length() <= 50); // Длина марки не должна превышать 50 символов
        assert(model.length() <= 50); // Длина модели не должна превышать 50 символов
        assert(price >= 0); // Цена не может быть отрицательной
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
};

class CarManager {
private:
    vector<Car> cars;

public:
    // Метод для добавления автомобиля
    void addCar(const Car& car) {
        // Проверка на уникальность регистрационного номера
        for (const auto& existingCar : cars) {
            assert(existingCar.getRegNumber() != car.getRegNumber()); // Рег номер должен быть уникальным
        }
        cars.push_back(car);
    }

    // a) Список автомобилей заданной марки
    void listCarsByBrand(const string& brand) {
        cout << "Cars of brand " << brand << ":\n";
        for (const auto& car : cars) {
            if (car.getBrand() == brand) {
                cout << car.toString() << endl;
            }
        }
    }

    // b) Список автомобилей заданной модели, которые эксплуатируются больше n лет
    void listCarsByModelAndAge(const string& model, int n) {
        cout << "Cars of model " << model << " older than " << n << " years:\n";
        for (const auto& car : cars) {
            if (car.getModel() == model && (CURRENT_YEAR - car.getYear() > n)) {
                cout << car.toString() << endl;
            }
        }
    }

    // c) Список автомобилей заданного года выпуска, цена которых больше указанной
    void listCarsByYearAndPrice(int year, double minPrice) {
        cout << "Cars from year " << year << " with price greater than " << minPrice << ":\n";
        for (const auto& car : cars) {
            if (car.getYear() == year && car.getPrice() > minPrice) {
                cout << car.toString() << endl;
            }
        }
    }
};

void tests() {
    // Тестирование конструктора и логики
    Car car1(1, "Toyota", "Corolla", 2015, "Red", 15000, "ABC123");
    Car car2(2, "Honda", "Civic", 2023, "Blue", 20000, "XYZ456");
    Car car3(3, "Ford", "Focus", 2020, "Black", 25000, "LMN789");

    // Проверка на корректность данных
    assert(car1.getYear() <= CURRENT_YEAR); // Год выпуска не больше текущего
    assert(!car1.getBrand().empty()); // Марка не пустая
    assert(!car1.getModel().empty()); // Модель не пустая
    assert(car1.getPrice() >= 0); // Цена не отрицательная
    assert(car1.getRegNumber() == "ABC123"); // Регистрационный номер
    cout << "Test for Car constructor passed." << endl;

    // Тест на добавление автомобиля с одинаковым регистрационным номером
    CarManager manager;
    manager.addCar(car1);
    manager.addCar(car2);
    manager.addCar(car3);

    // Проверка на уникальность регистрационного номера
    try { 
        manager.addCar(Car(4, "Toyota", "Camry", 2020, "Black", 25000, "ABC123")); // Дублирующий номер
        assert(false); // Если не выбрасывает исключение, тест провален
    }
    catch (const std::exception&) {
        cout << "Test for unique registration number passed." << endl; // Ожидаемое поведение
    }

    // Примеры использования
    manager.listCarsByBrand("Toyota");
    manager.listCarsByModelAndAge("Civic", 5);
    manager.listCarsByYearAndPrice(2015, 14000);
}

int main() {
    tests(); // Запуск тестов
    return 0;
}