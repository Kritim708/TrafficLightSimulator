#ifndef __VEHICLE_BASE_H__
#define __VEHICLE_BASE_H__

// enum: see http://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#S-enum
enum class Direction   {north, south, east, west};
enum class VehicleType {car, suv, truck};
enum class LightColor  {green, yellow, red};

class VehicleBase
{
   public:
      static int vehicleCount;

   protected:
      int         vehicleID;
      VehicleType vehicleType;
      Direction   vehicleDirection;

   public:
      VehicleBase(VehicleType type, Direction originalDirection);
      VehicleBase(const VehicleBase& other);
      VehicleBase(VehicleBase&& other) noexcept;
      VehicleBase& operator=(const VehicleBase& other);
      VehicleBase& operator=(VehicleBase&& other) noexcept;
      ~VehicleBase();

      inline int getVehicleID() const { return this->vehicleID; }

      inline VehicleType getVehicleType() const { return this->vehicleType; }
      inline Direction   getVehicleOriginalDirection() const { return this->vehicleDirection; }
};

#endif
