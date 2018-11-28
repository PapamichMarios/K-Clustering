CPP      = g++
OBJ      = cluster.o initialization.o assignment.o update.o utilities.o hyperplane.o fi.o g.o
BIN      = cluster
CFLAGS   = -g -std=c++11 -O2

$(BIN): $(OBJ)
	$(CPP) -o $(BIN) $(OBJ) $(CFLAGS)

cluster.o: cluster.cpp
	$(CPP) -c cluster.cpp $(CFLAGS)

initialization.o: initialization.cpp
	$(CPP) -c initialization.cpp $(CFLAGS)

assignment.o: assignment.cpp
	$(CPP) -c assignment.cpp $(CFLAGS)

update.o: update.cpp
	$(CPP) -c update.cpp $(CFLAGS)

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
