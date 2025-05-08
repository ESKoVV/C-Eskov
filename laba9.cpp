#include <iostream>
#include <fstream>
#include <vector>
#include <memory>
#include <algorithm>
#include <stdexcept>
#include <ctime>

// Класс исключения для случая смерти персонажа
class DeadCreatureException : public std::exception {
public:
    const char* what() const noexcept override {
        return "Creature has died!";
    }
};

// Базовый класс Creature
class Creature {
protected:
    std::string name;
    int health;
    int attack;
    int defense;

public:
    Creature(const std::string& n, int h, int a, int d)
        : name(n), health(h), attack(a), defense(d) {}

    virtual void takeDamage(int damage) {
        health -= damage;
        if (health <= 0) {
            health = 0;
            throw DeadCreatureException();
        }
    }

    virtual void attackTarget(Creature& target) = 0;

    virtual void displayInfo() const = 0;

    // Публичные геттеры для protected-полей
    int getHealth() const { return health; }
    int getAttack() const { return attack; }
    int getDefense() const { return defense; }
    const std::string& getName() const { return name; }

    virtual ~Creature() {}
};

// Класс Character
class Inventory; // Предварительное объявление

class Character : public Creature {
private:
    int level;
    int experience;
    Inventory* inventory;

public:
    Character(const std::string& n, int h, int a, int d)
        : Creature(n, h, a, d), level(1), experience(0), inventory(new Inventory()) {}

    ~Character() override {
        delete inventory;
    }

    void attackTarget(Creature& target) override {
        int damage = getAttack() - target.getDefense();
        if (damage > 0) {
            target.takeDamage(damage);
            std::cout << getName() << " attacks " << target.getName() << " for " << damage << " damage!" << std::endl;
        }
        else {
            std::cout << getName() << " attacks " << target.getName() << ", but it has no effect!" << std::endl;
        }
    }

    void heal(int amount) {
        health += amount;
        if (health > 100) health = 100;
        std::cout << getName() << " heals for " << amount << " HP!" << std::endl;
    }

    void gainExperience(int exp) {
        experience += exp;
        if (experience >= 100) {
            level++;
            experience -= 100;
            std::cout << getName() << " leveled up to level " << level << "!" << std::endl;
        }
    }

    void displayInfo() const override {
        std::cout << "Name: " << getName() << ", HP: " << getHealth()
            << ", Attack: " << getAttack() << ", Defense: " << getDefense()
            << ", Level: " << level << ", Experience: " << experience << std::endl;
    }

    int getLevel() const { return level; }
    int getExperience() const { return experience; }
    void setLevel(int l) { level = l; }
    void setExperience(int exp) { experience = exp; }

    Inventory* getInventory() const { return inventory; }
};

// Класс Inventory
class Inventory {
private:
    std::vector<std::string> items;

public:
    void addItem(const std::string& item) {
        items.push_back(item);
    }

    void removeItem(const std::string& item) {
        items.erase(std::remove(items.begin(), items.end(), item), items.end());
    }

    bool hasItem(const std::string& item) const {
        return std::find(items.begin(), items.end(), item) != items.end();
    }

    void display() const {
        std::cout << "Inventory:" << std::endl;
        for (const auto& item : items) {
            std::cout << "- " << item << std::endl;
        }
    }

    const std::vector<std::string>& getItems() const {
        return items;
    }

    void save(std::ofstream& out) const {
        out << items.size() << std::endl;
        for (const auto& item : items) {
            out << item << std::endl;
        }
    }

    void load(std::ifstream& in) {
        size_t count;
        in >> count;
        items.clear();
        for (size_t i = 0; i < count; ++i) {
            std::string item;
            in >> item;
            items.push_back(item);
        }
    }
};

// Класс Monster
class Monster : public Creature {
public:
    Monster(const std::string& n, int h, int a, int d)
        : Creature(n, h, a, d) {}

    virtual ~Monster() {}
};

// Конкретные монстры
class Goblin : public Monster {
public:
    Goblin() : Monster("Goblin", 30, 10, 5) {}

    void attackTarget(Creature& target) override {
        int damage = getAttack() - target.getDefense();
        if (damage > 0) {
            target.takeDamage(damage);
            std::cout << getName() << " stabs with dagger for " << damage << " damage!" << std::endl;
        }
        else {
            std::cout << getName() << " tries to stab, but fails!" << std::endl;
        }
    }

    void displayInfo() const override {
        std::cout << "Goblin: HP " << getHealth() << ", Attack " << getAttack() << ", Defense " << getDefense() << std::endl;
    }
};

class Dragon : public Monster {
public:
    Dragon() : Monster("Dragon", 100, 20, 10) {}

    void attackTarget(Creature& target) override {
        int damage = getAttack() + 5 - target.getDefense(); // Особый навык дракона
        if (damage > 0) {
            target.takeDamage(damage);
            std::cout << getName() << " breathes fire for " << damage << " damage!" << std::endl;
        }
        else {
            std::cout << getName() << " misses with fire breath!" << std::endl;
        }
    }

    void displayInfo() const override {
        std::cout << "Dragon: HP " << getHealth() << ", Attack " << getAttack() << ", Defense " << getDefense() << std::endl;
    }
};

class Skeleton : public Monster {
public:
    Skeleton() : Monster("Skeleton", 40, 15, 8) {}

