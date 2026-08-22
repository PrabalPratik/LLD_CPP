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


class Item{
private:
    int id;
    string name;
    int price;
public:
    Item(){

    }
    //getters
    int getId();
    string getName();
    int getPrice();
};

class User{
private:
    int id;
    string name;
public:
    User(){

    }
    //getters
    int getId();
    string getName();

};

class Cart{
private:
    unordered_map<Item*,int> Cart_Item_counter;
public:
    void addItem(Item* it,int q); 
    void removeItem(Item* it,int q); 
    int calculateTotal();
    void showCart();
};

enum PaymentStatus{
    PENDING,
    PAID
};

enum PaymentMethods{
    CARD,
    UPI,
    NET_BANKING
};

class Payment{
private:
    PaymentStatus status;
public:
    Payment(){
        status = PENDING;
    }
    bool pay(PaymentMethods m, int amount) { // Change Payment status
        cout<<"Payment of "<<amount<<"Completed"<<endl;
        status = PAID;
        return true;
    }

};


class CheckoutSystem{
private:
    unordered_map<int,Cart*> uCart; // Can be multiple users
    unordered_map<int,vector<Cart*>> orderCart; //UserID -> Cart ordered
    int orderID;
    
public:
    CheckoutSystem(){
        orderID = 0;
    }
    void addCart(User* u, Cart* c); // Add user with card
    void placeOrder(User* u, int coupon , PaymentMethods m){
        // get cart
        Cart* cart = uCart[u->getId()];
        // Calculate amount agian with discount
        int total = cart->calculateTotal();
        int pAmt = total;
        if(coupon < 100) pAmt = total - (total * coupon) / 100;
        // Payment
        Payment p;
        bool success = p.pay(m,pAmt);
        if(!success){
            cout<<"Payment Failed"<<endl;
            return;
        }
        // Print orderID with sucess messsage
        cout<<"Order Placed Successfully"<<endl;
        cout<<"Order ID : "<<orderID<<endl;
        orderID++;
        //Discard cart
        if(success) uCart.erase(u->getId());
        // Add cart to orders
        if(success) orderCart[u->getId()].push_back(cart);
    } 
};

int main(){

}