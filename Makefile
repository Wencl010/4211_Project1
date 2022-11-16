CXX=g++
CXXFLAGS = -std=c++11 -g

all: build server client

server: build serverMain.o RetainMSG.o Subscription.o
	$(CXX) $(CXXFLAGS) -pthread buildFiles/server/serverMain.o buildFiles/server/RetainMSG.o buildFiles/server/Subscription.o -o server.o 
serverMain.o: Server.cc
	$(CXX) $(CXXFLAGS) -pthread -c Server.cc -o buildFiles/server/serverMain.o
RetainMSG.o: RetainMSG.cc RetainMSG.h
	$(CXX) $(CXXFLAGS) -c RetainMSG.cc -o buildFiles/server/RetainMSG.o
Subscription.o: Subscription.cc Subscription.h
	$(CXX) $(CXXFLAGS) -c Subscription.cc -o buildFiles/server/Subscription.o

client: build
	$(CXX) $(CXXFLAGS) Client.cc -o client.o 

build:
	mkdir -p buildFiles
	mkdir -p buildFiles/server
	mkdir -p buildFiles/client

clean:
	rm -rf buildFiles
	rm *.o 

run_server: 
	./server.o

run_client: 
	./client.o