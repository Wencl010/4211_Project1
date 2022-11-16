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
int disconnectFromServer(int serverFd);
void listenOnServer(int serverFd);
void endListen(int sig);
void subscribeToTopic(int serverFd);
void publishMessage(int serverFd);

#define PORT 4211
 
bool listening = false;


int main(){
    int serverFd = connectToServer(); //open serverFD, exits the program upon error

    /* Runs main UI Menu*/
    std::string userInput;

    bool run = true;
    while(run){
        std::cout << "\n------------------------------------------------\n";
        std::cout << "Options: \nL - listen for messages in subscriptions\nS - subscribe to messages\nP - publish a message\nQ - quit\n";
        std::cout << "------------------------------------------------\n";
        std::cout << "Enter choice: ";

        getline(std::cin, userInput);
        std::cout << "\n\n\n\n\n\n";
        
        if(userInput == "L"){
            listenOnServer(serverFd);
        }
        else if(userInput == "S"){
            subscribeToTopic(serverFd);
        }
        else if(userInput == "P"){
            publishMessage(serverFd);
        }
        else if(userInput == "Q"){
            run = false;
        }
        else{
            std::cout << "Invalid option\n";
        }
    }

    /* End of execution cleanup*/
    disconnectFromServer(serverFd);
    return 0;
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
    std::cout << "Server socket open\n";


    //exchange connect messages with server
    MsgPacket connectMsg = initMsgPacket(); 
    connectMsg.type = MT_Connect;
    if(write(socketFd, &connectMsg, sizeof(MsgPacket))==-1){perror("Server Connection Failed"); exit(-1);}
    std::cout << "Connection request sent\n";

    //wait for response form server
    MsgPacket response = initMsgPacket(); 
    if(read(socketFd, &response, sizeof(MsgPacket))==-1){perror("Server Ack Failed"); exit(-1);}
    //process response from server
    if(response.type == MT_Conn_ACK){
        std::cout << "Connection acknowledged\n";
    }
    else{
        perror("Server Failed to acknowledge connection.");
        exit(-1); 
    }

    return socketFd;
}
 

/**
 * Ends connection with server with proper disconnect messages
 * 
 * @param serverFd the server to disconnect from
 * @return 0 if successfully disconnected 1 otherwise 
 */
int disconnectFromServer(int serverFd){
    //exchange connect messages with server
    MsgPacket disconnectMsg = initMsgPacket(); 
    disconnectMsg.type = MT_Disconnect;
    if(write(serverFd, &disconnectMsg, sizeof(MsgPacket))==-1){perror("Server Disconnect Failed"); return -1;}
    std::cout << "Disconnect request sent\n";

    //wait for response from server
    MsgPacket response = initMsgPacket(); 
    if(read(serverFd, &response, sizeof(MsgPacket))==-1){perror("Server Ack Failed"); return -1;}
    //process response from server
    if(response.type == MT_Disc_ACK){
        std::cout << "Disconnect acknowledge\n";
    }
    else{
        perror("Server Failed to acknowledge disconnect.");
        return -1;
    }

    close(serverFd);
    std::cout<<"Server closed\n";
    return 0;
}

/**
 * Read From the server socket and print out any publish messages received. 
 * Will Listen until a keyboard interrupt of "ctrl + C" is received
 */
void listenOnServer(int serverFd){
    //Set "ctrl + c" interrupt to return to the options menu
    struct sigaction action;
    action.sa_handler = endListen;
    sigemptyset(&action.sa_mask);
    sigaddset(&action.sa_mask, SIGINT);
    action.sa_flags = 0;
    sigaction(SIGINT, &action, NULL);

    listening = true; //changed to false by the interrupt handler on ^C

    MsgPacket serverMsg = initMsgPacket();
    while(listening){
        std::cout << "Waiting for messages... Use ^C to return to menu\n";

        //Wait for message from server
        int readSize = read(serverFd, &serverMsg, sizeof(MsgPacket));
        if(readSize == -1){perror("Read Failed");}

        if(serverMsg.type == MT_Publish){ //Check if the message is a published message, print the message if it is
            std::cout << "Message Received: \n\n";

            std::cout << "Topic: " << serverMsg.topic << "\n";
            std::cout << "Message: " << serverMsg.msg << "\n\n";
        }
    }

    //Restore "ctrl + C" to normal functionality of ending program
    action.sa_handler = SIG_DFL;
    sigemptyset(&action.sa_mask);
    sigaddset(&action.sa_mask, SIGINT);
    action.sa_flags = 0;
    sigaction(SIGINT, &action, NULL);
}

/**
 * Signal handler that sets the global listening to false to end the listeningLoop 
 * and return to the menu
 */
void endListen(int sig){
    listening = false;
}

void subscribeToTopic(int serverFd){
    std::string userInput;
    MsgPacket sub = initMsgPacket();

    sub.type = MT_Subscribe;
    

    bool inputIsCorrect = false;
    do {
        std::cout << "Enter Topic: ";
        getline(std::cin, userInput);

        if(userInput.length() < 70){
            inputIsCorrect = true;
            strcpy(sub.topic, userInput.c_str());
        }
        else{
            std::cout << "\nTopic must be less than 70 characters.\n";
        }
    } while(!inputIsCorrect);

    if(write(serverFd, &sub, sizeof(MsgPacket))==-1){perror("Write Failed");}

    std::cout << "\nSubscription sent to server.\n";

    MsgPacket response = initMsgPacket();
    if(read(serverFd, &response, sizeof(MsgPacket)) == -1){perror("Read Failed");}

    if(response.type == MT_Success){
        std::cout << response.topic << "\n";
        
        //TODO: Implement Retain
    }
    else{
        std::cout << "Error: " << response.topic << "\n";
    }
}

void publishMessage(int serverFd){
    std::string userInput;
    MsgPacket pub = initMsgPacket();
    pub.type = MT_Publish;

    bool inputIsCorrect = false;
    do {
        std::cout << "Enter Topic: ";
        getline(std::cin, userInput);

        if(userInput.length() < 70){
            inputIsCorrect = true;
            strcpy(pub.topic, userInput.c_str());
        }
        else{
            std::cout << "\nTopic must be less than 70 characters.\n";
        }
    } while(!inputIsCorrect);

    std::cout<<"\n";

    inputIsCorrect = false;
    do {
        std::cout << "Enter Message: ";
        getline(std::cin, userInput);

        if(userInput.length() < 948){
            inputIsCorrect = true;
            strcpy(pub.msg, userInput.c_str());
        }
        else{
            std::cout << "\nMessage must be less than 948 characters.\n";
        }
    } while(!inputIsCorrect);

    //TODO: Implement Retain
    inputIsCorrect = false;
    do {
        std::cout << "Should this be set as the retained(Y or N): ";
        getline(std::cin, userInput);

        if(userInput == "Y"){
            inputIsCorrect = true;
            pub.retain = true;
        }
        else if(userInput == "N"){
            inputIsCorrect = true;
            pub.retain = false;
        }
        else{
            std::cout << "\nPlease Enter Y or N.\n";
        }
    } while(!inputIsCorrect);


    if(write(serverFd, &pub, sizeof(MsgPacket))==-1){perror("Write Failed");}

    std::cout << "\nPublish request sent to server.\n";

    MsgPacket response = initMsgPacket();
    if(read(serverFd, &response, sizeof(MsgPacket)) == -1){perror("Read Failed");}

    if(response.type == MT_Success){
        std::cout << response.topic << "\n";
        
        //TODO: Implement Retain
    }
    else{
        std::cout << "Error: " << response.topic << "\n";
    }

}
