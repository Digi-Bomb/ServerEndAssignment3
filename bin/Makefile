CXX			= g++

EXE			= Server

SRC 		= ../src
INCLUDE 	= ../include

FLAGS 		= -std=c++23 -I$(INCLUDE) -L../lib
CXXFLAGS 	= -c -Wall $(FLAGS)
LDFLAGS 	= $(FLAGS)

OBJECTS 	= main.o SocketManager.o FileManager.o

all: $(OBJECTS)
	$(CXX) $(LDFLAGS) *.o -o $(EXE)

main.o: $(SRC)/main.cpp $(INCLUDE)/main.h
	$(CXX) $(CXXFLAGS) $<

SocketManager.o: $(SRC)/SocketManager.cpp $(INCLUDE)/SocketManager.h
	$(CXX) $(CXXFLAGS) $<

FileManager.o: $(SRC)/FileManager.cpp $(INCLUDE)/FileManager.h
	$(CXX) $(CXXFLAGS) $<

clean:
	rm *.o $(EXE)
