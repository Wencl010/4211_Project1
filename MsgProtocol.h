#ifndef MSG_PROTOCOL_H
#define MSG_PROTOCOL_H


#include <cstring>

/**
 * Enumerator to communicate the purpose of the message
 */
enum MsgType {
    MT_NULL,
    MT_Connect, 
    MT_Conn_ACK,
    MT_Disconnect,
    MT_Disc_ACK,
    MT_Publish,
    MT_Subscribe,
    MT_Unsubscribe,
    MT_List,
    MT_List_Item,
    MT_Success,
    MT_Error
};

/**
 * Data struture to facilitate passing all pieces of the message
 */
struct MsgPacket {
    MsgType type;
    char topic[70];
    char msg[948];
    bool retain;
};

// //Helper functions for debugging transmission errors
// std::string typeToString(MsgType x){
//     switch(x){
//         case MT_Connect: return "Connect";
//         case MT_Conn_ACK: return "Acknowledge Connect";
//         case MT_Disconnect: return "Disconnect";
//         case MT_Disc_ACK: return "Acknowledge Disconnect";
//         case MT_Publish: return "Publish";
//         case MT_Subscribe: return "Subscribe";
//         default: return "not found";
//     }
// }
// void printMsg(MsgPacket msg){
//     std::cout << "\nType: " << typeToString(msg.type) <<"\n";
//     std::cout << "Retain: " << msg.retain<<"\n";
//     std::cout << "Topic: " << msg.topic<<"\n";
//     std::cout << "Msg: " << msg.msg<<"\n";
// }

/**
 * A brief function to create and initialize the values of a MsgPacket struct
 * 
 * @return MsgPacket a new initalized MsgPacket
 */
MsgPacket initMsgPacket(){
    MsgPacket newPck;
    newPck.type = MT_NULL;
    strcpy(newPck.topic, "");
    strcpy(newPck.msg, "");
    newPck.retain = false;
    return newPck;
}

#endif
