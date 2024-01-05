#ifndef CHESSBOARD_INCLUDE_H
#define CHESSBOARD_INCLUDE_H
#include "Chess.h"
#include "glm/glm.hpp"
#include "VulkanChessboard.h"
#include "glm/gtc/matrix_transform.hpp"
#define MAX_BYTE 0xff
// struct ChessboardChess{
//     Chess *jiang;//实际上是魏蜀吴汉
//     Chess *ma[2];
//     Chess *pao[2];
//     Chess *che[2];
//     Chess *shi[2];
//     Chess *bing[5];
//     Chess *xiang[2];
// };
// struct ChessboardSelectChess{
//     VkDescriptorSet set;
//     VulkanBuffer uniform;
//     void UpdateUniform(VkDevice device, const std::vector<Ranks>&canplays){
//         glm::mat4 model[SELECT_CHESS_UNIFORM_MAX];
//         const uint32_t uiCanplays = canplays.size();
//         if(uiCanplays > sizeof(model) / sizeof(glm::mat4)){
//             printf("%s函数:uiCanplays的数量:%d, 大于model[%d]的数量:%d\n", __FUNCTION__, uiCanplays, sizeof(model) / sizeof(glm::mat4), sizeof(model) / sizeof(glm::mat4));
//             return;
//         }
//         const glm::mat4 scale = glm::scale(glm::mat4(1), glm::vec3(CHESSBOARD_GRID_SIZE, CHESSBOARD_GRID_SIZE, 1));
//         for (size_t i = 0; i < sizeof(model) / sizeof(glm::mat4); ++i){
//             model[i] = glm::translate(glm::mat4(1), glm::vec3(COLUMN_TO_X(100, CHESSBOARD_GRID_SIZE, CHESSBOARD_LINE_WIDTH), ROW_TO_Y(100, CHESSBOARD_GRID_SIZE, CHESSBOARD_LINE_WIDTH), 0)) * scale;
//         }
//         for (size_t i = 0; i < canplays.size(); ++i){
//             model[i] = glm::translate(glm::mat4(1), glm::vec3(COLUMN_TO_X(canplays[i].column, CHESSBOARD_GRID_SIZE, CHESSBOARD_LINE_WIDTH) - CHESSBOARD_GRID_SIZE * .5, ROW_TO_Y(canplays[i].row, CHESSBOARD_GRID_SIZE, CHESSBOARD_LINE_WIDTH) - CHESSBOARD_GRID_SIZE * .5, 0)) * scale;
//         }
//         uniform.UpdateData(device, sizeof(model), model);
//     }
// };
//因为GetCountryBehind函数中的值是写死的，所以在修改颜色或国家位置后可能需要修改函数内的方向
class Chessboard:public VulkanChessboard{
    // ChessInfo *mSelected;
    //原棋子数为16,因为汉需要3个車,和1个炮, 所以需要加4//汉势力只赋值一个将
    Chess *mChess[4][COUNTRY_CHESS_COUNT];//因为棋子数都一样，而汉的棋子少，所以无所谓
    // std::vector<ChessInfo>mCanplays;
    void GetCountryChess(uint32_t srcCountry, uint32_t dstCountry);
    void InitWuChessRowAndColumn(ChessInfo chessInfo[4][COUNTRY_CHESS_COUNT]);
    void InitShuChessRowAndColumn(ChessInfo chessInfo[4][COUNTRY_CHESS_COUNT]);
    void InitWeiChessRowAndColumn(ChessInfo chessInfo[4][COUNTRY_CHESS_COUNT]);
    void InitChessInfo(uint32_t country, ChessInfo chessInfo[4][COUNTRY_CHESS_COUNT]);
public:
    Chessboard(/* args */);
    ~Chessboard();
    void Cleanup(VkDevice device);
    void InitChess(VkDevice device);
    //一次更新所有棋子，棋子大小也变为初始大小
    void UpdateChess(VkDevice device);
    void ClearCanPlay(VkDevice device);
    //返回真表示在边界上，不能下棋
    bool IsBoundary(uint32_t row, uint32_t column)const;
    // const ChessInfo *GetSelectInfo(const glm::vec2&pos);
    void DestroyCountry(VkDevice device, uint32_t country);
    const ChessInfo *GetChessInfos(const glm::vec2&mousePos);
    //目前该函数只用于ai
    void Select(VkDevice device, uint32_t country, uint32_t chess);
    void UnSelect(VkDevice device, uint32_t country, uint32_t chess);
    const ChessInfo *GetChessInfos(uint32_t row, uint32_t column)const;
    const ChessInfo *GetChessInfos(uint32_t country, const glm::vec2&mousePos)const;
    const ChessInfo *GetChessInfos(uint32_t country, uint32_t row, uint32_t column)const;
    void Move(VkDevice device, uint32_t country, uint32_t chess, uint32_t row, uint32_t column);
    void CaptureChess(uint32_t srcCountry, uint32_t srcChess, uint32_t dstCountry, uint32_t destChess);
    void Play(VkDevice device, uint32_t srcCountry, uint32_t srcChess, uint32_t dstCountry, uint32_t dstChess);

    // inline const ChessInfo *GetSelected(){
    //     return mSelected;
    // }
    inline auto GetChess()const{
        return mChess;
    }
    inline bool IsDeath(uint32_t country)const{
        return !mChess[country][JIANG_CHESS_INDEX];
    }
    // inline void SetSelected(const ChessInfo *pInfo){
    //     mSelected = (ChessInfo *)pInfo;
    // }
    // inline const std::vector<ChessInfo>&GetCanPlay(){
    //     return mCanplays;
    // }
    inline const ChessInfo *GetChessInfo(uint32_t country, uint32_t chess)const{
        const Chess *pChess = mChess[country][chess];
        if(pChess)
            return pChess->GetInfo();
        return nullptr;
    }
    inline const Chess *GetChess(uint32_t country, uint32_t chess)const{
        return mChess[country][chess];
    }
    //除蜀国外，a吃掉b的将后，a获得b剩下的所有棋子
    //如果可以。去掉蜀国和吴国的2个兵(其他棋子也可以考虑)，已体现魏国的优势
    //输赢交给外部判断
};
#endif