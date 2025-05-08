#include <iostream>
#include <memory>
#include <string>
#include <vector>

class Inventory {
private:
    std::vector<std::unique_ptr<std::string>> items;
    size_t capacity;

public:
    Inventory(size_t initial_capacity = 10) : capacity(initial_capacity) {
        items.reserve(capacity);
    }

    void addItem(const std::string& item) {
        if (items.size() < capacity) {
            items.push_back(std::make_unique<std::string>(item));
            std::cout << "Added item: " << item << std::endl;
        }
        else {
            std::cout << "Inventory is full! Cannot add " << item << std::endl;
        }
    }

    void displayInventory() const {
        std::cout << "\n=== Inventory Contents ===\n";
        if (items.empty()) {
            std::cout << "Inventory is empty!\n";
        }
        else {
            for (size_t i = 0; i < items.size(); ++i) {
                std::cout << i + 1 << ". " << *items[i] << "\n";
            }
        }
        std::cout << "=========================\n";
    }
};

int main() {
    Inventory playerInventory(5);

    playerInventory.addItem("Steel Sword");
    playerInventory.addItem("Health Potion");
    playerInventory.addItem("Magic Ring");
    playerInventory.addItem("Leather Armor");
    playerInventory.addItem("Mana Potion");

    playerInventory.addItem("Golden Key");

    playerInventory.displayInventory();

    {
        Inventory tempInventory(2);
        tempInventory.addItem("Torch");
        tempInventory.addItem("Rope");
        tempInventory.displayInventory();
    } 

    return 0;
}
