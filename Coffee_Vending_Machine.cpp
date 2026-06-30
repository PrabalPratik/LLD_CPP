#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <format>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <mutex>

using namespace std;


/*
Design a Coffee Vending Machine

Requirements

The coffee vending machine should support different types of coffee, such as espresso, cappuccino, and latte.
Each type of coffee should have a specific price and recipe (ingredients and their quantities).
The machine should have a menu to display the available coffee options and their prices.
Users should be able to select a coffee type and make a payment.
The machine should dispense the selected coffee and provide change if necessary.
The machine should track the inventory of ingredients and notify when they are running low.
The machine should handle multiple user requests concurrently and ensure thread safety.
*/

/*

Step 1: Requirement Analysis

Functional Requirements
- Support multiple coffee types.
- Each coffee has a price and recipe.
- Display coffee menu.
- Allow the user to select a coffee.
- Accept payment.
- Dispense coffee.
- Return change if necessary.
- Track ingredient inventory.
- Notify when ingredients are running low.

Non-Functional Requirements
- Thread-safe.
- Extensible.
- Maintainable.

Workflow : 

User
   │
   ▼
Display Menu
   │
   ▼
Select Coffee
   │
   ▼
Check Ingredient Availability
   │
   ├── Not Available
   │       │
   │       ▼
   │   Notify User
   │
   └── Available
           │
           ▼
Receive Payment
           │
           ├── Payment Failed
           │       │
           │       ▼
           │   Cancel Request
           │
           └── Payment Successful
                   │
                   ▼
Deduct Ingredients
                   │
                   ▼
Prepare & Dispense Coffee
                   │
                   ▼
Return Change
                   │
                   ▼
Check Low Inventory
                   │
                   ▼
Notify if Running Low

*/

class Coffee {
private:
    string name;
    int price;
    unordered_map<string, int> recipe;

public:
    Coffee(string n, int p, unordered_map<string, int> r) {
        name = n;
        price = p;
        recipe = r;
    }

    string getName() const {
        return name;
    }

    int getPrice() const {
        return price;
    }

    unordered_map<string, int> getRecipe() const {
        return recipe;
    }
};

enum PaymentStatus {
    SUCCESS,
    FAILURE
};

class PaymentSystem {
public:

    PaymentStatus makePayment(int amount, int paidAmount) {
        if (paidAmount < amount) {
            cout << "Insufficient funds received." << endl;
            return FAILURE;
        }
        cout << paidAmount << " received." << endl;
        int change = paidAmount - amount;
        if (change > 0) {
            cout << "Returning Change : " << change << endl;
            returnChange(change);
        }
        return SUCCESS;
    }

    void returnChange(int change) {
        cout << "Returned Change : " << change << endl;
    }
};

enum MachineStatus {
    IDLE,
    PREPARING,
    PREPARED
};

class CoffeeVendingMachine {
private:
    unordered_map<string, int> ingredients;
    vector<Coffee*> coffees;
    PaymentSystem payment;
    MachineStatus status;
    mutex mtx;

public:
    CoffeeVendingMachine() {
        status = IDLE;
    }

    void addCoffee(Coffee* coffee) {
        coffees.push_back(coffee);
    }

    void displayMenu() {
        cout << "MENU :" << endl;
        for (auto coffee : coffees) {
            cout << coffee->getName() << " : " << coffee->getPrice() << endl;
        }
    }

    bool checkIngredients(string coffeeName) {

        for (auto coffee : coffees) {
            if (coffee->getName() == coffeeName) {
                unordered_map<string, int> recipe = coffee->getRecipe();
                for (auto ingredient : recipe) {
                    string ingredientName = ingredient.first;
                    int requiredQuantity = ingredient.second;
                    if (ingredients[ingredientName] < requiredQuantity) {
                        return false;
                    }
                }
                return true;
            }
        }
        return false;
    }

    MachineStatus makeCoffee(string coffeeName, PaymentStatus paymentStatus) {
        if (paymentStatus == FAILURE) {
            cout << "Payment required first." << endl;
            return IDLE;
        }

        for (auto coffee : coffees) {
            if (coffee->getName() == coffeeName) {
                lock_guard<mutex> lock(mtx);
                status = PREPARING;
                unordered_map<string, int> recipe = coffee->getRecipe();
                for (auto ingredient : recipe) {

                    string ingredientName = ingredient.first;
                    int quantity = ingredient.second;
                    ingredients[ingredientName] -= quantity;
                    if (ingredients[ingredientName] < 10) { 
                        cout << ingredientName << " is running low." << endl;
                    }
                }
                status = PREPARED;
                cout << coffeeName << " is prepared." << endl;
                return status;
            }
        }
        return IDLE;
    }
    PaymentStatus makePayment(int amount, int paidAmount) {
        return payment.makePayment(amount, paidAmount);
    }

    void dispenseCoffee(string coffeeName) {
        if (status != PREPARED) {
            cout << "Coffee is not ready." << endl;
            return;
        }
        cout << coffeeName << " dispensed successfully." << endl;
        status = IDLE;
    }

    void refillIngredient(string ingredient, int quantity) {
        lock_guard<mutex> lock(mtx);
        ingredients[ingredient] += quantity;
    }
};

int main() {

    // Coffee Recipes
    unordered_map<string, int> espressoRecipe = {
        {"Coffee", 10},
        {"Water", 50}
    };

    unordered_map<string, int> latteRecipe = {
        {"Coffee", 10},
        {"Milk", 30},
        {"Water", 40}
    };

    // Coffee Objects
    Coffee* espresso = new Coffee("Espresso", 100, espressoRecipe);
    Coffee* latte = new Coffee("Latte", 150, latteRecipe);

    CoffeeVendingMachine machine;

    // Add coffee types
    machine.addCoffee(espresso);
    machine.addCoffee(latte);

    // Fill inventory
    machine.refillIngredient("Coffee", 100);
    machine.refillIngredient("Water", 500);
    machine.refillIngredient("Milk", 200);

    machine.displayMenu();

    string coffeeName = "Latte";

    if (!machine.checkIngredients(coffeeName)) {
        cout << "Ingredients not available." << endl;
        return 0;
    }

    PaymentStatus status = machine.makePayment(150, 200);

    if (machine.makeCoffee(coffeeName, status) == PREPARED) {
        machine.dispenseCoffee(coffeeName);
    }

    return 0;
}