#include <iostream>
#include <vector>
#include <unistd.h> 


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
    TopicManager topicMgr;    
    SubscriptionManager subMgr;

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

    std::cout << "start accepting connections:\n\n";
    socklen_t clientAddrSize = sizeof(clientAddr);

    char msg[] = "Hello World";

    for(int i =0; i<5; i++){
        tempClientFd = accept(serverFd, (struct sockaddr*) &clientAddr, &clientAddrSize);
        if(tempClientFd == -1){
            perror("Problem accepting connection");
            continue; //Jump to next loop and wait for next connection
        }

        std::cout<< i << ") Connection made: "<<inet_ntoa(clientAddr.sin_addr)<<"\n";
        int wrote = write(tempClientFd, msg, sizeof(msg));
        if(wrote == -1){perror("Write Failed");}
        
        clientFd.push_back(tempClientFd);
        std::cout << "Wrote: "<< wrote<<"\n";
    }
    std::cout << "stop accepting connections.\n";

    char msg2[] = "Goodbye";
    for(int i = 0; i<clientFd.size(); i++){
       if(write(clientFd.at(i), msg2, sizeof(msg2)) == -1){perror("Write Failed");}
        close(clientFd.at(i));
    }

    std::cout << "Goodbyes sent.\n";

    close(serverFd);

}
