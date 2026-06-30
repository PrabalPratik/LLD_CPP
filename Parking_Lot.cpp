#include <iostream>
#include <vector>
#include <string>

using namespace std;

// ==========================================
// 1. BASIC ENTITIES (The "Nouns")
// ==========================================

// Enum ensures strict type matching. A TRUCK cannot accidentally be treated as a BIKE.
enum VehicleType {
    CAR,
    BIKE,
    TRUCK
};

// Represents the customer's vehicle
class Vehicle { 
private:
    string ID;           // e.g., "MH-12-AB-1234"
    VehicleType type;    // CAR, BIKE, or TRUCK

public:
    // Constructor initializes the vehicle
    Vehicle(string id, VehicleType t) {
        ID = id;
        type = t;
    }
    
    // Getters so other classes can read these private variables safely
    VehicleType getType() const { return type; }
    string getID() const { return ID; }
};

// Represents a single physical parking space
class ParkingSpot { 
private:
    int spotNumber;      // e.g., Spot #101
    VehicleType type;    // What kind of vehicle fits here?
    bool available;      // Is it empty?

public:
    // Constructor defaults 'available' to true when created
    ParkingSpot(int spotNum, VehicleType t) {
        spotNumber = spotNum;
        type = t;
        available = true;
    }
    
    // Getters
    bool isAvailable() const { return available; }
    VehicleType getType() const { return type; }
    int getSpotNumber() const { return spotNumber; }
    
    // Actions to change the state of the spot
    void book() { available = false; }
    void free() { available = true; }
};

// Represents a single floor, which holds many spots (Composition: Floor HAS Spots)
class ParkingFloor { 
private:
    int floorNumber;
    vector<ParkingSpot> spots; // A dynamic array of simple ParkingSpot objects

public:
    ParkingFloor(int floorNum) {
        floorNumber = floorNum;
    }
    
    // Add a new spot to this specific floor
    void addSpot(ParkingSpot spot) {
        spots.push_back(spot);
    }
    
    // The '&' is crucial here! It passes the real list by reference, not a copy.
    // If we don't use '&', booking a spot only books a temporary copy of it.
    vector<ParkingSpot>& getSpots() { 
        return spots; 
    }
    
    int getFloorNumber() const { return floorNumber; }
};

// Represents the entire building, which holds many floors
class ParkingLot { 
private:
    string name;
    vector<ParkingFloor> floors; // Array of floors

public:
    ParkingLot(string lotName) {
        name = lotName;
    }
    
    void addFloor(ParkingFloor floor) {
        floors.push_back(floor);
    }
    
    // Pass by reference (&) to modify the actual floors
    vector<ParkingFloor>& getFloors() {
        return floors;
    }
    
    string getName() const { return name; }
};

// ==========================================
// 2. TRANSACTION ENTITIES
// ==========================================

// The receipt given to the driver. Since we don't use pointers, 
// we just save the Floor ID and Spot ID to know where the car went.
class Ticket {
private:
    string ticketID;
    Vehicle vehicle;
    int assignedSpotNumber; 
    int assignedFloorNumber;

public:
    Ticket(string id, Vehicle v, int spotNum, int floorNum) 
        : vehicle(v) // Initialize vehicle object
    {
        ticketID = id;
        assignedSpotNumber = spotNum;
        assignedFloorNumber = floorNum;
    }

    int getAssignedSpotNumber() const { return assignedSpotNumber; }
    int getAssignedFloorNumber() const { return assignedFloorNumber; }
    string getTicketID() const { return ticketID; }
    Vehicle getVehicle() const { return vehicle; }
};

// ==========================================
// 3. THE STRATEGY PATTERN (Finding a spot)
// ==========================================

// We use a simple struct to return the exact "coordinates" of an empty spot.
struct SpotLocation {
    bool found;      // Did we find a spot?
    int floorIndex;  // Which floor in the array?
    int spotIndex;   // Which spot in the array?
};

// The interface (Abstract Class). If we want to change how we find spots later, 
// we just create a new class that implements this.
class SpotAssignmentStrategy {
public:
    // Virtual function forces child classes to implement this method
    virtual SpotLocation findSpot(ParkingLot& lot, VehicleType type) = 0;
};

// Our specific strategy: Just find the very first empty spot that matches the vehicle type.
class FirstAvailableStrategy : public SpotAssignmentStrategy {
public:
    // Pass ParkingLot by reference (&) so we look at the real building
    SpotLocation findSpot(ParkingLot& lot, VehicleType type) override {
        vector<ParkingFloor>& floors = lot.getFloors();
        
        // Loop through all floors
        for (int i = 0; i < floors.size(); i++) {
            vector<ParkingSpot>& spots = floors[i].getSpots();
            
            // Loop through all spots on the current floor
            for (int j = 0; j < spots.size(); j++) {
                
                // If it is empty AND matches the car/bike/truck type perfectly
                if (spots[j].isAvailable() && spots[j].getType() == type) {
                    SpotLocation loc;
                    loc.found = true;
                    loc.floorIndex = i;
                    loc.spotIndex = j;
                    return loc; // Return the exact coordinates
                }
            }
        }
        
        // If we finish all loops and find nothing, return failure
        SpotLocation failedLoc;
        failedLoc.found = false;
        return failedLoc; 
    }
};

