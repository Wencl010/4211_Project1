#include <iostream>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>

#define PORT 4211
#define MAX_MSG 128

int main(){
    int msgSize; 
    char msg[MAX_MSG];


    //Open Socket File Descriptor
    int socketFd = socket(PF_INET, SOCK_STREAM, 0);
    if(serverFd == -1){
        perror("Socket error");
        exit(-1);
    }

    struct sockaddr serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(PORT);

    if(connect(socketFd, &serverAddr, sizeof(serverAddr)) == -1){
        perror("Connection Error");
        exit(-1);
    }
    std::cout<<"Connection Open:\n\n";

    if(msgSize = read(socketFd, msg, MAX_MSG-1) == -1){
        perror("Read Error");
        exit(-1);
    }
    msg[msgSize] = '\0';
    std::cout << msg;

    if(msgSize = read(socketFd, msg, MAX_MSG-1) == -1){
        perror("Read Error");
        exit(-1);
    }
    msg[msgSize] = '\0';
    std::cout << msg;
    close(socketFd);

    std::cout<<"Connection Closed";
}