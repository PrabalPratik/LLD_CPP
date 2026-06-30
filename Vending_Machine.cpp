#include <iostream>
#include <string>
#include <vector>
#include <map>

using namespace std;

/*

Designing a Vending Machine

Requirements

The vending machine should support multiple products with different prices and quantities.
The machine should accept coins and notes of different denominations.
The machine should dispense the selected product and return change if necessary.
The machine should keep track of the available products and their quantities.
The machine should handle multiple transactions concurrently and ensure data consistency.
The machine should provide an interface for restocking products and collecting money.
The machine should handle exceptional scenarios, such as insufficient funds or out-of-stock products.

*/

/*

Must Have Features : 

- Container -> Stores different products and displays their quantity along with price of that prod
- Cash Box -> Accepts different denominations and return change after deductions
- Items can be added in the container incrementing the quantity
- Money can be collected from cash box by authorised person -> Password
- Exception Hnadling of all the prominent features

Limits & System Constraints :

- Capacity Limit: Each product row in the Inventory has a strict maximum capacity (e.g., 10 units max).
- Denomination Rules: The machine only accepts specific, fixed denominations (e.g., 10, 20, 50, 100).
- Strict State Flow: A user cannot select a product before inserting money, and the machine cannot dispense unless the exact price is met or exceeded.


Ignoring

- Concurrency is not being included for now due to time constrains


*/


// 1. The State Machine Definitions
enum MachineState {
    IDLE,           // Waiting for user
    HAS_MONEY,      // User inserted cash, waiting for selection
    DISPENSING      // Processing the transaction and dropping the item
};

// 2. The Dynamic Product
class Item {
private:
    string code;    
    string name;    
    int price;
    int quantity;

public:
    Item(string c, string n, int p, int q) {
        code = c;
        name = n;
        price = p;
        quantity = q;
    }
    
    string getCode() const { return code; }
    string getName() const { return name; }
    int getPrice() const { return price; }
    int getQuantity() const { return quantity; }
    
    bool isAvailable() const { return quantity > 0; }
    void reduceQuantity() { if (quantity > 0) quantity--; }
};

// 3. The Inventory Subsystem
class Inventory {
private:
    unordered_map<string, Item> items; 

public:
    Inventory() {}

    void addItem(Item item) {
        items.insert({item.getCode(), item});
    }

    bool isAvailable(string code) {
        if (items.count(code) == 0) return false;
        return items.at(code).isAvailable();
    }

    Item getItem(string code) {
        return items.at(code);
    }

    void dispenseItem(string code) {
        if (items.count(code)) {
            items.at(code).reduceQuantity();
        }
    }
};

// 4. The Payment Subsystem
class CashBox {
private:
    unordered_map<int, int> denominations; 

public:
    void addCash(int denomination, int count) {
        denominations[denomination] += count;
    }

    void receiveCash(int denomination, int count) {
        denominations[denomination] += count;
    }

    unordered_map<int, int> returnChange(int targetAmount) {
        unordered_map<int, int> changeToDispense;
        return changeToDispense; 
    }
};

// 5. The Master Vending Machine
class VendingMachine {
private:
    Inventory inventory;
    CashBox cashBox;
    MachineState currentState;
    int currentBalance;       
    string selectedItemCode;  

public:
    VendingMachine() {
        currentState = IDLE;
        currentBalance = 0;
        selectedItemCode = "";
    }

    // --- Admin Methods for Setup ---
    void adminAddItem(Item item) {
        inventory.addItem(item);
    }

    void adminAddCash(int denomination, int count) {
        cashBox.addCash(denomination, count);
    }

    // --- Customer Actions ---
    void insertMoney(int denomination) {
        if(currentState == DISPENSING){
            cout << "System Error: Machine is currently dispensing. Please wait." << endl;
            return;
        }
        currentBalance += denomination;
        cashBox.receiveCash(denomination, 1);
        currentState = HAS_MONEY;
        cout << "Accepted: " << denomination << ". Current Balance: " << currentBalance << endl;
    }

    void selectProduct(string code) {
        if (currentState != HAS_MONEY) {
            cout << "Declined: Please insert money first." << endl;
            return;
        }
        if (!inventory.isAvailable(code)) {
            cout << "Declined: Item " << code << " is unavailable or out of stock." << endl;
            return;
        }
        
        int price = inventory.getItem(code).getPrice();
        
        if (currentBalance < price) {
            cout << "Declined: Insufficient funds. Price is " << price << ", Balance is " << currentBalance << endl;
            return;
        }
        
        currentState = DISPENSING;
        cout << "Processing transaction for " << inventory.getItem(code).getName() << "..." << endl;
        inventory.dispenseItem(code);
        
        int change = currentBalance - price;
        if (change > 0) {
            cout << "Dispensing change: " << change << endl;
        }
        
        cout << "Success: Item dispensed." << endl;
        currentBalance = 0;
        selectedItemCode = "";
        currentState = IDLE;
    }
};

int main() {
    cout << "=== VENDING MACHINE BOOT SEQUENCE ===" << endl;
    VendingMachine machine;

    // 1. Admin Setup: Load the machine
    cout << "\n[Admin] Loading inventory and cashbox..." << endl;
    machine.adminAddItem(Item("A1", "Coke Zero", 20, 2)); 
    machine.adminAddItem(Item("B2", "Mineral Water", 10, 0)); 
    machine.adminAddItem(Item("C3", "Energy Drink", 50, 5));
    
    machine.adminAddCash(10, 50); 
    machine.adminAddCash(20, 20); 

    // 2. Test Cases 
    cout << "\n--- TEST 1: The Impatient User ---" << endl;
    machine.selectProduct("A1");

    cout << "\n--- TEST 2: The Out-of-Stock Item ---" << endl;
    machine.insertMoney(20);
    machine.selectProduct("B2");

    cout << "\n--- TEST 3: Insufficient Funds ---" << endl;
    machine.selectProduct("C3");

    cout << "\n--- TEST 4: The Flawless Transaction ---" << endl;
    machine.insertMoney(50);
    machine.selectProduct("C3");

    cout << "\n--- TEST 5: State Machine Reset Check ---" << endl;
    machine.selectProduct("A1");

    cout << "\n=== SYSTEM SHUTDOWN ===" << endl;
    return 0;
}