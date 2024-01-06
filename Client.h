#ifndef CLIENT_INCLUDE_H
#define CLIENT_INCLUDE_H
#include <string.h>
#include "SocketFrame.h"
class Client{
    SOCKET mSocket;
    char mName[MAX_BYTE];//用户名
public:
    Client(/* args */);
    ~Client();
    void CreateClient(const char *serverIp);
    
    void RecvFrom(void *__buf, size_t __n);
    void SendTo(const void *__buf, size_t __n);

    inline const char *GetName(){
        return mName;
    }
    inline void SetScoket(SOCKET s){
        mSocket = s;
    }
    inline void SetName(const char *name){
        memcpy(mName, name, MAX_BYTE);
    }
    inline void Shutdown(){
        shutdown(mSocket, SHUT_RDWR);
    }
};
#endif