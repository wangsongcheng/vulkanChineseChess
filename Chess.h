#ifndef CHESS_INCLUDE
#define CHESS_INCLUDE
#include <vector>
#include <cstdint>
//需要用到以下文件几个宏, 所以包含
#include "VulkanChess.h"
#include "VulkanChessboard.h"
#define JIANG_CHESS_INDEX 0
#define MA_CHESS_INDEX (JIANG_CHESS_INDEX + JIANG_CHESS_COUNT)
#define PAO_CHESS_INDEX (MA_CHESS_INDEX + MA_CHESS_COUNT)
#define CHE_CHESS_INDEX (PAO_CHESS_INDEX + PAO_CHESS_COUNT)
#define SHI_CHESS_INDEX (CHE_CHESS_INDEX + CHE_CHESS_COUNT)
#define XIANG_CHESS_INDEX (SHI_CHESS_INDEX + SHI_CHESS_COUNT)
#define BING_CHESS_INDEX (XIANG_CHESS_INDEX + XIANG_CHESS_COUNT)

#define HAN_PAO_CHESS_INDEX 1
#define HAN_CHE_CHESS_INDEX (HAN_PAO_CHESS_INDEX + HAN_PAO_CHESS_COUNT)

#define WU_TERRITORY_INDEX WU_COUNTRY_INDEX
#define WEI_TERRITORY_INDEX WEI_COUNTRY_INDEX
#define SHU_TERRITORY_INDEX SHU_COUNTRY_INDEX
#define HAN_TERRITORY_INDEX HAN_COUNTRY_INDEX
#define CENTER_TERRITORY_INDEX MAX_COUNTRY_INDEX
#define INVALID_TERRITORY_INDEX INVALID_COUNTRY_INDEX

#define GET_TERRITORY_INDEX(COUNTRY, PLAYERCOUNTRY)((COUNTRY - PLAYERCOUNTRY) % 4)
class Chess{
    uint32_t mRow;
    glm::vec3 mPos;
    uint32_t mChess;//一般用上面的宏赋值(*_CHESS_INDEX_*)
    uint32_t mColumn;
    uint32_t mCountry;
    uint32_t mFontIndex;
protected:
    uint32_t mTerriory;
public:
    Chess(/* args */);
    Chess(uint32_t row, uint32_t column);
    Chess(uint32_t chess, uint32_t country, uint32_t row, uint32_t column);
    Chess(uint32_t chess, uint32_t country, uint32_t fontIndex, uint32_t row, uint32_t column);
    ~Chess();
    inline uint32_t GetRow()const{
        return mRow;
    }
    inline glm::vec2 GetPos()const{
        return mPos;
    }
    inline uint32_t GetColumn()const{
        return mColumn;
    }
    inline uint32_t GetChess()const{
        return mChess;
    }
    inline uint32_t GetCountry()const{
        return mCountry;
    }
    inline uint32_t GetFontIndex()const{
        return mFontIndex ;
    }
    inline void SetChess(uint32_t chess){
        mChess = chess;
    }
    inline void SetCountry(uint32_t country){
        mCountry = country;
    }
    inline void SetFontIndex(uint32_t fontIndex){
        mFontIndex = fontIndex;        
    }
    void SetPos(uint32_t row, uint32_t column);

    inline bool IsSelect(const glm::vec2&pos)const{
        return abs(pos.x - mPos.x) < CHESS_WIDTH && abs(pos.y - mPos.y) < CHESS_HEIGHT;
    }

    virtual void Selected(Chess *pChess[MAX_COUNTRY_INDEX][DRAW_COUNTRY_CHESS_COUNT], std::vector<Chess>&canplays)const;
    // virtual void Selected(Chess *pChess[MAX_COUNTRY_INDEX][DRAW_COUNTRY_CHESS_COUNT], std::vector<Chess>&canplays)const = 0;
};
class Wei:public Chess{
public:
    Wei();
    Wei(uint32_t row, uint32_t column);
    Wei(uint32_t chess, uint32_t country, uint32_t row, uint32_t column);
    Wei(uint32_t chess, uint32_t country, uint32_t fontIndex, uint32_t row, uint32_t column);
    ~Wei();
    virtual void Selected(Chess *pChess[MAX_COUNTRY_INDEX][DRAW_COUNTRY_CHESS_COUNT], std::vector<Chess>&canplays)const;
};

class Shu:public Chess{
public:
    Shu();
    Shu(uint32_t row, uint32_t column);
    Shu(uint32_t chess, uint32_t country, uint32_t row, uint32_t column);
    Shu(uint32_t chess, uint32_t country, uint32_t fontIndex, uint32_t row, uint32_t column);
    ~Shu();
    virtual void Selected(Chess *pChess[MAX_COUNTRY_INDEX][DRAW_COUNTRY_CHESS_COUNT], std::vector<Chess>&canplays)const;
};

