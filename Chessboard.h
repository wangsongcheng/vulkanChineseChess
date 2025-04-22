#ifndef CHESSBOARD_H
#define CHESSBOARD_H
#include <array>
#include "Chess.h"
#include "VulkanChessboard.h"
//数量当然能随意改，但只改数量意义不大，还需要把它们的坐标等信息通过uniform传给着色器
//除汉外, 每个国家的棋子数
#define BING_CHESS_COUNT 5
#define PAO_CHESS_COUNT 2
#define CHE_CHESS_COUNT 2
#define MA_CHESS_COUNT 2
#define XIANG_CHESS_COUNT 2
#define SHI_CHESS_COUNT 2
#define JIANG_CHESS_COUNT 1

#define HAN_CHE_CHESS_COUNT 3
#define HAN_PAO_CHESS_COUNT 1
#define HAN_JIANG_CHESS_COUNT 1

#define WU_CHESS_COUNT (BING_CHESS_COUNT + PAO_CHESS_COUNT + CHE_CHESS_COUNT + MA_CHESS_COUNT + XIANG_CHESS_COUNT + SHI_CHESS_COUNT + JIANG_CHESS_COUNT)
#define HAN_CHESS_COUNT WU_CHESS_COUNT
#define WEI_CHESS_COUNT WU_CHESS_COUNT
#define SHU_CHESS_COUNT WU_CHESS_COUNT

#define COUNTRY_CHESS_COUNT WEI_CHESS_COUNT

#define DRAW_COUNTRY_CHESS_COUNT (COUNTRY_CHESS_COUNT * 3)

#define CHESSBOARD_CHESS_TOTAL (DRAW_COUNTRY_CHESS_COUNT * 4)

// #ifndef ROW_COLUMN_CHESS_TO_INDEX
// #define ROW_COLUMN_CHESS_TO_INDEX(ROW_INDEX, COLUMN_INDEX)ROW_COLUMN_TO_INDEX(ROW_INDEX, COLUMN_INDEX, DRAW_COUNTRY_CHESS_COUNT)
// #endif

#define JIANG_CHESS_INDEX 0
#define MA_CHESS_INDEX (JIANG_CHESS_INDEX + JIANG_CHESS_COUNT)
#define PAO_CHESS_INDEX (MA_CHESS_INDEX + MA_CHESS_COUNT)
#define CHE_CHESS_INDEX (PAO_CHESS_INDEX + PAO_CHESS_COUNT)
#define SHI_CHESS_INDEX (CHE_CHESS_INDEX + CHE_CHESS_COUNT)
#define XIANG_CHESS_INDEX (SHI_CHESS_INDEX + SHI_CHESS_COUNT)
#define BING_CHESS_INDEX (XIANG_CHESS_INDEX + XIANG_CHESS_COUNT)

#define HAN_PAO_CHESS_INDEX 1
#define HAN_CHE_CHESS_INDEX (HAN_PAO_CHESS_INDEX + HAN_PAO_CHESS_COUNT)

#define INVALID_CHESS_INDEX -1
#ifndef WU_COUNTRY_INDEX
#define WU_COUNTRY_INDEX 0
#define WEI_COUNTRY_INDEX 1
#define SHU_COUNTRY_INDEX 2
#define HAN_COUNTRY_INDEX 3

#define INVALID_COUNTRY_INDEX -1
//最好不要修改该宏的值
#define MAX_COUNTRY_INDEX 4
#endif
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
    void CaptureChess(const Chess *play, const Chess *target);
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

    uint32_t IsInPalace(uint32_t row, uint32_t column)const;
    uint32_t IsPalaceCenter(uint32_t row, uint32_t column)const;

    // void Select(const Chess *pChess, std::vector<glm::vec2>&canplays);
    // void Select(uint32_t country, uint32_t chess, std::vector<glm::vec2>&canplays);

    inline auto GetChess()const{
        return mChess;
    }
    inline auto GetPalacesCenter(uint32_t country)const{
        return mPalacesCenter[country];
    }
    inline bool IsDeath(uint32_t country)const{
        return !mChess[country][JIANG_CHESS_INDEX];
    }
};
#endif