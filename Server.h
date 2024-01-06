#ifndef SERVER_INCLUDE_H
#define SERVER_INCLUDE_H
#include <array>
#include "Client.h"
#include "SocketFrame.h"
class Server{
    SOCKET mSocket;
    char mName[MAX_BYTE];
    std::array<Client, 3>mClients;
    void SendAllInfoation(uint32_t count);
    void SendSelfInfoation(uint32_t clientIndex);
public:
    Server(/* args */);
    ~Server();
    void CreateServer(int listenCount);
    void AcceptClient(uint32_t count, uint32_t&ai);
    void SendToAllClient(const void *__buf, size_t __n);
    void RecvFromClient(uint32_t client, void *__buf, size_t __n);
    void SendToClient(uint32_t client, const void *__buf, size_t __n);

    inline auto GetClientInfo(){
        return mClients;
    }
    inline const char *GetName(){
        return mName;
    }
    inline void ShutdownClient(){
        for (auto it = mClients.begin(); it != mClients.end(); ++it){
            it->Shutdown();
        }
    }
    inline void ShutdownServer(){
        shutdown(mSocket, SHUT_RDWR);
    }
    inline void SetName(const char *name){
        memcpy(mName, name, MAX_BYTE);
    }
    inline const char *GetClientName(uint32_t client){
        return mClients[client].GetName();
    }
    inline void SetClientName(uint32_t client, const char *name){
        mClients[client].SetName(name);
    }
};
#endif