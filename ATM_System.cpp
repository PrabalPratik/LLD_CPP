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

Design an ATM System

Requirements

The ATM system should support basic operations such as balance inquiry, cash withdrawal, and cash deposit.
Users should be able to authenticate themselves using a card and a PIN (Personal Identification Number).
The system should interact with a bank's backend system to validate user accounts and perform transactions.
The ATM should have a cash dispenser to dispense cash to users.
The system should handle concurrent access and ensure data consistency.
The ATM should have a user-friendly interface for users to interact with.

*/

/*
Functional Requirements : 

Authenticate user using card and PIN.
Balance inquiry.
Cash withdrawal.
Cash deposit.
Perform transactions only after successful authentication.
Validate accounts and process transactions through the bank backend.


Non-Functional Requirements : 

Handle concurrent access.
Maintain data consistency.
User-friendly interface.
Extensible.

*/

/*
Workflow : 

User --> Authentication(Card and PIN) --> Balance Enquiry(Optional) --> Cash Withdraw(Optional) --> Cash Deposit(Optional) --> Update Balance

*/

class User {
private:
    int cardID;
    int pin;
    string name;

public:
    User(int cid, int p, string n) {
        cardID = cid;
        pin = p;
        name = n;
    }
    int getCardID() const {
        return cardID;
    }
    int getPIN() const {
        return pin;
    }
    string getName() const {
        return name;
    }
};

enum ACCESS {
    ALLOWED,
    DENIED
};

class UserDatabase {
private:
    unordered_map<int, int> id_pass;
public:
    void addUser(const User& user) {
        id_pass[user.getCardID()] = user.getPIN();
    }
    ACCESS authenticate(int cardID, int pin) {
        if (id_pass.find(cardID) == id_pass.end()) {
            cout << "Invalid Card" << endl;
            return DENIED;
        }
        if (id_pass[cardID] != pin) {
            cout << "Wrong PIN" << endl;
            return DENIED;
        }
        return ALLOWED;
    }
};

class ATM {
private:
    UserDatabase userDB;
    unordered_map<int, int> account;   // CardID -> Balance
    ACCESS access;
    int currentUserID;
    mutex mtx;

public:
    ATM() {
        access = DENIED;
        currentUserID = -1;
    }

    // Add user
    void addUser(User user, int balance) {
        userDB.addUser(user);
        account[user.getCardID()] = balance;
    }

    // Authenticate User
    void authenticate(int cardID, int pin) {
        lock_guard<mutex> lock(mtx);
        access = userDB.authenticate(cardID, pin);
        if (access == ALLOWED) {
            currentUserID = cardID;
            cout << "Login Successful" << endl;
        }else{
            currentUserID = -1;
        }
    }

    // Display Balance
    void displayBalance() {
        lock_guard<mutex> lock(mtx);
        if (access == DENIED) {
            cout << "Authenticate first." << endl;
            return;
        }
        cout << "Balance : " << account[currentUserID] << endl;
    }

    // Deposit Cash
    void deposit(int amount) {
        lock_guard<mutex> lock(mtx);
        if (access == DENIED) {
            cout << "Authenticate first." << endl;
            return;
        }
        account[currentUserID] += amount;
        cout << "Deposited Successfully." << endl;
        cout << "New Balance : " << account[currentUserID] << endl;
    }

    // Withdraw Cash
    void withdraw(int amount) {
        lock_guard<mutex> lock(mtx);
        if (access == DENIED) {
            cout << "Authenticate first." << endl;
            return;
        }
        if (account[currentUserID] < amount) {
            cout << "Insufficient Balance." << endl;
            return;
        }
        account[currentUserID] -= amount;
        cout << "Please collect your cash." << endl;
        cout << "Remaining Balance : " << account[currentUserID] << endl;
    }

    // End Session
    void ejectCard() {
        lock_guard<mutex> lock(mtx);
        access = DENIED;
        currentUserID = -1;
        cout << "Card Ejected." << endl;
    }
};

int main() {

    ATM atm;

    User u1(101, 1234, "Prabal");
    User u2(102, 5678, "Rahul");

    atm.addUser(u1, 5000);
    atm.addUser(u2, 3000);

    atm.authenticate(101, 1234);

    atm.displayBalance();

    atm.deposit(2000);

    atm.withdraw(1500);

    atm.displayBalance();

    atm.ejectCard();

    return 0;
}