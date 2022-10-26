#include <iostream>
#include "Topic.h"
#include "Subscription.h"

int main(){
    TopicManager topicMgr;    
    SubscriptionManager subMgr;

    std::cout << topicMgr.getTopic("WEATHER")->getName()<<"\n";
    subMgr.getSubByClient(0);
}