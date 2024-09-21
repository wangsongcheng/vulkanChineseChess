#ifndef GAME_INCLUDE
#define GAME_INCLUDE
#include "Chess.h"
class Game{
    Chess *mChess[MAX_COUNTRY_INDEX][DRAW_COUNTRY_CHESS_COUNT];
    void GetCountryChess(uint32_t srcCountry, uint32_t dstCountry);
    void InitHanChessinfo(Chess *pChess[DRAW_COUNTRY_CHESS_COUNT]);
    void InitWuChessRowAndColumn(Chess *pChess[DRAW_COUNTRY_CHESS_COUNT]);
    void InitShuChessRowAndColumn(Chess *pChess[DRAW_COUNTRY_CHESS_COUNT]);
    void InitWeiChessRowAndColumn(Chess *pChess[DRAW_COUNTRY_CHESS_COUNT]);
    void InitHanChessRowAndColumn(Chess *pChess[DRAW_COUNTRY_CHESS_COUNT]);
    void InitChessInfo(uint32_t country, Chess *pChess[DRAW_COUNTRY_CHESS_COUNT]);
    void InitHanChessRowAndColumn(uint32_t country, Chess *pChess[DRAW_COUNTRY_CHESS_COUNT]);
public:
    Game(/* args */);
    ~Game();
    void InitializeChess(uint32_t playerCountry);

    Chess *GetChess(uint32_t row, uint32_t column)const;
    Chess *GetChess(uint32_t country, const glm::vec2&mousePos)const;
    Chess *GetChess(uint32_t country, uint32_t row, uint32_t column)const;
    
    void DestroyCountry(uint32_t country);
    void CaptureChess(const Chess *play, const Chess *pTarget);

    inline auto GetChess(){
        return mChess;
    }
    inline bool IsDeath(uint32_t country)const{
        return !mChess[country][JIANG_CHESS_INDEX];
    }
};
#endif