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
#include<set>
#include<list>
#include <climits>
#include <cstdlib>

using namespace std;

/*

Designing an Elevator System


Requirements

The elevator system should consist of multiple elevators serving multiple floors.
Each elevator should have a capacity limit and should not exceed it.
Users should be able to request an elevator from any floor and select a destination floor.
The elevator system should efficiently handle user requests and optimize the movement of elevators to minimize waiting time.
The system should prioritize requests based on the direction of travel and the proximity of the elevators to the requested floor.
The elevators should be able to handle multiple requests concurrently and process them in an optimal order.
The system should ensure thread safety and prevent race conditions when multiple threads interact with the elevators.


*/

/*

# Functional

Multiple elevators and floors.
Configurable capacity per elevator.
Request elevator from a floor with a direction.
Select destination after entering.
Assign suitable elevators to requests.
Serve compatible requests along the elevator's current direction.
Respect elevator capacity.
Handle concurrent requests.

# Non-Functional

Thread safety.
Scalability.
Extensibility.
Efficient request handling.

## Example Case : 

User at floor 5 presses UP and wants to go to floor 10.

Case 1 : 
Elevator : (Curr_floor > 5 && Dir = UP)
    Push Req in UpQ
    Continue

Case 2:
Elevator : (Dir = DOWN && FLOOR DON'T MATER)
    Push Req in UpQ
    Continue

Case 3:
Elevator : (Dir = UP && curr_floor <= 5)
    Push Req in UpQ
    When curr_floor = 5
    Case 3.1 : curr_pass < max_cap
        Pick User
    Case 3.2 : curr_pass >= max_cap
        Don't pick user


## Elevator Assignment :

UP:
  same direction / IDLE
  AND elevator <= user floor
  → choose nearest

DOWN:
  same direction / IDLE
  AND elevator >= user floor
  → choose nearest


## Overall HighLevel WorkFlow

1. User creates Hall Request (floor + direction)
2. System selects suitable elevator
3. Elevator moves toward requester
4. Elevator picks up user if capacity is available
5. Hall Request is removed
6. User selects destination
7. Destination is validated against elevator's floor range
8. Destination Request is added to elevator
9. Elevator moves according to its current direction
10. Passenger exits at destination
11. Passenger count decreases
12. Destination Request is completed
13. If no request remains in current direction:
       - reverse if opposite-direction requests exist
       - otherwise become IDLE

*/


enum Direction{
    UP,
    DOWN
};

enum Status{
    IDLE,
    MOVING_UP,
    MOVING_DOWN
};

class User{
private:
    int id;
    Direction dir;
    int curr_floor;
    int dest_floor;
public:
    User(int i,Direction d,int cf){
        id=i;
        dir=d;
        curr_floor=cf;
        dest_floor=-1;
    }
    int getID() const{
        return id;
    }
    Direction getDir() const{
        return dir;
    }
    int getCurrFloor() const{
        return curr_floor;
    }
    int getDestFloor() const{
        return dest_floor;
    }
    void setDestination(int floor){
        dest_floor=floor;
    }
};

