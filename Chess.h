#ifndef CHESS_INCLUDE
#define CHESS_INCLUDE
#include <vector>
#include <cstdint>
#include "stdafx.h"
#include "glm/glm.hpp"
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
        Ma_Chess,
        Pao_Chess,
        Che_Chess,
        Shi_Chess,
        Xiang_Chess,
        Bing_Chess,
        // Han_Pao_Chess = COUNTRY_CHESS_COUNT,
        // Han_Che_Chess = COUNTRY_CHESS_COUNT + 1,
    };
protected:
    Type mChess;
    uint32_t mRow;
    uint32_t mColumn;
    Country mCountry;
    Territory mTerritory;
    uint32_t mChessOffset = 0;//用于计算棋子的uniform偏移等
    uint32_t GetTerritoryIndex()const;
    Territory GetTerritoryIndex(uint32_t row, uint32_t column)const;
    virtual void RemoveInvalidChess(const void *pBoard, std::vector<glm::vec2>&legal_moves)const;
public:
    Chess(/* args */);
    //explicit​​防止编译器进行隐式类型转换​​，要求构造函数必须显式调用。如:Chess c = Chess(...);
    explicit Chess(Country country, Territory territory, Type chess, uint32_t fontIndex, bool hasExitPermission);
    explicit Chess(Country country, Territory territory, Type chess, uint32_t fontIndex, uint32_t row, uint32_t column, bool hasExitPermission);
    virtual ~Chess();
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
    inline auto GetCountry()const{
        return mCountry;
    }
    inline auto GetTerritory()const{
        return mTerritory;
    }
    inline uint32_t GetFontIndex()const{
        return mFontIndex ;
    }
    inline void SetChess(Type chess){
        mChess = chess;
    }
    inline void SetCountry(Country country){
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

    virtual void Select(const void *pBoard, std::vector<glm::vec2>&legal_moves)const;
};
class Wei:public Chess{
public:
    Wei();
    explicit Wei(Territory territory);
    explicit Wei(Territory territory, uint32_t row, uint32_t column);
    ~Wei();
    virtual void Select(const void *pBoard, std::vector<glm::vec2>&legal_moves)const;
};

class Shu:public Chess{
public:
    Shu();
    explicit Shu(Territory territory);
    explicit Shu(Territory territory, uint32_t row, uint32_t column);
    ~Shu();
    virtual void Select(const void *pBoard, std::vector<glm::vec2>&legal_moves)const;
};

class Wu:public Chess{
public:
    Wu();
    explicit Wu(Territory territory);
    explicit Wu(Territory territory, uint32_t row, uint32_t column);
    ~Wu();
    virtual void Select(const void *pBoard, std::vector<glm::vec2>&legal_moves)const;
};

class Han:public Chess{
public:
    Han();
    explicit Han(Territory territory);
    explicit Han(Territory territory, uint32_t row, uint32_t column);
    ~Han();
    virtual void Select(const void *pBoard, std::vector<glm::vec2>&legal_moves)const;
};
class Bing:public Chess{
    glm::vec2 GetBingBack()const;
    bool IsAbroad(uint32_t row, uint32_t column)const;
public:
    Bing();
    explicit Bing(Country country, Territory territory);
    explicit Bing(Country country, Territory territory, uint32_t row, uint32_t column);
    ~Bing();
    virtual void Select(const void *pBoard, std::vector<glm::vec2>&legal_moves)const;
};
class Pao:public Chess{
    // virtual void InPalaceMove(const void *pBoard, std::vector<glm::vec2>&legal_moves)const;
public:
    Pao();
    explicit Pao(Country country, Territory territory);
    explicit Pao(Country country, Territory territory, uint32_t row, uint32_t column);
    ~Pao();
    virtual void Select(const void *pBoard, std::vector<glm::vec2>&legal_moves)const;
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
    explicit Che(Country country, Territory territory);
    explicit Che(Country country, Territory territory, uint32_t row, uint32_t column);
    ~Che();
    virtual void Select(const void *pBoard, std::vector<glm::vec2>&legal_moves)const;
};
class Ma:public Chess{
    virtual void RemoveInvalidChess(const void *pBoard, std::vector<glm::vec2>&legal_moves)const;
public:
    Ma();
    explicit Ma(Country country, Territory territory);
    explicit Ma(Country country, Territory territory, uint32_t row, uint32_t column);
    ~Ma();
    virtual void Select(const void *pBoard, std::vector<glm::vec2>&legal_moves)const;
};
class Xiang:public Chess{
    bool IsAbroad(uint32_t row, uint32_t column)const;
public:
    Xiang();
    explicit Xiang(Country country, Territory territory);
    explicit Xiang(Country country, Territory territory, uint32_t row, uint32_t column);
    ~Xiang();
    virtual void Select(const void *pBoard, std::vector<glm::vec2>&legal_moves)const;
};
class Shi:public Chess{
    void InPalaceMove(const void *pBoard, std::vector<glm::vec2>&legal_moves)const;
    public:
    Shi();
    explicit Shi(Country country, Territory territory);
    explicit Shi(Country country, Territory territory, uint32_t row, uint32_t column);
    ~Shi();
    virtual void Select(const void *pBoard, std::vector<glm::vec2>&legal_moves)const;
};
class ChaoJiBing:public Chess{
    glm::vec2 GetBingBack()const;
    bool IsAbroad(uint32_t row, uint32_t column)const;
public:
    ChaoJiBing();
    explicit ChaoJiBing(Country country, Territory territory);
    explicit ChaoJiBing(Country country, Territory territory, uint32_t row, uint32_t column);
    ~ChaoJiBing();
    virtual void Select(const void *pBoard, std::vector<glm::vec2>&legal_moves)const;
};
#endif