#ifndef __VEHICLE_CPP__
#define __VEHICLE_CPP__
#include <utility>
#include "Vehicle.h"

//Constructor
Vehicle::Vehicle(VehicleType type, Direction originalDirection, TurnType turnType) :
    VehicleBase(type, originalDirection), backIndex{-1}, frontIndex{-1}, inTransition{false}, 
    turnType{turnType}, currDirection{originalDirection} {
        
    if (type == VehicleType::car) {
        length = 2;
    } else if (type == VehicleType::suv) {
        length = 3;
    } else if (type == VehicleType::truck) {
        length = 4;
    }
}

//Setters
void Vehicle::setTransition(bool transitionStatus) {
    this->inTransition = transitionStatus;
}

void Vehicle::setBackIndex(int newBackIndex) {
    this->backIndex = newBackIndex;
}

void Vehicle::setFrontIndex(int newFrontIndex) {
    this->frontIndex = newFrontIndex;
}

void Vehicle::setDirection(Direction direction){
    this->currDirection = direction;
}


//Copy Constructor
Vehicle::Vehicle(const Vehicle& other) : VehicleBase(other){ 
    backIndex = other.backIndex;
    frontIndex = other.frontIndex; 
    inTransition = other.inTransition; 
    turnType = other.turnType;
    currDirection = other.currDirection;
    length = other.length;
}

//Move Constructor
Vehicle::Vehicle(Vehicle&& other) noexcept : VehicleBase(std::move(other)) {
    VehicleBase(std::move(other));
    backIndex = other.backIndex;
    other.backIndex = -1;
    frontIndex = other.frontIndex;
    other.frontIndex = -1;
    inTransition = other.inTransition;
    other.inTransition = false;
    turnType = other.turnType;
    currDirection = other.currDirection;
    length = other.length;
}

//Copy Assignment
Vehicle& Vehicle::operator=(const Vehicle& other) {
    if (this == &other)
        return *this;

    vehicleType = other.vehicleType;
    vehicleDirection = other.vehicleDirection;
    vehicleID = other.vehicleID;
    backIndex = other.backIndex;
    frontIndex = other.frontIndex;
    inTransition = other.inTransition;
    turnType = other.turnType;
    currDirection = other.currDirection;
    return *this;
}


//Move Assignment
Vehicle& Vehicle::operator=(Vehicle&& other) noexcept {
    if (this == &other)
        return *this;

    vehicleType = other.vehicleType;
    vehicleDirection = other.vehicleDirection;
    vehicleID = other.vehicleID;
    backIndex = other.backIndex;
    frontIndex = other.frontIndex;
    inTransition = other.inTransition;
    turnType = other.turnType;
    currDirection = other.currDirection;

    other.vehicleType = VehicleType::car;
    other.vehicleDirection = Direction::north;
    other.vehicleID = -1;
    other.backIndex = -2;
    other.frontIndex = -2;
    other.inTransition = false;
    other.turnType = TurnType::nulled;
    other.currDirection = Direction::north;
    return *this;
}

//Destructor
Vehicle::~Vehicle() {}

#endif
