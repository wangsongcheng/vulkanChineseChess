#ifndef CHESS_INCLUDE
#define CHESS_INCLUDE
#include <vector>
#include <cstdint>
#include "stdafx.h"
#include "glm/glm.hpp"

#define WU_TERRITORY_INDEX WU_COUNTRY_INDEX
#define WEI_TERRITORY_INDEX WEI_COUNTRY_INDEX
#define SHU_TERRITORY_INDEX SHU_COUNTRY_INDEX
#define HAN_TERRITORY_INDEX HAN_COUNTRY_INDEX
#define CENTER_TERRITORY_INDEX (HAN_TERRITORY_INDEX + 1)
#define INVALID_TERRITORY_INDEX INVALID_COUNTRY_INDEX
//获取COUNTRY在谁的地盘;和势力不同，地盘是固定的:上魏,下蜀,左汉, 右吴
#define GET_TERRITORY_INDEX(COUNTRY, PLAYERCOUNTRY)((COUNTRY - PLAYERCOUNTRY) % MAX_COUNTRY_INDEX)
class Chess{
    glm::vec3 mPos;
    uint32_t mFontIndex;
    bool mHasExitPermission;
public:
    //一般情况不需要修改。添加新棋的话，应该从最后加索引, 也就是目前的兵
    enum Type{
        Jiang_Chess = 0,
        Ma_Chess = Jiang_Chess + 1,
        Pao_Chess = Ma_Chess + MA_CHESS_COUNT,
        Che_Chess = Pao_Chess + PAO_CHESS_COUNT,
        Shi_Chess = Che_Chess + CHE_CHESS_COUNT,
        Xiang_Chess = Shi_Chess + SHI_CHESS_COUNT,
        Bing_Chess = Xiang_Chess + XIANG_CHESS_COUNT
    };
protected:
    Type mChess;
    uint32_t mRow;
    uint32_t mColumn;
    uint32_t mCountry;
    uint32_t mChessOffset = 0;//用于计算棋子的uniform偏移等
    uint32_t mTerritory = INVALID_TERRITORY_INDEX;
    uint32_t GetTerritoryIndex()const;
    uint32_t GetTerritoryIndex(uint32_t row, uint32_t column)const;
    void RemoveInvalidChess(const void *pBoard, std::vector<glm::vec2>&canplays)const;
    virtual void InPalaceMove(const void *pBoard, std::vector<glm::vec2>&canplays)const;
public:
    Chess(/* args */);
    Chess(uint32_t country, Type chess, uint32_t fontIndex, bool hasExitPermission);
    Chess(uint32_t country, Type chess, uint32_t fontIndex, uint32_t row, uint32_t column, bool hasExitPermission);
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
    inline auto GetChess()const{
        return mChess;
    }
    inline uint32_t GetCountry()const{
        return mCountry;
    }
    inline uint32_t GetFontIndex()const{
        return mFontIndex ;
    }
    inline void SetChess(Type chess){
        mChess = chess;
    }
    inline void SetCountry(uint32_t country){
        mCountry = country;
    }
    inline void SetFontIndex(uint32_t fontIndex){
        mFontIndex = fontIndex;        
    }
    inline bool GetHasExitPermission()const{
        return mHasExitPermission;
    }
    inline void SetChessOffset(uint32_t offset){
        mChessOffset = offset;
    }
    inline auto GetChessOffset()const{
        return mChessOffset;
    }
    void SetPos(uint32_t row, uint32_t column);

    inline bool IsSelect(const glm::vec2&pos)const{
        return abs(pos.x - mPos.x) < CHESS_WIDTH && abs(pos.y - mPos.y) < CHESS_HEIGHT;
    }

    virtual void Select(const void *pBoard, std::vector<glm::vec2>&canplays)const;
};
class Wei:public Chess{
public:
    Wei();
    Wei(uint32_t row, uint32_t column);
    ~Wei();
    virtual void Select(const void *pBoard, std::vector<glm::vec2>&canplays)const;
};

class Shu:public Chess{
public:
    Shu();
    Shu(uint32_t row, uint32_t column);
    ~Shu();
    virtual void Select(const void *pBoard, std::vector<glm::vec2>&canplays)const;
};

class Wu:public Chess{
public:
    Wu();
    Wu(uint32_t row, uint32_t column);
    ~Wu();
    virtual void Select(const void *pBoard, std::vector<glm::vec2>&canplays)const;
};

class Han:public Chess{
public:
    Han();
    Han(uint32_t row, uint32_t column);
    ~Han();
    virtual void Select(const void *pBoard, std::vector<glm::vec2>&canplays)const;
};
class Bing:public Chess{
    glm::vec2 GetBingBack()const;
    bool IsAbroad(uint32_t row, uint32_t column)const;
public:
    Bing();
    Bing(uint32_t country);
    Bing(uint32_t country, uint32_t row, uint32_t column);
    ~Bing();
    virtual void Select(const void *pBoard, std::vector<glm::vec2>&canplays)const;
};
class Pao:public Chess{
    virtual void InPalaceMove(const void *pBoard, std::vector<glm::vec2>&canplays)const;
public:
    Pao();
    Pao(uint32_t country);
    Pao(uint32_t country, uint32_t row, uint32_t column);
    ~Pao();
    virtual void Select(const void *pBoard, std::vector<glm::vec2>&canplays)const;
};
class Che:public Chess{
    // //测试性函数
    // float determinant(float *m, float column);
    // void floatToMatrix(const float *m, glm::mat4&r);
    // //removeColumn不包含0
    // void RemoveRowColumn(const float *m, uint32_t removeColumn, uint32_t column, float *r);
    // void SetMatrix(const void *pBoard, float m[CHESSBOARD_ROW * CHESSBOARD_COLUMN]);
public:
    Che();
    Che(uint32_t country);
    Che(uint32_t country, uint32_t row, uint32_t column);
    ~Che();
    virtual void Select(const void *pBoard, std::vector<glm::vec2>&canplays)const;
};
class Ma:public Chess{
public:
    Ma();
    Ma(uint32_t country);
    Ma(uint32_t country, uint32_t row, uint32_t column);
    ~Ma();
    virtual void Select(const void *pBoard, std::vector<glm::vec2>&canplays)const;
};
class Xiang:public Chess{
    bool IsAbroad(uint32_t row, uint32_t column)const;
public:
    Xiang();
    Xiang(uint32_t country);
    Xiang(uint32_t country, uint32_t row, uint32_t column);
    ~Xiang();
    virtual void Select(const void *pBoard, std::vector<glm::vec2>&canplays)const;
};
class Shi:public Chess{
public:
    Shi();
    Shi(uint32_t country);
    Shi(uint32_t country, uint32_t row, uint32_t column);
    ~Shi();
    virtual void Select(const void *pBoard, std::vector<glm::vec2>&canplays)const;
};
#endif