#ifndef GAME_INCLUDE
#define GAME_INCLUDE
#include <iostream>
#include "Chessboard.h"
#include "VulkanWireframe.h"
#ifdef __linux
#include <unistd.h>
#endif
//建头文件，把所有宏加进去，其他类包含即可
#ifndef MAX_BYTE
#define MAX_BYTE 0xff
#endif
//不应该修改以下四个宏的值,否则，程序肯定乱套
#define WU_COUNTRY_INDEX 0
#define WEI_COUNTRY_INDEX 1
//汉能走的情况下, 蜀应该为3，汉为2
#define SHU_COUNTRY_INDEX 2
#define HAN_COUNTRY_INDEX 3

#define INVALID_COUNTRY_INDEX -1
//最好不要修改该宏的值
#define MAX_COUNTRY_INDEX 4
#define GET_NOT_ALLIANCE_COUNTRY(COUNTRY, ALLIANCE_COUNTRY) (MAX_COUNTRY_INDEX - 1 - COUNTRY - ALLIANCE_COUNTRY)
#define GET_ALLIANCE_COUNTRY(COUNTRY, NOT_ALLIANCE_COUNTRY) GET_NOT_ALLIANCE_COUNTRY(COUNTRY, NOT_ALLIANCE_COUNTRY)
struct GameInof{
    bool bOnline;
    bool bGameStart;
};
/*
    写得怎样不好说。
    但代码混乱，各种无意义的头文件包含
*/
class Game{
    GameInof mInfo;
    bool mHanCanPlay = false;
    uint32_t mCountryCount;
    Chessboard mChessboard;
    uint32_t mCurrentCountry;
    //如果该值非无效，说明另外两方联盟了
    uint32_t notAllianceCountry = INVALID_COUNTRY_INDEX;
    uint32_t mPlayer = INVALID_COUNTRY_INDEX;//, mBackupPlayer = INVALID_COUNTRY_INDEX;
public:
    Game(/* args */);
    ~Game();
    void areKingsFacing();
    bool areKingsFacing(uint32_t srcCountry, uint32_t dstCountry);

    bool GameOver();

    void InitinalizeGame(int32_t player = -1, int32_t currentCountry = -1);

    // void Cleanup();
    // void Setup(VulkanDevice device, VkDescriptorSetLayout setLayout, VkQueue graphics, VulkanPool pool);
    void NextCountry();
    // uint32_t GetCountry();
    uint32_t GetNextCountry()const;
    uint32_t GetNextCountry(uint32_t country)const;

    void ExtraTurn(uint32_t country);

    void RemoveInvalidTarget(const Chess *pChess, std::vector<glm::vec2>&canplays);

    // void UnSelectChess();
    // void PlayChess(Chess *pChess, uint32_t dstRow, uint32_t dstColumn);

    // void MoveChess(Chess *pStart, const Chess *pEnd);
    // void MoveChess(const glm::vec2&start, const glm::vec2&end, uint32_t fontIndex, uint32_t dynamicOffsets);

    // void SelectChess(VkDevice device, const Chess *pChess);
    void CaptureChess(const Chess *play, const Chess *target);

    //自选棋子

    //-1表示需要重新随机
    // void NewGame(int32_t play = -1);

    //返回的某势力的'将', 该将下一步会被吃掉
    const Chess *Check(uint32_t *srcCountry)const;
    //返回的棋子下一步能吃掉country的'将'
    const Chess *Check(uint32_t country)const;
    void SetNotAllianceCountry(uint32_t country, uint32_t row, uint32_t column);

    inline void SaveStep(uint32_t srcRow, uint32_t srcColumn, uint32_t dstRow, uint32_t dstColumn){
        mChessboard.SaveStep(srcRow, srcColumn, dstRow, dstColumn);
    }

    // inline const Chess *Check(uint32_t srcCountry, uint32_t dstCountry, uint32_t chess) const{
    //     return mChessboard.Check(srcCountry, dstCountry, chess);
    // }
    inline auto GetPalacesCenter(uint32_t country)const{
        return mChessboard.GetPalacesCenter(country);
    }
    inline uint32_t GetNotAllianceCountry()const{
        return notAllianceCountry;
    }
    inline uint32_t GetCountryCount()const{
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
    inline bool IsGameStart()const{
        return mInfo.bGameStart;
    }
    inline bool IsHanCanPslay()const{
        return mHanCanPlay;
    }
    inline void EnableHan(){
        mHanCanPlay = true;
        mCountryCount = MAX_COUNTRY_INDEX;
    }
    inline void DiscardHan(){
        mHanCanPlay = false;
        mCountryCount = MAX_COUNTRY_INDEX - 1;
    }
    inline uint32_t GetCurrentCountry()const{
        return mCurrentCountry;
    }
    inline void SetCurrentCountry(uint32_t country){
        mCurrentCountry = country;        
    }
    inline uint32_t GetPlayer()const{
        return mPlayer;
    }
    // inline auto GetChess()const{
    //     return mChessboard.GetChess();
    // }
    inline auto GetChessBoard()const{
        return &mChessboard;
    }
    inline uint32_t GetChessCount(uint32_t country)const{
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
    inline bool isCurrentCountry(uint32_t country)const{
        return country == mCurrentCountry;
    }
    inline void DestroyCountry(uint32_t country){
        mChessboard.DestroyCountry(country);
    }
    inline bool IsDeath(uint32_t country)const{
        return mChessboard.IsDeath(country);
    }
};
#endif