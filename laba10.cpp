#include <iostream>
#include <fstream>
#include <vector>
#include <memory>
#include <algorithm>
#include <sstream>
#include <stdexcept>

// === Базовый класс User ===
class User {
protected:
    std::string name;
    int id;
    int accessLevel;

public:
    User(const std::string& n, int i, int a)
        : name(n), id(i), accessLevel(a) {
        if (name.empty()) throw std::invalid_argument("Name cannot be empty!");
        if (a < 0) throw std::invalid_argument("Access level must be >= 0");
    }

    virtual void displayInfo() const = 0;

    std::string getName() const { return name; }
    int getId() const { return id; }
    int getAccessLevel() const { return accessLevel; }

    virtual ~User() = default;
};

// === Класс Student ===
class Student : public User {
private:
    std::string group;

public:
    Student(const std::string& n, int i, int a, const std::string& g)
        : User(n, i, a), group(g) {}

    void displayInfo() const override {
        std::cout << "Student: " << getName()
            << ", ID: " << getId()
            << ", Group: " << group
            << ", Access Level: " << getAccessLevel() << std::endl;
    }
};

// === Класс Teacher ===
class Teacher : public User {
private:
    std::string department;

public:
    Teacher(const std::string& n, int i, int a, const std::string& d)
        : User(n, i, a), department(d) {}

    void displayInfo() const override {
        std::cout << "Teacher: " << getName()
            << ", ID: " << getId()
            << ", Department: " << department
            << ", Access Level: " << getAccessLevel() << std::endl;
    }
};

// === Класс Administrator ===
class Administrator : public User {
private:
    std::string role;

public:
    Administrator(const std::string& n, int i, int a, const std::string& r)
        : User(n, i, a), role(r) {}

    void displayInfo() const override {
        std::cout << "Administrator: " << getName()
            << ", ID: " << getId()
            << ", Role: " << role
            << ", Access Level: " << getAccessLevel() << std::endl;
    }
};

// === Класс Resource ===
class Resource {
private:
    std::string name;
    int requiredAccessLevel;

public:
    Resource(const std::string& n, int r)
        : name(n), requiredAccessLevel(r) {
        if (r < 0) throw std::invalid_argument("Required access level must be >= 0");
    }

    bool checkAccess(const User& user) const {
        return user.getAccessLevel() >= requiredAccessLevel;
    }

    void displayInfo() const {
        std::cout << "Resource: " << name
            << ", Required Access Level: " << requiredAccessLevel << std::endl;
    }

    std::string getName() const { return name; }
};

// === Шаблонный класс AccessControlSystem ===
template <typename T>
class AccessControlSystem {
private:
    std::vector<std::unique_ptr<User>> users;
    std::vector<std::unique_ptr<Resource>> resources;

public:
    void addUser(std::unique_ptr<User> user) {
        users.push_back(std::move(user));
    }

    void addResource(std::unique_ptr<Resource> resource) {
        resources.push_back(std::move(resource));
    }

    void listUsers() const {
        for (const auto& user : users) {
            user->displayInfo();
        }
    }

    void listResources() const {
        for (const auto& resource : resources) {
            resource->displayInfo();
        }
    }

    void checkAccessForAll() const {
        for (const auto& user : users) {
            std::cout << "\nChecking access for: " << user->getName() << std::endl;
            for (const auto& resource : resources) {
                if (resource->checkAccess(*user)) {
                    std::cout << " - Has access to " << resource->getName() << std::endl;
                }
                else {
                    std::cout << " - NO access to " << resource->getName() << std::endl;
                }
            }
        }
    }

