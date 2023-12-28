#ifndef __VEHICLE_H__
#define __VEHICLE_H__

#include "VehicleBase.h"

enum class TurnType {straight, right, left, nulled};

class Vehicle : public VehicleBase {
    private:
        int backIndex;
        int frontIndex;
        int length;
        bool inTransition;
        TurnType turnType;
        Direction currDirection;
    
    public:
        Vehicle(VehicleType type, Direction originalDirection, TurnType turnType);
        Vehicle(const Vehicle& other);
        Vehicle(Vehicle&& other) noexcept;
        Vehicle& operator=(const Vehicle& other);
        Vehicle& operator=(Vehicle&& other) noexcept;
        ~Vehicle();

        void setTransition(bool transitionStatus);
        void setBackIndex(int newBackIndex);
        void setFrontIndex(int newFrontIndex);
        void setDirection(Direction direction);

        inline int getBackIndex() { return backIndex; };
        inline int getFrontIndex() { return frontIndex; };
        inline int getLength() { return length; };
        inline bool getInTransition() { return inTransition; };
        inline TurnType getTurn() { return turnType; };
        inline Direction getDirection() { return currDirection; }
};

#endif