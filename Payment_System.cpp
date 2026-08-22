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
#include<set>
#include<list>

using namespace std;


class User{
private:
    int id;
    string name;
public:
    User(int i, string n) {
        id = i;
        name = n;
    }
    //getters
    int getID() const{
        return id;
    }
    string getName() const{
        return name;
    }
};

class Merchant{
private:
    int id;
    string name;
public:
    Merchant(int i, string n) {
        id = i;
        name = n;
    }
    //getters
    int getID() const{
        return id;
    }
    string getName() const{
        return name;
    }
};

class PaymentMethod {
public:
    virtual bool pay(int amount) = 0;
    virtual ~PaymentMethod() = default;
};

class UPIPayment : public PaymentMethod {
public:
    bool pay(int amount) override {
        cout << "Processing UPI payment\n";
        return true;
    }
};

class CardPayment : public PaymentMethod {
public:
    bool pay(int amount) override {
        cout << "Processing Card payment\n";
        return true;
    }
};

class NetBankingPayment : public PaymentMethod {
public:
    bool pay(int amount) override {
        cout << "Processing Net Banking payment\n";
        return true;
    }
};

enum PaymentStatus{
    PENDING,
    PAID, 
    FAIL
};

class Transaction{
private:
    User* user;
    Merchant* merch;
    int amount;
    int transactionID;
    PaymentMethod *method;
    PaymentStatus status;
public:
    Transaction(User* u, Merchant* m,int a,PaymentMethod *pm, int tid){
        user = u;
        merch = m;
        amount = a;
        method = pm;
        status = PENDING;
        transactionID = tid;
    }
    int getTransactionID() const {
        return transactionID;
    }
    PaymentStatus getStatus() const{
        return status;
    }
    bool markSuccessful(){
        status = PAID;
        return true;
    }
    bool markFail(){
        status = FAIL;
        return false;
    }
};

class PaymentSystem{
private:
    unordered_map<int,vector<Transaction*>> userTransaction; //User Transection history
    unordered_map<int,vector<Transaction*>>mercahntTransaction; //Merchant Transection history
    int TransactionID;
    mutex mtx;
public:
    PaymentSystem(){
        TransactionID = 0;
    };
    void Payment(User* u, Merchant* m, PaymentMethod *meth, int amount){
        lock_guard<mutex> lock(mtx);
        //Create new Transection
        Transaction *t = new Transaction(u,m,amount,meth,TransactionID++);
        //Mark Sucess
        bool success = meth->pay(amount);
        if(success)
            t->markSuccessful();
        else
            t->markFail();
        //if success store in history
        userTransaction[u->getID()].push_back(t);
        mercahntTransaction[m->getID()].push_back(t);
    }   
    void DisplayUserHistory(User* u){
        if(!userTransaction.count(u->getID())){
            cout<<"No Transactions Found"<<endl;
            return;
        }
        cout<<"\nTransaction History of "<<u->getName()<<endl;
        for(auto t : userTransaction[u->getID()]){
            cout<<"Transaction ID : "<<t->getTransactionID()<<" ";
            if(t->getStatus() == PAID){
                cout<<"SUCCESS";
            }
            else if(t->getStatus() == FAIL){
                cout<<"FAILED";
            }
            else{
                cout<<"PENDING";
            }
            cout<<endl;
        }
    }
    void DisplayMerchHistory(Merchant* m){
        if(!mercahntTransaction.count(m->getID())){
            cout<<"No Transactions Found"<<endl;
            return;
        }
        cout<<"\nTransaction History of "<<m->getName()<<endl;
        for(auto t : mercahntTransaction[m->getID()]){
            cout<<"Transaction ID : "<<t->getTransactionID()<<" ";
            if(t->getStatus() == PAID){
                cout<<"SUCCESS";
            }
            else if(t->getStatus() == FAIL){
                cout<<"FAILED";
            }
            else{
                cout<<"PENDING";
            }
            cout<<endl;
        }
    }
};

int main(){

    PaymentSystem system;

    User* u1 = new User(1,"Prabal");
    User* u2 = new User(2,"Rahul");

    Merchant* m1 = new Merchant(101,"Amazon");
    Merchant* m2 = new Merchant(102,"Netflix");

    PaymentMethod* upi = new UPIPayment();
    PaymentMethod* card = new CardPayment();
    PaymentMethod* net = new NetBankingPayment();

    cout<<"Making Payments\n\n";

    system.Payment(u1,m1,upi,500);

    system.Payment(u1,m2,card,999);

    system.Payment(u2,m1,net,2500);

    system.Payment(u2,m2,upi,1200);

    system.DisplayUserHistory(u1);

    system.DisplayUserHistory(u2);

    system.DisplayMerchHistory(m1);

    system.DisplayMerchHistory(m2);

    delete upi;
    delete card;
    delete net;

    delete u1;
    delete u2;

    delete m1;
    delete m2;

    return 0;
}