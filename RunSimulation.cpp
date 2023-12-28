#include "Simulator.h"

using namespace std;

int main(int argc, char* argv[]) {

    // Checking for appropriate inputs
    if (3 != argc) {
        cerr << "Invalid number of arguments. Required: 3" << endl;
        cerr << "Usage: ./RunSimulation [file_name] [seed]" << endl;
        exit(0);
    }

    // Running the simulation class
    Simulator sim = Simulator(argv[1], stoi(argv[2]));
    sim.runSimulation();
}