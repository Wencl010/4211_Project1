#include <iostream>
#include <unistd.h> 

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 4212
#define MAX_MSG 128

int main(){
    int msgSize; 
    char msg[1024];

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

    msgSize = read(socketFd, msg, MAX_MSG - 1);
    if(msgSize == -1){
        perror("Read Error");
        exit(-1);
    }
    msg[msgSize] = '\0';
    std::cout << msg <<"\n";

    msgSize = read(socketFd, msg, MAX_MSG - 1);
    if(msgSize == -1){
        perror("Read Error");
        exit(-1);
    }
    msg[msgSize] = '\0';
    std::cout << msg<<"\n";

    close(socketFd);

    std::cout<<"Connection Closed\n";
}