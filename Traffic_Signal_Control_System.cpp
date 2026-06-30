#include <iostream>
#include <unordered_map>
#include <queue>
#include <string>

using namespace std;

// 1. The Strict States
// Ordered clockwise so (current + 1) % 4 naturally rotates the lights!
enum Direction { NORTH, EAST, SOUTH, WEST }; 
enum LightColor { RED, GREEN };

// Helper function for clean console printing
string dirToString(Direction d) {
    switch(d) {
        case NORTH: return "NORTH";
        case EAST: return "EAST";
        case SOUTH: return "SOUTH";
        case WEST: return "WEST";
        default: return "UNKNOWN";
    }
}

// 2. The Physical Traffic Light
class TrafficLight {
private:
    Direction dir;
    LightColor color;
    int vehiclesWaiting;

public:
    TrafficLight(Direction d) {
        dir = d;
        color = RED; 
        vehiclesWaiting = 0;
    }
    
    // Getters and Setters
    void setColor(LightColor c) { color = c; }
    void addVehicle() { vehiclesWaiting++; }
    
    // Simulates cars driving through when light turns green
    int clearVehicles() {
        int cleared = vehiclesWaiting;
        vehiclesWaiting = 0;
        return cleared;
    }
};

// 3. The Master Intersection Controller
class TrafficController {
private:
    unordered_map<Direction, TrafficLight> lights;
    Direction currentGreenDirection;
    queue<Direction> emergencyQueue; 

public:
    TrafficController() {
        // Initialize the 4 intersection poles
        lights.insert({NORTH, TrafficLight(NORTH)});
        lights.insert({EAST, TrafficLight(EAST)});
        lights.insert({SOUTH, TrafficLight(SOUTH)});
        lights.insert({WEST, TrafficLight(WEST)});
        
        // Start system safely: Make North green, all others are already red
        currentGreenDirection = NORTH;
        lights.at(NORTH).setColor(GREEN);
        
        cout << "[System Boot] Traffic lights initialized. NORTH is GREEN." << endl;
        cout << "---------------------------------------------------" << endl;
    }

    // --- Asynchronous Events (Triggers from outside) ---

    void vehicleArrived(Direction d) {
        lights.at(d).addVehicle();
        // cout << "Vehicle arrived at " << dirToString(d) << "." << endl;
    }

    void emergencyVehicleArrived(Direction d) {
        emergencyQueue.push(d);
        cout << "[ALERT] Emergency vehicle detected at " << dirToString(d) << "!" << endl;
    }

    // --- The Master Loop (Called every few seconds by the system) ---

    void tick() {
        // Step 1: Safety First - Turn current green light to RED
        lights.at(currentGreenDirection).setColor(RED);
        cout << "[Signal Change] " << dirToString(currentGreenDirection) << " turns RED." << endl;

        // Step 2: Determine who gets the Green light next
        if (!emergencyQueue.empty()) {
            // Priority override!
            currentGreenDirection = emergencyQueue.front();
            emergencyQueue.pop();
            cout << "[EMERGENCY OVERRIDE ACTIVATED]" << endl;
        } else {
            // Normal Round Robin logic (mod 4)
            int nextDir = (currentGreenDirection + 1) % 4;
            currentGreenDirection = static_cast<Direction>(nextDir);
        }

        // Step 3: Execute the Green light
        lights.at(currentGreenDirection).setColor(GREEN);
        int clearedCars = lights.at(currentGreenDirection).clearVehicles();
        
        cout << "[Signal Change] " << dirToString(currentGreenDirection) 
             << " turns GREEN. (" << clearedCars << " waiting vehicles passed through)." << endl;
        cout << "---------------------------------------------------" << endl;
    }
};

int main() {
    TrafficController intersection;

    // Simulate normal traffic building up
    intersection.vehicleArrived(EAST);
    intersection.vehicleArrived(EAST);
    intersection.vehicleArrived(SOUTH);

    // System ticks forward to the next light (NORTH -> EAST)
    intersection.tick();

    // More traffic builds up
    intersection.vehicleArrived(WEST);
    intersection.vehicleArrived(WEST);
    intersection.vehicleArrived(WEST);
    intersection.vehicleArrived(NORTH);

    // Suddenly, an ambulance arrives at the WEST intersection!
    intersection.emergencyVehicleArrived(WEST);

    // System ticks forward. Instead of going EAST -> SOUTH, it should jump to WEST!
    intersection.tick();

    // System ticks again. The ambulance is gone. It should resume normal flow.
    // Since it's at WEST (3), (3+1)%4 = 0 (NORTH).
    intersection.tick();

    return 0;
}