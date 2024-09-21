#include "Server.h"
void Server::SendSelfInfoation(uint32_t clientIndex){
    GameMessage message;
    message.event = SELF_PLAYER_INFORMATION_GAME_EVENT;
    message.clientIndex = clientIndex;
    SendToClient(clientIndex, &message, sizeof(message));
}
void Server::SendPlayerNameInfoation(uint32_t count, const std::array<Player, 3>&players){
    GameMessage message;
    message.event = JOINS_GAME_GAME_EVENT;
    for (uint32_t uiClient = 0; uiClient < count; ++uiClient){
        message.clientIndex = uiClient;
        strcpy(message.player.name, players[uiClient].name);
        SendToAllClient(&message, sizeof(message));
    }
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

void Server::SendToAllClient(const void *__buf, size_t __n){
    for (auto it = mClients.begin(); it != mClients.end(); ++it){
        it->SendTo(__buf, __n);
    }
}
SOCKET Server::AcceptClient(uint32_t client, void *__buf, size_t __n){
    struct sockaddr_in client_addr;
    socklen_t size = sizeof(client_addr);
    SOCKET s = accept(mSocket, (struct sockaddr *)&client_addr, &size);
    mClients[client].SetScoket(s);
    mClients[client].RecvFrom(__buf, __n);
    GameMessage *pMessage = (GameMessage *)__buf;
    // printf("in function %s:recv client %d, event %d\n", __FUNCTION__, client, pMessage->event);
    if(pMessage->event != AI_JOIN_GAME_GAME_EVENT)SendSelfInfoation(client);
    return s;
}
void Server::RecvFromClient(uint32_t client, void *__buf, size_t __n){
    mClients[client].RecvFrom(__buf, __n);
}

void Server::SendToClient(uint32_t client, const void *__buf, size_t __n){
    mClients[client].SendTo(__buf, __n);
}
// SOCKET Server::AcceptClient(uint32_t count, std::array<Player, 3>&players) {
//     GameMessage message;
//     struct sockaddr_in client_addr;
//     socklen_t size = sizeof(client_addr);
//     SOCKET clientSocket = INVALID_SOCKET;
//     for (size_t i = 0; i < count; i++) {
//         clientSocket = accept(mSocket, (struct sockaddr*)&client_addr, &size);
//         if (clientSocket != INVALID_SOCKET) {
//             mClients[i].SetScoket(clientSocket);
//             //客户端连接后会发送自身信息
//             mClients[i].RecvFrom(&message, sizeof(message));
//             if (message.event == AI_JOIN_GAME_GAME_EVENT) {
//                 players[i].ai = true;
//                 players[i].index = i;
//                 strcpy(players[i].name, "ai");
//                 mClients[i].Shutdown();
//             }
//             else {
//                 sprintf(message.player.name, "player%d", i + 1);
//                 strcpy(players[i].name, message.player.name);

//                 SendSelfInfoation(i);
//             }
//             SendPlayerNameInfoation(i + 1, players);
//         }
//         else {
//             perror("process_server:accept");
//             // shutdown(mClientsockets[i], SHUT_RDWR);
//         }
//     }
//     return clientSocket;
// }