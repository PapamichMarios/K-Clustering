CPP      = g++
OBJ      = cluster.o utilities.o hyperplane.o fi.o g.o
BIN      = cluster
CFLAGS   = -Wall -g -std=c++11

$(BIN): $(OBJ)
	$(CPP) -o $(BIN) $(OBJ) $(CFLAGS)

cluster.o: cluster.cpp
	$(CPP) -c cluster.cpp $(CFLAGS)

utilities.o: utilities.cpp
	$(CPP) -c utilities.cpp $(CFLAGS)

hyperplane.o: hyperplane.cpp
	$(CPP) -c hyperplane.cpp $(CFLAGS)

fi.o: fi.cpp
	$(CPP) -c fi.cpp $(CFLAGS)

g.o: g.cpp
	$(CPP) -c g.cpp $(CFLAGS)

PHONY: clean

clean: 
	rm -f $(OBJ) $(BIN) 
