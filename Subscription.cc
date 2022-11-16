#include "Subscription.h"

/*************************Subscription*************************/
Subscription::Subscription(int clientConFd, std::vector<std::string> topic){
    clientSocketFd = clientConFd;
    this->topic = topic;
}

int Subscription::getClient(){
    return clientSocketFd;
}

std::vector<std::string> Subscription::getTopic(){
    return topic;
}

bool Subscription::matchesTopic(std::vector<std::string> checkTopic){
    //if the topic provided is shorter than the subscription topic
    //it is automaticaly false as only the subscription topic can have 
    //wildcards
    if(checkTopic.size() < topic.size()){  
        return false;
    }

    //Check if every topic level in the sub's topic matches the checked 
    //topic's corresponding level or is a wildcard. If # is reached 
    //everything up to it has matched and so is automatically true
    //When a plus is reached it doesn't matter if that level matches
    for(int i = 0; i < topic.size(); i++){
        if(topic.at(i) == "#"){
            return true;
        }
        if(topic.at(i) != "+" && topic.at(i) != checkTopic.at(i)){
            return false;
        }
    }

    //At this point every level in topic has matched the corresponding 
    //level in checkTopic and there is no # wildcard so it's only a true 
    //match if the number of levels matches between topic and checkTopic
    return (checkTopic.size() == topic.size());
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
std::vector<Subscription*> SubscriptionManager::getSubByTopic(std::vector<std::string> topic){
    std::vector<Subscription*> subs; //Array to hold all the subscriptions

    //loop through all subscriptions, keeping track of topic matches
    Subscription* current = NULL;
    for(int i = 0; i < subscriptions.size(); i++){
        current = subscriptions.at(i);
        if(current->matchesTopic(topic)){
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
void SubscriptionManager::addSubscription(int clientConFd, std::vector<std::string>  topic){
    std::vector<Subscription*> clientSubs = SubscriptionManager::getSubByClient(clientConFd);
    
    //loop through all the clients subscriptions, if a subscription to the topic is found exit
    Subscription* current = NULL;
    for(int i = 0; i < clientSubs.size(); i++){
        current = clientSubs.at(i);
        if(current->matchesTopic(topic)){
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
void SubscriptionManager::removeSubscription(int clientConFd, std::vector<std::string> topic){

    //loop through all subscriptions, if a subscription to the client and topic is found delete it
    Subscription* current = NULL;
    for(int i = 0; i < subscriptions.size(); i++){
        current = subscriptions.at(i);
        if(current->getClient() == clientConFd && current->matchesTopic(topic)){
            delete current;
            subscriptions.erase(subscriptions.begin() + i);
            i--;
        }
    }
}

// std::string SubscriptionManager::printSubs(){
//     std::string print = "";
//     Subscription* current = NULL;
//     for(int i = 0; i < subscriptions.size(); i++){
//         current = subscriptions.at(i);
        
//         print += std::to_string(current->getClient()) + ": " + current->getTopic()->getName() + "\n";
//     }
//     return print;
// }