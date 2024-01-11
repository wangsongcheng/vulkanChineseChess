#ifndef CLIENT_INCLUDE_H
#define CLIENT_INCLUDE_H
#include <string.h>
#include "SocketFrame.h"
class Client{
    SOCKET mSocket;
public:
    Client(/* args */);
    ~Client();
    void CreateClient(const char *serverIp);
    
    void RecvFrom(void *__buf, size_t __n);
    void SendTo(const void *__buf, size_t __n);

    inline void SetScoket(SOCKET s){
        mSocket = s;
    }
    inline void Shutdown(){
        shutdown(mSocket, SHUT_RDWR);
        mSocket = INVALID_SOCKET;
    }
};
#endif