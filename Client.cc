#include <iostream>
#include <unistd.h>
#include <string>
#include <cstring>
#include <signal.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "MsgProtocol.h"

int connectToServer();
int disconnectFromServer();
void listenOnServer();
void endListen(int sig);

#define PORT 4211

struct sigaction action; 
int serverFd;
bool listening = false;


int main(){
    serverFd = connectToServer();
    std::cout<<"Connection Open:\n\n";

    std::string userInput;

    bool run = true;
    while(run){
        std::cout << "\nOptions: \nL - listen for messages in subscriptions\nS - subscribe to messages\nP - publish a message\nQ - quit\n";
        std::cout << "Enter choice: ";

        std::cin >> userInput;

        std::cout << "\n";

        if(userInput == "L"){
            listenOnServer();
        }
        else if(userInput == "S"){
            
        }
        else if(userInput == "P"){
            
        }
        else if(userInput == "Q"){
            run = false;
        }
        else{
            std::cout << "Invalid option\n";
        }



    }


    disconnectFromServer();

    std::cout<<"Connection Closed\n";
    exit(1);
}



/**
 * Sets up the connection between client and server, then sends the connect msg
 * and waits for the serves response before proceeding.
 * 
 * @return the server socket's file descriptor 
 */
int connectToServer(){
    //Open Socket File Descriptor
    int socketFd = socket(PF_INET, SOCK_STREAM, 0);
    if(socketFd == -1){
        perror("Socket error");
        exit(-1);
    }

    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(PORT);

    if(connect(socketFd, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) == -1){
        perror("Connection Error");
        exit(-1);
    }


    //exchange connect messages with server
    MsgPacket connectMsg; 
    connectMsg.type = MT_Connect;
    if(write(socketFd, &connectMsg, sizeof(MsgPacket))==-1){perror("Server Connection Failed"); exit(1);}

    //wait for response form server
    MsgPacket response; 
    if(read(socketFd, &response, sizeof(MsgPacket))==-1){perror("Server Ack Failed"); exit(1);}
    //process response from server
    if(response.type != MT_Conn_ACK){
        perror("Server Failed to acknowledge connection.");
        exit(-1);
    }

    printMsg(response);

    return socketFd;
}
 
/**
 * Ends connection with server with proper disconnect messages
 * 
 * @param serverFd the server to disconnect from
 * @return 1 if successfully disconnected 0 otherwise 
 */
int disconnectFromServer(){
    //exchange connect messages with server
    MsgPacket disconnectMsg; 
    disconnectMsg.type = MT_Disconnect;
    if(write(serverFd, &disconnectMsg, sizeof(MsgPacket))==-1){perror("Server Disconnect Failed"); return -1;}

    //wait for response from server
    MsgPacket response; 
    if(read(serverFd, &response, sizeof(MsgPacket))==-1){perror("Server Ack Failed"); return -1;}
    //process response from server
    if(response.type != MT_Disc_ACK){
        perror("Server Failed to acknowledge disconnect.");
        return -1;
    }

    printMsg(response);
    return 1;
}

void listenOnServer(){
    //Set "ctrl + c" interrupt to return to exit listening mode
    action.sa_handler = endListen;
    sigemptyset(&action.sa_mask);
    sigaddset(&action.sa_mask, SIGINT);
    action.sa_flags = 0;
    sigaction(SIGINT, &action, NULL);

    listening = true;

    MsgPacket serverMsg;
    while(listening){
        std::cout << "Waiting for messages...\n";
        int readSize = read(serverFd, &serverMsg, sizeof(MsgPacket));
        if(readSize == -1){perror("Write Failed");}
        std::cout << "Message Received: \n\n";


        std::cout << "Topic: " << serverMsg.topic << "\n";
        std::cout << "Message: " << serverMsg.msg << "\n\n";
    }

    //Restore "ctrl + C" to normal functionality
    action.sa_handler = SIG_DFL;
    sigemptyset(&action.sa_mask);
    sigaddset(&action.sa_mask, SIGINT);
    action.sa_flags = 0;
    sigaction(SIGINT, &action, NULL);
}

void endListen(int sig){
    listening = false;
}
