#include <iostream>
#include <string>

class Entity {
protected:
    std::string name;
    int health;

public:
    Entity(const std::string& n, int h) : name(n), health(h) {}
    virtual void displayInfo() const {
        std::cout << "Name: " << name << ", HP: " << health << std::endl;
    }
    virtual ~Entity() {}
};

class Enemy : public Entity {
private:
    std::string type;

public:
    Enemy(const std::string& n, int h, const std::string& t)
        : Entity(n, h), type(t) {}

    void displayInfo() const override {
        Entity::displayInfo();
        std::cout << "Type: " << type << std::endl;
    }
};

class Boss : public Enemy {
private:
    std::string specialAbility;

public:
    Boss(const std::string& n, int h, const std::string& t, const std::string& ability)
        : Enemy(n, h, t), specialAbility(ability) {}

    void displayInfo() const override {
        Enemy::displayInfo();
        std::cout << "Special Ability: " << specialAbility << std::endl;
    }
};

int main() {
    Boss dragon("Dragon", 200, "Dragon", "Fire Breath");
    dragon.displayInfo();
    return 0;
}
