#ifndef __SIMULATOR_CPP__
#define __SIMULATOR_CPP__

#include "Simulator.h"
#include "Vehicle.h"
#include "VehicleBase.h"
#include "Animator.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <random>

using namespace std;

Simulator::Simulator(string file, int seed) {

    this->seed = seed;

    ifstream infile {file};

    if (!infile) {
        cerr << "Unable to open file: " << file << endl;
        exit(0);
    }

    string paramName;
    double paramValue;

    map<string, double> parameters;

    while (!infile.eof()) {
        infile >> paramName;
        infile >> paramValue;
        parameters[paramName] = paramValue;
    }

    simTime = parameters["maximum_simulated_time:"];
    roadLen = parameters["number_of_sections_before_intersection:"];
    greenNS = parameters["green_north_south:"];
    yellowNS = parameters["yellow_north_south:"];
    greenEW = parameters["green_east_west:"];
    yellowEW = parameters["yellow_east_west:"];
    probNB = parameters["prob_new_vehicle_northbound:"];
    probSB = parameters["prob_new_vehicle_southbound:"];
    probEB = parameters["prob_new_vehicle_eastbound:"];
    probWB = parameters["prob_new_vehicle_westbound:"];
    proportionCars = parameters["proportion_of_cars:"];
    proportionSUVs = parameters["proportion_of_SUVs:"];
    proportionCarRight = parameters["proportion_right_turn_cars:"];
    proportionCarLeft = parameters["proportion_left_turn_cars:"];
    proportionSUVRight = parameters["proportion_right_turn_SUVs:"];
    proportionSUVLeft = parameters["proportion_left_turn_SUVs:"];
    proportionTruckRight = parameters["proportion_right_turn_trucks:"];
    proportionTruckLeft = parameters["proportion_left_turn_trucks:"];

    lightCycle = greenEW + yellowEW + greenNS + yellowNS;

    // construct vectors of VehicleBase* of appropriate size, init to nullptr
    westbound.resize(roadLen * 2 + 2);
    eastbound.resize(roadLen * 2 + 2);
    southbound.resize(roadLen * 2 + 2);
    northbound.resize(roadLen * 2 + 2);
    
}

void Simulator::runSimulation() {

    mt19937 randomNumberGenerator; // Mersenne twister
    uniform_real_distribution<double> rand_double(0, 1);

    randomNumberGenerator.seed(this->seed);

    //Null vector to re-initialize
    vector<VehicleBase*> nullVector(roadLen * 2 + 2, nullptr);

    // construct vectors of VehicleBase* of appropriate size, init to nullptr
    westbound.assign(nullVector.begin(), nullVector.end());
    eastbound.assign(nullVector.begin(), nullVector.end());
    southbound.assign(nullVector.begin(), nullVector.end());
    northbound.assign(nullVector.begin(), nullVector.end());

    vector<vector<VehicleBase*>*> allBounds {&northbound, &westbound, &southbound, &eastbound};

    char dummy;

    Animator anim(roadLen);

    string lightNSState = "green";
    int NSTimeToRed = greenNS + yellowNS;

    string lightEWState = "red";
    int EWTimeToRed = 0; 

    // Section checks
    int NESec = 0;
    int NWSec = 0;
    int SESec = 0;
    int SWSec = 0;


    for (int i = 0; i < simTime; i++) {
        // Clearing the Lanes
        westbound.assign(nullVector.begin(), nullVector.end());
        eastbound.assign(nullVector.begin(), nullVector.end());
        southbound.assign(nullVector.begin(), nullVector.end());
        northbound.assign(nullVector.begin(), nullVector.end());

        // Creating vehicles to add
        addVehicle(northbound, Direction::north, probNB, rand_double(randomNumberGenerator), rand_double(randomNumberGenerator), rand_double(randomNumberGenerator));
        addVehicle(southbound, Direction::south, probSB, rand_double(randomNumberGenerator), rand_double(randomNumberGenerator), rand_double(randomNumberGenerator));
        addVehicle(eastbound, Direction::east, probEB, rand_double(randomNumberGenerator), rand_double(randomNumberGenerator), rand_double(randomNumberGenerator));
        addVehicle(westbound, Direction::west, probWB, rand_double(randomNumberGenerator), rand_double(randomNumberGenerator), rand_double(randomNumberGenerator));

        // Setting the lights
        setLights(i, anim, lightNSState, NSTimeToRed, lightEWState, EWTimeToRed);


        for (auto& vehicle : vehicles) {
            // Past Transition Vehicles
            if (vehicle.getBackIndex()  > roadLen + 2) {
                moveStraight(vehicle);
            // During Transition
            } else if (vehicle.getInTransition() && vehicle.getTurn() == TurnType::right) {
                moveTransition(vehicle, allBounds);
            } else if (vehicle.getInTransition() && vehicle.getTurn() == TurnType::left) {
                moveTransitionLeft(vehicle, allBounds);
            // Vehicle right before getting into the transition: 
            } else if (vehicle.getFrontIndex() + 1 == roadLen) {
                if (checkLight(vehicle, lightNSState, lightEWState) && 
                        checkMove(vehicle, NSTimeToRed, EWTimeToRed) && 
                        clearPathTransition(vehicle, NESec, NWSec, SESec, SWSec)) {
                    moveStraight(vehicle);
                    if (vehicle.getTurn() != TurnType::straight) {
                        vehicle.setTransition(true);
                    }
                //Vehicle can't move forward
                } else {
                    printVehicle(vehicle);
                }
            } else {
                //Vehicle moving in straight line at the beginning
                if (clearPath(vehicle)) {
                    moveStraight(vehicle);
                } else {
                    printVehicle(vehicle);
                }
            }
        }

        // Regulating the section reservations
        NESec = max(0, NESec-1);
        NWSec = max(0, NWSec-1);
        SESec = max(0, SESec-1);
        SWSec = max(0, SWSec-1);

        // Setting up the animation
        // Adding the bounds in the animations
        anim.setVehiclesNorthbound(northbound);
        anim.setVehiclesWestbound(westbound);
        anim.setVehiclesSouthbound(southbound);
        anim.setVehiclesEastbound(eastbound);

        // Drawing the Animation
        anim.draw(i);

        // Asking for input
        cin.get(dummy);
    }
}


