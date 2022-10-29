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

#define PORT 4212
#define QUEUE_LEN 15
#define MSGSIZE 128

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

int main(){
    // TopicManager topicMgr;    
    // SubscriptionManager subMgr;

    int serverFd; 
    int tempClientFd;
    std::vector<int> clientFd; //ToDo: consider only using subscriptions to manage these.

    struct sockaddr_in serverAddr;
    struct sockaddr_in clientAddr;

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

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(PORT);
    
    //Bind server to localhost
    if(bind(serverFd, (struct sockaddr*) &serverAddr, sizeof(serverAddr)) == -1){
        perror("Bind error");
        exit(-1);
    }

    if(listen(serverFd, QUEUE_LEN) == -1){
        perror("Listen Error");
        exit(-1);
    }

    std::cout << "Start accepting client connections:\n";
    socklen_t clientAddrSize = sizeof(clientAddr);

    MsgPacket msgPck;
    msgPck.type = MT_Conn_ACK;
    msgPck.retain = true;
    strcpy(msgPck.topic,"Test");

    for(int i =0; i<3; i++){
        tempClientFd = accept(serverFd, (struct sockaddr*) &clientAddr, &clientAddrSize);
        if(tempClientFd == -1){
            perror("Problem accepting connection");
            continue; //Jump to next loop and wait for next connection
        }

        strcpy(msgPck.msg, "imessaghhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhe");

        std::cout<<"Connection made: "<<inet_ntoa(clientAddr.sin_addr)<<"\n";
        
        int wrote = write(tempClientFd, &msgPck, sizeof(MsgPacket));
        if(wrote == -1){perror("Write Failed");}
        
        clientFd.push_back(tempClientFd);
        std::cout << "Wrote: "<< wrote<<" of: "<<sizeof(msgPck)<<"\n";
    }

    msgPck.type = MT_Disconnect;
    msgPck.retain = false;
     strcpy(msgPck.topic, "");
     strcpy(msgPck.msg, "bye");

    for(int i = 0; i<clientFd.size(); i++){
       if(write(clientFd.at(i), &msgPck, sizeof(MsgPacket)) == -1){perror("Write Failed");}
        close(clientFd.at(i));
    }

    std::cout << "Goodbyes sent.\n";

    close(serverFd);

}