    void saveToFile(const std::string& filename) const {
        std::ofstream out(filename);
        if (!out) {
            throw std::runtime_error("Could not open file for writing: " + filename);
        }

        // Сохраняем пользователей
        out << users.size() << std::endl;
        for (const auto& user : users) {
            out << typeid(*user).name() << " "
                << user->getName() << " "
                << user->getId() << " "
                << user->getAccessLevel() << " ";
            if (auto student = dynamic_cast<Student*>(user.get())) {
                out << student->group << std::endl;
            }
            else if (auto teacher = dynamic_cast<Teacher*>(user.get())) {
                out << teacher->department << std::endl;
            }
            else if (auto admin = dynamic_cast<Administrator*>(user.get())) {
                out << admin->role << std::endl;
            }
        }

        // Сохраняем ресурсы
        out << resources.size() << std::endl;
        for (const auto& res : resources) {
            out << res->getName() << " " << res->requiredAccessLevel << std::endl;
        }
    }

    void loadFromFile(const std::string& filename) {
        std::ifstream in(filename);
        if (!in) {
            throw std::runtime_error("Could not open file for reading: " + filename);
        }

        size_t userCount;
        in >> userCount;
        in.ignore();

        for (size_t i = 0; i < userCount; ++i) {
            std::string type, name, extra;
            int id, access;

            in >> type >> name >> id >> access;
            std::getline(in, extra); // остальная часть строки

            if (type == typeid(Student).name()) {
                std::istringstream ss(extra);
                std::string group;
                ss >> group;
                users.push_back(std::make_unique<Student>(name, id, access, group));
            }
            else if (type == typeid(Teacher).name()) {
                std::istringstream ss(extra);
                std::string dept;
                ss >> dept;
                users.push_back(std::make_unique<Teacher>(name, id, access, dept));
            }
            else if (type == typeid(Administrator).name()) {
                std::istringstream ss(extra);
                std::string role;
                ss >> role;
                users.push_back(std::make_unique<Administrator>(name, id, access, role));
            }
        }

        size_t resCount;
        in >> resCount;
        for (size_t i = 0; i < resCount; ++i) {
            std::string name;
            int reqAccess;
            in >> name >> reqAccess;
            resources.push_back(std::make_unique<Resource>(name, reqAccess));
        }
    }

    // Поиск пользователя по имени
    std::vector<User*> findUsersByName(const std::string& name) const {
        std::vector<User*> result;
        for (const auto& user : users) {
            if (user->getName().find(name) != std::string::npos)
                result.push_back(user.get());
        }
        return result;
    }

    // Сортировка по уровню доступа
    void sortUsersByAccessLevel() {
        std::sort(users.begin(), users.end(),
            [](const std::unique_ptr<User>& a, const std::unique_ptr<User>& b) {
                return a->getAccessLevel() < b->getAccessLevel();
            });
    }
};

// === Точка входа ===
int main() {
    try {
        AccessControlSystem<void> system;

        // Добавляем пользователей
        system.addUser(std::make_unique<Student>("Alice", 101, 1, "CS-101"));
        system.addUser(std::make_unique<Teacher>("Bob", 201, 3, "Computer Science"));
        system.addUser(std::make_unique<Administrator>("Charlie", 301, 5, "IT"));

        // Добавляем ресурсы
        system.addResource(std::make_unique<Resource>("Library", 1));
        system.addResource(std::make_unique<Resource>("Server Room", 4));

        // Выводим информацию
        std::cout << "=== Users ===" << std::endl;
        system.listUsers();

        std::cout << "\n=== Resources ===" << std::endl;
        system.listResources();

        std::cout << "\n=== Checking Access ===" << std::endl;
        system.checkAccessForAll();

        // Сохраняем в файл
        system.saveToFile("university_data.txt");

        // Загружаем обратно
        AccessControlSystem<void> loadedSystem;
        loadedSystem.loadFromFile("university_data.txt");

        std::cout << "\n=== Loaded Users ===" << std::endl;
        loadedSystem.listUsers();

        std::cout << "\n=== Searching for Alice ===" << std::endl;
        auto found = loadedSystem.findUsersByName("Alice");
        for (auto* u : found) {
            u->displayInfo();
        }

        std::cout << "\n=== Sorting by Access Level ===" << std::endl;
        loadedSystem.sortUsersByAccessLevel();
        loadedSystem.listUsers();

    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}
