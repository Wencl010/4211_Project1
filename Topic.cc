#include "Topic.h"

/**
 * Construct a new Topic
 * @param topicName the name for the new topic
 */
Topic::Topic(std::string topicName){
    name = topicName;
}

/**
 * Delete all the subtopics to this topic
*/
Topic::~Topic(){ 
    // TODO: delete all subtopics in the vector
}

/**
 * Accessor method for the topic's name
 * @return std::string 
 */
std::string Topic::getName(){
    return name
}

/**
 * Accessor method for the topic's retained message
 * @return std::string 
 */
std::string Topic::getRetainMsg(){
    return retainMsg
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
} 

/**
 * Search for a given topic name among a topic's subtopics. 
 * Returns null if none are found
 * @param name the name of the topic to find
 * @return A Topic* to the sub-topic with the given name
 */
Topic* Topic::getSubTopic(std::string name){
    Topic* current = null;
    for(int i = 0; i < subTopics.size(), i++){
        current = subTopics.at(i);
        if(name.compare(current->getName()) == 0){
            return current;
        }
    }
    return null
}