class Wu:public Chess{
public:
    Wu();
    Wu(uint32_t row, uint32_t column);
    Wu(uint32_t chess, uint32_t country, uint32_t row, uint32_t column);
    Wu(uint32_t chess, uint32_t country, uint32_t fontIndex, uint32_t row, uint32_t column);
    ~Wu();
    virtual void Selected(Chess *pChess[MAX_COUNTRY_INDEX][DRAW_COUNTRY_CHESS_COUNT], std::vector<Chess>&canplays)const;
};

class Han:public Chess{
public:
    Han();
    Han(uint32_t row, uint32_t column);
    Han(uint32_t chess, uint32_t country, uint32_t row, uint32_t column);
    Han(uint32_t chess, uint32_t country, uint32_t fontIndex, uint32_t row, uint32_t column);
    ~Han();
    virtual void Selected(Chess *pChess[MAX_COUNTRY_INDEX][DRAW_COUNTRY_CHESS_COUNT], std::vector<Chess>&canplays)const;
};
class Bing:public Chess{
    glm::vec2 GetBingBack()const;
    bool IsAbroad(uint32_t row, uint32_t column)const;
public:
    Bing();
    Bing(uint32_t row, uint32_t column);
    Bing(uint32_t chess, uint32_t country, uint32_t row, uint32_t column);
    Bing(uint32_t chess, uint32_t country, uint32_t fontIndex, uint32_t row, uint32_t column);
    ~Bing();
    virtual void Selected(Chess *pChess[MAX_COUNTRY_INDEX][DRAW_COUNTRY_CHESS_COUNT], std::vector<Chess>&canplays)const;
};
class Pao:public Chess{
public:
    Pao();
    Pao(uint32_t row, uint32_t column);
    Pao(uint32_t chess, uint32_t country, uint32_t row, uint32_t column);
    Pao(uint32_t chess, uint32_t country, uint32_t fontIndex, uint32_t row, uint32_t column);
    ~Pao();
    virtual void Selected(Chess *pChess[MAX_COUNTRY_INDEX][DRAW_COUNTRY_CHESS_COUNT], std::vector<Chess>&canplays)const;
};
class Che:public Chess{
public:
    Che();
    Che(uint32_t row, uint32_t column);
    Che(uint32_t chess, uint32_t country, uint32_t row, uint32_t column);
    Che(uint32_t chess, uint32_t country, uint32_t fontIndex, uint32_t row, uint32_t column);
    ~Che();
    virtual void Selected(Chess *pChess[MAX_COUNTRY_INDEX][DRAW_COUNTRY_CHESS_COUNT], std::vector<Chess>&canplays)const;
};
class Ma:public Chess{
public:
    Ma();
    Ma(uint32_t row, uint32_t column);
    Ma(uint32_t chess, uint32_t country, uint32_t row, uint32_t column);
    Ma(uint32_t chess, uint32_t country, uint32_t fontIndex, uint32_t row, uint32_t column);
    ~Ma();
    virtual void Selected(Chess *pChess[MAX_COUNTRY_INDEX][DRAW_COUNTRY_CHESS_COUNT], std::vector<Chess>&canplays)const;
};
class Xiang:public Chess{
    bool IsAbroad(uint32_t row, uint32_t column)const;
public:
    Xiang();
    Xiang(uint32_t row, uint32_t column);
    Xiang(uint32_t chess, uint32_t country, uint32_t row, uint32_t column);
    Xiang(uint32_t chess, uint32_t country, uint32_t fontIndex, uint32_t row, uint32_t column);
    ~Xiang();
    virtual void Selected(Chess *pChess[MAX_COUNTRY_INDEX][DRAW_COUNTRY_CHESS_COUNT], std::vector<Chess>&canplays)const;
};
class Shi:public Chess{
public:
    Shi();
    Shi(uint32_t row, uint32_t column);
    Shi(uint32_t chess, uint32_t country, uint32_t row, uint32_t column);
    Shi(uint32_t chess, uint32_t country, uint32_t fontIndex, uint32_t row, uint32_t column);
    ~Shi();
    virtual void Selected(Chess *pChess[MAX_COUNTRY_INDEX][DRAW_COUNTRY_CHESS_COUNT], std::vector<Chess>&canplays)const;
};
//main.cpp需要用到
uint32_t GetTerritoryIndex(uint32_t row, uint32_t column);
#endif