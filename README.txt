CMSC 240 — Final Project

Kritim Rijal and Tolya Evdokimov

DESIGN DECISIONS

Lanes are organized into 4 vectors of VehicleBase* which point to 
Vehicle objects in another vector that contains all vehicles in the
simulation. During each tick, a loop goes thorugh all vehicles in the
simulation and adjusts the indices (back and front) of each Vehicle
based on whether it can move forward in the simulation. When vehicles
leave the simulation, they are not deleted from the vehicles vector but
are removed from the displayed bounds.

### Left and Right Turns

We had a chance to implement both left and right turns. The logic
of turning works the same for both left and right, but what is different
is which lane the vehicle "jumps" to.

Once the path in the intersections is clear, the inTransition attribute
is set to true, which means that a Vehicle began transitioning bounds.
Based on the original bound, next bound, and vehicle type, the moveTransition()
or moveTransitionLeft() methods will move vehicles in stages based on the vehicle length.
After vehicles are done jumping lanes, inTransition is set to false.

### Resolving the intersection priority

clearPathTransition() method takes care of clearing the path for the
vehicle ready to transition. It contains counters for each of the 4 middle
sections in the intersection. A vehicle has to check that the counters of 
the sections that it is going to occupy during the transition are 0 before starting 
transitioning. When it starts transitioning, the section counters it's occupying 
are set to its length. As the vehicle leaves the intersection, the counters are 
decreasing until they reach 0 (vehicle fully left the section).

COMPILING THE CODE

To compile the code, run "make" command in the terminal. This will create
a RunSimulation executable. To remove object files and executables, run "make clean".

RUNNING THE CODE

Once compiled, run

./RunSimulation [input file name] [seed]


