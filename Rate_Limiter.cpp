#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <format>
#include <queue>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <set>
#include <list>
#include <climits>
#include <cstdlib>
#include <unordered_map>
#include <deque>

using namespace std;

class User{
private:
    int id;
    int limit;
    int window;

public:
    User(int i,int l,int w){
        id = i;
        limit = l;
        window = w;
    }

    int getID() const {
        return id;
    }

    int getlim() const {
        return limit;
    }

    int getWindow() const{
        return window;
    }
};

class Limiter{
private:
    unordered_map<int,User*> users;
    unordered_map<int,deque<int>> req;
    mutex mtx;

public:
    void addUser(User* u){
        lock_guard<mutex> lock(mtx);
        users[u->getID()] = u;
    }

    bool request(User* u,int time){
        lock_guard<mutex> lock(mtx);

        int id = u->getID();

        if(!users.count(id)){
            cout<<"User Not found"<<endl;
            return false;
        }

        auto& requests = req[id];

        while(!requests.empty() && time - requests.front() >= u->getWindow()){
            requests.pop_front();
        }

        if(requests.size() >= u->getlim()){
            cout<<"Request rejected"<<endl;
            return false;
        }

        requests.push_back(time);

        cout<<"Request accepted"<<endl;
        return true;
    }
};

int main(){
    User u1(1,3,10);

    Limiter limiter;

    limiter.addUser(&u1);

    limiter.request(&u1,1);
    limiter.request(&u1,2);
    limiter.request(&u1,3);
    limiter.request(&u1,4);

    limiter.request(&u1,11);

    return 0;
}