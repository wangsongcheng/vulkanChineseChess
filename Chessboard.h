#ifndef CHESSBOARD_H
#define CHESSBOARD_H
#include "Chess.h"
#include "VulkanChess.h"
class Chessboard{
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
    void CaptureChess(const Chess *play, const Chess *target);
    //srcCountry获得dstCountry的棋子
    void GetCountryChess(uint32_t srcCountry, uint32_t dstCountry);
    void InitializeChess(uint32_t playerCountry, bool bHanCanPlay = false);
    //返回的棋子属于srcCountry, 该棋子下一步能吃掉dstCountry的chess
    const Chess *Check(uint32_t srcCountry, uint32_t dstCountry, uint32_t chess)const;

    uint32_t GetChessCount(uint32_t country)const;
    Chess *GetChess(uint32_t row, uint32_t column)const;
    Chess *GetChess(uint32_t country, const glm::vec2&mousePos)const;
    Chess *GetChess(uint32_t country, uint32_t row, uint32_t column)const;

    void DestroyCountry(uint32_t country);

    inline Chess **GetChess()const{
        return (Chess **)mChess;
    }
    inline bool IsDeath(uint32_t country)const{
        return !mChess[country][JIANG_CHESS_INDEX];
    }
};
#endif