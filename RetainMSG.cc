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

