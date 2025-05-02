#ifndef GAME_H
#define GAME_H
#ifndef MAX_BYTE
#define MAX_BYTE 0xff
#endif
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
        uint32_t country = INVALID_COUNTRY_INDEX;
    }player;
    Chessboard mChessboard;
    uint32_t mCurrentCountry;
    uint32_t mNotAllianceCountry;
    uint32_t mMaxCountryCount = 3;
    void UpdateChessUniform(VkDevice device);
    void UpdateSelectChessUniform(VkDevice device, std::vector<glm::vec2>&canplays);
    uint32_t GetCanPlay(const glm::vec2&mousePos, const std::vector<glm::vec2>&canplays);
public:
    Game(/* args */);
    ~Game();
    void areKingsFacing();
    bool areKingsFacing(uint32_t srcCountry, uint32_t dstCountry);
    //返回被将的棋子
    const Chess *Check(uint32_t *srcCountry)const;

    bool GameOver();
    uint32_t GetNextCountry()const;
    uint32_t GetNextCountry(uint32_t country)const;

    void InitinalizeGame(int32_t playerCountry = INVALID_COUNTRY_INDEX, int32_t currentCountry = INVALID_COUNTRY_INDEX);

    void MoveChess(const glm::vec2&start, const glm::vec2&end, uint32_t fontIndex, uint32_t dynamicOffsets);

    void PlayChess(Chess *pChess, uint32_t dstRow, uint32_t dstColumn);
    glm::vec4 PrepareChess(const Chess *pSelect, const glm::vec2&mousePos);

    void RemoveInvalidTarget(const Chess *pChess, std::vector<glm::vec2>&canplays);

    void SelectChess(const Chess *pChess);

    void SetNotAllianceCountry(uint32_t country, uint32_t row, uint32_t column);

    void UnSelectChess();

    void NextCountry();
    void NewGame(int32_t playerCountry = INVALID_COUNTRY_INDEX, int32_t currentCountry = INVALID_COUNTRY_INDEX);
    //绘图方面的函数
    void Cleanup(VkDevice device);

    void Draw(VkCommandBuffer command, VkPipelineLayout layout);
    void DrawFont(VkCommandBuffer command, VkPipelineLayout layout);
    void DrawWireframe(VkCommandBuffer command, VkPipelineLayout layout);

    void Setup(VulkanDevice device, VkDescriptorSetLayout setLayout, VkQueue graphics, VulkanPool pool);

    void UpdateUniform(VkDevice device, uint32_t windowWidth);
    //内联
    inline void EnableOnline(){
        state.isOnline = true;
    }
    inline void DiscardHan(){
        state.isControllable = false;
    }
    inline void EndGame(){
        state.isGameStart = false;
    }
    inline void EnableHan(){
        state.isControllable = true;
    }
    inline uint32_t GetCountryCount(){
        return mMaxCountryCount;
    }
    inline uint32_t GetPlayerCountry(){
        return player.country;
    }
    inline uint32_t GetCurrentCountry(){
        return mCurrentCountry;
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
    inline void SetCurrentCountry(uint32_t country){
        mCurrentCountry = country;
    }
};
#endif