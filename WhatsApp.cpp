#include <iostream>
#include <vector>
#include <unordered_map>
#include <string>
#include <mutex>

using namespace std;

enum MessageStatus {
    SENT,
    DELIVERED,
    READ
};

class User {
private:
    int userID;
    string name;
    string password;
    bool online;

public:

    User(int id, string n, string pass) {
        userID = id;
        name = n;
        password = pass;
        online = false;
    }

    int getUserID() const {
        return userID;
    }

    string getName() const {
        return name;
    }

    string getPassword() const {
        return password;
    }

    bool isOnline() const {
        return online;
    }

    void setOnline(bool status) {
        online = status;
    }
};

class Message {
private:
    int messageID;
    User* sender;
    string content;
    MessageStatus status;
    string timestamp;

public:

    Message(int id, User* s, string msg, string time) {
        messageID = id;
        sender = s;
        content = msg;
        timestamp = time;
        status = SENT;
    }

    int getMessageID() const {
        return messageID;
    }

    User* getSender() const {
        return sender;
    }

    string getContent() const {
        return content;
    }

    string getTimestamp() const {
        return timestamp;
    }

    MessageStatus getStatus() const {
        return status;
    }

    void setStatus(MessageStatus s) {
        status = s;
    }
};

class Chat {
private:
    int chatID;
    vector<User*> participants;
    vector<Message*> messages;

public:

    Chat(int id, vector<User*> users) {
        chatID = id;
        participants = users;
    }

    int getChatID() const {
        return chatID;
    }

    vector<User*> getParticipants() const {
        return participants;
    }

    vector<Message*> getMessages() const {
        return messages;
    }

    bool isParticipant(User* user) {
        for (auto u : participants) {
            if (u->getUserID() == user->getUserID()) {
                return true;
            }
        }
        return false;
    }

    void addParticipant(User* user) {
        participants.push_back(user);
    }

    void addMessage(Message* msg) {
        messages.push_back(msg);
    }

    void showChatHistory() {
        cout << "\n===== Chat History =====\n";

        for (auto msg : messages) {
            cout << "[" << msg->getTimestamp() << "] ";
            cout << msg->getSender()->getName();
            cout << " : ";
            cout << msg->getContent();

            if (msg->getStatus() == SENT) {
                cout << " (SENT)";
            }
            else if (msg->getStatus() == DELIVERED) {
                cout << " (DELIVERED)";
            }
            else {
                cout << " (READ)";
            }

            cout << endl;
        }

        cout << endl;
    }

    void searchMessage(string keyword) {
        cout << "\nSearch Results\n";

        bool found = false;

        for (auto msg : messages) {
            if (msg->getContent().find(keyword) != string::npos) {
                cout << msg->getSender()->getName();
                cout << " : ";
                cout << msg->getContent();
                cout << endl;
                found = true;
            }
        }

        if (!found) {
            cout << "No Message Found\n";
        }

        cout << endl;
    }
};

class ChatSystem {
private:
    unordered_map<int,User*> users;
    unordered_map<int,Chat*> chats;
    int nextChatID;
    int nextMessageID;
    mutex mtx;

public:
    ChatSystem(){
        nextChatID = 1;
        nextMessageID = 1;
    }

    void registerUser(User* user){
        lock_guard<mutex> lock(mtx);
        users[user->getUserID()] = user;
        cout<<"User Registered Successfully"<<endl;
    }

    void login(int uid,string password){
        lock_guard<mutex> lock(mtx);

        if(users.find(uid)==users.end()){
            cout<<"User Not Found"<<endl;
            return;
        }

        if(users[uid]->getPassword()!=password){
            cout<<"Wrong Password"<<endl;
            return;
        }

        users[uid]->setOnline(true);
        cout<<users[uid]->getName()<<" Logged In"<<endl;
    }

    void logout(int uid){
        lock_guard<mutex> lock(mtx);

        if(users.find(uid)==users.end()){
            return;
        }

        users[uid]->setOnline(false);
        cout<<users[uid]->getName()<<" Logged Out"<<endl;
    }

    Chat* createChat(vector<User*> participants){
        lock_guard<mutex> lock(mtx);

        Chat* chat = new Chat(nextChatID++,participants);

        chats[chat->getChatID()] = chat;

        cout<<"Chat Created Successfully"<<endl;

        return chat;
    }

    void sendMessage(Chat* chat,User* sender,string content,string time){
        lock_guard<mutex> lock(mtx);

        if(!chat->isParticipant(sender)){
            cout<<"Sender not part of chat"<<endl;
            return;
        }

        Message* msg = new Message(nextMessageID++,sender,content,time);

        msg->setStatus(DELIVERED);

        chat->addMessage(msg);

        cout<<"Message Sent Successfully"<<endl;
    }
};

int main(){

    ChatSystem chatSystem;

    User* u1 = new User(1,"Prabal","1234");
    User* u2 = new User(2,"Rahul","5678");
    User* u3 = new User(3,"Aman","1111");

    chatSystem.registerUser(u1);
    chatSystem.registerUser(u2);
    chatSystem.registerUser(u3);

    chatSystem.login(1,"1234");
    chatSystem.login(2,"5678");
    chatSystem.login(3,"1111");

    vector<User*> participants = {u1,u2,u3};

    Chat* groupChat = chatSystem.createChat(participants);

    chatSystem.sendMessage(groupChat,u1,"Hello Everyone","10:00 AM");
    chatSystem.sendMessage(groupChat,u2,"Hi Prabal","10:01 AM");
    chatSystem.sendMessage(groupChat,u3,"Good Morning","10:02 AM");

    cout<<endl;

    groupChat->showChatHistory();

    cout<<endl;

    groupChat->searchMessage("Hello");

    chatSystem.logout(1);
    chatSystem.logout(2);
    chatSystem.logout(3);

    return 0;
}