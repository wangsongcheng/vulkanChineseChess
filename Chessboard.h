#ifndef CHESSBOARD_INCLUDE_H
#define CHESSBOARD_INCLUDE_H
#include "Chess.h"
#include "glm/glm.hpp"
#include "VulkanChessboard.h"
#include "glm/gtc/matrix_transform.hpp"
#define MAX_BYTE 0xff
class Chessboard:public VulkanChessboard{
    // uint32_t mPlayerCountry;
    Chess *mChess[4][COUNTRY_CHESS_COUNT];
    // std::vector<ChessInfo>mCanplays;
    void GetCountryChess(uint32_t srcCountry, uint32_t dstCountry);
    void InitWuChessRowAndColumn(ChessInfo chessInfo[COUNTRY_CHESS_COUNT]);
    void InitShuChessRowAndColumn(ChessInfo chessInfo[COUNTRY_CHESS_COUNT]);
    void InitWeiChessRowAndColumn(ChessInfo chessInfo[COUNTRY_CHESS_COUNT]);
    void InitHanChessRowAndColumn(ChessInfo chessInfo[COUNTRY_CHESS_COUNT]);
    void InitHanChessRowAndColumn(uint32_t country, ChessInfo chessInfo[COUNTRY_CHESS_COUNT]);
    void InitChessInfo(uint32_t country, uint32_t playerCountry, ChessInfo chessInfo[4][COUNTRY_CHESS_COUNT]);
public:
    Chessboard(/* args */);
    ~Chessboard();
    void Cleanup(VkDevice device);
    //一次更新所有棋子，棋子大小也变为初始大小
    void UpdateChess(VkDevice device);
    void ClearCanPlay(VkDevice device);
    //返回真表示在边界上，不能下棋
    bool IsBoundary(uint32_t row, uint32_t column)const;
    // const ChessInfo *GetSelectInfo(const glm::vec2&pos);
    void DestroyCountry(VkDevice device, uint32_t country);
    void InitChess(VkDevice device, uint32_t playerCountry);
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