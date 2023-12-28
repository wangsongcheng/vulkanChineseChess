#ifndef CHESS_INCLUDE_H
#define CHESS_INCLUDE_H
#include <vector>
#include "VulkanChess.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
//除非增加新棋,否则在不包括汉棋子的情况下, 其他棋子的值应该在16以下;这些值主要用于动态偏移
#define MA_CHESS_INDEX_1 1
#define MA_CHESS_INDEX_2 2
#define JIANG_CHESS_INDEX 0
#define PAO_CHESS_INDEX_1 7
#define PAO_CHESS_INDEX_2 8
#define CHE_CHESS_INDEX_1 9
#define CHE_CHESS_INDEX_2 10
#define SHI_CHESS_INDEX_1 3
#define SHI_CHESS_INDEX_2 4
#define XIANG_CHESS_INDEX_1 5
#define XIANG_CHESS_INDEX_2 6
#define BING_CHESS_INDEX_1 11
#define BING_CHESS_INDEX_2 12
#define BING_CHESS_INDEX_3 13
#define BING_CHESS_INDEX_4 14
#define BING_CHESS_INDEX_5 15
//因为漢也有'将'所以从1开始
#define PAO_CHESS_INDEX_3 1
#define CHE_CHESS_INDEX_3 2
#define CHE_CHESS_INDEX_4 3
#define CHE_CHESS_INDEX_5 4

#define CHESS_WIDTH 20
#define CHESS_HEIGHT CHESS_WIDTH
#define ROW_TO_Y(ROW)((CHESSBOARD_GRID_SIZE) + (ROW) * (CHESSBOARD_GRID_SIZE) + ((ROW) + 1) * CHESSBOARD_LINE_WIDTH)
#define COLUMN_TO_X(COLUMN)(ROW_TO_Y(COLUMN))
struct ChessInfo{
    uint32_t row;
    uint32_t chess;//一般用上面的宏赋值(*_CHESS_INDEX_*)
    uint32_t column;
    uint32_t country;
    uint32_t fontIndex;
    ChessInfo(){
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
    const ChessInfo *GetChessInfo(const ChessInfo *pInfo, uint32_t count, uint32_t row, uint32_t column)const;
public:
    Chess(const ChessInfo&info);
    ~Chess();
    void ResetPos(uint32_t row, uint32_t column);
    //返回真表示在边界上，不能下棋
    bool IsBoundary(uint32_t row, uint32_t column)const;
    bool IsInPalace(uint32_t row, uint32_t column, uint32_t centerCount, const ChessInfo *center)const;
    bool IsPalaceCenter(uint32_t row, uint32_t column, uint32_t centerCount, const ChessInfo *center)const;
    //该函数不考虑其他地方是否有棋子, 因此调用完后需要在判断一次
    void SelectChessInPalace(uint32_t centerCount, const ChessInfo *center, std::vector<ChessInfo>&canplays)const;

        //可以通过返回mInfo.chess来得到棋子索引
//     bool isSelect(const glm::vec2&pos);
//     inline glm::vec3 GetCountryColor()const{
//         return mCountryColor;
//     }
    inline bool IsSelect(const glm::vec2&pos)const{
        return abs(pos.x - mPos.x) < CHESS_WIDTH && abs(pos.y - mPos.y) < CHESS_HEIGHT;
    }
//     inline void GetChessRanks(uint32_t&row, uint32_t&column)const{
//         row = mRow;
//         column = mColumn;
//     }
    virtual void Selected(const ChessInfo *pInfo, uint32_t count, std::vector<ChessInfo>&canplays) = 0;
};

class Wei:public Chess{
    ChessInfo mCenter;//九宫格中心
public:
    Wei(const ChessInfo&info);
    ~Wei();
    virtual void Selected(const ChessInfo *pInfo, uint32_t count, std::vector<ChessInfo>&canplays);
};

class Shu:public Chess{
    ChessInfo mCenter;//九宫格中心
public:
    Shu(const ChessInfo&info);
    ~Shu();
    virtual void Selected(const ChessInfo *pInfo, uint32_t count, std::vector<ChessInfo>&canplays);
};

class Wu:public Chess{
    ChessInfo mCenter;//九宫格中心
public:
    Wu(const ChessInfo&info);
    ~Wu();
    virtual void Selected(const ChessInfo *pInfo, uint32_t count, std::vector<ChessInfo>&canplays);
};

class Han:public Chess{
public:
    Han(const ChessInfo&info);
    ~Han();
    virtual void Selected(const ChessInfo *pInfo, uint32_t count, std::vector<ChessInfo>&canplays);
};
class Bing:public Chess{
    ChessInfo mCenter[2];//九宫格中心
    glm::vec2 GetCountryBack(uint32_t country)const;
public:
    Bing(const ChessInfo&info);
    ~Bing();
    virtual void Selected(const ChessInfo *pInfo, uint32_t count, std::vector<ChessInfo>&canplays);
};
class Pao:public Chess{
    ChessInfo mCenter[3];//九宫格中心
public:
    Pao(const ChessInfo&info);
    ~Pao();
    virtual void Selected(const ChessInfo *pInfo, uint32_t count, std::vector<ChessInfo>&canplays);
};
class Che:public Chess{
    ChessInfo mCenter[3];//九宫格中心
public:
    Che(const ChessInfo&info);
    ~Che();
    virtual void Selected(const ChessInfo *pInfo, uint32_t count, std::vector<ChessInfo>&canplays);
};
class Ma:public Chess{
    ChessInfo mCenter[3];//九宫格中心
public:
    Ma(const ChessInfo&info);
    ~Ma();
    virtual void Selected(const ChessInfo *pInfo, uint32_t count, std::vector<ChessInfo>&canplays);
};
class Xiang:public Chess{
public:
    Xiang(const ChessInfo&info);
    ~Xiang();
    virtual void Selected(const ChessInfo *pInfo, uint32_t count, std::vector<ChessInfo>&canplays);
};
class Shi:public Chess{
    ChessInfo mCenter;//九宫格中心
public:
    Shi(const ChessInfo&info);
    ~Shi();
    virtual void Selected(const ChessInfo *pInfo, uint32_t count, std::vector<ChessInfo>&canplays);
};
#endif