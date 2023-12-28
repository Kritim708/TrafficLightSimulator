#ifndef __VEHICLE_BASE_CPP__
#define __VEHICLE_BASE_CPP__

#include "VehicleBase.h"

int VehicleBase::vehicleCount = 0;

VehicleBase::VehicleBase(VehicleType type, Direction direction)
    : vehicleID(VehicleBase::vehicleCount++), 
      vehicleType(type),
      vehicleDirection(direction)
{}

VehicleBase::VehicleBase(const VehicleBase& other)
    : vehicleID(other.vehicleID),
      vehicleType(other.vehicleType),
      vehicleDirection(other.vehicleDirection)
{}

VehicleBase::VehicleBase(VehicleBase&& other) noexcept
    : vehicleID{other.vehicleID},
      vehicleType{other.vehicleType},
      vehicleDirection{other.vehicleDirection} {
        other.vehicleID = -1;
        other.vehicleType = VehicleType::car;
        other.vehicleDirection = Direction::north;
      }

VehicleBase& VehicleBase::operator=(const VehicleBase& other) {
  if (this == &other)
    return *this;
  vehicleID = other.vehicleID;
  vehicleType = other.vehicleType;
  vehicleDirection = other.vehicleDirection;

  return *this;
}

VehicleBase& VehicleBase::operator=(VehicleBase&& other) noexcept {
  if (this == &other)
    return *this;

  vehicleID = other.vehicleID;
  vehicleType = other.vehicleType;
  vehicleDirection = other.vehicleDirection;

  other.vehicleID = -1;
  other.vehicleType = VehicleType::car;
  other.vehicleDirection = Direction::north;

  return *this;
}

VehicleBase::~VehicleBase() {}

#endif
