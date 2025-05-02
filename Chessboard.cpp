#include "Chessboard.h"
void Chessboard::InitWuChessRowAndColumn(std::array<Chess *, DRAW_CHESS_COUNT>&pChess){
    pChess[JIANG_CHESS_INDEX]->SetPos(CHESSBOARD_ROW / 2, CHESSBOARD_COLUMN);
    std::array<int32_t, MA_CHESS_COUNT>maOffset = { -3, 3 };
    for (size_t i = 0; i < MA_CHESS_COUNT; i++){
        if(pChess[MA_CHESS_INDEX + i])pChess[MA_CHESS_INDEX + i]->SetPos(pChess[JIANG_CHESS_INDEX]->GetRow() + maOffset[i], pChess[JIANG_CHESS_INDEX]->GetColumn());
    }
    std::array<int32_t, SHI_CHESS_COUNT>shiOffset = { -1, 1 };
    for (size_t i = 0; i < SHI_CHESS_COUNT; i++){
        if(pChess[SHI_CHESS_INDEX + i])pChess[SHI_CHESS_INDEX + i]->SetPos(pChess[JIANG_CHESS_INDEX]->GetRow() + shiOffset[i], pChess[JIANG_CHESS_INDEX]->GetColumn());
    }
    std::array<int32_t, CHE_CHESS_COUNT>cheOffset = { -4, 4 };
    for (size_t i = 0; i < CHE_CHESS_COUNT; i++){
        if(pChess[CHE_CHESS_INDEX + i])pChess[CHE_CHESS_INDEX + i]->SetPos(pChess[JIANG_CHESS_INDEX]->GetRow() + cheOffset[i], pChess[JIANG_CHESS_INDEX]->GetColumn());
    }
    std::array<int32_t, XIANG_CHESS_COUNT>xiangOffset = { -2, 2 };
    for (size_t i = 0; i < XIANG_CHESS_COUNT; i++){
        if(pChess[XIANG_CHESS_INDEX + i])pChess[XIANG_CHESS_INDEX + i]->SetPos(pChess[JIANG_CHESS_INDEX]->GetRow() + xiangOffset[i], pChess[JIANG_CHESS_INDEX]->GetColumn());
    }
#ifdef ENABLE_BATTLE_FAN_GUANYU
    std::array<int32_t, PAO_CHESS_COUNT>paoOffset = { -2, 2 };
    for (size_t i = 0; i < PAO_CHESS_COUNT; i++){
        if(pChess[PAO_CHESS_INDEX + i])pChess[PAO_CHESS_INDEX + i]->SetPos(pChess[MA_CHESS_INDEX + i].->GetRow(), pChess[XIANG_CHESS_INDEX + i * 1]->GetColumn() - 2);
    }
    std::array<int32_t, BING_CHESS_INDEX>bingOffset = { -2, 2 };
    if(pChess[BING_CHESS_INDEX])pChess[BING_CHESS_INDEX]->SetPos(MAX_CHESSBOARD_ROW - CHESSBOARD_BING_GRID_DENSITY, CHESSBOARD_BING_GRID_DENSITY + 1);
    for (size_t i = 1; i < BING_CHESS_COUNT; ++i){
        if(pChess[i + BING_CHESS_INDEX])pChess[i + BING_CHESS_INDEX]->SetPos(pChess[BING_CHESS_INDEX]->GetRow(), pChess[i + BING_CHESS_INDEX - 1]->GetColumn() + bingOffset);
    }
#else
    // std::array<int32_t, PAO_CHESS_COUNT>paoOffset = { 2, -2 };
    for (size_t i = 0; i < PAO_CHESS_COUNT; i++){
        if(pChess[PAO_CHESS_INDEX + i])pChess[PAO_CHESS_INDEX + i]->SetPos(pChess[MA_CHESS_INDEX + i]->GetRow(), CHESSBOARD_COLUMN - 2);
    }
    // std::array<int32_t, BING_CHESS_INDEX>bingOffset = { 2, -2 };
    if(pChess[BING_CHESS_INDEX])pChess[BING_CHESS_INDEX]->SetPos(CHESSBOARD_ROW - CHESSBOARD_BING_GRID_DENSITY, CHESSBOARD_COLUMN - 3);
    for (size_t i = 1; i < BING_CHESS_COUNT; ++i){
        if(pChess[i + BING_CHESS_INDEX])pChess[i + BING_CHESS_INDEX]->SetPos(pChess[i + BING_CHESS_INDEX - 1]->GetRow() - 2, pChess[BING_CHESS_INDEX]->GetColumn());
    }
#endif
}
void Chessboard::InitShuChessRowAndColumn(std::array<Chess *, DRAW_CHESS_COUNT>&pChess){
    pChess[JIANG_CHESS_INDEX]->SetPos(CHESSBOARD_ROW, CHESSBOARD_COLUMN / 2);
    // 偏移也偏移变量(offset)不能共用,因为每个棋子的数量可能不同，也可能被修改
    std::array<int32_t, MA_CHESS_COUNT>maOffset = { -3, 3 };
    for (size_t i = 0; i < MA_CHESS_COUNT; i++){
        if(pChess[MA_CHESS_INDEX + i])pChess[MA_CHESS_INDEX + i]->SetPos(pChess[JIANG_CHESS_INDEX]->GetRow(), pChess[JIANG_CHESS_INDEX]->GetColumn() + maOffset[i]);
    }
    std::array<int32_t, SHI_CHESS_COUNT>shiOffset = { -1, 1 };
    for (size_t i = 0; i < SHI_CHESS_COUNT; i++){
        if(pChess[SHI_CHESS_INDEX + i])pChess[SHI_CHESS_INDEX + i]->SetPos(pChess[JIANG_CHESS_INDEX]->GetRow(), pChess[JIANG_CHESS_INDEX]->GetColumn() + shiOffset[i]);
    }
    std::array<int32_t, CHE_CHESS_COUNT>cheOffset = { -4, 4 };
    for (size_t i = 0; i < CHE_CHESS_COUNT; i++){
        if(pChess[CHE_CHESS_INDEX + i])pChess[CHE_CHESS_INDEX + i]->SetPos(pChess[JIANG_CHESS_INDEX]->GetRow(), pChess[JIANG_CHESS_INDEX]->GetColumn() + cheOffset[i]);
    }
    std::array<int32_t, XIANG_CHESS_COUNT>xiangOffset = { -2, 2 };
    for (size_t i = 0; i < XIANG_CHESS_COUNT; i++){
        if(pChess[XIANG_CHESS_INDEX + i])pChess[XIANG_CHESS_INDEX + i]->SetPos(pChess[JIANG_CHESS_INDEX]->GetRow(), pChess[JIANG_CHESS_INDEX]->GetColumn() + xiangOffset[i]);
    }
#ifdef ENABLE_BATTLE_FAN_GUANYU
    std::array<int32_t, PAO_CHESS_COUNT>paoOffset = { -2, 2 };
    for (size_t i = 0; i < PAO_CHESS_COUNT; i++){
        if(pChess[PAO_CHESS_INDEX + i])pChess[PAO_CHESS_INDEX + i]->SetPos(MAX_CHESSBOARD_ROW - 2, pChess[XIANG_CHESS_INDEX + i * 1]->GetColumn());
    }
    std::array<int32_t, BING_CHESS_INDEX>bingOffset = { -2, 2 };
    if(pChess[BING_CHESS_INDEX])pChess[BING_CHESS_INDEX]->SetPos(MAX_CHESSBOARD_ROW - 3, CHESSBOARD_BING_GRID_DENSITY + 1);
    for (size_t i = 1; i < BING_CHESS_COUNT; ++i){
        if(pChess[i + BING_CHESS_INDEX])pChess[i + BING_CHESS_INDEX]->SetPos(pChess[BING_CHESS_INDEX]->GetRow(), pChess[i + BING_CHESS_INDEX - 1]->GetColumn() + bingOffset);
    }
#else
    // std::array<int32_t, PAO_CHESS_COUNT>paoOffset = { -2, 2 };
    for (size_t i = 0; i < PAO_CHESS_COUNT; i++){
        if(pChess[PAO_CHESS_INDEX + i])pChess[PAO_CHESS_INDEX + i]->SetPos(CHESSBOARD_ROW - 2, pChess[MA_CHESS_INDEX + i]->GetColumn());
    }
    // std::array<int32_t, BING_CHESS_INDEX>bingOffset = { -2, 2 };
    if(pChess[BING_CHESS_INDEX])pChess[BING_CHESS_INDEX]->SetPos(CHESSBOARD_ROW - 3, CHESSBOARD_BING_GRID_DENSITY);
    for (size_t i = 1; i < BING_CHESS_COUNT; ++i){
        if(pChess[i + BING_CHESS_INDEX])pChess[i + BING_CHESS_INDEX]->SetPos(pChess[BING_CHESS_INDEX]->GetRow(), pChess[i + BING_CHESS_INDEX - 1]->GetColumn() + 2);
    }
#endif
}
void Chessboard::InitWeiChessRowAndColumn(std::array<Chess *, DRAW_CHESS_COUNT>&pChess){
    pChess[JIANG_CHESS_INDEX]->SetPos(0, CHESSBOARD_COLUMN / 2);

    std::array<int32_t, MA_CHESS_COUNT>maOffset = { 3, -3 };
    for (size_t i = 0; i < MA_CHESS_COUNT; i++){
        if(pChess[MA_CHESS_INDEX + i])pChess[MA_CHESS_INDEX + i]->SetPos(pChess[JIANG_CHESS_INDEX]->GetRow(), pChess[JIANG_CHESS_INDEX]->GetColumn() + maOffset[i]);
    }
    std::array<int32_t, SHI_CHESS_COUNT>shiOffset = { 1, -1 };
    for (size_t i = 0; i < SHI_CHESS_COUNT; i++){
        if(pChess[SHI_CHESS_INDEX + i])pChess[SHI_CHESS_INDEX + i]->SetPos(pChess[JIANG_CHESS_INDEX]->GetRow(), pChess[JIANG_CHESS_INDEX]->GetColumn() + shiOffset[i]);
    }
    std::array<int32_t, CHE_CHESS_COUNT>cheOffset = { 4, -4 };
    for (size_t i = 0; i < CHE_CHESS_COUNT; i++){
        if(pChess[CHE_CHESS_INDEX + i])pChess[CHE_CHESS_INDEX + i]->SetPos(pChess[JIANG_CHESS_INDEX]->GetRow(), pChess[JIANG_CHESS_INDEX]->GetColumn() + cheOffset[i]);
    }
    std::array<int32_t, XIANG_CHESS_COUNT>xiangOffset = { 2, -2 };
    for (size_t i = 0; i < XIANG_CHESS_COUNT; i++){
        if(pChess[XIANG_CHESS_INDEX + i])pChess[XIANG_CHESS_INDEX + i]->SetPos(pChess[JIANG_CHESS_INDEX]->GetRow(), pChess[JIANG_CHESS_INDEX]->GetColumn() + xiangOffset[i]);
    }
#ifdef ENABLE_BATTLE_FAN_GUANYU
    std::array<int32_t, PAO_CHESS_COUNT>paoOffset = { -2, 2 };
    for (size_t i = 0; i < PAO_CHESS_COUNT; i++){
        if(pChess[PAO_CHESS_INDEX + i])pChess[PAO_CHESS_INDEX + i]->SetPos(MAX_CHESSBOARD_ROW - 2, pChess[XIANG_CHESS_INDEX + i * 1]->GetColumn());
    }
    std::array<int32_t, BING_CHESS_INDEX>bingOffset = { -2, 2 };
    if(pChess[BING_CHESS_INDEX])pChess[BING_CHESS_INDEX]->SetPos(MAX_CHESSBOARD_ROW - 3, CHESSBOARD_BING_GRID_DENSITY + 1);
    for (size_t i = 1; i < BING_CHESS_COUNT; ++i){
        if(pChess[i + BING_CHESS_INDEX])pChess[i + BING_CHESS_INDEX]->SetPos(pChess[BING_CHESS_INDEX]->GetRow(), pChess[i + BING_CHESS_INDEX - 1]->GetColumn() + bingOffset);
    }
#else
    // std::array<int32_t, PAO_CHESS_COUNT>paoOffset = { -2, 2 };
    for (size_t i = 0; i < PAO_CHESS_COUNT; i++){
        if(pChess[PAO_CHESS_INDEX + i])pChess[PAO_CHESS_INDEX + i]->SetPos(2, pChess[MA_CHESS_INDEX + i]->GetColumn());
    }
    // std::array<int32_t, BING_CHESS_INDEX>bingOffset = { -2, 2 };
    if(pChess[BING_CHESS_INDEX])pChess[BING_CHESS_INDEX]->SetPos(3, CHESSBOARD_COLUMN - CHESSBOARD_BING_GRID_DENSITY);
    for (size_t i = 1; i < BING_CHESS_COUNT; ++i){
        if(pChess[i + BING_CHESS_INDEX])pChess[i + BING_CHESS_INDEX]->SetPos(pChess[BING_CHESS_INDEX]->GetRow(), pChess[i + BING_CHESS_INDEX - 1]->GetColumn() - 2);
    }
#endif
}
void Chessboard::InitHanChessRowAndColumn(std::array<Chess *, DRAW_CHESS_COUNT>&pChess){
    pChess[JIANG_CHESS_INDEX]->SetPos(CHESSBOARD_ROW / 2, 0);

    std::array<int32_t, MA_CHESS_COUNT>maOffset = { 3, -3 };
    for (size_t i = 0; i < MA_CHESS_COUNT; i++){
        if(pChess[MA_CHESS_INDEX + i])pChess[MA_CHESS_INDEX + i]->SetPos(pChess[JIANG_CHESS_INDEX]->GetRow() + maOffset[i], pChess[JIANG_CHESS_INDEX]->GetColumn());
    }
    std::array<int32_t, SHI_CHESS_COUNT>shiOffset = { 1, -1 };
    for (size_t i = 0; i < SHI_CHESS_COUNT; i++){
        if(pChess[SHI_CHESS_INDEX + i])pChess[SHI_CHESS_INDEX + i]->SetPos(pChess[JIANG_CHESS_INDEX]->GetRow() + shiOffset[i], pChess[JIANG_CHESS_INDEX]->GetColumn());
    }
    std::array<int32_t, CHE_CHESS_COUNT>cheOffset = { 4, -4 };
    for (size_t i = 0; i < CHE_CHESS_COUNT; i++){
        if(pChess[CHE_CHESS_INDEX + i])pChess[CHE_CHESS_INDEX + i]->SetPos(pChess[JIANG_CHESS_INDEX]->GetRow() + cheOffset[i], pChess[JIANG_CHESS_INDEX]->GetColumn());
    }
    std::array<int32_t, XIANG_CHESS_COUNT>xiangOffset = { 2, -2 };
    for (size_t i = 0; i < XIANG_CHESS_COUNT; i++){
        if(pChess[XIANG_CHESS_INDEX + i])pChess[XIANG_CHESS_INDEX + i]->SetPos(pChess[JIANG_CHESS_INDEX]->GetRow() + xiangOffset[i], pChess[JIANG_CHESS_INDEX]->GetColumn());
    }
#ifdef ENABLE_BATTLE_FAN_GUANYU
    std::array<int32_t, PAO_CHESS_COUNT>paoOffset = { 2, -2 };
    for (size_t i = 0; i < PAO_CHESS_COUNT; i++){
        if(pChess[PAO_CHESS_INDEX + i])pChess[PAO_CHESS_INDEX + i]->SetPos(pChess[MA_CHESS_INDEX + i].->GetRow(), pChess[XIANG_CHESS_INDEX + i * 1]->GetColumn() - 2);
    }
    std::array<int32_t, BING_CHESS_INDEX>bingOffset = { 2, -2 };
    if(pChess[BING_CHESS_INDEX])pChess[BING_CHESS_INDEX]->SetPos(MAX_CHESSBOARD_ROW - CHESSBOARD_BING_GRID_DENSITY, CHESSBOARD_BING_GRID_DENSITY + 1);
    for (size_t i = 1; i < BING_CHESS_COUNT; ++i){
        if(pChess[i + BING_CHESS_INDEX])pChess[i + BING_CHESS_INDEX]->SetPos(pChess[BING_CHESS_INDEX]->GetRow(), pChess[i + BING_CHESS_INDEX - 1]->GetColumn() + bingOffset);
    }
#else
    // std::array<int32_t, PAO_CHESS_COUNT>paoOffset = { 2, -2 };
    for (size_t i = 0; i < PAO_CHESS_COUNT; i++){
        if(pChess[PAO_CHESS_INDEX + i])pChess[PAO_CHESS_INDEX + i]->SetPos(pChess[MA_CHESS_INDEX + i]->GetRow(), 2);
    }
    // std::array<int32_t, BING_CHESS_INDEX>bingOffset = { 2, -2 };
    if(pChess[BING_CHESS_INDEX])pChess[BING_CHESS_INDEX]->SetPos(CHESSBOARD_BING_GRID_DENSITY, 3);
    for (size_t i = 1; i < BING_CHESS_COUNT; ++i){
        if(pChess[i + BING_CHESS_INDEX])pChess[i + BING_CHESS_INDEX]->SetPos(pChess[i + BING_CHESS_INDEX - 1]->GetRow() + 2, pChess[BING_CHESS_INDEX]->GetColumn());
    }
#endif
}
void Chessboard::InitHanChessRowAndColumn(uint32_t country, std::array<Chess *, DRAW_CHESS_COUNT>&pChess){
    if(country == WU_COUNTRY_INDEX){
        pChess[JIANG_CHESS_INDEX]->SetPos(0, CHESSBOARD_COLUMN / 2);
#ifdef ENABLE_BATTLE_FAN_GUANYU
        std::array<int32_t, HAN_CHE_CHESS_COUNT> offset = { -1, 1 };
        for (size_t i = 0; i < HAN_CHE_CHESS_COUNT; i++){
            pChess[HAN_CHE_CHESS_INDEX + i]->SetPos(pChess[JIANG_CHESS_INDEX]->GetRow() + 2, pChess[JIANG_CHESS_INDEX]->GetColumn() + offset[i]);
        }
#else
        std::array<int32_t, HAN_CHE_CHESS_COUNT> offset = { 2, 0, -2 };
        for (size_t i = 0; i < HAN_CHE_CHESS_COUNT; i++){
            pChess[HAN_CHE_CHESS_INDEX + i]->SetPos(pChess[JIANG_CHESS_INDEX]->GetRow() + 3, pChess[JIANG_CHESS_INDEX]->GetColumn() + offset[i]);
        }
#endif
        pChess[HAN_PAO_CHESS_INDEX]->SetPos(pChess[JIANG_CHESS_INDEX]->GetRow() + 2, pChess[JIANG_CHESS_INDEX]->GetColumn());
    }
    else if(country == WEI_COUNTRY_INDEX){
        pChess[JIANG_CHESS_INDEX]->SetPos(CHESSBOARD_ROW / 2, CHESSBOARD_COLUMN);
#ifdef ENABLE_BATTLE_FAN_GUANYU
        std::array<int32_t, HAN_CHE_CHESS_COUNT> offset = { -1, 1 };
        for (size_t i = 0; i < HAN_CHE_CHESS_COUNT; i++){
            pChess[HAN_CHE_CHESS_INDEX + i]->SetPos(pChess[JIANG_CHESS_INDEX]->GetRow() + offset[i], pChess[JIANG_CHESS_INDEX]->GetColumn() - 2);
        }
#else
        std::array<int32_t, HAN_CHE_CHESS_COUNT> offset = { 2, 0, -2 };
        for (size_t i = 0; i < HAN_CHE_CHESS_COUNT; i++){
            pChess[HAN_CHE_CHESS_INDEX + i]->SetPos(pChess[JIANG_CHESS_INDEX]->GetRow() + offset[i], pChess[JIANG_CHESS_INDEX]->GetColumn() - 3);
        }
#endif
        pChess[HAN_PAO_CHESS_INDEX]->SetPos(pChess[JIANG_CHESS_INDEX]->GetRow(), pChess[JIANG_CHESS_INDEX]->GetColumn() - 2);
    }
    else if(country == HAN_COUNTRY_INDEX){
        pChess[JIANG_CHESS_INDEX]->SetPos(CHESSBOARD_ROW, CHESSBOARD_COLUMN / 2);
#ifdef ENABLE_BATTLE_FAN_GUANYU
        std::array<int32_t, HAN_CHE_CHESS_COUNT> offset = { -1, 1 };
        for (size_t i = 0; i < HAN_CHE_CHESS_COUNT; i++){
            pChess[HAN_CHE_CHESS_INDEX + i]->SetPos(pChess[JIANG_CHESS_INDEX]->GetRow() - 2, pChess[JIANG_CHESS_INDEX]->GetColumn() + offset[i]);
        }
#else
        std::array<int32_t, HAN_CHE_CHESS_COUNT> offset = { -2, 0, 2 };
        for (size_t i = 0; i < HAN_CHE_CHESS_COUNT; i++){
            pChess[HAN_CHE_CHESS_INDEX + i]->SetPos(pChess[JIANG_CHESS_INDEX]->GetRow() - 3, pChess[JIANG_CHESS_INDEX]->GetColumn() + offset[i]);
        }
#endif
        pChess[HAN_PAO_CHESS_INDEX]->SetPos(pChess[JIANG_CHESS_INDEX]->GetRow() - 2, pChess[JIANG_CHESS_INDEX]->GetColumn());
    }
    else{
        //那就是蜀了
        pChess[JIANG_CHESS_INDEX]->SetPos(CHESSBOARD_ROW / 2, 0);
#ifdef ENABLE_BATTLE_FAN_GUANYU
        std::array<int32_t, HAN_CHE_CHESS_COUNT> offset = { -1, 1 };
        for (size_t i = 0; i < HAN_CHE_CHESS_COUNT; i++){
            pChess[HAN_CHE_CHESS_INDEX + i]->SetPos(pChess[JIANG_CHESS_INDEX]->GetRow() + offset[i], pChess[JIANG_CHESS_INDEX]->GetColumn() + 2);
        }
#else
        std::array<int32_t, HAN_CHE_CHESS_COUNT> offset = { -2, 0, 2 };
        for (size_t i = 0; i < HAN_CHE_CHESS_COUNT; i++){
            pChess[HAN_CHE_CHESS_INDEX + i]->SetPos(pChess[JIANG_CHESS_INDEX]->GetRow() + offset[i], pChess[JIANG_CHESS_INDEX]->GetColumn() + 3);
        }
#endif
        pChess[HAN_PAO_CHESS_INDEX]->SetPos(pChess[JIANG_CHESS_INDEX]->GetRow(), pChess[JIANG_CHESS_INDEX]->GetColumn() + 2);
    }
}
Chessboard::Chessboard(/* args */){
}

