#ifndef CHESSBOARD_H
#define CHESSBOARD_H
#include <array>
#include "Chess.h"
#include "stdafx.h"

#define MAX_UNDO_STEP 500
typedef struct {
    Country country;
    //保存被销毁的棋子信息
    std::vector<Chess>chess;
}Rercord;
typedef struct {
    Chess chess;
    Rercord death;//如果国家灭亡，则不需要恢复captured, 直接按这里的棋子恢复
    Chess captured;
    bool is_capture;
    char notation[8];// 棋步记录(如"炮二平五")
    int move_number;
    std::array<Rercord, 2>facing;//因为见面被灭亡后，棋子应该记这里
    bool is_facing, is_death, is_check;
} ChessMove;
class Chessboard{
    uint32_t mCountryCount;
    std::vector<ChessMove>mRecord;
    std::array<Territory, MAX_COUNTRY_INDEX>mTerritory;
    std::array<glm::vec2, MAX_COUNTRY_INDEX>mPalacesCenter;
    std::array<std::array<Chess *, DRAW_CHESS_COUNT>, MAX_COUNTRY_INDEX>mChess;

    void DestoryChess(Country country, uint32_t offset);

    uint32_t GetCountryCount();

    void InitWuChessRowAndColumn(std::array<Chess *, DRAW_CHESS_COUNT>&pChess);
    void InitShuChessRowAndColumn(std::array<Chess *, DRAW_CHESS_COUNT>&pChess);
    void InitWeiChessRowAndColumn(std::array<Chess *, DRAW_CHESS_COUNT>&pChess);
    void InitHanChessRowAndColumn(std::array<Chess *, DRAW_CHESS_COUNT>&pChess);
    void InitHanChessRowAndColumn(uint32_t country, std::array<Chess *, DRAW_CHESS_COUNT>&pChess);

    void UndoStep();
public:
    Chessboard(/* args */);
    ~Chessboard();
    bool areKingsFacing(Country srcCountry, Country dstCountry);

    void CaptureChess(const Chess *srcChess, const Chess *dstChess);
    // //返回的棋子属于srcCountry, 该棋子下一步能吃掉dstCountry的chess
    // const Chess *Check(uint32_t srcCountry, uint32_t dstCountry, uint32_t chess)const;
    //返回的棋子属于country, 该棋子下一步能走到dstRow,dstColumn
    const Chess *Check(Country country, uint32_t dstRow, uint32_t dstColumn)const;
    Chess *CreateChess(Country country, Chess::Type chess, uint32_t row, uint32_t column);

    void DestroyCountry();
    void DestroyCountry(Country country);

    //srcCountry获得dstCountry的棋子
    void GetCountryChess(Country srcCountry, Country dstCountry);

    uint32_t GetChessCount(uint32_t country)const;
    Chess *GetChess(uint32_t row, uint32_t column)const;
    Chess *GetChess(Country country, Chess::Type chess)const;
    Chess *GetChess(Country country, const glm::vec2&mousePos)const;
    Chess *GetChess(Country country, uint32_t row, uint32_t column)const;


    void InitializeChess(Country player, bool isControllable = false, uint32_t countryCount = MAX_COUNTRY_INDEX);

    Territory IsInPalace(uint32_t row, uint32_t column)const;
    Territory IsPalaceCenter(uint32_t row, uint32_t column)const;

    // void Select(const Chess *pChess, std::vector<glm::vec2>&canplays);
    // void Select(uint32_t country, uint32_t chess, std::vector<glm::vec2>&canplays);
    bool IsHasExitPermission(Country country);
    //单步下没问题，但如果是调用destroycountry销毁的呢
    void SaveStep(const ChessMove&dStep);
    //注意:想实现联网下的撤销功能时, 发出撤销请求后需要所有人投票同意后才能撤销
    void UndoStep(uint32_t step);
    // inline auto GetChess()const{
    //     return mChess;
    // }
    inline auto GetChess(Country country)const{
        return mChess[country];
    }
    inline auto&GetPalacesCenter(Territory territory)const{
        return mPalacesCenter[territory];
    }
    inline bool IsDeath(Country country)const{
        return !mChess[country][Chess::Type::Jiang_Chess];
    }
};
#endif