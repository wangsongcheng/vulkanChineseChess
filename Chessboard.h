#ifndef CHESSBOARD_H
#define CHESSBOARD_H
#include <array>
#include "Chess.h"
#include "stdafx.h"

#define MAX_UNDO_STEP 500
typedef struct {
    uint32_t country;
    //保存被销毁的棋子信息
    std::vector<Chess>chess;
}Facing;
typedef struct {
    Chess chess;
    Chess captured;
    bool is_capture;
    int move_number;
    bool is_facing, is_death, is_check;
    std::array<Facing, 2>facing;//如果因为见面被销毁或被灭亡后，棋子可以记这里
    char notation[8];            // 棋步记录(如"炮二平五")
} ChessMove;
class Chessboard{
    uint32_t mCountryCount;
    std::vector<ChessMove>mRecord;
    std::array<glm::vec2, MAX_COUNTRY_INDEX>mPalacesCenter;
    std::array<std::array<Chess *, DRAW_CHESS_COUNT>, MAX_COUNTRY_INDEX>mChess;

    uint32_t GetCountryCount();

    void InitWuChessRowAndColumn(std::array<Chess *, DRAW_CHESS_COUNT>&pChess);
    void InitShuChessRowAndColumn(std::array<Chess *, DRAW_CHESS_COUNT>&pChess);
    void InitWeiChessRowAndColumn(std::array<Chess *, DRAW_CHESS_COUNT>&pChess);
    void InitHanChessRowAndColumn(std::array<Chess *, DRAW_CHESS_COUNT>&pChess);
    void InitHanChessRowAndColumn(uint32_t country, std::array<Chess *, DRAW_CHESS_COUNT>&pChess);
public:
    Chessboard(/* args */);
    ~Chessboard();
    bool areKingsFacing(uint32_t srcCountry, uint32_t dstCountry);

    void CaptureChess(const Chess *srcChess, const Chess *dstChess);
    // //返回的棋子属于srcCountry, 该棋子下一步能吃掉dstCountry的chess
    // const Chess *Check(uint32_t srcCountry, uint32_t dstCountry, uint32_t chess)const;
    //返回的棋子属于country, 该棋子下一步能走到dstRow,dstColumn
    const Chess *Check(uint32_t country, uint32_t dstRow, uint32_t dstColumn)const;
    Chess *CreateChess(uint32_t country, Chess::Type chess, uint32_t row, uint32_t column);

    void DestroyCountry(uint32_t country);

    //srcCountry获得dstCountry的棋子
    void GetCountryChess(uint32_t srcCountry, uint32_t dstCountry);

    uint32_t GetChessCount(uint32_t country)const;
    Chess *GetChess(uint32_t row, uint32_t column)const;
    Chess *GetChess(uint32_t country, Chess::Type chess)const;
    Chess *GetChess(uint32_t country, const glm::vec2&mousePos)const;
    Chess *GetChess(uint32_t country, uint32_t row, uint32_t column)const;


    void InitializeChess(uint32_t playerCountry, bool isControllable = false, uint32_t countryCount = MAX_COUNTRY_INDEX);

    uint32_t IsInPalace(uint32_t row, uint32_t column)const;
    uint32_t IsPalaceCenter(uint32_t row, uint32_t column)const;

    // void Select(const Chess *pChess, std::vector<glm::vec2>&canplays);
    // void Select(uint32_t country, uint32_t chess, std::vector<glm::vec2>&canplays);
    bool IsHasExitPermission(uint32_t country);
    //单步下没问题，但如果是调用destroycountry销毁的呢
    void SaveStep(const ChessMove&dStep);
    //注意:想实现联网下的撤销功能时, 发出撤销请求后需要所有人投票同意后才能撤销
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