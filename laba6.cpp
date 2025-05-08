#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>

template <typename T>
class Queue {
private:
    std::vector<T> elements;

public:
    void push(const T& item) {
        elements.push_back(item);
        std::cout << "Added: " << item << std::endl;
    }

    T pop() {
        if (elements.empty()) {
            throw std::out_of_range("Queue is empty - cannot pop!");
        }
        T first = elements.front();
        elements.erase(elements.begin());
        std::cout << "Removed: " << first << std::endl;
        return first;
    }

    void display() const {
        if (elements.empty()) {
            std::cout << "Queue is empty" << std::endl;
            return;
        }
        std::cout << "Queue contents: ";
        for (const auto& item : elements) {
            std::cout << item << " ";
        }
        std::cout << std::endl;
    }
};

int main() {
    try {
        std::cout << "=== Testing Int Queue ===" << std::endl;
        Queue<int> intQueue;

        intQueue.push(10);
        intQueue.push(20);
        intQueue.display();

        intQueue.pop();
        intQueue.pop();
        intQueue.display();

        intQueue.pop();

    }
    catch (const std::out_of_range& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    try {
        std::cout << "\n=== Testing String Queue ===" << std::endl;
        Queue<std::string> stringQueue;

        stringQueue.push("First");
        stringQueue.push("Second");
        stringQueue.display();

        stringQueue.pop();
        stringQueue.pop();

        stringQueue.pop(); 

    }
    catch (const std::out_of_range& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}