/*
 * Creates vehicles using given probabilites and add them to vector vehicles 
 * @param vector<VehicleBase*>& bound; vector storing all bounds
 * @param Direction direction the bound to which a vehicle will be added
 * @param double inputLaneProb the probability of a vehicle appearing in a given bound
 * @param double spawnProb a randomly generated double which will determine whether a vehicle is created
 * @param double typeProba randomly generated number which will determine what type of vehicle is spawned
 * @param turnProb a randomly generated number which will determine what direction the behicle is going to turn
 */
void Simulator::addVehicle(vector<VehicleBase*>& bound, Direction direction, double inputLaneProb, double spawnProb, double typeProb, double turnProb) {
    if (bound[0] == nullptr && spawnProb <= inputLaneProb) {
        if (typeProb <= proportionCars) {
            if (turnProb <= proportionCarRight) {
                // create a right-turn car
                vehicles.push_back(Vehicle(VehicleType::car, direction, TurnType::right));
                return;
            } else if (turnProb <= proportionCarRight + proportionCarLeft) {
                // create a left-turn car
                vehicles.push_back(Vehicle(VehicleType::car, direction, TurnType::left));
                return;
            } else {
                // create a straight car
                vehicles.push_back(Vehicle(VehicleType::car, direction, TurnType::straight));
                return;
            }
        } else if (typeProb <= proportionCars + proportionSUVs) {
            if (turnProb <= proportionSUVRight) {
                vehicles.push_back(Vehicle(VehicleType::suv, direction, TurnType::right));
                return;
            } else if (turnProb <= proportionSUVRight + proportionSUVLeft) {
                // create a left-turn SUV
                vehicles.push_back(Vehicle(VehicleType::suv, direction, TurnType::left));
                return;
            } else {
                // create a straight SUV
                vehicles.push_back(Vehicle(VehicleType::suv, direction, TurnType::straight));
                return;
            }
        } else {
            if (turnProb <= proportionTruckRight) {
                // create a right-turn Truck
                vehicles.push_back(Vehicle(VehicleType::truck, direction, TurnType::right));
                return;
            } else if (turnProb <= proportionTruckRight + proportionTruckLeft) {
                // create a left-turn truck
                vehicles.push_back(Vehicle(VehicleType::truck, direction, TurnType::left));
                return;
            } else {
                // create a straight truck
                vehicles.push_back(Vehicle(VehicleType::truck, direction, TurnType::straight));
                return;
            }
        }
    }
}

