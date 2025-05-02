#ifndef CHESSBOARD_H
#define CHESSBOARD_H
#include <array>
#include "Chess.h"
#include "stdafx.h"

#define MAX_UNDO_STEP 100

class Chessboard{
    uint32_t mCountryCount;
    std::vector<std::array<Chess, 2>>mRecord;
    std::array<glm::vec2, MAX_COUNTRY_INDEX>mPalacesCenter;
    std::array<Chess *, DRAW_CHESS_COUNT>mChess[MAX_COUNTRY_INDEX];

    Chess *CreateChess(uint32_t country, Chess::Type chess, uint32_t row, uint32_t column);

    void InitWuChessRowAndColumn(std::array<Chess *, DRAW_CHESS_COUNT>&pChess);
    void InitShuChessRowAndColumn(std::array<Chess *, DRAW_CHESS_COUNT>&pChess);
    void InitWeiChessRowAndColumn(std::array<Chess *, DRAW_CHESS_COUNT>&pChess);
    void InitHanChessRowAndColumn(std::array<Chess *, DRAW_CHESS_COUNT>&pChess);
    void InitHanChessRowAndColumn(uint32_t country, std::array<Chess *, DRAW_CHESS_COUNT>&pChess);
public:
    Chessboard(/* args */);
    ~Chessboard();
    void CaptureChess(const Chess *srcChess, const Chess *dstChess);
    // //返回的棋子属于srcCountry, 该棋子下一步能吃掉dstCountry的chess
    // const Chess *Check(uint32_t srcCountry, uint32_t dstCountry, uint32_t chess)const;
    //返回的棋子属于country, 该棋子下一步能走到dstRow,dstColumn
    const Chess *Check(uint32_t country, uint32_t dstRow, uint32_t dstColumn)const;

    void DestroyCountry(uint32_t country);

    //srcCountry获得dstCountry的棋子
    void GetCountryChess(uint32_t srcCountry, uint32_t dstCountry);

    uint32_t GetChessCount(uint32_t country)const;
    Chess *GetChess(uint32_t row, uint32_t column)const;
    Chess *GetChess(uint32_t country, const glm::vec2&mousePos)const;
    Chess *GetChess(uint32_t country, uint32_t row, uint32_t column)const;


    void InitializeChess(uint32_t playerCountry, bool isControllable = false, uint32_t countryCount = MAX_COUNTRY_INDEX);

    uint32_t IsInPalace(uint32_t row, uint32_t column)const;
    uint32_t IsPalaceCenter(uint32_t row, uint32_t column)const;

    // void Select(const Chess *pChess, std::vector<glm::vec2>&canplays);
    // void Select(uint32_t country, uint32_t chess, std::vector<glm::vec2>&canplays);
    bool IsHasExitPermission(uint32_t country);

    void SaveStep(uint32_t srcRow, uint32_t srcColumn, uint32_t dstRow, uint32_t dstColumn);

    void UndoStep(uint32_t step = 1);
    // inline auto GetChess()const{
    //     return mChess;
    // }
    inline auto GetChess(uint32_t country)const{
        return mChess[country];
    }
    inline auto&GetPalacesCenter(uint32_t territory)const{
        return mPalacesCenter[territory];
    }
    inline bool IsDeath(uint32_t country)const{
        return !mChess[country][Chess::Type::Jiang_Chess];
    }
};
#endif