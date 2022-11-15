#include "Topic.h"
#include <iostream>

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
    if(getSubTopic(topic->getName()) == NULL){
        subTopics.push_back(topic);
    }
} 

/**
 * Search for a given topic name among a topic's subtopics. 
 * Returns null if none are found
 * @param name the name of the topic to find
 * @return A Topic* to the sub-topic with the given name
 */
Topic* Topic::getSubTopic(std::string name){
    std::cout<<"start\n";
    Topic* current = NULL;
    for(int j = 0; j < subTopics.size(); j++){
        std::cout<<"j: "<<j<<" topics: "<<subTopics.size()<<"\n";

        current = subTopics.at(j);
        if(name.compare(current->getName()) == 0){
            return current;
        }
    }
    return NULL;
}


/*************************Topic Manager*************************/
TopicManager::TopicManager(){}

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
    //Split path into individual levels
    std::vector<std::string> topicNames = splitPath(topicPath);
    if(topicNames.size() == 0){
        return NULL;
    }

    //Get the root topic level
    Topic* root = NULL;
    for(int i = 0; i < rootTopics.size(); i++){
        root = rootTopics.at(i);
        if((topicNames.at(0)).compare(root->getName()) == 0){
            break;
        }
        root = NULL;
    }
    if(root == NULL){return NULL;}//Topic Root not found

    //Looking through sublevels
    Topic* current = root;
    for(int i = 1; i < topicNames.size(); i++){
        std::cout<<"116\n";
        current = current->getSubTopic(topicNames.at(i));
        if(current == NULL){return NULL;}
    }

    return current;
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

    Topic* existing = getTopic(topicPath);
    if(existing != NULL){
        return existing;
    }

    //Split path into individual levels
    std::vector<std::string> topicNames = splitPath(topicPath);
    if(topicNames.size() == 0){
        return NULL;
    }

    Topic* newTopic = NULL;
    //Get the root topic level
    for(int i = 0; i < rootTopics.size(); i++){
        newTopic = rootTopics.at(i);
        if((topicNames.at(0)).compare(newTopic->getName()) == 0){
            break;
        }
        newTopic = NULL;
    }
    if(newTopic == NULL){
        //create root topic level if it doesn't exist
        newTopic = new Topic(topicNames.at(0));
        rootTopics.push_back(newTopic);
    }

    //Looking through sublevels
    for(int i = 1; i < topicNames.size(); i++){
        Topic* checkTopic = NULL;
        checkTopic = newTopic->getSubTopic(topicNames.at(i));

        if(checkTopic == NULL){
            Topic* temp = new Topic(topicNames.at(i));
            newTopic->addSubTopic(temp);
            newTopic = temp;

        }
        else{
            newTopic = checkTopic;
        }
    }

    return newTopic;
}

/**
 * Splits the provided path into it's individual path names and checks
 * for path errors.
 * 
 * @return a vector of path names, with postion 0 being the outermost name. If invalid path NULL
 */
std::vector<std::string> TopicManager::splitPath(std::string path){
    std::vector<std::string> pathVec;

    //check for path errors
    if(path.at(path.length()-1) == '/'){
        return pathVec;
    }
    if(path.length() > 1){
        int symbol = path.find("#");
        if(symbol != -1){
            if(symbol != (path.length()-1) || path.at(symbol-1) != '/'){
                return pathVec;
            }
        }
        symbol = path.find("+");
        while(symbol != -1){
            if(path.at(symbol-1) != '/'){
                return pathVec;
            }
            symbol = path.find("+",symbol+1);
        }
    }
    else if(path.length() == 0){
        return pathVec;
    }



    int start = 0;
    int end = path.find("/"); 

    while(end != -1){
        pathVec.push_back(path.substr(start, end-start));
        start = end+1;
        end = path.find('/',start);
    }   

    pathVec.push_back(path.substr(start));

    return pathVec;
}
