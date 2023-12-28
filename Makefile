EXECS = RunSimulation
OBJS = Simulator.o Animator.o VehicleBase.o Vehicle.o RunSimulation.o

#### use next two lines for Mac
#CC = clang++
#CCFLAGS = -std=gnu++2a -Wall

#### use next two lines for mathcs* machines:
CC = g++
CCFLAGS = -std=c++17 -Wall

all: $(EXECS)

RunSimulation: $(OBJS)
	$(CC) $(CCFLAGS) $^ -o $@

%.o: %.cpp *.h
	$(CC) $(CCFLAGS) -c $<

%.o: %.cpp
	$(CC) $(CCFLAGS) -c $<

clean:
	/bin/rm -f a.out $(OBJS) $(EXECS)
