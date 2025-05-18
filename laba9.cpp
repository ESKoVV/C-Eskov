#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <mutex>
#include <memory>
#include <stdexcept>

// ==================== Логгер ====================
template<typename T>
class Logger {
private:
    static std::ofstream logFile;  // Файл для записи логов
    static std::mutex mtx;         // Мьютекс для потокобезопасности

public:
    static void log(const T& message);  // Метод для записи сообщения в лог
};

// Инициализация статических членов класса
template<typename T>
std::ofstream Logger<T>::logFile("game.log", std::ios::app);

template<typename T>
std::mutex Logger<T>::mtx;

// Специализация метода log для строк
template<>
void Logger<std::string>::log(const std::string& message) {
    std::lock_guard<std::mutex> lock(mtx);  // Блокировка мьютекса
    if (logFile.is_open()) {
        logFile << message << std::endl;  // Запись сообщения в файл
    }
}

// ==================== Инвентарь ====================
class Inventory {
private:
    std::vector<std::string> items;  // Список предметов

public:
    // Добавление предмета в инвентарь
    void addItem(const std::string& item) {
        items.push_back(item);
        std::cout << "Добавлен предмет: " << item << " в инвентарь." << std::endl;
    }

    // Удаление предмета из инвентаря
    void removeItem(const std::string& item) {
        for (auto it = items.begin(); it != items.end(); ++it) {
            if (*it == item) {
                items.erase(it);
                std::cout << "Удален предмет: " << item << " из инвентаря." << std::endl;
                return;
            }
        }
        std::cout << "Предмет не найден в инвентаре." << std::endl;
    }

    // Отображение содержимого инвентаря
    void displayItems() const {
        std::cout << "Инвентарь:" << std::endl;
        for (const auto& item : items) {
            std::cout << "- " << item << std::endl;
        }
    }
};

// ==================== Персонаж ====================
class Monster;  // Предварительное объявление класса Монстр

class Character {
private:
    std::string name;      // Имя персонажа
    int health;            // Здоровье
    int attack;            // Сила атаки
    int defense;           // Защита
    int level;             // Уровень
    int experience;        // Опыт
    Inventory inventory;   // Инвентарь

public:
    Character(const std::string& n, int h, int a, int d)
        : name(n), health(h), attack(a), defense(d), level(1), experience(0) {
    }

    // Методы персонажа
    void attackEnemy(Monster& enemy);          // Атаковать врага
    void heal(int amount);                     // Лечение
    void gainExperience(int exp);              // Получение опыта
    void displayInfo() const;                  // Отображение информации
    void addItemToInventory(const std::string& item);  // Добавить предмет
    void showInventory() const;                // Показать инвентарь
    
    // Геттеры
    std::string getName() const { return name; }
    int getHealth() const { return health; }
    int getAttack() const { return attack; }
    int getDefense() const { return defense; }
};

// ==================== Монстр ====================
class Monster {
protected:
    std::string name;    // Имя монстра
    int health;          // Здоровье
    int attack;          // Сила атаки
    int defense;         // Защита

public:
    virtual ~Monster() = default;
    
    // Виртуальные методы
    virtual void takeDamage(int damage);       // Получение урона
    virtual void attackPlayer(Character& player); // Атака игрока
    virtual void displayInfo() const;          // Отображение информации
    
    // Геттеры
    virtual std::string getName() const { return name; }
    virtual int getHealth() const { return health; }
    virtual int getAttack() const { return attack; }
    virtual int getDefense() const { return defense; }
};

// Классы конкретных монстров
class Goblin : public Monster {
public:
    Goblin() {
        name = "Гоблин";
        health = 30;
        attack = 8;
        defense = 2;
    }
};

class Dragon : public Monster {
public:
    Dragon() {
        name = "Дракон";
        health = 150;
        attack = 25;
        defense = 10;
    }
};

class Skeleton : public Monster {
public:
    Skeleton() {
        name = "Скелет";
        health = 40;
        attack = 10;
        defense = 5;
    }
};

// Реализация методов персонажа
void Character::attackEnemy(Monster& enemy) {
    int damage = getAttack() - enemy.getDefense();
    if (damage > 0) {
        enemy.takeDamage(damage);
        std::cout << name << " атакует " << enemy.getName() << " и наносит " << damage << " урона!" << std::endl;
        Logger<std::string>::log(name + " атакует " + enemy.getName() + " и наносит " + std::to_string(damage) + " урона!");
    }
    else {
        std::cout << name << " атакует " << enemy.getName() << ", но безрезультатно!" << std::endl;
        Logger<std::string>::log(name + " атакует " + enemy.getName() + ", но безрезультатно!");
    }
}

