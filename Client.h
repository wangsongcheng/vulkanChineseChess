#ifndef CLIENT_INCLUDE_H
#define CLIENT_INCLUDE_H
#include <cstring>
#include <iostream>
#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <WS2tcpip.h>
#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")
#define SHUT_RDWR 2
#else
#include <netdb.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/socket.h>
typedef unsigned long long SOCKET;
#endif
#ifndef MAX_BYTE
#define MAX_BYTE 0xff
#endif
#define INVALID_SOCKET 10000
#define INTERNET_PORT 10086
#define INVALID_PLAYER_INDEX 10
class Client{
    SOCKET mSocket;
    void Recv(SOCKET __fd, void *__buf, size_t __n, int __flags)const;
    void Send(SOCKET __fd, const void *__buf, size_t __n, int __flags)const;
public:
    Client(/* args */);
    ~Client();
    bool CreateClient(const char *serverIp);
    
    void RecvFrom(void *__buf, size_t __n)const;
    void SendTo(const void *__buf, size_t __n)const;

    inline void SetScoket(SOCKET s){
        mSocket = s;
    }
    inline void Shutdown(){
        shutdown(mSocket, SHUT_RDWR);
        mSocket = INVALID_SOCKET;
    }
};
#endif