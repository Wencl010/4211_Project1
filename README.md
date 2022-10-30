# 4211_Project1
Billy Wencl  
5544600  
Wencl010@umn.edu  
  
  
## Build
To compile the whole project at once use: 
```bash
    cd /path/to/repo/project
    make
 ```
  
  
  
To compile/recompile only the server files use:
```bash
    cd /path/to/repo/project
    make server
```
  
  
  
To compile/recompile only the client files use:
```bash
    cd /path/to/repo/project
    make client
```



## Run
To start the server:
```bash
    make run_server
```



To start the client:
```bash
    make run_client
```
## Summary
This project is an implementation of a message board server. Clients can publish messages to and subscribe to various topics.  The server stores the current subscriptions and distributes published messages to all subscribers. 

## Current Limitations
- Topic paths must be less than 70 characters in length
- Messages must be less than 948 characters in length

## Implmenataion
#### Basic Comunication:
This project was implemented with C++ socketprogramming. A messaging format was defined in MsgProtocl.h as seen below.
```c++
struct MsgPacket {
    MsgType type;
    char topic[70];
    char msg[948];
    bool retain;
};
```
This struct is the sole data format passed between client and server. The MsgType enumerator is used to identify what type of message is being passed. For example: MT_Connect, MT_Conn_ACK, MT_Disconnect, MT_Subscribe... Once the server or client checks the MsgType it knows which fields of the struct to interact with.  
  
#### Client Interface:

