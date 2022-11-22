#ifndef RetainMSG_H
#define RetainMSG_H

#include <string>
#include <vector>


/**
 * Class to represent a topic that has a retained messages
 */
class RetainMSG{
    private:
        std::vector<std::string> name;

        std::string retainMsg; //the message that the server sends immediately upon subscription

    public:
        RetainMSG(std::vector<std::string> name, std::string retainMsg);

        std::vector<std::string> getName();
        std::string getRetainMsg();
        void setRetainMsg(std::string msg);
        bool matchesTopic(std::vector<std::string> checkTopic);

};

/**
 * A manager class to handle all the subclass retain msgs
 */
class RetainManager{
    private:
        std::vector<RetainMSG*> messages;
    
    public:
        ~RetainManager();
        void setRetainMsg(std::string message, std::vector<std::string> topic);
        std::vector<std::string> getRetainMsg(std::vector<std::string> topic);
};

#endif