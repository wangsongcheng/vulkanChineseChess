#ifndef ONLINE_H
#define ONLINE_H
#include "Server.h"
enum GameEvent{
    GAME_OVER_GAME_EVENT = 0,
    GAME_START_GAME_EVENT,
    JOINS_GAME_GAME_EVENT,
    PLAY_CHESS_GAME_EVENT,
    ENABLE_HANG_GAME_EVENT,
    AI_JOIN_GAME_GAME_EVENT,
    CURRENT_COUNTRY_GAME_EVENT,
    SELECTED_COUNTRY_GAME_EVENT,
    CHANGE_PLAYER_NAME_GAME_EVENT,
    CHANGE_PLAYER_COUNTRY_GAME_EVENT,
    SELF_CLIENT_INFORMATION_GAME_EVENT,
    CHANGE_PLAYER_INFORMATION_GAME_EVENT,
};
struct Player{
    bool ai;
    bool random;
    uint32_t uCountry;
    char name[MAX_BYTE];
    char country[MAX_BYTE];
    Player(){
        ai = false;
        random = true;
        strcpy(country, "?");
        uCountry = INVALID_PLAYER_INDEX;
    }
};
struct GameMessage{
    Chess target;
    Chess select;
    Player player;
    GameEvent event;
    // glm::vec2 mousePos;
    uint32_t clientIndex;
};
class OnLine{
    Client mClient;
    bool mIsServer;
    Server mServer;
    char mLocalIp[MAX_BYTE];
    uint32_t clientIndex = INVALID_PLAYER_INDEX;
    //3人游戏模式下, 最多1个ai....4人游戏模式下, 最多2个ai//只有服务器需要该值
    int32_t aIClientIndex[2] = { INVALID_PLAYER_INDEX, INVALID_PLAYER_INDEX};
    void GetLocalIp(char outIp[]);
    void SendSelfInfoation(uint32_t clientIndex);
    void SendPlayerNameInfoation(uint32_t count, const std::array<Player, 3>&players);
public:
    OnLine(/* args */);
    ~OnLine();
    void Init();
    void Cleanup();
    void JoinsGame();
    void AddAi(const char *serverIp);
    void SendCurrentCountry(uint32_t currentCountry);
    SOCKET AcceptClient(uint32_t client, void *__buf, size_t __n);
    void SendClientInfoation(uint32_t clientIndex, const Player *player);
    //只有服务端需要调用这个函数;调用这个函数前，必须随机好国家等信息
    void SendPlayersInfoation(uint32_t clientIndex, const Player *player);
    void SendCountryInfomation(uint32_t clientIndex, const char *country);
    void SendPlayChessMessage(const Player&game, const Chess *pSelect, const Chess *pTarget)const;

    inline void ShutdownClient(uint32_t clientIndex){
        mServer.ShutdownClient(clientIndex);
    }
    inline void ClientRecvFrom(void *__buf, size_t __n)const{
        mClient.RecvFrom(__buf, __n);
    }
    inline void RecvFromClient(uint32_t client, void *__buf, size_t __n)const{
        mServer.RecvFromClient(client, __buf, __n);
    }
    inline void SendToAllClient(void *__buf, size_t __n)const{
        mServer.SendToAllClient(__buf, __n);
    }
    inline std::string GetLocalIp()const{
        return mLocalIp;
    }
    inline bool IsServer()const{
        return mIsServer;
    }
    inline bool CeateServer(uint32_t countryCount){
        mIsServer = true;
        return mServer.CreateServer(countryCount);
    }
    inline bool CreateClient(const char *serverIp){
        return mClient.CreateClient(serverIp);
    }
    inline uint32_t GetClientIndex()const{
        return clientIndex;
    }
    inline void SetClientIndex(uint32_t index){
        clientIndex = index;
    }
    inline auto GetAiClientIndex()const{
        return aIClientIndex;
    }
    inline void SetAiClientIndex(uint32_t index, uint32_t clientIndex){
        aIClientIndex[index] = clientIndex;
    }
    inline auto GetAiClientIndex(uint32_t index)const{
        return aIClientIndex[index];
    }
    // inline auto GetPlayer(){
    //     return mPlayers;
    // }
    // inline auto GetPlayer(uint32_t index){
    //     return mPlayers[index];
    // }
    // inline auto GetCurrentPlayer(){
    //     return mPlayers[clientIndex];
    // }
    // inline void SetAi(uint32_t clinetIndex){
    //     mPlayers[clientIndex].ai = true;
    // }
};
#endif