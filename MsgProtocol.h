#include <string>

enum MsgType {
    MT_Connect, 
    MT_Conn_ACK,
    MT_Disconnect,
    MT_Disc_ACK,
    MT_Publish,
    MT_Subscribe
};

struct msgPacket {
    MsgType type;
    std::string topic;
    std::string msg;
    bool retain;
}