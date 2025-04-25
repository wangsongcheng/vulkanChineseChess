#ifndef CHESSBOARD_H
#define CHESSBOARD_H
#include <array>
#include "Chess.h"
#include "VulkanChessboard.h"

#define HAN_PAO_CHESS_INDEX 1
#define HAN_CHE_CHESS_INDEX (HAN_PAO_CHESS_INDEX + HAN_PAO_CHESS_COUNT)

class Chessboard{
    uint32_t mCountryCount;
    std::array<glm::vec2, MAX_COUNTRY_INDEX>mPalacesCenter;
    std::array<Chess *, DRAW_COUNTRY_CHESS_COUNT>mChess[MAX_COUNTRY_INDEX];
    void InitHanChessinfo(std::array<Chess *, DRAW_COUNTRY_CHESS_COUNT>&pChess);
    void InitWuChessRowAndColumn(std::array<Chess *, DRAW_COUNTRY_CHESS_COUNT>&pChess);
    void InitShuChessRowAndColumn(std::array<Chess *, DRAW_COUNTRY_CHESS_COUNT>&pChess);
    void InitWeiChessRowAndColumn(std::array<Chess *, DRAW_COUNTRY_CHESS_COUNT>&pChess);
    void InitHanChessRowAndColumn(std::array<Chess *, DRAW_COUNTRY_CHESS_COUNT>&pChess);
    void InitChessInfo(uint32_t country, std::array<Chess *, DRAW_COUNTRY_CHESS_COUNT>&pChess);
    void InitHanChessRowAndColumn(uint32_t country, std::array<Chess *, DRAW_COUNTRY_CHESS_COUNT>&pChess);
public:
    Chessboard(/* args */);
    ~Chessboard();
    void CaptureChess(uint32_t srcCountry, uint32_t dstCountry, uint32_t dstChess);
    //返回的棋子属于srcCountry, 该棋子下一步能吃掉dstCountry的chess
    const Chess *Check(uint32_t srcCountry, uint32_t dstCountry, uint32_t chess)const;

    void DestroyCountry(uint32_t country);

    //srcCountry获得dstCountry的棋子
    void GetCountryChess(uint32_t srcCountry, uint32_t dstCountry);

    uint32_t GetChessCount(uint32_t country)const;
    Chess *GetChess(uint32_t row, uint32_t column)const;
    Chess *GetChess(uint32_t country, const glm::vec2&mousePos)const;
    Chess *GetChess(uint32_t country, uint32_t row, uint32_t column)const;


    void InitializeChess(uint32_t playerCountry, bool bHanCanPlay = false, uint32_t countryCount = 4);

    int32_t IsInPalace(uint32_t row, uint32_t column)const;
    int32_t IsPalaceCenter(uint32_t row, uint32_t column)const;

    // void Select(const Chess *pChess, std::vector<glm::vec2>&canplays);
    // void Select(uint32_t country, uint32_t chess, std::vector<glm::vec2>&canplays);

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
        return !mChess[country][JIANG_CHESS_INDEX];
    }
};
#endif