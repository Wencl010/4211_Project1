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
/**
 * Delete all the remaining subscriptions
*/
SubscriptionManager::~SubscriptionManager(){
    for(int i = 0; i < subscriptions.size(); i++){
        delete subscriptions.at(i);
    }
}

/**
 * Create and return a vector of all the subscriptions a particular client has
 * @param clientConFd the client to find subscriptions for 
 * @return std::vector<Subscription*> a vector of the clients subscriptions
 */
std::vector<Subscription*> SubscriptionManager::getSubByClient(int clientConFd){
    std::vector<Subscription*> subs; //Array to hold all the subscriptions

    //loop through all subscriptions, keeping track of client matches
    Subscription* current = NULL;
    for(int i = 0; i < subscriptions.size(); i++){
        current = subscriptions.at(i);
        if(current->getClient() == clientConFd){
            subs.push_back(current);
        }
    }
    return subs;
}

/**
 * Create and return a vector of all the subscriptions a particular topic has
 * @param topic the topic to find subscriptions for 
 * @return std::vector<Subscription*> a vector of the topics subscriptions
 */
std::vector<Subscription*> SubscriptionManager::getSubByTopic(Topic* topic){
    std::vector<Subscription*> subs; //Array to hold all the subscriptions

    //loop through all subscriptions, keeping track of topic matches
    Subscription* current = NULL;
    for(int i = 0; i < subscriptions.size(); i++){
        current = subscriptions.at(i);
        if(current->getTopic() == topic){
            subs.push_back(current);
        }
    }
    return subs;
}

/**
 * If a subscription doesn't already exist for a client and topic create it
 * @param clientConFd the client for the subscription
 * @param topic the topic for the subscription
 */
void SubscriptionManager::addSubscription(int clientConFd, Topic* topic){
    std::vector<Subscription*> clientSubs = SubscriptionManager::getSubByClient(clientConFd);
    
    //loop through all the clients subscriptions, if a subscription to the topic is found exit
    Subscription* current = NULL;
    for(int i = 0; i < clientSubs.size(); i++){
        current = clientSubs.at(i);
        if(current->getTopic() == topic){
           return;
        }
    }

    //no subscription found, create a new subscription for it
    subscriptions.push_back(new Subscription(clientConFd, topic));
}

/**
 * If a subscription exists for a client and topic delete it
 * @param clientConFd the client of the subscription to be deleted
 * @param topic the topic of the subscription to be deleted
 */
void SubscriptionManager::removeSubscription(int clientConFd, Topic* topic){

    //loop through all subscriptions, if a subscription to the client and topic is found delete it
    Subscription* current = NULL;
    for(int i = 0; i < subscriptions.size(); i++){
        current = subscriptions.at(i);
        if(current->getClient() == clientConFd && current->getTopic() == topic){
            delete current;
            subscriptions.erase(vec.begin() + i);
            i--;
        }
    }
}

std::string SubscriptionManager::printSubs(){
    std::string print = "";
    Subscription* current = NULL;
    for(int i = 0; i < subscriptions.size(); i++){
        current = subscriptions.at(i);
        
        print += std::to_string(current->getClient()) + ": " + current->getTopic()->getName() + "\n";
    }
    return print;
}