/*
 * Moves the vehicle (passed as the parameter) a step forward in its own bound based on the currDirection value
 * Doesn't check for any condition: if the road ahead is clear or not
 * Uses printVehicle method
 * @param Vehicle& vhehicle
 */
void Simulator::moveStraight(Vehicle& vehicle){

    int vehicleLength = vehicle.getLength();
    int maxIndex = roadLen * 2 + 1;

    // Adjusting front and back index considering the edges 
    if (vehicle.getFrontIndex() < roadLen){
        vehicle.setFrontIndex(vehicle.getFrontIndex() + 1);
        vehicle.setBackIndex(max(-1, (vehicle.getFrontIndex() - vehicleLength)));
    } else {
        vehicle.setBackIndex(vehicle.getBackIndex() + 1);
        vehicle.setFrontIndex(min(maxIndex, (vehicle.getBackIndex() + vehicleLength)));
    }
    printVehicle(vehicle);
}


/*
 * Places the vehicle in its own bound based on its FrontIndex and BackIndex and currDirection
 * @param Vehicle& vehicle
 */
void Simulator::printVehicle(Vehicle& vehicle){
    if (vehicle.getDirection() == Direction::north){
        for (int i = vehicle.getFrontIndex(); i > vehicle.getBackIndex(); i--) {
            northbound[i] = &vehicle;
        }
    } else if (vehicle.getDirection() == Direction::east){
        for (int i = vehicle.getFrontIndex(); i > vehicle.getBackIndex(); i--) {
            eastbound[i] = &vehicle;
        }
    }else if (vehicle.getDirection() == Direction::west){
        for (int i = vehicle.getFrontIndex(); i > vehicle.getBackIndex(); i--) {
            westbound[i] = &vehicle;
        }
    } else {
        for (int i = vehicle.getFrontIndex(); i > vehicle.getBackIndex(); i--) {
            southbound[i] = &vehicle;
        }
    }
}



/* Finds the color of light in each bound and sets the animation as such also calculates the time to be red 
 * for each light
 * @param int i value of the iteration from simulated time 
 * @param Animation& anim animator to set the lights
 * @param string& lightNSState tells the color of the NorthSouth bound
 * @param string& lightEWState tells the color of the EastWest bound
 * @param int& NSTimeToRed stores the time for NorthSouth light to be red
 * @param int& EWTimeToRed stores the time for EastWest light to be red
 */
void Simulator::setLights(int i, Animator& anim, string& lightNSState, int& NSTimeToRed, string& lightEWState, int& EWTimeToRed) {
    
    int modValue = i % lightCycle;

    /*
    Format:
        if (condition) {
            setting the light in the animation
            adding the string value to LightNS or LightEW's 0th position
            adding "time left for it to be red light" to LightNS or LightEW's 1st position
        }
    */
    
    // Light for North South
    if (modValue < greenNS) {
        anim.setLightNorthSouth(LightColor::green);
        lightNSState = "green"; 
        NSTimeToRed = yellowNS + (greenNS - modValue);
    } else if (modValue < greenNS + yellowNS) {
        anim.setLightNorthSouth(LightColor::yellow);
        lightNSState = "yellow";
        NSTimeToRed = greenNS + yellowNS - modValue;
    } else {
        anim.setLightNorthSouth(LightColor::red);
        lightNSState = "red";
        NSTimeToRed = 0;
    }

    // Light for East West
    if (modValue < greenNS + yellowNS) {
        anim.setLightEastWest(LightColor::red);
        lightEWState = "red";
        EWTimeToRed = 0;
    } else if ( modValue < greenNS + yellowNS + greenEW) {
        anim.setLightEastWest(LightColor::green);
        lightEWState = "green";
        EWTimeToRed = (greenNS + yellowNS + greenEW + yellowEW) - modValue;
    } else {
        anim.setLightEastWest(LightColor::yellow);
        lightEWState = "yellow";
        EWTimeToRed = lightCycle - modValue;
    }
}


/*
 * Checks if the section ahead of the vehicle is occupied or not
 * @param Vehicle& vehicle
 * @return bool value
 */
bool Simulator::clearPath(Vehicle& vehicle) {
    // The case of a vehicle moving straight
    if (vehicle.getDirection() == Direction::north){
        return northbound[vehicle.getFrontIndex()+1] == nullptr;
    } else if (vehicle.getDirection() == Direction::east){
        return eastbound[vehicle.getFrontIndex()+1] == nullptr;
    }else if (vehicle.getDirection() == Direction::west){
        return westbound[vehicle.getFrontIndex()+1] == nullptr;
    } else {
        return southbound[vehicle.getFrontIndex()+1] == nullptr;
    }
}

