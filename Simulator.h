#ifndef __SIMULATOR_H__
#define __SIMULATOR_H__

#include <iostream>
#include <vector>
#include <tuple>
#include "Animator.h"
#include "Vehicle.h"
#include "VehicleBase.h"

using namespace std;

class Simulator{
    private:
        int seed;
        int simTime;
        int roadLen;
        int greenNS;
        int yellowNS;
        int greenEW;
        int yellowEW;
        int lightCycle;
        double probNB;
        double probSB;
        double probEB;
        double probWB;
        double proportionCars;
        double proportionSUVs;
        double proportionCarRight;
        double proportionCarLeft;
        double proportionSUVRight;
        double proportionSUVLeft;
        double proportionTruckRight;
        double proportionTruckLeft;


        vector<VehicleBase*> westbound;
        vector<VehicleBase*> eastbound;
        vector<VehicleBase*> southbound;
        vector<VehicleBase*> northbound;

        vector<Vehicle> vehicles;

    public:
        Simulator(string file, int seed);
        void runSimulation();
        void setLights(int i, Animator& anim, string& lightNSState, int& NSTimeToRed, string& lightEWState, int& EWTimeToRed);
        void moveStraight(Vehicle& vehicle);
        void printVehicle(Vehicle& vehicle);
        bool clearPath(Vehicle& vehicle);
        bool clearPathTransition(Vehicle& vehicle, int& NESec, int& NWSec, int& SESec, int& SWSec);
        void moveTransition(Vehicle& vehicle, vector<vector<VehicleBase*>*>& allBounds);
        bool checkLight(Vehicle& vehicle, string& lightNSState, string& LightEWState);
        bool checkMove(Vehicle& vehicle, int NSTimeToRed, int EWTimeToRed);
        void addVehicle(vector<VehicleBase*>& bound, Direction direction, double inputLaneProb, double spawnProb, double typeProb, double turnProb);
        void moveTransitionLeft(Vehicle& vehicle, vector<vector<VehicleBase*>*>& allBounds);
};

#endif