#include "Server.h"
void Server::SendAllInfoation(uint32_t count){
    GameMessage message;
    message.event = JOINS_GAME_GAMEEVENT;
    for (uint32_t uiClient = 0; uiClient < count; ++uiClient){
        message.index = uiClient;
        strcpy(message.name, GetClientName(uiClient));
        SendToAllClient(&message, sizeof(message));
    }
}
void Server::SendSelfInfoation(uint32_t clientIndex){
    GameMessage message;
    message.event = SELF_PLAYER_INFORMATION_GAMEEVENT;
    message.index = clientIndex;
    SendToClient(clientIndex, &message, sizeof(message));
}
Server::Server(/* args */){
}

Server::~Server(){
}

void Server::CreateServer(int listenCount){
    mSocket = socket(AF_INET, SOCK_STREAM, 0);
    if(mSocket == -1){
        perror("create server:function:socket");
        // herror("create server:function:socket");
        return;
    }
    struct sockaddr_in my_addr;
    my_addr.sin_family = AF_INET; /* host byte order */
    my_addr.sin_addr.s_addr = INADDR_ANY; /* auto-fill with my IP */
    my_addr.sin_port = htons(INTERNET_PORT); /* short, network byte order */
#ifdef WIN32
    ZeroMemory(&my_addr.sin_zero, sizeof(my_addr.sin_zero)); /* zero the rest of the struct */
#else
    bzero(&my_addr.sin_zero, sizeof(my_addr.sin_zero)); /* zero the rest of the struct */
#endif
    if(bind(mSocket, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) == -1){
        perror("create server:function:bind");
        // herror("create server:function:bind");
        return;
    }
    if (listen(mSocket, listenCount) == -1){
        perror("create server:function:listen");
        // herror("create server:function:listen");
        return;
    }
}
void Server::AcceptClient(uint32_t count, uint32_t&ai){
    GameMessage message;
    struct sockaddr_in client_addr;
    socklen_t size = sizeof(client_addr);
    for (size_t i = 0; i < count; i++){
        SOCKET s = accept(mSocket, (struct sockaddr *)&client_addr, &size);
        if(s != INVALID_SOCKET){
            mClients[i].SetScoket(s);
            //客户端连接后会发送自身信息
            mClients[i].RecvFrom(&message, sizeof(message));
            if(message.event == AI_JOIN_GAME_GAMEEVENT){
                ai = i;
                mClients[i].SetName("ai");
                mClients[i].Shutdown();
                mClients[i].SetScoket(INVALID_SOCKET);
            }
            else{
                mClients[i].SetName(message.name);

                message.index = i;
                SendSelfInfoation(i);
            }
            SendAllInfoation(i + 1);
        }
        else{
            perror("process_server:accept");
            // shutdown(mClientsockets[i], SHUT_RDWR);
        }
    }
}

void Server::SendToAllClient(const void *__buf, size_t __n){
    for (auto it = mClients.begin(); it != mClients.end(); ++it){
        it->SendTo(__buf, __n);
    }
}

void Server::RecvFromClient(uint32_t client, void *__buf, size_t __n){
    mClients[client].RecvFrom(__buf, __n);
}

void Server::SendToClient(uint32_t client, const void *__buf, size_t __n){
    mClients[client].SendTo(__buf, __n);
}
