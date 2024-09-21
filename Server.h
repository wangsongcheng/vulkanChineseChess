#ifndef SERVER_INCLUDE_H
#define SERVER_INCLUDE_H
#include <array>
#include "Client.h"
#include "SocketFrame.h"
class Server{
    SOCKET mSocket;
    char mName[MAX_BYTE];
    std::array<Client, 4>mClients;
    void SendSelfInfoation(uint32_t clientIndex);
    void SendPlayerNameInfoation(uint32_t count, const std::array<Player, 3>&players);
public:
    Server(/* args */);
    ~Server();
    void CreateServer(int listenCount);
    void SendToAllClient(const void *__buf, size_t __n);
    SOCKET AcceptClient(uint32_t client, void *__buf, size_t __n);
    void RecvFromClient(uint32_t client, void *__buf, size_t __n);
    void SendToClient(uint32_t client, const void *__buf, size_t __n);
    // SOCKET AcceptClient(uint32_t count, std::array<Player, 3>&players);

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
    inline void ShutdownClient(uint32_t client){
        mClients[client].Shutdown();
    }
};
#endif