CXX			= g++

EXE			= Server

SRC 		= ../src
INCLUDE 	= ../include

FLAGS 		= -std=c++23 -I$(INCLUDE) -L../lib
CXXFLAGS 	= -c -Wall $(FLAGS)
LDFLAGS 	= $(FLAGS)

OBJECTS 	= main.o SocketManager.o

all: $(OBJECTS)
	$(CXX) $(LDFLAGS) *.o -o $(EXE)

main.o: $(SRC)/main.cpp $(INCLUDE)/main.h
	$(CXX) $(CXXFLAGS) $<

SocketManager.o: $(SRC)/SocketManager.cpp $(INCLUDE)/SocketManager.h
	$(CXX) $(CXXFLAGS) $<

clean:
	rm *.o $(EXE)
