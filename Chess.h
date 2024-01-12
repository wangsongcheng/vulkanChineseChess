#ifndef CHESS_INCLUDE_H
#define CHESS_INCLUDE_H
#include <vector>
#include "VulkanChess.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#define WU_TERRITORY_INDEX WU_COUNTRY_INDEX
#define WEI_TERRITORY_INDEX WEI_COUNTRY_INDEX
#define SHU_TERRITORY_INDEX SHU_COUNTRY_INDEX
#define HAN_TERRITORY_INDEX HAN_COUNTRY_INDEX
#define INVALID_TERRITORY_INDEX INVALID_COUNTRY_INDEX
//以下索引、魏国索引。现在变成不能随意修改了。这确实是痛点
//除非增加新棋,否则在不包括汉棋子的情况下, 其他棋子的值应该在16以下;这些值主要用于动态偏移
#define MA_CHESS_INDEX_1 1
#define MA_CHESS_INDEX_2 2
#define JIANG_CHESS_INDEX 0
#define PAO_CHESS_INDEX_1 3
#define PAO_CHESS_INDEX_2 4
#define CHE_CHESS_INDEX_1 5
#define SHI_CHESS_INDEX_1 6
#define SHI_CHESS_INDEX_2 7
#define CHE_CHESS_INDEX_2 8
#define BING_CHESS_INDEX_1 9
#define BING_CHESS_INDEX_2 10
#define BING_CHESS_INDEX_3 11
#define BING_CHESS_INDEX_4 12
#define BING_CHESS_INDEX_5 13
#define XIANG_CHESS_INDEX_1 14
#define XIANG_CHESS_INDEX_2 15

#define PAO_CHESS_INDEX_3 1
#define CHE_CHESS_INDEX_3 2
#define CHE_CHESS_INDEX_4 3
#define CHE_CHESS_INDEX_5 4

#define MAX_CHESS_INDEX XIANG_CHESS_INDEX_2
// #define PAO_CHESS_INDEX_3 16
// #define CHE_CHESS_INDEX_3 17
// #define CHE_CHESS_INDEX_4 18
// #define CHE_CHESS_INDEX_5 19

#define CHESS_WIDTH 20
#define CHESS_HEIGHT CHESS_WIDTH
#define ROW_TO_Y(ROW)((CHESSBOARD_GRID_SIZE) + (ROW) * (CHESSBOARD_GRID_SIZE) + ((ROW) + 1) * CHESSBOARD_LINE_WIDTH)
#define COLUMN_TO_X(COLUMN)(ROW_TO_Y(COLUMN))
#define GET_PLAYER_COUNTRY(COUNTRY, TERRITORY, COUNTRY_COUNT)((COUNTRY + TERRITORY) % COUNTRY_COUNT)
#define GET_COUNTRY_INDEX(COUNTRY, PLAYER_COUNTRY, COUNTRY_COUNT)((COUNTRY + COUNTRY_COUNT - PLAYER_COUNTRY) % COUNTRY_COUNT)
struct ChessInfo{
    uint32_t row;
    uint32_t chess;//一般用上面的宏赋值(*_CHESS_INDEX_*)
    uint32_t column;
    uint32_t country;
    uint32_t fontIndex;
    ChessInfo(){
        country = -1;//该值为-1时, chess和fontIndex无效
        row = 100;
        column = 100;
    }
    ChessInfo(uint32_t row, uint32_t column){
        this->row = row;
        this->column = column;
    }
    // ChessInfo(uint32_t country, uint32_t row, uint32_t column){
    //     this->row = row;
    //     this->column = column;
    //     this->country = country;
    // }
    ~ChessInfo(){

    }
};
class Chess{
    glm::vec2 mPos;
protected:
    ChessInfo mInfo;
    void SwapCenter(ChessInfo *src, ChessInfo *dst);
    //以蜀为参考视角;位置不准确但能用
    uint32_t GetTerritoryIndex(uint32_t row, uint32_t column)const;
    // //country是在territory上的势力
    // uint32_t GetPlayerCountry(uint32_t country, uint32_t territory)const;
    const ChessInfo *GetChessInfo(uint32_t row, uint32_t column, const Chess *pChess[4][COUNTRY_CHESS_COUNT])const;
    const ChessInfo *GetChessInfo(uint32_t country, uint32_t row, uint32_t column, const Chess *pChess[4][COUNTRY_CHESS_COUNT])const;
public:
    Chess(const ChessInfo&info);
    ~Chess();
    void ResetPos(uint32_t row, uint32_t column);
    //返回真表示在边界上，不能下棋
    bool IsBoundary(uint32_t row, uint32_t column)const;
    bool IsPalaceCenter(uint32_t centerCount, const ChessInfo *center)const;
    bool IsInPalace(uint32_t row, uint32_t column, const ChessInfo *pCenter)const;
    void SelectChessInPalace(const Chess *pChess[4][COUNTRY_CHESS_COUNT], uint32_t centerCount, const ChessInfo *center, std::vector<ChessInfo>&canplays)const;

