#include <iostream>
#include <vector>
#include <string> 

#include "Topic.h"
#include "Subscription.h"

std::string printSubs(std::vector<Subscription*> subs, bool printTopics){
    std::string print = "# ";
    Subscription* current = NULL;
    for(int i = 0; i < subs.size(); i++){
        current = subs.at(i);
        
        if(printTopics){
            print += current->getTopic()->getName() + ", ";

        }
        else{
            print += std::to_string(current->getClient()) + ", ";
        }
    }
    return print;
}

int main(){
    TopicManager topicMgr;    
    SubscriptionManager subMgr;

    Topic* weather = topicMgr.getTopic("WEATHER");
    Topic* news = topicMgr.getTopic("NEWS");
    Topic* security = topicMgr.getTopic("SECURITY");
    Topic* health = topicMgr.getTopic("HEALTH");
 
    std::cout << weather->getName()<<"\n";
    std::cout << news->getName()<<"\n";
    std::cout << security->getName()<<"\n";
    std::cout << health->getName()<<"\n";



    subMgr.addSubscription(1, security);
    subMgr.addSubscription(1, security);
    subMgr.addSubscription(1, health);

    subMgr.addSubscription(2, health);
    subMgr.addSubscription(2, news);


    subMgr.addSubscription(4, news);
    subMgr.addSubscription(4, security);
    subMgr.addSubscription(4, health);


    std::cout << printSubs(subMgr.getSubByClient(1), true) <<"\n";

    std::cout << printSubs(subMgr.getSubByClient(2), true) <<"\n";

    std::cout << printSubs(subMgr.getSubByClient(3), true) <<"\n";

    std::cout << printSubs(subMgr.getSubByClient(4), true) <<"\n";

    std::cout << printSubs(subMgr.getSubByClient(5), true) <<"\n";


    std::cout << printSubs(subMgr.getSubByTopic(news), false) <<"\n";
    std::cout << printSubs(subMgr.getSubByTopic(weather), false) <<"\n";
    std::cout << printSubs(subMgr.getSubByTopic(security), false) <<"\n";
    std::cout << printSubs(subMgr.getSubByTopic(health), false) <<"\n";

}
