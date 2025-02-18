#ifndef CHESSBOARD_H
#define CHESSBOARD_H
#include "Chess.h"
#include "VulkanChess.h"
class Chessboard{
    bool mHanCanPlay = false;
    Chess *mChess[MAX_COUNTRY_INDEX][DRAW_COUNTRY_CHESS_COUNT];
    void InitHanChessinfo(Chess *pChess[DRAW_COUNTRY_CHESS_COUNT]);
    void InitWuChessRowAndColumn(Chess *pChess[DRAW_COUNTRY_CHESS_COUNT]);
    void InitShuChessRowAndColumn(Chess *pChess[DRAW_COUNTRY_CHESS_COUNT]);
    void InitWeiChessRowAndColumn(Chess *pChess[DRAW_COUNTRY_CHESS_COUNT]);
    void InitHanChessRowAndColumn(Chess *pChess[DRAW_COUNTRY_CHESS_COUNT]);
    void InitChessInfo(uint32_t country, Chess *pChess[DRAW_COUNTRY_CHESS_COUNT]);
    void InitHanChessRowAndColumn(uint32_t country, Chess *pChess[DRAW_COUNTRY_CHESS_COUNT]);
public:
    Chessboard(/* args */);
    ~Chessboard();
    void InitializeChess(uint32_t playerCountry);
    void CaptureChess(const Chess *play, const Chess *target);
    void GetCountryChess(uint32_t srcCountry, uint32_t dstCountry);
    //返回的是被将军的势力, 该势力需要想办法解除将军
    uint32_t Check();
    //country是被将军的国家
    bool Check(uint32_t country);

    uint32_t GetChessCount(uint32_t country);
    Chess *GetChess(uint32_t row, uint32_t column)const;
    Chess *GetChess(uint32_t country, const glm::vec2&mousePos)const;
    Chess *GetChess(uint32_t country, uint32_t row, uint32_t column)const;

    void DestroyCountry(uint32_t country);

    inline Chess **GetChess()const{
        return (Chess **)mChess;
    }
    inline bool HanCanPslay()const{
        return mHanCanPlay;
    }
    inline void EnableHan(){
        mHanCanPlay = true;
    }
    inline void DiscardHan(){
        mHanCanPlay = false;
    }
    inline bool IsDeath(uint32_t country)const{
        return !mChess[country][JIANG_CHESS_INDEX];
    }
};
#endif