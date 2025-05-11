#ifndef GAME_H
#define GAME_H
#include <mutex>
#include "Chessboard.h"
#include "VulkanChess.h"
#include "VulkanWireframe.h"
#include "VulkanChessboard.h"
#define GET_NOT_ALLIANCE_COUNTRY(COUNTRY, ALLIANCE_COUNTRY) (MAX_COUNTRY_INDEX - 1 - COUNTRY - ALLIANCE_COUNTRY)
#define GET_ALLIANCE_COUNTRY(COUNTRY, NOT_ALLIANCE_COUNTRY) GET_NOT_ALLIANCE_COUNTRY(COUNTRY, NOT_ALLIANCE_COUNTRY)
class Game{
    struct{
        VulkanChess chess;
        VulkanDevice device;
        VulkanWireframe select;
        VulkanChessboard chessboard;
    }vulkan;
    struct{
        bool isOnline = false;
        bool isGameStart = false;
        //代表汉能不能被操作
        bool isControllable = false;
    }state;
    struct{
        Country country = Invald_Country;
    }player;
    std::mutex mMutex;
    Country mCurrent;
    Country mNotAlliance;
    Chessboard mChessboard;
    uint32_t mMaxCountryCount = 3;
    ChessMove GetSaveStep(Chess *pChess, uint32_t dstRow, uint32_t dstColumn);
    uint32_t GetCanPlay(const glm::vec2&mousePos, const std::vector<glm::vec2>&canplays);

    void UpdateChessUniform(VkDevice device);
    void UpdateSelectChessUniform(VkDevice device, std::vector<glm::vec2>&canplays);
public:
    Game(/* args */);
    ~Game();
    bool areKingsFacing();
    //返回被将的棋子
    const Chess *Check(Country *srcCountry)const;

    bool GameOver();
    Country GetNextCountry()const;
    Country GetLastCountry()const;
    Country GetLastCountry(Country currentCountry)const;
    Country GetNextCountry(Country country)const;

    void InitinalizeGame(Country playerCountry = Invald_Country, Country currentCountry = Invald_Country);

    void MoveChess(const glm::vec2&start, const glm::vec2&end, uint32_t fontIndex, uint32_t country, uint32_t dynamicOffsets);

    void PlayChess(Chess *pChess, uint32_t dstRow, uint32_t dstColumn);
    glm::vec4 PrepareChess(const Chess *pSelect, const glm::vec2&mousePos);

    void RemoveInvalidTarget(std::vector<glm::vec2>&canplays);

    void SelectChess(const Chess *pChess);
    void SetNotAllianceCountry(uint32_t country, uint32_t row, uint32_t column);

    void UnSelectChess();

    void LastCountry();
    void NextCountry();
    void NewGame(Country playerCountry = Invald_Country, Country currentCountry = Invald_Country);
    //绘图方面的函数
    void Cleanup(VkDevice device);

    void Draw(VkCommandBuffer command, VkPipelineLayout layout);
    void DrawFont(VkCommandBuffer command, VkPipelineLayout layout);
    void DrawWireframe(VkCommandBuffer command, VkPipelineLayout layout);

    void EndGame();

    void Setup(VulkanDevice device, VkDescriptorSetLayout setLayout, VkQueue graphics, VulkanPool pool);

    void UpdateUniform(VkDevice device, uint32_t windowWidth);
    //内联
    inline void EnableOnline(){
        state.isOnline = true;
    }
    inline void DiscardHan(){
        state.isControllable = false;
    }
    inline void EnableHan(){
        state.isControllable = true;
    }
    inline auto GetNotAllianceCountry(){
        return mNotAlliance;
    }
    inline uint32_t GetCountryCount(){
        return mMaxCountryCount;
    }
    inline auto GetPlayerCountry(){
        return player.country;
    }
    inline auto GetCurrentCountry(){
        return mCurrent;
    }
    inline Chessboard *GetChessboard(){
        return &mChessboard;
    }
    inline bool IsOnline(){
        return state.isOnline;
    }
    inline bool IsGameStart(){
        return state.isGameStart;
    }
    inline bool IsControllable(){
        return state.isControllable;
    }
    inline void StartGame(){
        state.isGameStart = true;
    }
    // inline void SetPlayerCountry(uint32_t country){
    //     player.country = country;
    // }
    inline void SetCurrentCountry(Country country){
        mCurrent = country;
    }
};
#endif