// ==========================================
// 4. THE GATES (Interacting with the system)
// ==========================================

class EntryGate {
private:
    string gateID;
    SpotAssignmentStrategy& assignmentStrategy; // Reference to our search algorithm

public:
    // Constructor requires a strategy so the gate knows HOW to look for spots
    EntryGate(string id, SpotAssignmentStrategy& strategy) 
        : gateID(id), assignmentStrategy(strategy) {}

    // Pass ParkingLot by reference (&) so we book a spot in the real lot
    Ticket processEntry(Vehicle vehicle, ParkingLot& lot) {
        
        // Step 1: Ask the strategy to find coordinates for this vehicle
        SpotLocation loc = assignmentStrategy.findSpot(lot, vehicle.getType());
        
        if (!loc.found) {
            cout << "Entry Rejected: No available spots for vehicle " << vehicle.getID() << endl;
            // Return a dummy ticket to signify failure
            return Ticket("INVALID", vehicle, -1, -1); 
        }

        // Step 2: Go exactly to those coordinates in our real parking lot and book it
        ParkingSpot& assignedSpot = lot.getFloors()[loc.floorIndex].getSpots()[loc.spotIndex];
        assignedSpot.book();

        // Step 3: Print success and return the ticket
        string ticketId = "TKT-" + vehicle.getID();
        Ticket ticket(ticketId, vehicle, assignedSpot.getSpotNumber(), lot.getFloors()[loc.floorIndex].getFloorNumber());
        
        cout << "Entry Successful! Vehicle " << vehicle.getID() 
             << " assigned to Floor " << ticket.getAssignedFloorNumber() 
             << ", Spot " << ticket.getAssignedSpotNumber() << endl;
             
        return ticket;
    }
};

class ExitGate {
private:
    string gateID;

public:
    ExitGate(string id) {
        gateID = id;
    }

    void processExit(Ticket ticket, ParkingLot& lot) {
        if (ticket.getTicketID() == "INVALID") return; // Ignore failed entries

        // Step 1: Read the floor and spot numbers from the ticket
        int targetFloor = ticket.getAssignedFloorNumber();
        int targetSpot = ticket.getAssignedSpotNumber();

        // Step 2: Find that exact spot in the lot and mark it free
        vector<ParkingFloor>& floors = lot.getFloors();
        for (int i = 0; i < floors.size(); i++) {
            if (floors[i].getFloorNumber() == targetFloor) {
                
                vector<ParkingSpot>& spots = floors[i].getSpots();
                for (int j = 0; j < spots.size(); j++) {
                    if (spots[j].getSpotNumber() == targetSpot) {
                        
                        spots[j].free(); // Spot is now empty again!
                        cout << "Exit Successful! Vehicle " << ticket.getVehicle().getID() 
                             << " left Floor " << targetFloor << ", Spot " << targetSpot << endl;
                        return; // Stop searching once we found and freed it
                    }
                }
            }
        }
    }
};

// ==========================================
// 5. MAIN EXECUTION (Testing it out)
// ==========================================

int main() {
    // 1. Build the physical building (no 'new' keyword, standard objects)
    ParkingLot lot("Tech Park Garage");
    ParkingFloor floor1(1);
    
    // Add two CAR spots and one BIKE spot
    floor1.addSpot(ParkingSpot(101, CAR));
    floor1.addSpot(ParkingSpot(102, CAR));
    floor1.addSpot(ParkingSpot(103, BIKE));
    
    lot.addFloor(floor1);

    // 2. Setup the Brains (Strategy) and the Physical Gates
    FirstAvailableStrategy strategy;
    EntryGate entryGate("Gate-1", strategy);
    ExitGate exitGate("Gate-2");

    cout << "--- System Initialized ---" << endl << endl;

    // 3. Create some vehicles
    Vehicle car1("JEEP-1234", CAR);
    Vehicle bike1("KA-01-9999", BIKE);
    Vehicle car2("JEEP-5678", CAR);
    Vehicle car3("EXTRA-CAR", CAR);

    // 4. Test Entries
    cout << "--- Processing Entries ---" << endl;
    Ticket t1 = entryGate.processEntry(car1, lot); // Takes Spot 101
    Ticket t2 = entryGate.processEntry(bike1, lot); // Takes Spot 103 (Bike spot)
    Ticket t3 = entryGate.processEntry(car2, lot); // Takes Spot 102
    
    // This should FAIL because both CAR spots (101 and 102) are taken
    Ticket t4 = entryGate.processEntry(car3, lot); 

    // 5. Test Exits
    cout << "\n--- Processing Exits ---" << endl;
    exitGate.processExit(t1, lot); // Frees Spot 101
    
    // Now that car1 left, car3 can finally enter and take Spot 101
    Ticket t5 = entryGate.processEntry(car3, lot); 

    return 0;
}