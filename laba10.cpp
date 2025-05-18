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
    static std::ofstream logFile;
    static std::mutex mtx;

public:
    static void log(const T& message);
};

// 
template<typename T>
std::ofstream Logger<T>::logFile("game.log", std::ios::app);

template<typename T>
std::mutex Logger<T>::mtx;

template<>
void Logger<std::string>::log(const std::string& message) {
    std::lock_guard<std::mutex> lock(mtx);
    if (logFile.is_open()) {
        logFile << message << std::endl;
    }
}

// ==================== Инвентарь ====================
class Inventory {
private:
    std::vector<std::string> items;

public:
    void addItem(const std::string& item) {
        items.push_back(item);
        std::cout << "Added " << item << " to inventory." << std::endl;
    }

    void removeItem(const std::string& item) {
        for (auto it = items.begin(); it != items.end(); ++it) {
            if (*it == item) {
                items.erase(it);
                std::cout << "Removed " << item << " from inventory." << std::endl;
                return;
            }
        }
        std::cout << "Item not found in inventory." << std::endl;
    }

    void displayItems() const {
        std::cout << "Inventory:" << std::endl;
        for (const auto& item : items) {
            std::cout << "- " << item << std::endl;
        }
    }
};

// ==================== Character ====================
class Monster; 

class Character {
private:
    std::string name;
    int health;
    int attack;
    int defense;
    int level;
    int experience;
    Inventory inventory;

public:
    Character(const std::string& n, int h, int a, int d)
        : name(n), health(h), attack(a), defense(d), level(1), experience(0) {
    }

    void attackEnemy(Monster& enemy);
    void heal(int amount);
    void gainExperience(int exp);
    void displayInfo() const;
    void addItemToInventory(const std::string& item);
    void showInventory() const;
    std::string getName() const { return name; }
    int getHealth() const { return health; }

    
    int getAttack() const { return attack; }
    int getDefense() const { return defense; }
};

// ==================== Монстрик ====================
class Monster {
protected:
    std::string name;
    int health;
    int attack;
    int defense;

public:
    virtual ~Monster() = default;
    virtual void takeDamage(int damage);
    virtual void attackPlayer(Character& player);
    virtual void displayInfo() const;
    virtual std::string getName() const { return name; }
    virtual int getHealth() const { return health; }
    virtual int getAttack() const { return attack; }
    virtual int getDefense() const { return defense; }
};

class Goblin : public Monster {
public:
    Goblin() {
        name = "Goblin";
        health = 30;
        attack = 8;
        defense = 2;
    }
};

class Dragon : public Monster {
public:
    Dragon() {
        name = "Dragon";
        health = 150;
        attack = 25;
        defense = 10;
    }
};

class Skeleton : public Monster {
public:
    Skeleton() {
        name = "Skeleton";
        health = 40;
        attack = 10;
        defense = 5;
    }
};


void Character::attackEnemy(Monster& enemy) {
    int damage = getAttack() - enemy.getDefense();
    if (damage > 0) {
        enemy.takeDamage(damage);
        std::cout << name << " attacks " << enemy.getName() << " for " << damage << " damage!" << std::endl;
        Logger<std::string>::log(name + " attacks " + enemy.getName() + " for " + std::to_string(damage) + " damage!");
    }
    else {
        std::cout << name << " attacks " << enemy.getName() << ", but it has no effect!" << std::endl;
        Logger<std::string>::log(name + " attacks " + enemy.getName() + ", but it has no effect!");
    }
}

void Character::heal(int amount) {
    health += amount;
    if (health > 100) health = 100;
    std::cout << name << " heals for " << amount << " HP!" << std::endl;
    Logger<std::string>::log(name + " heals for " + std::to_string(amount) + " HP!");
}

void Character::gainExperience(int exp) {
    experience += exp;
    while (experience >= 100 * level) {
        level++;
        experience -= 100 * level;
        attack += 2;
        defense += 1;
        std::cout << name << " leveled up to level " << level << "!" << std::endl;
        Logger<std::string>::log(name + " leveled up to level " + std::to_string(level) + "!");
    }
}

void Character::displayInfo() const {
    std::cout << "Name: " << name << ", HP: " << health
        << ", Attack: " << attack << ", Defense: " << defense
        << ", Level: " << level << ", Experience: " << experience << std::endl;
}

void Character::addItemToInventory(const std::string& item) {
    inventory.addItem(item);
}

void Character::showInventory() const {
    inventory.displayItems();
}


void Monster::takeDamage(int damage) {
    health -= damage;
    if (health <= 0) {
        health = 0;
        std::cout << name << " has been defeated!" << std::endl;
        Logger<std::string>::log(name + " has been defeated!");
    }
}

void Monster::attackPlayer(Character& player) {
    int damage = attack - player.getDefense(); 
    if (damage > 0) {
        player.attackEnemy(*this);
    }
    else {
        std::cout << name << " attacks " << player.getName() << ", but it has no effect!" << std::endl;
        Logger<std::string>::log(name + " attacks " + player.getName() + ", but it has no effect!");
    }
}

void Monster::displayInfo() const {
    std::cout << "Name: " << name << ", HP: " << health
        << ", Attack: " << attack << ", Defense: " << defense << std::endl;
}

// ==================== Игрушка ====================
class Game {
private:
    Character player;

public:
    Game() : player("Hero", 100, 10, 5) {}

    void start();
    void battle(Monster& monster);
    void saveGame();
    void loadGame();
};

void Game::start() {
    std::cout << "Welcome to the RPG Adventure!" << std::endl;
    player.displayInfo();

    Goblin goblin;
    battle(goblin);

    Skeleton skeleton;
    battle(skeleton);

    Dragon dragon;
    battle(dragon);

    player.addItemToInventory("Sword");
    player.addItemToInventory("Potion of Healing");

    player.showInventory();

    saveGame();
    loadGame();
}

void Game::battle(Monster& monster) {
    std::cout << "\nA wild " << monster.getName() << " appears!\n";

    while (player.getHealth() > 0 && monster.getHealth() > 0) {
        player.attackEnemy(monster);
        if (monster.getHealth() <= 0) break;

        monster.attackPlayer(player);
        player.displayInfo();
    }

    if (player.getHealth() > 0) {
        std::cout << "You defeated the " << monster.getName() << "!\n";
        player.gainExperience(50);
    }
    else {
        std::cout << "You were defeated...\n";
        exit(0);
    }
}

void Game::saveGame() {
    std::ofstream out("savegame.dat");
    if (!out.is_open()) {
        std::cerr << "Failed to open file for saving.\n";
        return;
    }

    out << player.getName() << " "
        << player.getHealth() << " "
        << player.getAttack() << " "
        << player.getDefense() << " "
        << 1 << " "
        << 0 << std::endl;

    std::cout << "Game saved.\n";
}

void Game::loadGame() {
    std::ifstream in("savegame.dat");
    if (!in.is_open()) {
        std::cerr << "Failed to open file for loading.\n";
        return;
    }

    std::string name;
    int hp, att, def, level, exp;
    in >> name >> hp >> att >> def >> level >> exp;

    std::cout << "Game loaded.\n";
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
        std::cerr << "Error: " << ex.what() << std::endl;
    }
    catch (...) {
        std::cerr << "Unknown error occurred!" << std::endl;
    }

    return 0;
}

Перепиши комментарии на русский
