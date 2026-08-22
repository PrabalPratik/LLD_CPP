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

enum VehicleType {
    MOTORCYCLE,
    CAR,
    TRUCK
};

class Vehicle{
private:
    string number;
    VehicleType type;
public:
    Vehicle(string n , VehicleType t){
        number = n;
        type = t;
    }
    string getNumber() const {
        return number;
    }
    VehicleType getType() const {
        return type;
    }
};



class ParkingSpot{
private:
    int spotID;
    VehicleType type; // type of vehicle that can be accomodated
    bool isOccupied;
public:
    ParkingSpot(int id , VehicleType t){
        spotID = id;
        type = t;
        isOccupied = false;
    }
    int getID() const {
        return spotID;
    }
    VehicleType getType() const{
        return type;
    }
    bool isAvailable() const {
        return !isOccupied;
    }
    void park(){
        isOccupied = true;
    }
    void freeSpot(){
        isOccupied = false;
    }
};

class ParkingFloor{
private:
    int floorNumber;
    vector<ParkingSpot*> spots;
public:
    ParkingFloor(int num){
        floorNumber = num;
    }
    int getFloorNumber() const {
        return floorNumber;
    }
    void addSpot(ParkingSpot* p){
        spots.push_back(p);
    }
    ParkingSpot* findAvailableSpot(VehicleType type){
        for(auto s : spots){
            if(s->getType() == type && s->isAvailable()){
                return s;
            }
        }
        return nullptr;
    }
    void displaySpots(){
        for(auto s : spots){
            if(s->isAvailable()){
                cout<< s->getID() << s->getType()<<endl;
            }
        }
    }
};

enum PaymentStatus{
    PENDING,
    PAID
};

class Ticket{
private:
    int ticketID;
    Vehicle* vehicle;
    ParkingSpot* spot;
    PaymentStatus paymentStatus;

public:

    Ticket(int id,Vehicle* v,ParkingSpot* s){

        ticketID = id;
        vehicle = v;
        spot = s;
        paymentStatus = PENDING;
    }

    int getTicketID() const{
        return ticketID;
    }

    Vehicle* getVehicle() const{
        return vehicle;
    }

    ParkingSpot* getSpot() const{
        return spot;
    }


    PaymentStatus getStatus() const{
        return paymentStatus;
    }

    int calculateFare(int time){

        int duration = time;

        if(vehicle->getType() == CAR){
            return duration * 100;
        }

        if(vehicle->getType() == TRUCK){
            return duration * 200;
        }

        return duration * 50;
    }

    void markPaid(){
        paymentStatus = PAID;
    }
};


class ParkingLot{
private:
    vector<ParkingFloor*> lot;
    unordered_map<string,Ticket*> activeTickets;
    int ticketID;
    mutex mtx;
public:

    ParkingLot(){
        ticketID = 1;
    }

    void addFloor(ParkingFloor* floor){
        lot.push_back(floor);
    }

    ParkingSpot* findSpot(VehicleType type){
        for(auto floor : lot){
            ParkingSpot* spot = floor->findAvailableSpot(type);
            if(spot != nullptr){
                return spot;
            }
        }
        return nullptr;
    }

    void displayAvailableSpots(){
        for(auto floor : lot){
            floor->displaySpots();
        }
    }

    void parkVehicle(Vehicle* vehicle){
        lock_guard<mutex> lock(mtx);
        ParkingSpot* spot = findSpot(vehicle->getType());

        if(spot == nullptr){
            cout<<"No Parking Spot Available"<<endl;
            return;
        }
        spot->park();
        Ticket* ticket = new Ticket(ticketID++,vehicle,spot);

        activeTickets[vehicle->getNumber()] = ticket;
        cout<<"Vehicle Parked Successfully"<<endl;
        cout<<"Ticket ID : "<<ticket->getTicketID()<<endl;
        cout<<"Spot ID : "<<spot->getID()<<endl;
    }

    void unparkVehicle(string vehicleNumber,int total_time){
        lock_guard<mutex> lock(mtx);
        if(!activeTickets.count(vehicleNumber)){
            cout<<"Vehicle Not Found"<<endl;
            return;
        }

        Ticket* ticket = activeTickets[vehicleNumber];

        int fare = ticket->calculateFare(total_time);

        cout<<"Parking Fare : "<<fare<<endl;

        ticket->markPaid();

        ticket->getSpot()->freeSpot();

        activeTickets.erase(vehicleNumber);

        cout<<"Vehicle Unparked Successfully"<<endl;
    }
};


int main() {

    ParkingLot parkingLot;

    ParkingFloor* floor1 = new ParkingFloor(1);
    ParkingFloor* floor2 = new ParkingFloor(2);

    floor1->addSpot(new ParkingSpot(101, MOTORCYCLE));
    floor1->addSpot(new ParkingSpot(102, CAR));
    floor1->addSpot(new ParkingSpot(103, TRUCK));

    floor2->addSpot(new ParkingSpot(201, MOTORCYCLE));
    floor2->addSpot(new ParkingSpot(202, CAR));
    floor2->addSpot(new ParkingSpot(203, TRUCK));

    parkingLot.addFloor(floor1);
    parkingLot.addFloor(floor2);

    Vehicle* bike = new Vehicle("BR01AA1111", MOTORCYCLE);
    Vehicle* car = new Vehicle("BR01BB2222", CAR);
    Vehicle* truck = new Vehicle("BR01CC3333", TRUCK);

    cout << "\nAvailable Spots Initially\n";
    parkingLot.displayAvailableSpots();

    cout << "\nParking Vehicles\n";
    parkingLot.parkVehicle(bike);
    parkingLot.parkVehicle(car);
    parkingLot.parkVehicle(truck);

    cout << "\nAvailable Spots After Parking\n";
    parkingLot.displayAvailableSpots();

    cout << "\nUnparking Car\n";
    parkingLot.unparkVehicle("BR01BB2222", 5);

    cout << "\nAvailable Spots After Unparking\n";
    parkingLot.displayAvailableSpots();

    cout << "\nTrying to Unpark Again\n";
    parkingLot.unparkVehicle("BR01BB2222", 5);

    return 0;
}