/*Checks if the path is clear for vehicle to move
 *If the vehicle can move, it reserves the corresponding section spot in the transition sections
 *@param Vehicle& vehicle that needs to be checked for transition
 *@param int& NESec int variable responsible to reserve spot for NorthEast section of transition
 *@param int& NWSec int variable responsible to reserve spot for NorthWest section of transition
 *@param int& SESec int variable responsible to reserve spot for SouthEast section of transition
 *@param int& SESec int variable responsible to reserve spot for SourthWest section of transition
 *@bool true if the vehicle can move else false
 */
bool Simulator::clearPathTransition(Vehicle& vehicle, int& NESec, int& NWSec, int& SESec, int& SWSec) {
    // For vehicles moving straight
    if (vehicle.getTurn() == TurnType::straight) {
        if (vehicle.getDirection() == Direction::north) {
            if ((NESec == 0) && (NWSec == 0)){
                NESec = vehicle.getLength()-1;
                NWSec = vehicle.getLength();
                return true;
            } else {
                return false;
            }
        } else if (vehicle.getDirection() == Direction::east) {
            if ((SESec == 0) && (NESec == 0)){
                SESec = vehicle.getLength()-1;
                NESec = vehicle.getLength();
                return true;
            } else {
                return false;
            }
        }else if (vehicle.getDirection() == Direction::west) {
            if ((NWSec == 0) && (SWSec == 0)) {
                NWSec = vehicle.getLength()-1;
                SWSec = vehicle.getLength();
                return true;
            } else {
                return false;
            }
        } else {
            if ((SWSec == 0) && (SESec == 0)) {
                SWSec = vehicle.getLength()-1;
                SESec = vehicle.getLength();
                return true;
            } else {
                return false;
            }
        }


    // For right turn vehicles   
    } else if (vehicle.getTurn() == TurnType::right) {
        if (vehicle.getDirection() == Direction::north) {
            if (NESec == 0) {
                NESec = vehicle.getLength();
                return true;
            } else {
                return false;
            }
        } else if (vehicle.getDirection() == Direction::east) {
            if (SESec == 0) {
                SESec = vehicle.getLength();
                return true;
            } else {
                return false;
            }
        }else if (vehicle.getDirection() == Direction::west) {
            if (NWSec == 0) {
                NWSec = vehicle.getLength();
                return true;
            } else {
                return false;
            }
        } else {
            if (SWSec == 0) {
                SWSec = vehicle.getLength();
                return true;
            } else {
                return false;
            }
        }

    // For left turns
    } else {
         if (vehicle.getDirection() == Direction::north) {
            if ((NESec == 0) && (SWSec == 0)) {
                NESec = vehicle.getLength();
                SWSec = vehicle.getLength() + 1;
                return true;
            } else {
                return false;
            }
        } else if (vehicle.getDirection() == Direction::east) {
            if ((SESec == 0) && (NWSec == 0)) {
                SESec = vehicle.getLength();
                NWSec = vehicle.getLength() + 1;
                return true;
            } else {
                return false;
            }
        }else if (vehicle.getDirection() == Direction::west) {
            if ((NWSec == 0) && (SESec == 0)){
                NWSec = vehicle.getLength();
                SESec = vehicle.getLength() + 1;
                return true;
            } else {
                return false;
            }
        } else {
            if ((SWSec == 0) && (NESec == 0)) {
                SWSec = vehicle.getLength();
                NESec = vehicle.getLength() + 1;
                return true;
            } else {
                return false;
            }
        }
    }
}

/*
 * @param Vehcile& vehicle the light of the vehicle's lane will be checked
 * @param string& lightNSState tells the color of the North-South lane
 * @param string& lightEWState tells the color of the East-West lane
 * @return bool value: true if the vehicle can move, false if the vehicle cannot move (light is red)
 */
bool Simulator::checkLight(Vehicle& vehicle, string& lightNSState, string& LightEWState) {
    if ((vehicle.getVehicleOriginalDirection() == Direction::north) 
            || (vehicle.getVehicleOriginalDirection() == Direction::south)) {
        return (lightNSState != "red");
    } else {
        return (LightEWState != "red");
    }
}