Chessboard::~Chessboard(){
}
void Chessboard::InitializeChess(uint32_t playerCountry, bool isControllable, uint32_t countryCount){
    mCountryCount = countryCount;
    mPalacesCenter[HAN_TERRITORY_INDEX] = glm::vec2(1, CHESSBOARD_ROW / 2);
    mPalacesCenter[WEI_TERRITORY_INDEX] = glm::vec2(CHESSBOARD_COLUMN / 2, 1);
    mPalacesCenter[WU_TERRITORY_INDEX] = glm::vec2(CHESSBOARD_COLUMN - 1, CHESSBOARD_ROW / 2);
    mPalacesCenter[SHU_TERRITORY_INDEX] = glm::vec2(CHESSBOARD_COLUMN / 2, CHESSBOARD_ROW - 1);
    for (uint32_t uiCountry = 0; uiCountry < MAX_COUNTRY_INDEX; ++uiCountry){
        DestroyCountry(uiCountry);
    }
    mChess[WU_COUNTRY_INDEX][JIANG_CHESS_INDEX] = new Wu();
    mChess[WEI_COUNTRY_INDEX][JIANG_CHESS_INDEX] = new Wei();
    mChess[SHU_COUNTRY_INDEX][JIANG_CHESS_INDEX] = new Shu();
    mChess[HAN_COUNTRY_INDEX][JIANG_CHESS_INDEX] = new Han();
    if(isControllable){
        for (uint32_t uiCountry = 0; uiCountry < MAX_COUNTRY_INDEX; ++uiCountry){
            for (uint32_t uiChess = 0; uiChess < COUNTRY_CHESS_COUNT; ++uiChess){
                if(uiChess >= MA_CHESS_INDEX && uiChess < MA_CHESS_INDEX + MA_CHESS_COUNT){
                    mChess[uiCountry][uiChess] = new Ma(uiCountry);
                    mChess[uiCountry][uiChess]->SetChessOffset(uiChess);
                }
                else if(uiChess >= PAO_CHESS_INDEX && uiChess < PAO_CHESS_INDEX + PAO_CHESS_COUNT){
                    mChess[uiCountry][uiChess] = new Pao(uiCountry);
                    mChess[uiCountry][uiChess]->SetChessOffset(uiChess);
                }
                else if(uiChess >= CHE_CHESS_INDEX && uiChess < CHE_CHESS_INDEX + CHE_CHESS_COUNT){
                    mChess[uiCountry][uiChess] = new Che(uiCountry);
                    mChess[uiCountry][uiChess]->SetChessOffset(uiChess);
                }
                else if(uiChess >= SHI_CHESS_INDEX && uiChess < SHI_CHESS_INDEX + SHI_CHESS_COUNT){
                    mChess[uiCountry][uiChess] = new Shi(uiCountry);
                    mChess[uiCountry][uiChess]->SetChessOffset(uiChess);
                }
                else if(uiChess >= XIANG_CHESS_INDEX && uiChess < XIANG_CHESS_INDEX + XIANG_CHESS_COUNT){
                    mChess[uiCountry][uiChess] = new Xiang(uiCountry);
                    mChess[uiCountry][uiChess]->SetChessOffset(uiChess);
                }
                else if(uiChess >= BING_CHESS_INDEX&& uiChess < BING_CHESS_INDEX + BING_CHESS_COUNT){
                    mChess[uiCountry][uiChess] = new Bing(uiCountry);
                    mChess[uiCountry][uiChess]->SetChessOffset(uiChess);
                }
            }
        }
    }
    else{
        for (uint32_t uiCountry = 0; uiCountry < MAX_COUNTRY_INDEX; ++uiCountry){
            if(uiCountry != HAN_COUNTRY_INDEX){
                for (uint32_t uiChess = MA_CHESS_INDEX; uiChess < COUNTRY_CHESS_COUNT; ++uiChess){
                    if(uiChess >= MA_CHESS_INDEX && uiChess < MA_CHESS_INDEX + MA_CHESS_COUNT){
                        mChess[uiCountry][uiChess] = new Ma(uiCountry);
                       mChess[uiCountry][uiChess]->SetChessOffset(uiChess);
                }
                    else if(uiChess >= PAO_CHESS_INDEX && uiChess < PAO_CHESS_INDEX + PAO_CHESS_COUNT){
                        mChess[uiCountry][uiChess] = new Pao(uiCountry);
                        mChess[uiCountry][uiChess]->SetChessOffset(uiChess);
                    }
                    else if(uiChess >= CHE_CHESS_INDEX && uiChess < CHE_CHESS_INDEX + CHE_CHESS_COUNT){
                        mChess[uiCountry][uiChess] = new Che(uiCountry);
                        mChess[uiCountry][uiChess]->SetChessOffset(uiChess);
                    }
                    else if(uiChess >= SHI_CHESS_INDEX && uiChess < SHI_CHESS_INDEX + SHI_CHESS_COUNT){
                        mChess[uiCountry][uiChess] = new Shi(uiCountry);
                        mChess[uiCountry][uiChess]->SetChessOffset(uiChess);
                    }
                    else if(uiChess >= XIANG_CHESS_INDEX && uiChess < XIANG_CHESS_INDEX + XIANG_CHESS_COUNT){
                        mChess[uiCountry][uiChess] = new Xiang(uiCountry);
                        mChess[uiCountry][uiChess]->SetChessOffset(uiChess);
                    }
                    else if(uiChess >= BING_CHESS_INDEX&& uiChess < BING_CHESS_INDEX + BING_CHESS_COUNT){
                        mChess[uiCountry][uiChess] = new Bing(uiCountry);
                        mChess[uiCountry][uiChess]->SetChessOffset(uiChess);
                    }
                }
            }
            else{
                    mChess[uiCountry][HAN_PAO_CHESS_INDEX] = new Pao(uiCountry);
                    for (uint32_t uiChess = HAN_CHE_CHESS_INDEX; uiChess < HAN_CHE_CHESS_INDEX + HAN_CHE_CHESS_COUNT; ++uiChess){
                        mChess[uiCountry][uiChess] = new Che(uiCountry);
                        mChess[uiCountry][uiChess]->SetChessOffset(uiChess);
                    }
            }
        }
    }
    /*
        以蜀为参照
        上魏下蜀，左汉右吴
    */
    //把函数名中的国家坐标复制给chessInfo[playerCountry]
    InitShuChessRowAndColumn(mChess[playerCountry]);
    if(!isControllable)InitHanChessRowAndColumn(playerCountry, mChess[HAN_COUNTRY_INDEX]);
    if(WU_COUNTRY_INDEX == playerCountry){
        InitWuChessRowAndColumn(mChess[WEI_COUNTRY_INDEX]);
        InitHanChessRowAndColumn(mChess[SHU_COUNTRY_INDEX]);
        if(isControllable)InitWeiChessRowAndColumn(mChess[HAN_COUNTRY_INDEX]);
    }
    else if(WEI_COUNTRY_INDEX == playerCountry){
        InitHanChessRowAndColumn(mChess[WU_COUNTRY_INDEX]);
        InitWeiChessRowAndColumn(mChess[SHU_COUNTRY_INDEX]);
        if(isControllable)InitWuChessRowAndColumn(mChess[HAN_COUNTRY_INDEX]);
    }
    else if(SHU_COUNTRY_INDEX == playerCountry){
        InitWuChessRowAndColumn(mChess[WU_COUNTRY_INDEX]);
        InitWeiChessRowAndColumn(mChess[WEI_COUNTRY_INDEX]);
        if(isControllable)InitHanChessRowAndColumn(mChess[HAN_COUNTRY_INDEX]);
    }
    else if(HAN_COUNTRY_INDEX == playerCountry){
        if(isControllable){
            InitWeiChessRowAndColumn(mChess[WU_COUNTRY_INDEX]);
            InitHanChessRowAndColumn(mChess[WEI_COUNTRY_INDEX]);
            InitWuChessRowAndColumn(mChess[SHU_COUNTRY_INDEX]);
        }
    }
}
int32_t Chessboard::IsPalaceCenter(uint32_t row, uint32_t column)const{
    uint32_t country = INVALID_COUNTRY_INDEX;
    for (size_t i = 0; i < MAX_COUNTRY_INDEX; ++i){
        if(row == mPalacesCenter[i].y && column == mPalacesCenter[i].x){
            // printf("%d国,%d行%d列的棋子:%d, 在中心为%d行%d列的九宫格内\n", mInfo.country, row, column, mInfo.chess, center[i].GetRow(), center[i].GetColumn());
            country = i;
            break;
        }   
    }
    return country;
}
bool Chessboard::IsHasExitPermission(uint32_t country){
    bool has = false;
    for (uint32_t uiChess = 0; uiChess < DRAW_CHESS_COUNT; ++uiChess){
        if(mChess[country][uiChess] && mChess[country][uiChess]->GetHasExitPermission()){
            has = true;
            break;
        }
    }
    return has;
}
void Chessboard::SaveStep(uint32_t srcRow, uint32_t srcColumn, uint32_t dstRow, uint32_t dstColumn){
    //这个功能目前没有用到
    //想写撤回功能的话, 还需要记录是不是吃子。另外，棋子被吃后也别直接销毁棋子，而是加一个标记然后不显示
    if(mRecord.size() >= MAX_UNDO_STEP){
        mRecord.erase(mRecord.begin());
    }
    mRecord.push_back(glm::vec4(srcRow, srcColumn, dstRow, dstColumn));
}
void Chessboard::UndoStep(uint32_t step){
    //撤回的步数等于sterp
    // if(mRecord.size() > 0){
    //     auto last = mRecord.back();
    //     mRecord.pop_back();
    //     PlayChess(g_Game.GetChess(last.x, last.y), last.z, last.w);
    // }
}
int32_t Chessboard::IsInPalace(uint32_t row, uint32_t column) const{
    int32_t country = IsPalaceCenter(row, column);
    if(country != INVALID_COUNTRY_INDEX)return country;
    for (size_t i = 0; i < MAX_COUNTRY_INDEX; ++i){
        if(abs((float)row - mPalacesCenter[i].y) < 2 && abs((float)column - mPalacesCenter[i].x) < 2){
            country = i;
            break;
        }
    }
    return country;
}
void Chessboard::CaptureChess(const Chess *srcChess, const Chess *dstChess){
    const uint32_t srcCountry = srcChess->GetCountry(), dstCountry = dstChess->GetCountry();
    if(dstChess->GetChess() == JIANG_CHESS_INDEX){
        GetCountryChess(srcCountry, dstCountry);
        DestroyCountry(dstCountry);
    }
    delete mChess[dstCountry][dstChess->GetChessOffset()];
    mChess[dstCountry][dstChess->GetChessOffset()] = nullptr;
}
void Chessboard::GetCountryChess(uint32_t srcCountry, uint32_t dstCountry){
    uint32_t fontIndex = 0;
    for (uint32_t i = JIANG_CHESS_INDEX + 1; i < DRAW_CHESS_COUNT; ++i){
        Chess *pChess = mChess[dstCountry][i];
        if(!pChess)continue;
        const uint32_t uiChess = pChess->GetChess(), offset = pChess->GetChessOffset();
        if(uiChess == MA_CHESS_INDEX){
            fontIndex = FONT_INDEX_MA;
            if(mChess[srcCountry][offset + COUNTRY_CHESS_COUNT]){
                mChess[srcCountry][offset + COUNTRY_CHESS_COUNT * 2] = new Ma(srcCountry, pChess->GetRow(), pChess->GetColumn());
            }
            else{
                mChess[srcCountry][offset + COUNTRY_CHESS_COUNT] = new Ma(srcCountry, pChess->GetRow(), pChess->GetColumn());
            }
        }
        else if(uiChess == PAO_CHESS_INDEX || uiChess == HAN_PAO_CHESS_INDEX){
            fontIndex = FONT_INDEX_PAO;
            if(mChess[srcCountry][offset + COUNTRY_CHESS_COUNT]){
                mChess[srcCountry][offset + COUNTRY_CHESS_COUNT * 2] = new Pao(srcCountry, pChess->GetRow(), pChess->GetColumn());
            }
            else{
                mChess[srcCountry][offset + COUNTRY_CHESS_COUNT] = new Pao(srcCountry, pChess->GetRow(), pChess->GetColumn());
            }
        }
        else if(uiChess == CHE_CHESS_INDEX || uiChess == HAN_CHE_CHESS_INDEX){
            fontIndex = FONT_INDEX_CHE;
            if(mChess[srcCountry][offset + COUNTRY_CHESS_COUNT]){
                mChess[srcCountry][offset + COUNTRY_CHESS_COUNT * 2] = new Che(srcCountry, pChess->GetRow(), pChess->GetColumn());
            }
            else{
                mChess[srcCountry][offset + COUNTRY_CHESS_COUNT] = new Che(srcCountry, pChess->GetRow(), pChess->GetColumn());
            }
        }
        else if(uiChess == SHI_CHESS_INDEX){
            fontIndex = FONT_INDEX_SHI;
            if(mChess[srcCountry][offset + COUNTRY_CHESS_COUNT]){
                mChess[srcCountry][offset + COUNTRY_CHESS_COUNT * 2] = new Shi(srcCountry, pChess->GetRow(), pChess->GetColumn());
            }
            else{
                mChess[srcCountry][offset + COUNTRY_CHESS_COUNT] = new Shi(srcCountry, pChess->GetRow(), pChess->GetColumn());
            }
        }
        else if(uiChess == XIANG_CHESS_INDEX){
            fontIndex = FONT_INDEX_XIANG;
            if(mChess[srcCountry][offset + COUNTRY_CHESS_COUNT]){
                mChess[srcCountry][offset + COUNTRY_CHESS_COUNT * 2] = new Xiang(srcCountry, pChess->GetRow(), pChess->GetColumn());
            }
            else{
                mChess[srcCountry][offset + COUNTRY_CHESS_COUNT] = new Xiang(srcCountry, pChess->GetRow(), pChess->GetColumn());
            }
        }
        else if(uiChess == BING_CHESS_INDEX){
            fontIndex = FONT_INDEX_BING;
            if(mChess[srcCountry][offset + COUNTRY_CHESS_COUNT]){
                mChess[srcCountry][offset + COUNTRY_CHESS_COUNT * 2] = new Bing(srcCountry, pChess->GetRow(), pChess->GetColumn());
            }
            else{
                mChess[srcCountry][offset + COUNTRY_CHESS_COUNT] = new Bing(srcCountry, pChess->GetRow(), pChess->GetColumn());
            }
        }
        if(mChess[srcCountry][offset + COUNTRY_CHESS_COUNT]){
            mChess[srcCountry][offset + COUNTRY_CHESS_COUNT]->SetChessOffset(offset + COUNTRY_CHESS_COUNT);
        }
        else if(mChess[srcCountry][offset + COUNTRY_CHESS_COUNT * 2]){
            mChess[srcCountry][offset + COUNTRY_CHESS_COUNT * 2]->SetChessOffset(offset + COUNTRY_CHESS_COUNT * 2);
        }
    }
}
// const Chess *Chessboard::Check(uint32_t srcCountry, uint32_t dstCountry, uint32_t chess)const{
//     if(srcCountry == dstCountry)return nullptr;
//     for (size_t uiChess = 0; uiChess < DRAW_CHESS_COUNT; ++uiChess){
//         std::vector<glm::vec2>canplays;
//         if(mChess[srcCountry][uiChess]){
//             mChess[srcCountry][uiChess]->Select(this, canplays);
//             for(auto it:canplays){
//                 const Chess *pChess = GetChess(it.y, it.x);
//                 if(pChess && pChess->GetChess() == chess && pChess->GetCountry() == dstCountry){
//                     return mChess[srcCountry][uiChess];
//                 }
//             }
//         }             
//     }
//     return nullptr;
// }
uint32_t Chessboard::GetChessCount(uint32_t country)const{
    uint32_t count = 0;
    for (size_t i = 0; i < DRAW_CHESS_COUNT; i++){
        if(mChess[country][i])++count;
    }
    return count;
}
Chess *Chessboard::GetChess(uint32_t row, uint32_t column) const{
    Chess *pChess = nullptr;
    for (uint32_t uiCountry = 0; uiCountry < MAX_COUNTRY_INDEX; ++uiCountry){
        pChess = GetChess(uiCountry, row, column);
        if(pChess)break;
    }
    return pChess;
}
Chess *Chessboard::GetChess(uint32_t country, const glm::vec2&mousePos)const{
    Chess *pChess = nullptr;
    for (size_t uiChess = 0; uiChess < DRAW_CHESS_COUNT; ++uiChess){
        if(mChess[country][uiChess] && mChess[country][uiChess]->IsSelect(mousePos)){
            pChess = mChess[country][uiChess];
            break;
        }
    }
    return pChess;
}