    void attackTarget(Creature& target) override {
        int damage = getAttack() + 2 - target.getDefense(); // Особенность скелета
        if (damage > 0) {
            target.takeDamage(damage);
            std::cout << getName() << " swings bone sword for " << damage << " damage!" << std::endl;
        }
        else {
            std::cout << getName() << " misses with bone sword!" << std::endl;
        }
    }

    void displayInfo() const override {
        std::cout << "Skeleton: HP " << getHealth() << ", Attack " << getAttack() << ", Defense " << getDefense() << std::endl;
    }
};

// Шаблонный класс Logger
template <typename T>
class Logger {
private:
    std::ofstream file;

public:
    Logger(const std::string& filename) {
        file.open(filename, std::ios::app);
        if (!file) {
            std::cerr << "Failed to open log file!" << std::endl;
        }
    }

    ~Logger() {
        if (file.is_open()) {
            file.close();
        }
    }

    void log(const T& message) {
        if (file.is_open()) {
            file << message << std::endl;
        }
        std::cout << message << std::endl;
    }
};

// Класс Game
class Game {
private:
    Character* player;
    std::vector<Monster*> monsters;
    Logger<std::string> logger;

public:
    Game(const std::string& playerName)
        : logger("game.log") {
        player = new Character(playerName, 100, 10, 5);
        monsters.push_back(new Goblin());
        monsters.push_back(new Dragon());
        monsters.push_back(new Skeleton());
    }

    ~Game() {
        delete player;
        for (auto m : monsters) {
            delete m;
        }
    }

    void start() {
        std::cout << "Welcome to the RPG game!" << std::endl;
        while (true) {
            std::cout << "\nChoose an action:\n1. Fight\n2. Check Status\n3. Save\n4. Load\n5. Exit" << std::endl;
            int choice;
            std::cin >> choice;
            switch (choice) {
            case 1: fight(); break;
            case 2: checkStatus(); break;
            case 3: saveGame(); break;
            case 4: loadGame(); break;
            case 5: return;
            default: std::cout << "Invalid choice!" << std::endl;
            }
        }
    }

private:
    void checkStatus() {
        player->displayInfo();
        player->getInventory()->display();
    }

    void fight() {
        if (monsters.empty()) {
            std::cout << "All monsters are defeated!" << std::endl;
            return;
        }

        Monster* monster = monsters[rand() % monsters.size()];
        std::cout << "A wild " << monster->getName() << " appears!" << std::endl;

        while (player->getHealth() > 0 && monster->getHealth() > 0) {
            std::cout << "\nYour turn: 1. Attack 2. Use Health Potion" << std::endl;
            int choice;
            std::cin >> choice;

            try {
                if (choice == 1) {
                    player->attackTarget(*monster);
                    if (monster->getHealth() > 0) {
                        monster->attackTarget(*player);
                    }
                }
                else if (choice == 2) {
                    if (player->getInventory()->hasItem("Health Potion")) {
                        player->heal(20);
                        player->getInventory()->removeItem("Health Potion");
                        std::cout << "Used Health Potion!" << std::endl;
                        if (monster->getHealth() > 0) {
                            monster->attackTarget(*player);
                        }
                    }
                    else {
                        std::cout << "No Health Potions available!" << std::endl;
                        continue;
                    }
                }
                else {
                    std::cout << "Invalid choice!" << std::endl;
                    continue;
                }
            }
            catch (const DeadCreatureException& e) {
                if (player->getHealth() == 0) {
                    std::cout << "You were defeated by " << monster->getName() << "!" << std::endl;
                    logger.log("Player was defeated by " + monster->getName());
                    exit(0);
                }
                else {
                    std::cout << "You defeated the " << monster->getName() << "!" << std::endl;
                    logger.log("Player defeated " + monster->getName());
                    player->gainExperience(50);
                    player->getInventory()->addItem("Health Potion");
                    // Удаляем монстра из списка
                    for (auto it = monsters.begin(); it != monsters.end(); ++it) {
                        if (*it == monster) {
                            delete* it;
                            monsters.erase(it);
                            break;
                        }
                    }
                    break;
                }
            }
        }
    }

    void saveGame() {
        std::ofstream out("save.txt");
        if (!out) {
            std::cerr << "Error opening save file!" << std::endl;
            return;
        }

        out << player->getName() << std::endl;
        out << player->getHealth() << " " << player->getAttack() << " " << player->getDefense() << std::endl;
        out << player->getLevel() << " " << player->getExperience() << std::endl;

        player->getInventory()->save(out);

        std::cout << "Game saved!" << std::endl;
        logger.log("Game saved");
    }

    void loadGame() {
        std::ifstream in("save.txt");
        if (!in) {
            std::cerr << "No save file found!" << std::endl;
            return;
        }

        std::string name;
        int h, a, d, l, exp;
        in >> name >> h >> a >> d >> l >> exp;

        delete player;
        player = new Character(name, h, a, d);
        player->setLevel(l);
        player->setExperience(exp);

        player->getInventory()->load(in);

        std::cout << "Game loaded!" << std::endl;
        logger.log("Game loaded");
    }
};

int main() {
    std::srand(static_cast<unsigned int>(std::time(0)));
    std::string playerName;
    std::cout << "Enter your character's name: ";
    std::cin >> playerName;

    Game game(playerName);
    game.start();

    return 0;
}
