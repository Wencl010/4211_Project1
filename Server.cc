#include <iostream>
#include <vector>
#include <unistd.h> 
#include <cstring>
#include <thread>
#include <mutex>
#include <signal.h>
#include <string>



//socket includes
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>

#include "RetainMSG.h"
#include "Subscription.h"
#include "MsgProtocol.h"


#define PORT 4211
#define QUEUE_LEN 15

RetainManager* retainMgr; 
SubscriptionManager* subMgr;
std::mutex* subLock;
std::mutex* topicLock;


int serverFd;


/**
 * Splits the provided path into it's individual path names and checks
 * for path errors.
 * 
 * @return a vector of path names, with postion 0 being the outermost name. If invalid path NULL
 */
std::vector<std::string> splitPath(std::string path){
    std::vector<std::string> pathVec;

    //check for path errors
    if(path.at(path.length()-1) == '/'){
        return pathVec;
    }
    if(path.length() > 1){
        int symbol = path.find("#");
        if(symbol != -1){
            if(symbol != (path.length()-1) || path.at(symbol-1) != '/'){
                return pathVec;
            }
        }
        symbol = path.find("+");
        while(symbol != -1){
            if(path.at(symbol-1) != '/'){
                return pathVec;
            }
            symbol = path.find("+",symbol+1);
        }
    }
    else if(path.length() == 0){
        return pathVec;
    }



    int start = 0;
    int end = path.find("/"); 

    while(end != -1){
        pathVec.push_back(path.substr(start, end-start));
        start = end+1;
        end = path.find('/',start);
    }   

    pathVec.push_back(path.substr(start));

    return pathVec;
}


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
 * Send given error msg to the client.
 * @param socketFd the socket to send the message to
 * @param errorMsg error message to send
 */
void sendError(int socketFd, std::string errorMsg){
    MsgPacket errorPck = initMsgPacket(); 
    errorPck.type = MT_Error;
    strcpy(errorPck.topic, errorMsg.c_str());
    if(write(socketFd, &errorPck, sizeof(MsgPacket))==-1){perror("Message Transmit Failed"); return;}
}

/**
 * Send given success msg to the client.
 * @param socketFd the socket to send the message to
 * @param errorMsg error message to send
 */
void sendSuccess(int socketFd, std::string successMsg){
    MsgPacket successPck = initMsgPacket(); 
    successPck.type = MT_Success;
    strcpy(successPck.topic, successMsg.c_str());
    if(write(socketFd, &successPck, sizeof(MsgPacket))==-1){perror("Message Transmit Failed"); return;}
}

/**
 * Subscribe client to topic
 * @param socketFd the client's socket
 * @param response the MsgPacket containing the topic
 */
void subscribe(int socketFd, MsgPacket response){
    std::string respTopic = response.topic;

    std::vector<std::string> topicPath = splitPath(respTopic);
    if(topicPath.size() == 0){
        sendError(socketFd, "Invalid Topic String\n");
        std::cout << socketFd <<": Sub Error - Invalid Topic String\n";
        return;
    }
    
    subLock->lock();
    subMgr->addSubscription(socketFd, topicPath);
    subLock->unlock();

    std::string topicMsg =  retainMgr->getRetainMsg(topicPath);

    if(topicMsg != ""){
        sendSuccess(socketFd, "Successfully subscribed. Topic has the following message waiting:\n" + topicMsg + "\n");
    }
    else{
        sendSuccess(socketFd, "Successfully subscribed\n");
    }
    std::cout << socketFd <<": Subscribed to "<< respTopic <<"\n";
    return;
}


/**
  * Publishes a message out to all subscribers of the topic
  * @param socketFd the publisher client's socket
  * @param response the MsgPacket containing the topic
  */
