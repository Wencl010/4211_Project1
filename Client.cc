#include <iostream>
#include <unistd.h> 

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "MsgProtocol.h"

#define PORT 4212

int main(){
    int msgSize; 
    MsgPacket msgPck;
    MsgPacket msgPck2;


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
    std::cout<<"Connection Open:\n\n";

    msgSize = read(socketFd, &msgPck, MSG_PACKET_SIZE);
    if(msgSize == -1){
        perror("Read Error");
        exit(-1);
    }

    std::cout << msgSize <<"\n";
    printMsg(msgPck);
    std::cout <<"\n";


    msgSize = read(socketFd, &msgPck2, MSG_PACKET_SIZE);
    if(msgSize == -1){
        perror("Read Error");
        exit(-1);
    }
    std::cout << msgSize <<"\n";
    printMsg(msgPck2);
    close(socketFd);

    std::cout<<"Connection Closed\n";
}