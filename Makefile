CXX=g++
CXXFLAGS = -std=c++11 -g

all: build server

server: build serverMain.o Topic.o Subscription.o
	$(CXX) $(CXXFLAGS) buildFiles/server/serverMain.o buildFiles/server/Topic.o buildFiles/server/Subscription.o -o server.o 
serverMain.o: Server.cc
	$(CXX) $(CXXFLAGS) -c Server.cc -o buildFiles/server/serverMain.o
Topic.o: Topic.cc Topic.h
	$(CXX) $(CXXFLAGS) -c Topic.cc -o buildFiles/server/Topic.o
Subscription.o: Subscription.cc Subscription.h
	$(CXX) $(CXXFLAGS) -c Subscription.cc -o buildFiles/server/Subscription.o


build:
	mkdir -p buildFiles
	mkdir -p buildFiles/server
	mkdir -p buildFiles/client

clean:
	rm -rf buildFiles
	rm *.o 

runServer: 
	./server.o