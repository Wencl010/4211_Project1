#include "RetainMSG.h"

/*************************Topic*************************/

/**
 * Construct a new Topic
 * @param topicName the name for the new topic
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

bool RetainMSG::matchesTopic(std::vector<std::string> checkTopic){
    //if the topic provided does not have the same number of levels
    //it cannot be a match
    if(checkTopic.size() != name.size()){  
        return false;
    }

    //Check if every topic level in the sub's topic matches the checked 
    //topic's corresponding level
    for(int i = 0; i < name.size(); i++){
        if(name.at(i) != checkTopic.at(i)){
            return false;
        }
    }

    return true;
}

RetainManager::~RetainManager(){
    for(int i = 0; i < messages.size(); i++){
        delete messages.at(i);
    }
}

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

std::string  RetainManager::getRetainMsg(std::vector<std::string> topic){
    RetainMSG* current = NULL;
    for(int i = 0; i < messages.size(); i++){
        current = messages.at(i);
        if(current->matchesTopic(topic)){
            return current->getRetainMsg();
        }
    }
    //no match found
    return "";
}