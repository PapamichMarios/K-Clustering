CPP      = g++
OBJ      = cluster.o utilities.o
BIN      = cluster
CFLAGS   = -Wall -g -std=c++11

$(BIN): $(OBJ)
	$(CPP) -o $(BIN) $(OBJ) $(CFLAGS)

cluster.o: cluster.cpp
	$(CPP) -c cluster.cpp $(CFLAGS)

utilities.o: utilities.cpp
	$(CPP) -c utilities.cpp $(CFLAGS)

PHONY: clean

clean: 
	rm -f $(OBJ) $(BIN) 