void Character::heal(int amount) {
    health += amount;
    if (health > 100) health = 100;
    std::cout << name << " восстанавливает " << amount << " здоровья!" << std::endl;
    Logger<std::string>::log(name + " восстанавливает " + std::to_string(amount) + " здоровья!");
}

void Character::gainExperience(int exp) {
    experience += exp;
    while (experience >= 100 * level) {
        level++;
        experience -= 100 * level;
        attack += 2;
        defense += 1;
        std::cout << name << " достиг уровня " << level << "!" << std::endl;
        Logger<std::string>::log(name + " достиг уровня " + std::to_string(level) + "!");
    }
}

void Character::displayInfo() const {
    std::cout << "Имя: " << name << ", HP: " << health
        << ", Атака: " << attack << ", Защита: " << defense
        << ", Уровень: " << level << ", Опыт: " << experience << std::endl;
}

void Character::addItemToInventory(const std::string& item) {
    inventory.addItem(item);
}

void Character::showInventory() const {
    inventory.displayItems();
}

// Реализация методов монстра
void Monster::takeDamage(int damage) {
    health -= damage;
    if (health <= 0) {
        health = 0;
        std::cout << name << " побежден!" << std::endl;
        Logger<std::string>::log(name + " побежден!");
    }
}

void Monster::attackPlayer(Character& player) {
    int damage = attack - player.getDefense();
    if (damage > 0) {
        player.attackEnemy(*this);
    }
    else {
        std::cout << name << " атакует " << player.getName() << ", но безрезультатно!" << std::endl;
        Logger<std::string>::log(name + " атакует " + player.getName() + ", но безрезультатно!");
    }
}

void Monster::displayInfo() const {
    std::cout << "Имя: " << name << ", HP: " << health
        << ", Атака: " << attack << ", Защита: " << defense << std::endl;
}

// ==================== Игра ====================
class Game {
private:
    Character player;  // Игровой персонаж

public:
    Game() : player("Герой", 100, 10, 5) {}

    void start();        // Начать игру
    void battle(Monster& monster);  // Битва с монстром
    void saveGame();     // Сохранить игру
    void loadGame();     // Загрузить игру
};

void Game::start() {
    std::cout << "Добро пожаловать в RPG приключение!" << std::endl;
    player.displayInfo();

    // Последовательные битвы с монстрами
    Goblin goblin;
    battle(goblin);

    Skeleton skeleton;
    battle(skeleton);

    Dragon dragon;
    battle(dragon);

    // Добавление предметов в инвентарь
    player.addItemToInventory("Меч");
    player.addItemToInventory("Зелье лечения");

    player.showInventory();

    saveGame();
    loadGame();
}

void Game::battle(Monster& monster) {
    std::cout << "\nПоявился дикий " << monster.getName() << "!\n";

    // Цикл битвы
    while (player.getHealth() > 0 && monster.getHealth() > 0) {
        player.attackEnemy(monster);
        if (monster.getHealth() <= 0) break;

        monster.attackPlayer(player);
        player.displayInfo();
    }

    if (player.getHealth() > 0) {
        std::cout << "Вы победили " << monster.getName() << "!\n";
        player.gainExperience(50);
    }
    else {
        std::cout << "Вы проиграли...\n";
        exit(0);
    }
}

void Game::saveGame() {
    std::ofstream out("savegame.dat");
    if (!out.is_open()) {
        std::cerr << "Ошибка открытия файла для сохранения.\n";
        return;
    }

    // Запись данных персонажа
    out << player.getName() << " "
        << player.getHealth() << " "
        << player.getAttack() << " "
        << player.getDefense() << " "
        << 1 << " "
        << 0 << std::endl;

    std::cout << "Игра сохранена.\n";
}

void Game::loadGame() {
    std::ifstream in("savegame.dat");
    if (!in.is_open()) {
        std::cerr << "Ошибка открытия файла для загрузки.\n";
        return;
    }

    // Чтение данных персонажа
    std::string name;
    int hp, att, def, level, exp;
    in >> name >> hp >> att >> def >> level >> exp;

    std::cout << "Игра загружена.\n";
    player = Character(name, hp, att, def);
    player.displayInfo();
}

// ==================== Главная функция ====================
int main() {
    try {
        Game game;
        game.start();
    }
    catch (const std::exception& ex) {
        std::cerr << "Ошибка: " << ex.what() << std::endl;
    }
    catch (...) {
        std::cerr << "Неизвестная ошибка!" << std::endl;
    }

    return 0;
}