void publish(int socketFd, MsgPacket response){
    std::string respTopic = response.topic;

    if(respTopic.find('#') != -1 || respTopic.find('+') != -1){
        sendError(socketFd, "Invalid Topic String\n");
        std::cout << socketFd <<": Sub Error - Invalid Topic String\n";
        return;
    }

    std::vector<std::string> topicPath = splitPath(respTopic);

    if(response.retain){
        std::string respMsg = response.msg;
        topicLock->lock();
        retainMgr->setRetainMsg(respMsg, topicPath);
        topicLock->unlock();
    }

    //read only, doesn't need a lock to access subscriptionManager. A client subscribe at the exact time the message is sent can be safely ignored as a minor inconvenience
    std::vector<Subscription*> topicSubs = subMgr->getSubByTopic(topicPath);

    Subscription* current = NULL;
    for(int i = 0; i < topicSubs.size(); i++){
        current = topicSubs.at(i);
        if(write(current->getClient(), &response, sizeof(MsgPacket))==-1){perror("Message Transmit Failed"); return;}
    }

    sendSuccess(socketFd, "Successfully Published\n");
    std::cout << socketFd <<": Published to "<< respTopic <<"\n";
}


/**
 * Process a response and take the appropriate action such as add the client as 
 * a subscriber to a topic, publishing a topic, or disconnecting a client
 * 
 * @param socketFd a file descriptor of the socket that the response was from
 * @param response a MsgOacket object that holds the response details
 * @return 0 if the port is functioning properly. 1 if the fd has been closed or there is a port error
 */
int handleResponse(int socketFd, MsgPacket response){
    if(response.type == MT_Connect){
        MsgPacket msgPck = initMsgPacket();
        msgPck.type = MT_Conn_ACK;
        if(write(socketFd, &msgPck, sizeof(MsgPacket))==-1){perror("Write Failed"); return -1;}
        std::cout << socketFd <<": Connection Acknowledged\n";
        return 0;
    }
    else if(response.type == MT_Disconnect){
        MsgPacket msgPck = initMsgPacket();
        msgPck.type = MT_Disc_ACK;
        if(write(socketFd, &msgPck, sizeof(MsgPacket))==-1){perror("Write Failed"); return -1;}

        //TODO: Delete all subscriptions

        close(socketFd);
        std::cout << socketFd <<": Disconnected\n";
        return -1;
    }
    else if(response.type == MT_Publish){
        publish(socketFd, response);
        return 0;

    }
    else if(response.type == MT_Subscribe){
        subscribe(socketFd, response);
        return 0;
    }
    else{
        perror("Unknown Response");
        return -1;
    }
    
}


/**
 * Handles the read and response cycle for a single client
 * 
 * @param clientFd the file descriptor of the socket to handle
 */
void handleClient(int clientFd){
    MsgPacket response = initMsgPacket();
    int open = 0;
    while(open == 0){
        int readSize = read(clientFd, &response, sizeof(MsgPacket));
        if(readSize == -1){perror("Write Failed");}

        open = handleResponse(clientFd, response);
    }
}

/**
 * Handles cleanup of server variables
 */
void shutDownServer(int sig){
    std::cout << "Shutting Down.\n";

    close(serverFd);
    delete subMgr;
    delete retainMgr;
    delete subLock;
    delete topicLock;

    exit(0);
}

int main(){
    //Set "ctrl + c" interrupt to gracefully exit with mem deallocation
    struct sigaction action;
    action.sa_handler = shutDownServer;
    sigemptyset(&action.sa_mask);
    sigaddset(&action.sa_mask, SIGINT);
    action.sa_flags = 0;
    sigaction(SIGINT, &action, NULL);


    subMgr = new SubscriptionManager;
    retainMgr = new RetainManager;
    topicLock = new std::mutex;
    subLock = new std::mutex;

    serverFd = startServerSocket();

    std::cout << "Start accepting client connections:\n";
    struct sockaddr_in clientAddr;
    socklen_t clientAddrSize = sizeof(clientAddr);
    int tempClientFd;

    while(1){
        tempClientFd = accept(serverFd, (struct sockaddr*) &clientAddr, &clientAddrSize);
        if(tempClientFd == -1){
            perror("Problem accepting connection");
            continue; //Jump to next loop and wait for next connection
        }

        std::cout << tempClientFd <<": Socket opened to "<<inet_ntoa(clientAddr.sin_addr)<<"\n";
        
        std::thread clientThread(handleClient, tempClientFd);
        clientThread.detach(); //Todo: Figure out closing sockets when server closes.
        //ToDo: handle broken pipe by client 
        //Maybe use select and create thread that handle each individual response?
    }

    shutDownServer(0);
}