/*
 * Checks if the vehicle has enough time to make full transition before red light
 * @param Vehicle& vehicle 
 * @param int LightNS NS light status
 * @param int LightEW EW light status
 * @return bool value true: if the vehicle has enough time to make the transition
 * false: if the vehicle doesn't have enought time to make the transition
*/
bool Simulator::checkMove(Vehicle& vehicle, int NSTimeToRed, int EWTimeToRed) {
    int checkLength; 
    if (vehicle.getTurn() == TurnType::right) {
        checkLength = vehicle.getLength() - 1;
    } else {
        checkLength = vehicle.getLength(); 
    }
    // Check enough time before red-light for full transition
    if ((vehicle.getVehicleOriginalDirection() == Direction::north) 
            || (vehicle.getVehicleOriginalDirection() == Direction::south)) {
        return (NSTimeToRed > checkLength);
    } else {
        return (EWTimeToRed > checkLength);
    }
}

/*
 * Moves the vehicles that are turning right through the intersection in phases
 * depending on the type (length) of the vehicle
 * @param Vehicle& vehicle
 * @param vector<vector<VehicleBase*>*>& allBounds used to calculate the next bound for the
 * vehicle (the one it's transitioning into)
 */
void Simulator::moveTransition(Vehicle& vehicle, vector<vector<VehicleBase*>*>& allBounds){
    
    int orrIndex;
    int nextIndex;
    int vehicleLength;

    vector<Direction> directions = {Direction::north, Direction::west, Direction::south, Direction::east};

    // Getting indexes of current direction, then finding the next direction from it.
    orrIndex = distance(directions.begin(), find(directions.begin(), directions.end(), vehicle.getVehicleOriginalDirection()));
    nextIndex = (orrIndex + 3) % 4;
    vehicleLength = vehicle.getLength();

    // First Phase of Transition for all vehicles (only transition phase for car)
    if (vehicle.getFrontIndex() == roadLen) {
        // vehicle in the transitioning bound
        (*allBounds[nextIndex])[roadLen + 2] = &vehicle;

        // vehicle in its own original bound
        for (int i = roadLen; i > (roadLen - vehicleLength + 1); i--) {
            (*allBounds[orrIndex])[i] = &vehicle;
        }

        // Changing vehicle's Start Index as it changed after making the turn
        vehicle.setFrontIndex(roadLen + 2);

        // If vehicle is car, inTransition will be off from here
        if (vehicle.getVehicleType() == VehicleType::car){
            // Transition phase is over
            vehicle.setTransition(false);
            
            // Recompute the vehicle index as they have moved two steps forward
            vehicle.setBackIndex(roadLen);
            
            // new currDirection will be set
            vehicle.setDirection(directions[nextIndex]);
        }

    // Second Phase of Transition for all vehicles 
    } else if (vehicle.getFrontIndex() == roadLen + 2) { // The next step (Conditional on type of car)
        // Vehicle in the transitioning bound
        (*allBounds[nextIndex])[roadLen + 2] = &vehicle;
        (*allBounds[nextIndex])[roadLen + 3] = &vehicle;

        // Vehicle in its own original bound
        for (int i = roadLen; i > (roadLen - vehicleLength + 2); i--) {
            (*allBounds[orrIndex])[i] = &vehicle;
        }

        // Changing vehicle's Start Index
        vehicle.setFrontIndex(roadLen + 3);
        
        // If vehicle is SUV, inTransition will be off from here
        if (vehicle.getVehicleType() == VehicleType::suv) {
            
            //Transition phase is over
            vehicle.setTransition(false);

            // Recompute the vehicle index as they have moved two steps forward 
            // Realize the vehicle index has not been modified during the transition phase
            vehicle.setBackIndex(roadLen);

            // Change the value of the original direction of the vehicle for suv
            vehicle.setDirection(directions[nextIndex]);
        }

    // Third Phase of Transition (only possible for Trucks)   
    } else if (vehicle.getFrontIndex() == roadLen + 3){
        // Vehicle in the transitioning bound
        (*allBounds[nextIndex])[roadLen + 2] = &vehicle;
        (*allBounds[nextIndex])[roadLen + 3] = &vehicle;
        (*allBounds[nextIndex])[roadLen + 4] = &vehicle;    

        // Vehicle in its own original bound
        for (int i = roadLen; i > (roadLen - vehicleLength + 3); i--) {
            (*allBounds[orrIndex])[i] = &vehicle;
        }

        // No condition required here as we know it's a truck
        // Changing vehicle's Start Index
        vehicle.setFrontIndex(roadLen + 4);

        // Recalculating vehicle's front and back index (not updated at all during transition phase, so +4)
        vehicle.setBackIndex(roadLen);
        vehicle.setTransition(false);

        // Change the value of the original direction of the vehicle for truck
        vehicle.setDirection(directions[nextIndex]);
    }
}

