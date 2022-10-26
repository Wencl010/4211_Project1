#include "Subscription.h"

/*************************Subscription*************************/
Subscription::Subscription(int clientConFd, Topic* topic){
    clientSocketFd = clientConFd;
    this->topic = topic;
}

int Subscription::getClient(){
    return clientSocketFd;
}

Topic* Subscription::getTopic(){
    return topic;
}

/*************************Subscription Manager*************************/
