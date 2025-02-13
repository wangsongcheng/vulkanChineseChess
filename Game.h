#ifndef GAME_INCLUDE
#define GAME_INCLUDE
#include <iostream>
#include "Chessboard.h"
#include "VulkanDevice.h"
#include "VulkanWireframe.h"
#ifdef __linux
#include <unistd.h>
#define MAX_BYTE 0xff
#endif
// #define INTERNET_MODE
class Game{
    uint32_t mPlayer;
    // VulkanDevice mDevice;
    uint32_t mCurrentCountry;
#ifdef HAN_CAN_PLAY
    const uint32_t mCountryCount = MAX_COUNTRY_INDEX;
#else
    const uint32_t mCountryCount = MAX_COUNTRY_INDEX - 1;
#endif
    Chessboard mChessboard;
public:
    Game(/* args */);
    ~Game();
    bool GameOver();

    void InitinalizeGame();

    void InitializeChess();

    // void Cleanup();
    // void Setup(VulkanDevice device, VkDescriptorSetLayout setLayout, VkQueue graphics, VulkanPool pool);
    
    void NextCountry();
    uint32_t GetNextCountry(uint32_t country);

    // void UnSelectChess();
    // void PlayChess(Chess *pChess, uint32_t dstRow, uint32_t dstColumn);

    // void MoveChess(Chess *pStart, const Chess *pEnd);
    // void MoveChess(const glm::vec2&start, const glm::vec2&end, uint32_t fontIndex, uint32_t dynamicOffsets);

    // void SelectChess(VkDevice device, const Chess *pChess);
    void CaptureChess(const Chess *play, const Chess *target);

    // void UpdateChessUniform(VkDevice device);
    // void UpdateSelectChessUniform(VkDevice device, std::vector<Chess>&canplays);

    inline uint32_t GetCurrentCountry(){
        return mCurrentCountry;
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