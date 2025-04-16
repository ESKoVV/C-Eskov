
#include <iostream>
#include <string>
#include <ctime>
#include <cstdlib>

class Entity {
protected:
    std::string name;
    int health;
    int attack;
    int defense;

public:
    Entity(const std::string& n, int h, int a, int d)
        : name(n), health(h), attack(a), defense(d) {}

    virtual void attack(Entity& target) {
        int damage = attack - target.defense;
        if (damage > 0) {
            target.health -= damage;
            std::cout << name << " attacks " << target.name << " for " << damage << " damage!\n";
        } else {
            std::cout << name << " attacks " << target.name << ", but it has no effect!\n";
        }
    }

    virtual void displayInfo() const {
        std::cout << "Name: " << name << ", HP: " << health 
                  << ", Attack: " << attack << ", Defense: " << defense << std::endl;
    }

    virtual void heal(int amount) {
        health += amount;
        if (health > 100) health = 100;
        std::cout << name << " healed by " << amount << ". HP: " << health << std::endl;
    }

    virtual ~Entity() {}
};

class Character : public Entity {
public:
    Character(const std::string& n, int h, int a, int d)
        : Entity(n, h, a, d) {}

    void attack(Entity& target) override {
        int damage = attack - target.defense;
        if (damage > 0) {
            if (rand() % 100 < 20) {
                damage *= 2;
                std::cout << "Critical hit! ";
            }
            target.health -= damage;
            std::cout << name << " attacks " << target.name << " for " << damage << " damage!\n";
        } else {
            std::cout << name << " attacks " << target.name << ", but it has no effect!\n";
        }
    }

    void heal(int amount) override {
        Entity::heal(amount); // Используем базовую реализацию
        std::cout << "Character's health restored!" << std::endl;
    }
};

class Boss : public Entity {
public:
    Boss(const std::string& n, int h, int a, int d)
        : Entity(n, h, a, d) {}

    void attack(Entity& target) override {
        int damage = attack - target.defense + 10; // Огненный удар
        target.health -= damage;
        std::cout << name << " uses Fire Strike! " << target.name << " takes " << damage << " damage!\n";
    }
};

int main() {
    srand(static_cast<unsigned>(time(0)));
    Character hero("Hero", 100, 20, 10);
    Boss dragon("Dragon", 150, 25, 20);

    hero.displayInfo();
    dragon.displayInfo();

    hero.attack(dragon);
    dragon.attack(hero);

    hero.heal(30); // Персонаж лечится

    return 0;
}
