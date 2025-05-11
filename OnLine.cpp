#include "OnLine.h"
void OnLine::GetLocalIp(char outIp[]){
    char buffer[MAX_BYTE] = {0};
    if(!gethostname(buffer, sizeof(buffer))){
#ifdef WIN32
        struct addrinfo* result = NULL;
        //struct addrinfo* ptr = NULL;
        struct addrinfo hints;
        ZeroMemory(&hints, sizeof(hints));
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_protocol = IPPROTO_TCP;
        getaddrinfo(buffer, nullptr, &hints, &result);
        struct addrinfo* ptr;
        for (ptr = result; ptr != NULL;) {
            char* local_ip = NULL;
            struct sockaddr_in* sockaddr = (struct sockaddr_in*)ptr->ai_addr;
            local_ip = inet_ntoa(sockaddr->sin_addr);
            if (local_ip) {
                strcpy(outIp, local_ip);
                if(strcmp("127.0.0.1", outIp) && strcmp("0.0.0.0", outIp)){
                    break;
                }
            }
            ptr = ptr->ai_next;
        }
#else
        hostent* temp = gethostbyname(buffer);
        if (temp) {
            for (size_t i = 0; temp->h_addr_list[i]; ++i) {
                char* local_ip = NULL;
                local_ip = inet_ntoa(*(struct in_addr*)temp->h_addr_list[i]);
                if (local_ip) {
                    strcpy(outIp, local_ip);
                    if (strcmp("127.0.0.1", outIp)) {
                        break;
                    }
                }
            }
        }
#endif
    }
}
void OnLine::SendSelfInfoation(uint32_t clientIndex){
    GameMessage message;
    message.event = SELF_CLIENT_INFORMATION_GAME_EVENT;
    message.clientIndex = clientIndex;
    mServer.SendToClient(clientIndex, &message, sizeof(message));
}
void OnLine::SendPlayerNameInfoation(uint32_t count, const std::array<Player, 3>&players){
    GameMessage message;
    message.event = JOINS_GAME_GAME_EVENT;
    for (uint32_t uiClient = 0; uiClient < count; ++uiClient){
        message.clientIndex = uiClient;
        strcpy(message.player.name, players[uiClient].name);
        SendToAllClient(&message, sizeof(message));
    }
}
OnLine::OnLine(/* args */){
}

OnLine::~OnLine(){
}

void OnLine::Init(){
#ifdef WIN32
    WSADATA wsaData;
    int wsaResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if(wsaResult != 0){
        printf("in function:%s:WSAStartup error, result = %d", __FUNCTION__, wsaResult);
    }
#endif
    GetLocalIp(mLocalIp);
}

void OnLine::JoinsGame(){
    GameMessage message;
    // strcpy(message.name, clientName);
    message.event = JOINS_GAME_GAME_EVENT;
    mClient.SendTo(&message, sizeof(message));
}

void OnLine::Cleanup(){
    mServer.ShutdownClient();
    mServer.ShutdownServer();
    mClient.Shutdown();
#ifdef WIN32
    WSACleanup();
#endif
}

void OnLine::ExitGame(){
    GameMessage message;
    message.clientIndex = clientIndex;
    if(IsServer()){
        message.event = GAME_OVER_GAME_EVENT;
        mServer.SendToAllClient(&message, sizeof(message));
    }
    else{
        message.event = PLAYER_EXIT_GAME_EVENT;
        mClient.SendTo(&message, sizeof(message));
    }
}

void OnLine::AddAi(const char *serverIp){
    Client client;
    client.CreateClient(serverIp);
    GameMessage message;
    message.event = AI_JOIN_GAME_GAME_EVENT;
    client.SendTo(&message, sizeof(message));
    client.Shutdown();
}

void OnLine::SendCurrentCountry(Country currentCountry){
    GameMessage message;
    message.player.uCountry = currentCountry;
    message.event = CURRENT_COUNTRY_GAME_EVENT;
    mServer.SendToAllClient(&message, sizeof(message));
}

SOCKET OnLine::AcceptClient(uint32_t client, void *__buf, size_t __n){
    SOCKET s = mServer.AcceptClient(client);
    if(s != INVALID_SOCKET){
        mServer.RecvFromClient(client, __buf, __n);
        GameMessage *pMessage = (GameMessage *)__buf;
        if(pMessage->event != AI_JOIN_GAME_GAME_EVENT)SendSelfInfoation(client);
    }
    return s;
}

void OnLine::SendPlayersInfoation(uint32_t clientIndex, const Player *player){
    GameMessage message;
    message.event = CHANGE_PLAYER_INFORMATION_GAME_EVENT;
    message.clientIndex = clientIndex;
    message.player.ai = player->ai;
    strcpy(message.player.name, player->name);
    message.player.uCountry = player->uCountry;
    strcpy(message.player.country, player->country);
    mServer.SendToAllClient(&message, sizeof(message));
}

void OnLine::SendCountryInfomation(uint32_t clientIndex, const char *country){
    GameMessage message;
    message.clientIndex = clientIndex;
    strcpy(message.player.country, country);
    message.event = CHANGE_PLAYER_COUNTRY_GAME_EVENT;
    mClient.SendTo(&message, sizeof(message));
}

void OnLine::SendClientInfoation(uint32_t clientIndex, const Player *player){
    GameMessage message;
    message.clientIndex = clientIndex;
    message.event = JOINS_GAME_GAME_EVENT;
    memcpy(&message.player, player, sizeof(Player));
    mServer.SendToAllClient(&message, sizeof(message));
}

void OnLine::SendPlayChessMessage(const Player &game, const Chess *pSelect, const Chess *pTarget)const{
    GameMessage message;
    message.event = PLAY_CHESS_GAME_EVENT;
    message.player = game;
    // message.mousePos = mousePos;
    if(pSelect){
        memcpy(&message.select, pSelect, sizeof(Chess));
    }
    if(pTarget){
        memcpy(&message.target, pTarget, sizeof(Chess));
    }
    mClient.SendTo(&message, sizeof(message));
}