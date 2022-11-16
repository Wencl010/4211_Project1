#ifndef SUBSCRIPTION_H_
#define SUBSCRIPTION_H_

#include <vector>
#include <string>

/**
 * Represents an individual subscription of a client to a single topic
 */
class Subscription{
    private:
        int clientSocketFd; //file descriptor of the socket the client is on
        std::vector<std::string> topic;

    public:
        Subscription(int clientConFd, std::vector<std::string> topic);
        int getClient();
        std::vector<std::string> getTopic();
        bool matchesTopic(std::vector<std::string> checkTopic);
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
        std::vector<Subscription*> getSubByTopic(std::vector<std::string>  topic);

        void addSubscription(int clientConFd, std::vector<std::string> topic);
        void removeSubscription(int clientConFd, std::vector<std::string> topic);

        //std::string printSubs(); //TODO: delete
};


#endif