    inline const ChessInfo *GetInfo()const{
        return &mInfo;
    }
    inline void ResetInfo(const ChessInfo&info){
        mInfo = info;
        ResetPos(info.row, info.column);
    }
    inline bool IsSelect(uint32_t row, uint32_t column)const{
        return mInfo.row == row && mInfo.column == column;
    }
    inline bool IsSelect(const glm::vec2&pos)const{
        return abs(pos.x - mPos.x) < CHESS_WIDTH && abs(pos.y - mPos.y) < CHESS_HEIGHT;
    }

    virtual void Selected(const Chess *pChess[4][COUNTRY_CHESS_COUNT], std::vector<ChessInfo>&canplays)const = 0;
};

class Wei:public Chess{
    ChessInfo mCenter;//九宫格中心
public:
    Wei(const ChessInfo&info);
    ~Wei();
    virtual void Selected(const Chess *pChess[4][COUNTRY_CHESS_COUNT], std::vector<ChessInfo>&canplays)const;
};

class Shu:public Chess{
    ChessInfo mCenter;//九宫格中心
public:
    Shu(const ChessInfo&info);
    ~Shu();
    virtual void Selected(const Chess *pChess[4][COUNTRY_CHESS_COUNT], std::vector<ChessInfo>&canplays)const;
};

class Wu:public Chess{
    ChessInfo mCenter;//九宫格中心
public:
    Wu(const ChessInfo&info);
    ~Wu();
    virtual void Selected(const Chess *pChess[4][COUNTRY_CHESS_COUNT], std::vector<ChessInfo>&canplays)const;
};

class Han:public Chess{
    ChessInfo mCenter;//九宫格中心
public:
    Han(const ChessInfo&info);
    ~Han();
    virtual void Selected(const Chess *pChess[4][COUNTRY_CHESS_COUNT], std::vector<ChessInfo>&canplays)const;
};
class Bing:public Chess{
    uint32_t mTerriory;
    ChessInfo mCenter[4];//汉能走时, 3个
    glm::vec2 GetBingBack()const;
    bool IsAbroad(uint32_t row, uint32_t column)const;
public:
    Bing(const ChessInfo&info);
    ~Bing();
    virtual void Selected(const Chess *pChess[4][COUNTRY_CHESS_COUNT], std::vector<ChessInfo>&canplays)const;
};
class Pao:public Chess{
    ChessInfo mCenter[4];//汉能走时, 4个
public:
    Pao(const ChessInfo&info);
    ~Pao();
    virtual void Selected(const Chess *pChess[4][COUNTRY_CHESS_COUNT], std::vector<ChessInfo>&canplays)const;
};
class Che:public Chess{
    ChessInfo mCenter[4];//汉能走时, 4个
public:
    Che(const ChessInfo&info);
    ~Che();
    virtual void Selected(const Chess *pChess[4][COUNTRY_CHESS_COUNT], std::vector<ChessInfo>&canplays)const;
};
class Ma:public Chess{
public:
    Ma(const ChessInfo&info);
    ~Ma();
    virtual void Selected(const Chess *pChess[4][COUNTRY_CHESS_COUNT], std::vector<ChessInfo>&canplays)const;
};
class Xiang:public Chess{
    uint32_t mTerriory;
    bool IsAbroad(uint32_t row, uint32_t column)const;
public:
    Xiang(const ChessInfo&info);
    ~Xiang();
    virtual void Selected(const Chess *pChess[4][COUNTRY_CHESS_COUNT], std::vector<ChessInfo>&canplays)const;
};
class Shi:public Chess{
    ChessInfo mCenter;//九宫格中心
public:
    Shi(const ChessInfo&info);
    ~Shi();
    virtual void Selected(const Chess *pChess[4][COUNTRY_CHESS_COUNT], std::vector<ChessInfo>&canplays)const;
};
#endif