Chess *Chessboard::GetChess(uint32_t country, uint32_t row, uint32_t column)const{
    Chess *pChess = nullptr;
    if(country != INVALID_COUNTRY_INDEX){
        for (uint32_t uiChess = 0; uiChess < DRAW_CHESS_COUNT; ++uiChess){
            if(mChess[country][uiChess] && mChess[country][uiChess]->GetRow() == row && mChess[country][uiChess]->GetColumn() == column){
                pChess = mChess[country][uiChess];
                break;
            }
        }
    }
    return pChess;
}
const Chess *Chessboard::Check(uint32_t country, uint32_t dstRow, uint32_t dstColumn) const{
    for (size_t uiChess = 0; uiChess < DRAW_CHESS_COUNT; ++uiChess){
        std::vector<glm::vec2>canplays;
        if(mChess[country][uiChess]){
            //目前无法处理炮，先跳过
            if(mChess[country][uiChess]->GetChess() == PAO_CHESS_INDEX)continue;
            mChess[country][uiChess]->Select(this, canplays);
            for(auto it:canplays){
                if(it.x == dstColumn && dstRow == it.y){
                    return mChess[country][uiChess];
                }
            }
        }             
    }
    return nullptr;
}
// void Chessboard::Select(const Chess *pChess, std::vector<glm::vec2> &canplays){
//     pChess->Select(this, canplays);
//     for (auto it = canplays.begin(); it != canplays.end(); ++it){
//         const Chess *pChess = GetChess(it->y, it->x);
//         if(pChess && pChess->GetCountry() == pChess->GetCountry()){
//             it = canplays.erase(it);
//         }
//     }
//     //炮是必须获得棋盘的情况
//     // 可能需要判断位置是否合法
//     //而且，如果不是判断是否越界或者位置是否是队友的话，就非常麻烦
// }
// void Chessboard::Select(uint32_t country, uint32_t chess, std::vector<glm::vec2> &canplays){
//     Select(mChess[country][chess], canplays);
// }
void Chessboard::DestroyCountry(uint32_t country){
    for (size_t uiChess = 0; uiChess < DRAW_CHESS_COUNT; ++uiChess){
        if(mChess[country][uiChess]){
            delete mChess[country][uiChess];
            mChess[country][uiChess] = nullptr;
        }
    }
}