#ifndef MSG_PROTOCOL_H
#define MSG_PROTOCOL_H


#include <string>

enum MsgType {
    MT_Connect, 
    MT_Conn_ACK,
    MT_Disconnect,
    MT_Disc_ACK,
    MT_Publish,
    MT_Subscribe
};

struct MsgPacket {
    MsgType type;
    char topic[70];
    char msg[948];
    bool retain;
};

const int MSG_PACKET_SIZE = sizeof(MsgPacket);



std::string typeToString(MsgType x){
    switch(x){
        case MT_Connect: return "Connect";
        case MT_Conn_ACK: return "Acknowledge Connect";
        case MT_Disconnect: return "Disconnect";
        case MT_Disc_ACK: return "Acknowledge Disconnect";
        case MT_Publish: return "Publish";
        case MT_Subscribe: return "Subscribe";
        default: return "not found";
    }
}
void printMsg(MsgPacket msg){
    std::cout << "Type: " << typeToString(msg.type) <<"\n";
    std::cout << "Retain: " << msg.retain<<"\n";
    std::cout << "Topic: " << msg.topic<<"\n";
    std::cout << "Msg: " << msg.msg<<"\n";
}

#endif
