#ifndef GAME_INCLUDE
#define GAME_INCLUDE
#include <iostream>
#include "Chessboard.h"
#include "VulkanDevice.h"
#include "VulkanWireframe.h"
#ifdef __linux
#include <unistd.h>
#endif
#ifndef MAX_BYTE
#define MAX_BYTE 0xff
#endif
struct GameInof{
    bool bOnline;
    bool bGameStart;
};
class Game{
    GameInof mInfo;
    // VulkanDevice mDevice;
    uint32_t mCountryCount;
    Chessboard mChessboard;
    uint32_t mCurrentCountry;
    uint32_t mPlayer = INVALID_COUNTRY_INDEX, mBackupPlayer = INVALID_COUNTRY_INDEX;
public:
    Game(/* args */);
    ~Game();
    bool GameOver();

    void InitinalizeGame(int32_t player = -1, int32_t currentCountry = -1);

    void InitializeChess();

    // void Cleanup();
    // void Setup(VulkanDevice device, VkDescriptorSetLayout setLayout, VkQueue graphics, VulkanPool pool);
    void NextCountry();
    uint32_t GetNextCountry(uint32_t country);

    void ExtraTurn(uint32_t country);

    // void UnSelectChess();
    // void PlayChess(Chess *pChess, uint32_t dstRow, uint32_t dstColumn);

    // void MoveChess(Chess *pStart, const Chess *pEnd);
    // void MoveChess(const glm::vec2&start, const glm::vec2&end, uint32_t fontIndex, uint32_t dynamicOffsets);

    // void SelectChess(VkDevice device, const Chess *pChess);
    void CaptureChess(const Chess *play, const Chess *target);

    //自选棋子

    //-1表示需要重新随机
    // void NewGame(int32_t play = -1);

    // void UpdateChessUniform(VkDevice device);
    // void UpdateSelectChessUniform(VkDevice device, std::vector<Chess>&canplays);
    inline uint32_t Check(){
        return mChessboard.Check();
    }
    inline uint32_t GetCountryCount(){
        return mCountryCount;
    }
    inline void SetOnline(bool online = true){
        mInfo.bOnline = online;
    }
    inline bool IsOnline()const{
        return mInfo.bOnline;
    }
    inline void StartGame(bool start = true){
        mInfo.bGameStart = start;
    }
    inline bool IsGameStart(){
        return mInfo.bGameStart;
    }
    inline bool HanCanPslay()const{
        return mChessboard.HanCanPslay();
    }
    inline void EnableHan(){
        mChessboard.EnableHan();
        mCountryCount = MAX_COUNTRY_INDEX;
    }
    inline void DiscardHan(){
        mChessboard.DiscardHan();
        mCountryCount = MAX_COUNTRY_INDEX - 1;
    }
    inline uint32_t GetCurrentCountry(){
        return mCurrentCountry;
    }
    inline void SetCurrentCountry(uint32_t country){
        mCurrentCountry = country;        
    }
    inline uint32_t GetPlayer(){
        return mPlayer;
    }
    inline auto GetChess()const{
        return mChessboard.GetChess();
    }
    inline uint32_t GetChessCount(uint32_t country){
        return mChessboard.GetChessCount(country);
    }
    inline auto GetChess(uint32_t country, const glm::vec2&mousePos)const{
        return mChessboard.GetChess(country, mousePos);
    }
    inline auto GetChess(uint32_t row, uint32_t column)const{
        return mChessboard.GetChess(row, column);
    }
    inline auto GetChess(uint32_t country, uint32_t row, uint32_t column)const{
        return mChessboard.GetChess(country, row, column);
    }
    inline bool isCurrentCountry(uint32_t country){
        return country == mCurrentCountry;
    }
    inline void DestroyCountry(uint32_t country){
        return mChessboard.DestroyCountry(country);
    }
    inline bool IsDeath(uint32_t country)const{
        return mChessboard.IsDeath(country);
    }
};
#endif