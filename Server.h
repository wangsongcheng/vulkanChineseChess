#ifndef SERVER_INCLUDE_H
#define SERVER_INCLUDE_H
#include <vector>
#include "Client.h"
class Server{
    SOCKET mSocket;
    char mName[MAX_BYTE];
    std::vector<Client>mClients;
public:
    Server(/* args */);
    ~Server();
    bool CreateServer(int listenCount);
    SOCKET AcceptClient(uint32_t client);
    void SendToAllClient(const void *__buf, size_t __n)const;
    void RecvFromClient(uint32_t client, void *__buf, size_t __n)const;
    void SendToClient(uint32_t client, const void *__buf, size_t __n)const;

    inline auto GetClientInfo()const{
        return mClients;
    }
    inline const char *GetName()const{
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