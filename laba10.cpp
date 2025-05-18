#include <iostream>
#include <vector>
#include <memory>
#include <fstream>
#include <sstream>
#include <algorithm>

// Базовый класс Пользователь
class User {
private:
    std::string name;
    int id;
    int accessLevel;

public:
    User(const std::string& name, int id, int accessLevel)
        : name(name), id(id), accessLevel(accessLevel) {
        if (name.empty()) throw std::invalid_argument("Имя не может быть пустым.");
        if (accessLevel < 0) throw std::invalid_argument("Уровень доступа не может быть отрицательным.");
    }

    virtual ~User() = default;

    const std::string& getName() const { return name; }
    int getId() const { return id; }
    int getAccessLevel() const { return accessLevel; }

    void setName(const std::string& newName) {
        if (newName.empty()) throw std::invalid_argument("Имя не может быть пустым.");
        name = newName;
    }

    void setId(int newId) { id = newId; }
    void setAccessLevel(int level) {
        if (level < 0) throw std::invalid_argument("Уровень доступа не может быть отрицательным.");
        accessLevel = level;
    }

    virtual void displayInfo() const {
        std::cout << "Имя: " << name
            << ", ID: " << id
            << ", Уровень доступа: " << accessLevel;
    }
};

// Производные классы
class Student : public User {
private:
    std::string group;

public:
    Student(const std::string& name, int id, int accessLevel, const std::string& group)
        : User(name, id, accessLevel), group(group) {
    }

    void displayInfo() const override {
        User::displayInfo();
        std::cout << ", Группа: " << group << std::endl;
    }
};

class Teacher : public User {
private:
    std::string department;

public:
    Teacher(const std::string& name, int id, int accessLevel, const std::string& department)
        : User(name, id, accessLevel), department(department) {
    }

    void displayInfo() const override {
        User::displayInfo();
        std::cout << ", Кафедра: " << department << std::endl;
    }
};

class Administrator : public User {
public:
    Administrator(const std::string& name, int id, int accessLevel)
        : User(name, id, accessLevel) {
    }

    void displayInfo() const override {
        User::displayInfo();
        std::cout << ", Роль: Администратор" << std::endl;
    }
};

// Класс Ресурс
class Resource {
private:
    std::string name;
    int requiredAccessLevel;

public:
    Resource(const std::string& name, int requiredAccessLevel)
        : name(name), requiredAccessLevel(requiredAccessLevel) {
    }

    // Проверка доступа пользователя к ресурсу
    bool checkAccess(const User& user) const {
        return user.getAccessLevel() >= requiredAccessLevel;
    }

    void displayInfo() const {
        std::cout << "Ресурс: " << name
            << ", Требуемый уровень доступа: " << requiredAccessLevel << std::endl;
    }

    const std::string& getName() const { return name; }
};

// Шаблонный класс системы контроля доступа
template<typename T>
class AccessControlSystem {
private:
    std::vector<std::unique_ptr<T>> items;

public:
    // Добавление элемента в систему
    void addItem(std::unique_ptr<T> item) {
        items.push_back(std::move(item));
    }

    // Отображение всех элементов
    void displayAll() const {
        for (const auto& item : items) {
            item->displayInfo(); // Полиморфизм здесь
        }
    }

    const std::vector<std::unique_ptr<T>>& getAllItems() const {
        return items;
    }

    // Сохранение в файл
    void saveToFile(const std::string& filename) const {
        std::ofstream out(filename);
        if (!out.is_open()) {
            throw std::runtime_error("Не удалось открыть файл для записи.");
        }

        for (const auto& item : items) {
            out << item->getName() << std::endl;
        }
    }

    // Загрузка из файла
    void loadFromFile(const std::string& filename) {
        std::ifstream in(filename);
        if (!in.is_open()) {
            throw std::runtime_error("Не удалось открыть файл для чтения.");
        }

        std::string name;
        while (std::getline(in, name)) {
            items.push_back(std::make_unique<Resource>(name, 2)); // Упрощенный пример
        }
    }

    // Сортировка элементов
    template<typename Compare>
    void sortItems(Compare comp) {
        std::sort(items.begin(), items.end(), comp);
    }
};

int main() {
    try {
        std::vector<std::unique_ptr<User>> users;
        users.push_back(std::make_unique<Student>("Иван Петров", 1, 2, "Группа А"));
        users.push_back(std::make_unique<Teacher>("Анна Смирнова", 2, 5, "Физика"));
        users.push_back(std::make_unique<Administrator>("Дмитрий Иванов", 3, 7));

        std::cout << "Информация о пользователях:\n";
        for (const auto& user : users) {
            user->displayInfo();
            std::cout << std::endl;
        }

        std::vector<std::unique_ptr<Resource>> resources;
        resources.push_back(std::make_unique<Resource>("Библиотека", 3));
        resources.push_back(std::make_unique<Resource>("Лаборатория", 5));

        std::cout << "\nПроверка доступа:\n";
        for (const auto& resource : resources) {
            for (const auto& user : users) {
                std::cout << user->getName() << " -> " << resource->getName() << ": ";
                std::cout << (resource->checkAccess(*user) ? "Доступ разрешен" : "Доступ запрещен") << std::endl;
            }
        }

        // Использование шаблонного класса
        AccessControlSystem<Resource> resSystem;
        resSystem.addItem(std::make_unique<Resource>("Комната 101", 2));
        resSystem.addItem(std::make_unique<Resource>("Компьютерный класс", 4));
        resSystem.displayAll();

        // Работа с файлами
        resSystem.saveToFile("resources.txt");
        AccessControlSystem<Resource> loadedSystem;
        loadedSystem.loadFromFile("resources.txt");
        std::cout << "\nЗагруженные ресурсы из файла:\n";
        loadedSystem.displayAll();

        // Сортировка пользователей по уровню доступа
        std::sort(users.begin(), users.end(), [](const std::unique_ptr<User>& a, const std::unique_ptr<User>& b) {
            return a->getAccessLevel() > b->getAccessLevel();
            });

        std::cout << "\nПользователи, отсортированные по уровню доступа:\n";
        for (const auto& user : users) {
            user->displayInfo();
            std::cout << std::endl;
        }

    }
    catch (const std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << std::endl;
    }

    return 0;
}
