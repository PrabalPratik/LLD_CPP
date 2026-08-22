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

enum NotificationChannel{
    EMAIL,
    SMS,
    PUSH
};

class User {
private:
    int User_ID;
    string name;
    string email;
    string phone;
    unordered_map<NotificationChannel,bool> channelAllowed;
public:
    User(int id,string n, string e, string p){
        User_ID = id;
        name = n;
        email = e;
        phone = p;
        channelAllowed = {{EMAIL, true}, {SMS, true}, {PUSH, true}};
    }
    int getUserID() const {
        return User_ID;
    }
    string getUserName() const {
        return name;
    }
    string getUserEmail() const {
        return email;
    }
    string getUserPhone() const {
        return phone;
    }
    void setChannel(NotificationChannel c, bool b){
        channelAllowed[c] = b;
    }
    bool isChannelAllowed(NotificationChannel c) const {
        auto it = channelAllowed.find(c);
        return it != channelAllowed.end() && it->second;
    }
};

enum NotificationPriority{
    HIGH,
    MEDIUM,
    LOW
};

class Notification{
private:
    string app;
    string content;
    set<NotificationChannel> TargetChannels;
    NotificationPriority pr;
public:
    Notification(string a,string c, NotificationPriority p, set<NotificationChannel> ch){
        app = a;
        content = c;
        pr = p;
        TargetChannels = ch;
    }
    string getApp() const {
        return app;
    }
    string getContent() const {
        return content;
    }
    NotificationPriority getPriority() const {
        return pr;
    }
    set<NotificationChannel> getChannels() const {
        return TargetChannels;
    }
    void setPriority(NotificationPriority p){
        pr = p;
    }
    void addChannel(NotificationChannel ch){
        if(TargetChannels.count(ch)){
            cout<<"Channel Already Exists"<<endl;
            return;
        }
        TargetChannels.insert(ch);
    }
    void removChannel(NotificationChannel ch){
        if(!TargetChannels.count(ch)){
            cout<<"Channel doesn't Exists"<<endl;
            return;
        }
        TargetChannels.erase(ch);
    }
};

class NotificationSystem {
private:
    using P = pair<int,Notification*>;

    unordered_map<string,vector<User*>> appUsers;
    unordered_map<int,vector<string>> history;
    priority_queue<P> pq;

    mutex mtx;

public:

    void registerUser(User* user,string app){
        lock_guard<mutex> lock(mtx);

        appUsers[app].push_back(user);
    }

    void publishNotification(Notification* notification){
        lock_guard<mutex> lock(mtx);

        if(!appUsers.count(notification->getApp())){
            cout<<"No Users Registered"<<endl;
            return;
        }

        int priority = 0;

        if(notification->getPriority() == HIGH){
            priority = 2;
        }
        else if(notification->getPriority() == MEDIUM){
            priority = 1;
        }

        pq.push({priority,notification});

        processNotifications();
    }

    void processNotifications(){

        while(!pq.empty()){

            auto [priority,notification] = pq.top();

            pq.pop();

            string app = notification->getApp();

            for(auto user : appUsers[app]){

                for(auto channel : notification->getChannels()){

                    if(!user->isChannelAllowed(channel)){
                        continue;
                    }

                    send(user,notification,channel);
                }
            }
        }
    }

    void send(User* user, Notification* notification, NotificationChannel channel){

        cout<<"Sending ";

        if(channel == EMAIL){
            cout<<"EMAIL";
        }
        else if(channel == SMS){
            cout<<"SMS";
        }
        else{
            cout<<"PUSH";
        }

        cout<<" to ";

        cout<<user->getUserName();

        cout<<" : ";

        cout<<notification->getContent();

        cout<<endl;

        history[user->getUserID()].push_back(notification->getContent());
    }

    void showHistory(User* user){

        cout<<"Notification History"<<endl;

        for(auto msg : history[user->getUserID()]){

            cout<<msg<<endl;
        }
    }
};

int main() {

    NotificationSystem system;

    User* u1 = new User(1, "Prabal", "prabal@gmail.com", "9999999999");
    User* u2 = new User(2, "Rahul", "rahul@gmail.com", "8888888888");
    User* u3 = new User(3, "Aman", "aman@gmail.com", "7777777777");

    // Disable SMS for Rahul
    u2->setChannel(SMS, false);

    // Register users for applications
    system.registerUser(u1, "WhatsApp");
    system.registerUser(u2, "WhatsApp");
    system.registerUser(u3, "Instagram");

    // Notification 1
    set<NotificationChannel> channels1 = {EMAIL, SMS, PUSH};

    Notification* n1 = new Notification(
        "WhatsApp",
        "You have received a new message.",
        HIGH,
        channels1
    );

    // Notification 2
    set<NotificationChannel> channels2 = {PUSH};

    Notification* n2 = new Notification(
        "Instagram",
        "Someone liked your post.",
        MEDIUM,
        channels2
    );

    cout << "\nPublishing Notification 1\n";
    system.publishNotification(n1);

    cout << "\nPublishing Notification 2\n";
    system.publishNotification(n2);

    cout << "\nPrabal History\n";
    system.showHistory(u1);

    cout << "\nRahul History\n";
    system.showHistory(u2);

    cout << "\nAman History\n";
    system.showHistory(u3);

    return 0;
}