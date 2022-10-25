#ifndef TOPIC_H
#define TOPIC_H

#include <string>
#include <vector>


/**
 * Class to represent an individual topic and keep track 
 * of the sub topics one level below it
 */
class Topic{
    private:
        std::string name;
        std::string retainMsg;
        std::vector<Topic*> subTopics;

    public:
        Topic(std::string topicName);
        ~Topic(); // delete subtopics in the vector

        std::string getName();

        std::string getRetainMsg();
        void setRetainMsg(std::string msg);

        void addSubTopic(Topic* topic); //adds a topic to the subtopic vector
        Topic* getSubTopic(std::string name);
};

/**
 * Manages the top level topics, handles retrieval and creation 
 * of topics and subtopics, and error checks provided paths
 */
class TopicManager{
    private:
        std::vector<Topic*> rootTopics;
    
    public:
        ~TopicManager();
        Topic* getTopic(std::string topicPath);
        Topic* createTopic(std::string topicPath);
};



#endif