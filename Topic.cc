#include "Topic.h"

/*************************Topic*************************/

/**
 * Construct a new Topic
 * @param topicName the name for the new topic
 */
Topic::Topic(std::string topicName){
    name = topicName;
    retainMsg = "";
}

/**
 * Delete all the subtopics to this topic
*/
Topic::~Topic(){ 
    for(int i = 0; i < subTopics.size(); i++){
        delete subTopics.at(i);
    }
}

/**
 * Accessor method for the topic's name
 * @return std::string 
 */
std::string Topic::getName(){
    return name;
}

/**
 * Accessor method for the topic's retained message
 * @return std::string 
 */
std::string Topic::getRetainMsg(){
    return retainMsg;
}

/**
 * Set the topic's retained message 
 * @param msg the message to retain
 */
void Topic::setRetainMsg(std::string msg){
    retainMsg = msg;
}

/**
 * Adds a new subtopic to this topic
 * @param topic a pointer to the topic to add
 */
void Topic::addSubTopic(Topic* topic){
    subTopics.push_back(topic);
    //ToDo: Check topic hasn't been made yet
} 

/**
 * Search for a given topic name among a topic's subtopics. 
 * Returns null if none are found
 * @param name the name of the topic to find
 * @return A Topic* to the sub-topic with the given name
 */
Topic* Topic::getSubTopic(std::string name){
    Topic* current = NULL;
    for(int i = 0; i < subTopics.size(); i++){
        current = subTopics.at(i);
        if(name.compare(current->getName()) == 0){
            return current;
        }
    }
    return NULL;
}


/*************************Topic Manager*************************/
TopicManager::TopicManager(){ //TODO: Remove Constructor for phase 2
    rootTopics.push_back(new Topic("WEATHER"));
    rootTopics.push_back(new Topic("NEWS"));
    rootTopics.push_back(new Topic("HEALTH"));
    rootTopics.push_back(new Topic("SECURITY"));

}

/**
 * Delete all the root topics in the manager
*/
TopicManager::~TopicManager(){
    for(int i = 0; i < rootTopics.size(); i++){
        delete rootTopics.at(i);
    }
}

/**
 * Splits the provided path on slashes. Then checks for errors such 
 * as wildcard characters. Handles fetching the topic from the provide 
 * path.
 * @param topicPath the path to the topic
 * @return A Topic pointer or null if the topic doesn't exist
 */
Topic* TopicManager::getTopic(std::string topicPath){
    //TODO: Implement Path Error Checking
    //TODO: Implement Sub Level Spliting

    Topic* current = NULL;
    for(int i = 0; i < rootTopics.size(); i++){
        current = rootTopics.at(i);
        if(topicPath.compare(current->getName()) == 0){
            return current;
        }
    }
    return NULL;
}

/**
 * Splits the provided path on slashes. Then checks for errors such 
 * as wildcard characters. Handles fetching the topic at the provided
 * subpath
 *
 * @param topicPath the path to the new topic
 * @return A pointer to the new topic
 */
Topic* TopicManager::createTopic(std::string topicPath){
    //TODO: Implement Path Error Checking
    //TODO: Implement Sub Level Spliting
    //TODO: Implement name checking, don't create path if it exists already

    Topic* newTopic = new Topic(topicPath);
    rootTopics.push_back(newTopic);
    return newTopic;
}
