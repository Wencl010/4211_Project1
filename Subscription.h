#ifndef SUBSCRIPTION_H_
#define SUBSCRIPTION_H_

#include "Topic.h"
#include <vector>

/**
 * Represents an individual subscription of a client to a single topic
 */
class Subscription{
    private:
        int clientSocketFd; //file descriptor of the socket the client is on
        Topic* topic;

    public:
        Subscription(int clientConFd, Topic* topic);
        int getClient();
        Topic* getTopic();
};

/**
 * Maintains a list of active subscriptions and provides 
 * methods for accessing them
 */
class SubscriptionManager{
    private:
        std::vector<Subscription*> subscriptions;

    public:
        ~SubscriptionManager(); //Deallocate remaining Subscriptions
        
        std::vector<Subscription*> getSubByClient(int clientConFd);
        std::vector<Subscription*> getSubByTopic(Topic* topic);

        void addSubscription(int clientConFd, Topic* topic);
        void removeSubscription(int clientConFd, Topic* topic);

        std::string printSubs(); //TODO: delete
};


#endif