class Elevator{
private:
    int id;
    int capacity;
    int curr_floor;
    int lower_limit;
    int upper_limit;
    Status E_status;
    int curr_users;
    map<int,queue<User*>> upRequests;
    map<int,queue<User*>> downRequests;
    set<int> upFloors;
    set<int> downFloors;
    map<int,queue<User*>> onboard;
    set<int> requestedFloors;
public:
    Elevator(int i,int cf,int ll,int ul,Status s,int cp){
        id=i;
        curr_floor=cf;
        lower_limit=ll;
        upper_limit=ul;
        E_status=s;
        capacity=cp;
        curr_users=0;
    }
    int getID() const{
        return id;
    }
    int getCurrFloor() const{
        return curr_floor;
    }
    Status getStatus() const{
        return E_status;
    }
    int getCapacityLeft() const{
        return capacity-curr_users;
    }
    int getLowerLimit() const{
        return lower_limit;
    }
    int getUpperLimit() const{
        return upper_limit;
    }
    void changeStatus(Status s){
        E_status=s;
    }
    void incrementFloor(){
        if(curr_floor<upper_limit)
            curr_floor++;
    }
    void decrementFloor(){
        if(curr_floor>lower_limit)
            curr_floor--;
    }
    bool addUser(){
        if(curr_users>=capacity)
            return false;
        curr_users++;
        return true;
    }
    void removeUser(){
        if(curr_users>0)
            curr_users--;
    }
    void addRequest(User* u){
        int floor=u->getCurrFloor();
        if(floor<lower_limit||floor>upper_limit)
            return;
        if(u->getDir()==UP){
            upRequests[floor].push(u);
            upFloors.insert(floor);
        }
        else{
            downRequests[floor].push(u);
            downFloors.insert(floor);
        }
    }
    bool hasUpWork(){
        auto destination=requestedFloors.lower_bound(curr_floor);
        auto pickup=upFloors.lower_bound(curr_floor);
        return destination!=requestedFloors.end()||pickup!=upFloors.end();
    }
    bool hasDownWork(){
        auto destination=requestedFloors.lower_bound(curr_floor);
        auto pickup=downFloors.lower_bound(curr_floor);
        return destination!=requestedFloors.begin()||pickup!=downFloors.begin();
    }
    void deboard(){
        if(!onboard.count(curr_floor))
            return;
        while(!onboard[curr_floor].empty()){
            onboard[curr_floor].pop();
            removeUser();
        }
        onboard.erase(curr_floor);
        requestedFloors.erase(curr_floor);
    }
    void pickup(queue<User*>& q){
        while(!q.empty()&&getCapacityLeft()>0){
            User* u=q.front();
            q.pop();
            if(u->getDestFloor()==-1)
                continue;
            if(u->getDestFloor()<lower_limit||u->getDestFloor()>upper_limit)
                continue;
            if(!addUser())
                break;
            int destination=u->getDestFloor();
            onboard[destination].push(u);
            requestedFloors.insert(destination);
        }
    }
    void moveUp(){
        changeStatus(MOVING_UP);
        while(true){
            deboard();
            if(upRequests.count(curr_floor)){
                pickup(upRequests[curr_floor]);
                if(upRequests[curr_floor].empty()){
                    upRequests.erase(curr_floor);
                    upFloors.erase(curr_floor);
                }
            }
            if(hasUpWork()){
                incrementFloor();
                continue;
            }
            if(hasDownWork()){
                moveDown();
            }
            else{
                changeStatus(IDLE);
            }
            break;
        }
    }
    void moveDown(){
        changeStatus(MOVING_DOWN);
        while(true){
            deboard();
            if(downRequests.count(curr_floor)){
                pickup(downRequests[curr_floor]);
                if(downRequests[curr_floor].empty()){
                    downRequests.erase(curr_floor);
                    downFloors.erase(curr_floor);
                }
            }
            if(hasDownWork()){
                decrementFloor();
                continue;
            }
            if(hasUpWork()){
                moveUp();
            }
            else{
                changeStatus(IDLE);
            }
            break;
        }
    }
    void start(){
        if(E_status!=IDLE)
            return;
        if(hasUpWork())
            moveUp();
        else if(hasDownWork())
            moveDown();
    }
};

class ElevatorSystem{
private:
    vector<Elevator*> elevators;
    map<int,queue<User*>> upRequests;
    map<int,queue<User*>> downRequests;
    mutex mtx;
public:
    void addElevator(Elevator* elevator){
        lock_guard<mutex> lock(mtx);
        elevators.push_back(elevator);
    }
    Elevator* findElevator(User* user){
        int userFloor=user->getCurrFloor();
        Direction dir=user->getDir();
        Elevator* best=nullptr;
        int minDistance=INT_MAX;
        for(auto elevator:elevators){
            int eFloor=elevator->getCurrFloor();
            Status status=elevator->getStatus();
            if(userFloor<elevator->getLowerLimit()||userFloor>elevator->getUpperLimit())
                continue;
            if(dir==UP&&status==MOVING_UP){
                if(eFloor>userFloor)
                    continue;
                int distance=userFloor-eFloor;
                if(distance<minDistance){
                    minDistance=distance;
                    best=elevator;
                }
            }
            else if(dir==DOWN&&status==MOVING_DOWN){
                if(eFloor<userFloor)
                    continue;
                int distance=eFloor-userFloor;
                if(distance<minDistance){
                    minDistance=distance;
                    best=elevator;
                }
            }
            else if(status==IDLE){
                int distance=abs(eFloor-userFloor);
                if(distance<minDistance){
                    minDistance=distance;
                    best=elevator;
                }
            }
        }
        return best;
    }
    void requestElevator(User* user){
        lock_guard<mutex> lock(mtx);
        Elevator* elevator=findElevator(user);
        if(elevator==nullptr){
            if(user->getDir()==UP)
                upRequests[user->getCurrFloor()].push(user);
            else
                downRequests[user->getCurrFloor()].push(user);
            cout<<"No suitable elevator currently available"<<endl;
            return;
        }
        elevator->addRequest(user);
        cout<<"Elevator "<<elevator->getID()<<" assigned to User "<<user->getID()<<endl;
        elevator->start();
    }
};

int main(){
    ElevatorSystem system;

    Elevator* e1=new Elevator(1,1,1,10,IDLE,2);
    Elevator* e2=new Elevator(2,8,1,10,IDLE,3);

    system.addElevator(e1);
    system.addElevator(e2);

    User* u1=new User(1,UP,5);
    u1->setDestination(10);
    system.requestElevator(u1);

    User* u2=new User(2,DOWN,8);
    u2->setDestination(3);
    system.requestElevator(u2);

    User* u3=new User(3,UP,2);
    u3->setDestination(7);
    system.requestElevator(u3);

    delete u1;
    delete u2;
    delete u3;
    delete e1;
    delete e2;

    return 0;
}