/*
 * Moves the vehicles that are turning left through the intersection in phases
 * depending on the type (length) of the vehicle (similar to moveTransition())
 * @param Vehicle& vehicle
 * @param vector<vector<VehicleBase*>*>& allBounds used to calculate the next bound for the
 * vehicle (the one it's transitioning into)
 */
void Simulator::moveTransitionLeft(Vehicle& vehicle, vector<vector<VehicleBase*>*>& allBounds) {
    int orrIndex;
    int nextIndex;
    int vehicleLength;

    vector<Direction> directions = {Direction::north, Direction::west, Direction::south, Direction::east};

    // Getting indexes of current direction, then finding the next direction from it.
    orrIndex = distance(directions.begin(), find(directions.begin(), directions.end(), vehicle.getVehicleOriginalDirection()));
    nextIndex = (orrIndex + 1) % 4;
    vehicleLength = vehicle.getLength();

    // First Phase of Transition for all vehicles (only transition phase for car)
    if (vehicle.getFrontIndex() == roadLen) {
        // Vehicle in the transitioning bound
        (*allBounds[nextIndex])[roadLen + 1] = &vehicle;

        // Vehicle in its own original bound
        for (int i = roadLen; i > (roadLen - vehicleLength + 1); i--) {
            (*allBounds[orrIndex])[i] = &vehicle;
        }

        // Changing vehicle's Start Index as it changed after making the turn
        vehicle.setFrontIndex(roadLen + 1);

    
        //if vehicle is car, inTransition will be off from here
        if (vehicle.getVehicleType() == VehicleType::car) {
            // Transition phase is over
            vehicle.setTransition(false);
            
            // Recompute the vehicle index as they have moved two steps forward
            vehicle.setBackIndex(roadLen - 1);
            
            // New currDirection will be set
            vehicle.setDirection(directions[nextIndex]);
        }


        // Second Phase of Transition for all vehicles 
    } else if (vehicle.getFrontIndex() == roadLen + 1) {
        // Vehicle in the transitioning bound
        (*allBounds[nextIndex])[roadLen + 2] = &vehicle;
        (*allBounds[nextIndex])[roadLen + 1] = &vehicle;

        // Vehicle in its own original bound
        for (int i = roadLen; i > (roadLen - vehicleLength + 2); i--) {
            (*allBounds[orrIndex])[i] = &vehicle;
        }

        // Changing vehicle's Start Index
        vehicle.setFrontIndex(roadLen + 2);
        
        // If vehicle is SUV, inTransition will be off from here
        if (vehicle.getVehicleType() == VehicleType::suv) {
            // Transition phase is over
            vehicle.setTransition(false);

            // Recompute the vehicle index as they have moved two steps forward 
            // Realize the vehicle index has not been modified during the transition phase
            vehicle.setBackIndex(roadLen - 1);

            //Change the value of the original direction of the vehicle for suv
            vehicle.setDirection(directions[nextIndex]);
        }

        // Third Phase of Transition (only possible for Trucks)   
    } else if (vehicle.getFrontIndex() == roadLen + 2){ //can be made an else statement
        // Vehicle in the transitioning bound
        (*allBounds[nextIndex])[roadLen + 1] = &vehicle;
        (*allBounds[nextIndex])[roadLen + 2] = &vehicle;
        (*allBounds[nextIndex])[roadLen + 3] = &vehicle;
        

        // Vehicle in its own original bound
        for (int i = roadLen; i > (roadLen - vehicleLength + 3); i--) {
            (*allBounds[orrIndex])[i] = &vehicle;
        }

        // No condition required here as we know it's a truck
        // Changing vehicle's Start Index
        vehicle.setFrontIndex(roadLen + 3);
        vehicle.setBackIndex(roadLen - 1);
        vehicle.setTransition(false);
        
        // Change the value of the original direction of the vehicle for truck
        vehicle.setDirection(directions[nextIndex]);
    }
}

#endif