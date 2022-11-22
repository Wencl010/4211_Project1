#include "RetainMSG.h"

/*************************RetainMSG*************************/

/**
 * Construct a new Topic
 * @param topicName the name for the new topic
 * @param retainMsg the msg the topic wil retain
 */
RetainMSG::RetainMSG(std::vector<std::string> topicName, std::string retainMsg){
    name = topicName;
    this->retainMsg = retainMsg;
}


/**
 * Accessor method for the topic's name
 * @return std::vector<std::string> 
 */
std::vector<std::string> RetainMSG::getName(){
    return name;
}

/**
 * Accessor method for the topic's retained message
 * @return std::string 
 */
std::string RetainMSG::getRetainMsg(){
    return retainMsg;
}

/**
 * Set the topic's retained message 
 * @param msg the message to retain
 */
void RetainMSG::setRetainMsg(std::string msg){
    retainMsg = msg;
}

/**
 * Checks if the argument topic name matches the internal retain name
 * 
 * @param checkTopic the RetainMSG that is being compared
 * @return true/false depending on if its a math or not
 */
bool RetainMSG::matchesTopic(std::vector<std::string> checkTopic){
     //if the checkTopic provided is shorter than the subscription checkTopic
    //it is automaticaly false as only the subscription checkTopic can have 
    //wildcards
    if(name.size() < checkTopic.size()){  
        return false;
    }

    //Check if every checkTopic level in the sub's checkTopic matches the checked 
    //checkTopic's corresponding level or is a wildcard. If # is reached 
    //everything up to it has matched and so is automatically true
    //When a plus is reached it doesn't matter if that level matches
    for(int i = 0; i < checkTopic.size(); i++){
        if(checkTopic.at(i) == "#"){
            return true;
        }
        if(checkTopic.at(i) != "+" && checkTopic.at(i) != name.at(i)){
            return false;
        }
    }

    //At this point every level in checkTopic has matched the corresponding 
    //level in name and there is no # wildcard so it's only a true 
    //match if the number of levels matches between checkTopic and name
    return (name.size() == checkTopic.size());
}

/**
 * Simple destructor that removes all messages stored in it
 */
RetainManager::~RetainManager(){
    for(int i = 0; i < messages.size(); i++){
        delete messages.at(i);
    }
}

/**
 * Sets the retain message of given topic
 * 
 * @param message the message to retain
 * @param topic the split out vector of topics
 */
void RetainManager::setRetainMsg(std::string message, std::vector<std::string> topic){
    RetainMSG* current = NULL;
    for(int i = 0; i < messages.size(); i++){
        current = messages.at(i);
        if(current->matchesTopic(topic)){
            current->setRetainMsg(message);
            return;
        }
    }
    //no match found
    messages.push_back(new RetainMSG(topic, message));
}

/**
 * Retrieve the retain message for a provided topic 
 * 
 * @param topic she topic that you want the retain message from
 * @return std::vector<std::string> The retain messages that match
 */
std::vector<std::string> RetainManager::getRetainMsg(std::vector<std::string> topic){
    std::vector<std::string> retainMsgs;
    RetainMSG* current = NULL;
    for(int i = 0; i < messages.size(); i++){
        current = messages.at(i);
        if(current->matchesTopic(topic)){
            retainMsgs.push_back(current->getRetainMsg());
        }
    }
    //no match found
    return retainMsgs;
}