#include <iostream>
#include <vector>
#include <unistd.h> 
#include "MsgProtocol.h"
#include <cstring>


//socket includes
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>

#include "Topic.h"
#include "Subscription.h"

#define PORT 4211
#define QUEUE_LEN 15

// std::string printSubs(std::vector<Subscription*> subs, bool printTopics){
//     std::string print = "# ";
//     Subscription* current = NULL;
//     for(int i = 0; i < subs.size(); i++){
//         current = subs.at(i);
        
//         if(printTopics){
//             print += current->getTopic()->getName() + ", ";

//         }
//         else{
//             print += std::to_string(current->getClient()) + ", ";
//         }
//     }
//     return print;
// }


/**
 * Handles the opening and setup of the sockot the server will operate on and
 * starts the server listening for connections
 * @return the filedescriptor to the server's socket
 */
int startServerSocket(){
    //setup info for the port the server should run on
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(PORT);

    int serverFd; 
    //Open Socket File Descriptor
    serverFd = socket(PF_INET, SOCK_STREAM, 0);
    if(serverFd == -1){
        perror("Socket error");
        exit(-1);
    }

    //Set Socket options to allow port reuse
    int enable = 1;
    if(setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) == -1){
        perror("Set reuse errors");
        exit(-1);
    }
    
    //Bind server to localhost
    if(bind(serverFd, (struct sockaddr*) &serverAddr, sizeof(serverAddr)) == -1){
        perror("Bind error");
        exit(-1);
    }

    //Set server to listen for connections
    if(listen(serverFd, QUEUE_LEN) == -1){
        perror("Listen Error");
        exit(-1);
    }

    return serverFd;
}


/**
 * Process a response and take the appropriate action such as add the client as 
 * a subscriber to a topic, publishing a topic, or disconnecting a client
 * 
 * @param socketFd a file descriptor of the socket that the response was from
 * @param response a MsgOacket object that holds the response details
 * @return 1 if the port is functioning properly. 0 if the fd has been closed or there is a port error
 */
int handleResponse(int socketFd, MsgPacket response){
    printMsg(response);

    if(response.type == MT_Connect){
        MsgPacket msgPck;
        msgPck.type = MT_Conn_ACK;
        if(write(socketFd, &msgPck, sizeof(MsgPacket))==-1){perror("Write Failed"); return -1;}
        return 1;
    }
    else if(response.type == MT_Disconnect){
        MsgPacket msgPck;
        msgPck.type = MT_Disc_ACK;
        if(write(socketFd, &msgPck, sizeof(MsgPacket))==-1){perror("Write Failed"); return -1;}

//TODO: Delete all subscriptions

        close(socketFd);
        return -1;
    }
    else if(response.type == MT_Publish){

    }
    else if(response.type == MT_Subscribe){

    }

    perror("Unknown Response");
    return -1;
    
}


/**
 * Handles the read and response cycle for a single client
 * 
 * @param clientFd the file descriptor of the socket to handle
 */
void handleClient(int clientFd){
    MsgPacket response;
    int open = 1;
    while(open == 1){
        int readSize = read(clientFd, &response, sizeof(MsgPacket));
        if(readSize == -1){perror("Write Failed");}

        open = handleResponse(clientFd, response);

    }
}

int main(){
    // TopicManager topicMgr;    
    // SubscriptionManager subMgr;

    int serverFd; 
    int tempClientFd;
    std::vector<int> clientFd; //ToDo: consider only using subscriptions to manage these.

    serverFd = startServerSocket();


    std::cout << "Start accepting client connections:\n";

    struct sockaddr_in clientAddr;
    socklen_t clientAddrSize = sizeof(clientAddr);

    MsgPacket msgPck;

    for(int i =0; i<1; i++){
        tempClientFd = accept(serverFd, (struct sockaddr*) &clientAddr, &clientAddrSize);
        if(tempClientFd == -1){
            perror("Problem accepting connection");
            continue; //Jump to next loop and wait for next connection
        }

        std::cout<<"Connection made: "<<inet_ntoa(clientAddr.sin_addr)<<"\n";
        
        //handleClient(tempClientFd);
        //clientFd.push_back(tempClientFd);
        int readSize = read(tempClientFd, &msgPck, sizeof(MsgPacket));
        if(readSize == -1){perror("Write Failed");}

        handleResponse(tempClientFd, msgPck);

        msgPck.type = MT_Publish;
        strcpy(msgPck.topic, "Test");
        strcpy(msgPck.msg, "Hello World");

        write(tempClientFd, &msgPck, sizeof(MsgPacket));

        sleep(10);

        msgPck.type = MT_Disc_ACK;
        strcpy(msgPck.topic, "Test2");
        strcpy(msgPck.msg, "Hello World");

        write(tempClientFd, &msgPck, sizeof(MsgPacket));

        sleep(10);

        msgPck.type = MT_Publish;
        strcpy(msgPck.topic, "Test3");
        strcpy(msgPck.msg, "Hello World");

        write(tempClientFd, &msgPck, sizeof(MsgPacket));

        readSize = read(tempClientFd, &msgPck, sizeof(MsgPacket));
        if(readSize == -1){perror("Write Failed");}

        handleResponse(tempClientFd, msgPck);

    }


    std::cout << "Shutting Down.\n";
    close(serverFd);

}
