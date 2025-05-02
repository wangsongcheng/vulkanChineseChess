#include "Chessboard.h"
Chess *Chessboard::CreateChess(uint32_t country, Chess::Type chess, uint32_t row, uint32_t column){
    switch (chess){
    case Chess::Type::Jiang_Chess:{
        if(country == WU_COUNTRY_INDEX)return new Wu(row, column);
        else if(country == WEI_COUNTRY_INDEX)return new Wei(row, column);
        else if(country == SHU_COUNTRY_INDEX)return new Shu(row, column);
        else if(country == HAN_COUNTRY_INDEX)return new Han(row, column);
    }
    case Chess::Type::Ma_Chess:return new Ma(country, row, column);
    case Chess::Type::Pao_Chess:return new Pao(country, row, column);
    case Chess::Type::Che_Chess:return new Che(country, row, column);
    case Chess::Type::Shi_Chess:return new Shi(country, row, column);
    case Chess::Type::Xiang_Chess:return new Xiang(country, row, column);
    case Chess::Type::Bing_Chess:return new Bing(country, row, column);
    }
    return nullptr;
}
void Chessboard::InitWuChessRowAndColumn(std::array<Chess *, DRAW_CHESS_COUNT> &pChess){
    pChess[Chess::Type::Jiang_Chess]->SetPos(CHESSBOARD_ROW / 2, CHESSBOARD_COLUMN);
    std::array<int32_t, MA_CHESS_COUNT>maOffset = { -3, 3 };
    for (size_t i = 0; i < MA_CHESS_COUNT; i++){
        if(pChess[Chess::Type::Ma_Chess + i])pChess[Chess::Type::Ma_Chess + i]->SetPos(pChess[Chess::Type::Jiang_Chess]->GetRow() + maOffset[i], pChess[Chess::Type::Jiang_Chess]->GetColumn());
    }
    std::array<int32_t, SHI_CHESS_COUNT>shiOffset = { -1, 1 };
    for (size_t i = 0; i < SHI_CHESS_COUNT; i++){
        if(pChess[Chess::Type::Shi_Chess + i])pChess[Chess::Type::Shi_Chess + i]->SetPos(pChess[Chess::Type::Jiang_Chess]->GetRow() + shiOffset[i], pChess[Chess::Type::Jiang_Chess]->GetColumn());
    }
    std::array<int32_t, CHE_CHESS_COUNT>cheOffset = { -4, 4 };
    for (size_t i = 0; i < CHE_CHESS_COUNT; i++){
        if(pChess[Chess::Type::Che_Chess + i])pChess[Chess::Type::Che_Chess + i]->SetPos(pChess[Chess::Type::Jiang_Chess]->GetRow() + cheOffset[i], pChess[Chess::Type::Jiang_Chess]->GetColumn());
    }
    std::array<int32_t, XIANG_CHESS_COUNT>xiangOffset = { -2, 2 };
    for (size_t i = 0; i < XIANG_CHESS_COUNT; i++){
        if(pChess[Chess::Type::Xiang_Chess + i])pChess[Chess::Type::Xiang_Chess + i]->SetPos(pChess[Chess::Type::Jiang_Chess]->GetRow() + xiangOffset[i], pChess[Chess::Type::Jiang_Chess]->GetColumn());
    }
#ifdef ENABLE_BATTLE_FAN_GUANYU
    std::array<int32_t, PAO_CHESS_COUNT>paoOffset = { -2, 2 };
    for (size_t i = 0; i < PAO_CHESS_COUNT; i++){
        if(pChess[Chess::Type::Pao_Chess + i])pChess[Chess::Type::Pao_Chess + i]->SetPos(pChess[Chess::Type::Ma_Chess + i].->GetRow(), pChess[Chess::Type::Xiang_Chess + i * 1]->GetColumn() - 2);
    }
    std::array<int32_t, Chess::Type::Bing_Chess>bingOffset = { -2, 2 };
    if(pChess[Chess::Type::Bing_Chess])pChess[Chess::Type::Bing_Chess]->SetPos(MAX_CHESSBOARD_ROW - CHESSBOARD_BING_GRID_DENSITY, CHESSBOARD_BING_GRID_DENSITY + 1);
    for (size_t i = 1; i < BING_CHESS_COUNT; ++i){
        if(pChess[i + Chess::Type::Bing_Chess])pChess[i + Chess::Type::Bing_Chess]->SetPos(pChess[Chess::Type::Bing_Chess]->GetRow(), pChess[i + Chess::Type::Bing_Chess - 1]->GetColumn() + bingOffset);
    }
#else
    // std::array<int32_t, PAO_CHESS_COUNT>paoOffset = { 2, -2 };
    for (size_t i = 0; i < PAO_CHESS_COUNT; i++){
        if(pChess[Chess::Type::Pao_Chess + i])pChess[Chess::Type::Pao_Chess + i]->SetPos(pChess[Chess::Type::Ma_Chess + i]->GetRow(), CHESSBOARD_COLUMN - 2);
    }
    // std::array<int32_t, Chess::Type::Bing_Chess>bingOffset = { 2, -2 };
    if(pChess[Chess::Type::Bing_Chess])pChess[Chess::Type::Bing_Chess]->SetPos(CHESSBOARD_ROW - CHESSBOARD_BING_GRID_DENSITY, CHESSBOARD_COLUMN - 3);
    for (size_t i = 1; i < BING_CHESS_COUNT; ++i){
        if(pChess[i + Chess::Type::Bing_Chess])pChess[i + Chess::Type::Bing_Chess]->SetPos(pChess[i + Chess::Type::Bing_Chess - 1]->GetRow() - 2, pChess[Chess::Type::Bing_Chess]->GetColumn());
    }
#endif
}
void Chessboard::InitShuChessRowAndColumn(std::array<Chess *, DRAW_CHESS_COUNT>&pChess){
    pChess[Chess::Type::Jiang_Chess]->SetPos(CHESSBOARD_ROW, CHESSBOARD_COLUMN / 2);
    // 偏移也偏移变量(offset)不能共用,因为每个棋子的数量可能不同，也可能被修改
    std::array<int32_t, MA_CHESS_COUNT>maOffset = { -3, 3 };
    for (size_t i = 0; i < MA_CHESS_COUNT; i++){
        if(pChess[Chess::Type::Ma_Chess + i])pChess[Chess::Type::Ma_Chess + i]->SetPos(pChess[Chess::Type::Jiang_Chess]->GetRow(), pChess[Chess::Type::Jiang_Chess]->GetColumn() + maOffset[i]);
    }
    std::array<int32_t, SHI_CHESS_COUNT>shiOffset = { -1, 1 };
    for (size_t i = 0; i < SHI_CHESS_COUNT; i++){
        if(pChess[Chess::Type::Shi_Chess + i])pChess[Chess::Type::Shi_Chess + i]->SetPos(pChess[Chess::Type::Jiang_Chess]->GetRow(), pChess[Chess::Type::Jiang_Chess]->GetColumn() + shiOffset[i]);
    }
    std::array<int32_t, CHE_CHESS_COUNT>cheOffset = { -4, 4 };
    for (size_t i = 0; i < CHE_CHESS_COUNT; i++){
        if(pChess[Chess::Type::Che_Chess + i])pChess[Chess::Type::Che_Chess + i]->SetPos(pChess[Chess::Type::Jiang_Chess]->GetRow(), pChess[Chess::Type::Jiang_Chess]->GetColumn() + cheOffset[i]);
    }
    std::array<int32_t, XIANG_CHESS_COUNT>xiangOffset = { -2, 2 };
    for (size_t i = 0; i < XIANG_CHESS_COUNT; i++){
        if(pChess[Chess::Type::Xiang_Chess + i])pChess[Chess::Type::Xiang_Chess + i]->SetPos(pChess[Chess::Type::Jiang_Chess]->GetRow(), pChess[Chess::Type::Jiang_Chess]->GetColumn() + xiangOffset[i]);
    }
#ifdef ENABLE_BATTLE_FAN_GUANYU
    std::array<int32_t, PAO_CHESS_COUNT>paoOffset = { -2, 2 };
    for (size_t i = 0; i < PAO_CHESS_COUNT; i++){
        if(pChess[Chess::Type::Pao_Chess + i])pChess[Chess::Type::Pao_Chess + i]->SetPos(MAX_CHESSBOARD_ROW - 2, pChess[Chess::Type::Xiang_Chess + i * 1]->GetColumn());
    }
    std::array<int32_t, Chess::Type::Bing_Chess>bingOffset = { -2, 2 };
    if(pChess[Chess::Type::Bing_Chess])pChess[Chess::Type::Bing_Chess]->SetPos(MAX_CHESSBOARD_ROW - 3, CHESSBOARD_BING_GRID_DENSITY + 1);
    for (size_t i = 1; i < BING_CHESS_COUNT; ++i){
        if(pChess[i + Chess::Type::Bing_Chess])pChess[i + Chess::Type::Bing_Chess]->SetPos(pChess[Chess::Type::Bing_Chess]->GetRow(), pChess[i + Chess::Type::Bing_Chess - 1]->GetColumn() + bingOffset);
    }
#else
    // std::array<int32_t, PAO_CHESS_COUNT>paoOffset = { -2, 2 };
    for (size_t i = 0; i < PAO_CHESS_COUNT; i++){
        if(pChess[Chess::Type::Pao_Chess + i])pChess[Chess::Type::Pao_Chess + i]->SetPos(CHESSBOARD_ROW - 2, pChess[Chess::Type::Ma_Chess + i]->GetColumn());
    }
    // std::array<int32_t, Chess::Type::Bing_Chess>bingOffset = { -2, 2 };
    if(pChess[Chess::Type::Bing_Chess])pChess[Chess::Type::Bing_Chess]->SetPos(CHESSBOARD_ROW - 3, CHESSBOARD_BING_GRID_DENSITY);
    for (size_t i = 1; i < BING_CHESS_COUNT; ++i){
        if(pChess[i + Chess::Type::Bing_Chess])pChess[i + Chess::Type::Bing_Chess]->SetPos(pChess[Chess::Type::Bing_Chess]->GetRow(), pChess[i + Chess::Type::Bing_Chess - 1]->GetColumn() + 2);
    }
#endif
}
void Chessboard::InitWeiChessRowAndColumn(std::array<Chess *, DRAW_CHESS_COUNT>&pChess){
    pChess[Chess::Type::Jiang_Chess]->SetPos(0, CHESSBOARD_COLUMN / 2);

    std::array<int32_t, MA_CHESS_COUNT>maOffset = { 3, -3 };
    for (size_t i = 0; i < MA_CHESS_COUNT; i++){
        if(pChess[Chess::Type::Ma_Chess + i])pChess[Chess::Type::Ma_Chess + i]->SetPos(pChess[Chess::Type::Jiang_Chess]->GetRow(), pChess[Chess::Type::Jiang_Chess]->GetColumn() + maOffset[i]);
    }
    std::array<int32_t, SHI_CHESS_COUNT>shiOffset = { 1, -1 };
    for (size_t i = 0; i < SHI_CHESS_COUNT; i++){
        if(pChess[Chess::Type::Shi_Chess + i])pChess[Chess::Type::Shi_Chess + i]->SetPos(pChess[Chess::Type::Jiang_Chess]->GetRow(), pChess[Chess::Type::Jiang_Chess]->GetColumn() + shiOffset[i]);
    }
    std::array<int32_t, CHE_CHESS_COUNT>cheOffset = { 4, -4 };
    for (size_t i = 0; i < CHE_CHESS_COUNT; i++){
        if(pChess[Chess::Type::Che_Chess + i])pChess[Chess::Type::Che_Chess + i]->SetPos(pChess[Chess::Type::Jiang_Chess]->GetRow(), pChess[Chess::Type::Jiang_Chess]->GetColumn() + cheOffset[i]);
    }
    std::array<int32_t, XIANG_CHESS_COUNT>xiangOffset = { 2, -2 };
    for (size_t i = 0; i < XIANG_CHESS_COUNT; i++){
        if(pChess[Chess::Type::Xiang_Chess + i])pChess[Chess::Type::Xiang_Chess + i]->SetPos(pChess[Chess::Type::Jiang_Chess]->GetRow(), pChess[Chess::Type::Jiang_Chess]->GetColumn() + xiangOffset[i]);
    }
#ifdef ENABLE_BATTLE_FAN_GUANYU
    std::array<int32_t, PAO_CHESS_COUNT>paoOffset = { -2, 2 };
    for (size_t i = 0; i < PAO_CHESS_COUNT; i++){
        if(pChess[Chess::Type::Pao_Chess + i])pChess[Chess::Type::Pao_Chess + i]->SetPos(MAX_CHESSBOARD_ROW - 2, pChess[Chess::Type::Xiang_Chess + i * 1]->GetColumn());
    }
    std::array<int32_t, Chess::Type::Bing_Chess>bingOffset = { -2, 2 };
    if(pChess[Chess::Type::Bing_Chess])pChess[Chess::Type::Bing_Chess]->SetPos(MAX_CHESSBOARD_ROW - 3, CHESSBOARD_BING_GRID_DENSITY + 1);
    for (size_t i = 1; i < BING_CHESS_COUNT; ++i){
        if(pChess[i + Chess::Type::Bing_Chess])pChess[i + Chess::Type::Bing_Chess]->SetPos(pChess[Chess::Type::Bing_Chess]->GetRow(), pChess[i + Chess::Type::Bing_Chess - 1]->GetColumn() + bingOffset);
    }
#else
    // std::array<int32_t, PAO_CHESS_COUNT>paoOffset = { -2, 2 };
    for (size_t i = 0; i < PAO_CHESS_COUNT; i++){
        if(pChess[Chess::Type::Pao_Chess + i])pChess[Chess::Type::Pao_Chess + i]->SetPos(2, pChess[Chess::Type::Ma_Chess + i]->GetColumn());
    }
    // std::array<int32_t, Chess::Type::Bing_Chess>bingOffset = { -2, 2 };
    if(pChess[Chess::Type::Bing_Chess])pChess[Chess::Type::Bing_Chess]->SetPos(3, CHESSBOARD_COLUMN - CHESSBOARD_BING_GRID_DENSITY);
    for (size_t i = 1; i < BING_CHESS_COUNT; ++i){
        if(pChess[i + Chess::Type::Bing_Chess])pChess[i + Chess::Type::Bing_Chess]->SetPos(pChess[Chess::Type::Bing_Chess]->GetRow(), pChess[i + Chess::Type::Bing_Chess - 1]->GetColumn() - 2);
    }
#endif
}
void Chessboard::InitHanChessRowAndColumn(std::array<Chess *, DRAW_CHESS_COUNT>&pChess){
    pChess[Chess::Type::Jiang_Chess]->SetPos(CHESSBOARD_ROW / 2, 0);

    std::array<int32_t, MA_CHESS_COUNT>maOffset = { 3, -3 };
    for (size_t i = 0; i < MA_CHESS_COUNT; i++){
        if(pChess[Chess::Type::Ma_Chess + i])pChess[Chess::Type::Ma_Chess + i]->SetPos(pChess[Chess::Type::Jiang_Chess]->GetRow() + maOffset[i], pChess[Chess::Type::Jiang_Chess]->GetColumn());
    }
    std::array<int32_t, SHI_CHESS_COUNT>shiOffset = { 1, -1 };
    for (size_t i = 0; i < SHI_CHESS_COUNT; i++){
        if(pChess[Chess::Type::Shi_Chess + i])pChess[Chess::Type::Shi_Chess + i]->SetPos(pChess[Chess::Type::Jiang_Chess]->GetRow() + shiOffset[i], pChess[Chess::Type::Jiang_Chess]->GetColumn());
    }
    std::array<int32_t, CHE_CHESS_COUNT>cheOffset = { 4, -4 };
    for (size_t i = 0; i < CHE_CHESS_COUNT; i++){
        if(pChess[Chess::Type::Che_Chess + i])pChess[Chess::Type::Che_Chess + i]->SetPos(pChess[Chess::Type::Jiang_Chess]->GetRow() + cheOffset[i], pChess[Chess::Type::Jiang_Chess]->GetColumn());
    }
    std::array<int32_t, XIANG_CHESS_COUNT>xiangOffset = { 2, -2 };
    for (size_t i = 0; i < XIANG_CHESS_COUNT; i++){
        if(pChess[Chess::Type::Xiang_Chess + i])pChess[Chess::Type::Xiang_Chess + i]->SetPos(pChess[Chess::Type::Jiang_Chess]->GetRow() + xiangOffset[i], pChess[Chess::Type::Jiang_Chess]->GetColumn());
    }
#ifdef ENABLE_BATTLE_FAN_GUANYU
    std::array<int32_t, PAO_CHESS_COUNT>paoOffset = { 2, -2 };
    for (size_t i = 0; i < PAO_CHESS_COUNT; i++){
        if(pChess[Chess::Type::Pao_Chess + i])pChess[Chess::Type::Pao_Chess + i]->SetPos(pChess[Chess::Type::Ma_Chess + i].->GetRow(), pChess[Chess::Type::Xiang_Chess + i * 1]->GetColumn() - 2);
    }
    std::array<int32_t, Chess::Type::Bing_Chess>bingOffset = { 2, -2 };
    if(pChess[Chess::Type::Bing_Chess])pChess[Chess::Type::Bing_Chess]->SetPos(MAX_CHESSBOARD_ROW - CHESSBOARD_BING_GRID_DENSITY, CHESSBOARD_BING_GRID_DENSITY + 1);
    for (size_t i = 1; i < BING_CHESS_COUNT; ++i){
        if(pChess[i + Chess::Type::Bing_Chess])pChess[i + Chess::Type::Bing_Chess]->SetPos(pChess[Chess::Type::Bing_Chess]->GetRow(), pChess[i + Chess::Type::Bing_Chess - 1]->GetColumn() + bingOffset);
    }
#else
    // std::array<int32_t, PAO_CHESS_COUNT>paoOffset = { 2, -2 };
    for (size_t i = 0; i < PAO_CHESS_COUNT; i++){
        if(pChess[Chess::Type::Pao_Chess + i])pChess[Chess::Type::Pao_Chess + i]->SetPos(pChess[Chess::Type::Ma_Chess + i]->GetRow(), 2);
    }
    // std::array<int32_t, Chess::Type::Bing_Chess>bingOffset = { 2, -2 };
    if(pChess[Chess::Type::Bing_Chess])pChess[Chess::Type::Bing_Chess]->SetPos(CHESSBOARD_BING_GRID_DENSITY, 3);
    for (size_t i = 1; i < BING_CHESS_COUNT; ++i){
        if(pChess[i + Chess::Type::Bing_Chess])pChess[i + Chess::Type::Bing_Chess]->SetPos(pChess[i + Chess::Type::Bing_Chess - 1]->GetRow() + 2, pChess[Chess::Type::Bing_Chess]->GetColumn());
    }
#endif
}
void Chessboard::InitHanChessRowAndColumn(uint32_t country, std::array<Chess *, DRAW_CHESS_COUNT>&pChess){
    if(country == WU_COUNTRY_INDEX){
        pChess[Chess::Type::Jiang_Chess]->SetPos(0, CHESSBOARD_COLUMN / 2);
#ifdef ENABLE_BATTLE_FAN_GUANYU
        std::array<int32_t, HAN_CHE_CHESS_COUNT> offset = { -1, 1 };
        for (size_t i = 0; i < HAN_CHE_CHESS_COUNT; i++){
            pChess[HAN_Chess::Type::Che_Chess + i]->SetPos(pChess[Chess::Type::Jiang_Chess]->GetRow() + 2, pChess[Chess::Type::Jiang_Chess]->GetColumn() + offset[i]);
        }
#else
        std::array<int32_t, HAN_CHE_CHESS_COUNT> offset = { 2, 0, -2 };
        for (size_t i = 0; i < HAN_CHE_CHESS_COUNT; i++){
            pChess[Chess::Type::Che_Chess + i]->SetPos(pChess[Chess::Type::Jiang_Chess]->GetRow() + 3, pChess[Chess::Type::Jiang_Chess]->GetColumn() + offset[i]);
        }
#endif
        pChess[Chess::Type::Pao_Chess]->SetPos(pChess[Chess::Type::Jiang_Chess]->GetRow() + 2, pChess[Chess::Type::Jiang_Chess]->GetColumn());
    }
    else if(country == WEI_COUNTRY_INDEX){
        pChess[Chess::Type::Jiang_Chess]->SetPos(CHESSBOARD_ROW / 2, CHESSBOARD_COLUMN);
#ifdef ENABLE_BATTLE_FAN_GUANYU
        std::array<int32_t, HAN_CHE_CHESS_COUNT> offset = { -1, 1 };
        for (size_t i = 0; i < HAN_CHE_CHESS_COUNT; i++){
            pChess[HAN_Chess::Type::Che_Chess + i]->SetPos(pChess[Chess::Type::Jiang_Chess]->GetRow() + offset[i], pChess[Chess::Type::Jiang_Chess]->GetColumn() - 2);
        }
#else
        std::array<int32_t, HAN_CHE_CHESS_COUNT> offset = { 2, 0, -2 };
        for (size_t i = 0; i < HAN_CHE_CHESS_COUNT; i++){
            pChess[Chess::Type::Che_Chess + i]->SetPos(pChess[Chess::Type::Jiang_Chess]->GetRow() + offset[i], pChess[Chess::Type::Jiang_Chess]->GetColumn() - 3);
        }
#endif
        pChess[Chess::Type::Pao_Chess]->SetPos(pChess[Chess::Type::Jiang_Chess]->GetRow(), pChess[Chess::Type::Jiang_Chess]->GetColumn() - 2);
    }
    else if(country == HAN_COUNTRY_INDEX){
        pChess[Chess::Type::Jiang_Chess]->SetPos(CHESSBOARD_ROW, CHESSBOARD_COLUMN / 2);
#ifdef ENABLE_BATTLE_FAN_GUANYU
        std::array<int32_t, HAN_CHE_CHESS_COUNT> offset = { -1, 1 };
        for (size_t i = 0; i < HAN_CHE_CHESS_COUNT; i++){
            pChess[Chess::Type::Che_Chess + i]->SetPos(pChess[Chess::Type::Jiang_Chess]->GetRow() - 2, pChess[Chess::Type::Jiang_Chess]->GetColumn() + offset[i]);
        }
#else
        std::array<int32_t, HAN_CHE_CHESS_COUNT> offset = { -2, 0, 2 };
        for (size_t i = 0; i < HAN_CHE_CHESS_COUNT; i++){
            pChess[Chess::Type::Che_Chess + i]->SetPos(pChess[Chess::Type::Jiang_Chess]->GetRow() - 3, pChess[Chess::Type::Jiang_Chess]->GetColumn() + offset[i]);
        }
#endif
        pChess[Chess::Type::Pao_Chess]->SetPos(pChess[Chess::Type::Jiang_Chess]->GetRow() - 2, pChess[Chess::Type::Jiang_Chess]->GetColumn());
    }
    else{
        //那就是蜀了
        pChess[Chess::Type::Jiang_Chess]->SetPos(CHESSBOARD_ROW / 2, 0);
#ifdef ENABLE_BATTLE_FAN_GUANYU
        std::array<int32_t, HAN_CHE_CHESS_COUNT> offset = { -1, 1 };
        for (size_t i = 0; i < HAN_CHE_CHESS_COUNT; i++){
            pChess[HAN_Chess::Type::Che_Chess + i]->SetPos(pChess[Chess::Type::Jiang_Chess]->GetRow() + offset[i], pChess[Chess::Type::Jiang_Chess]->GetColumn() + 2);
        }
#else
        std::array<int32_t, HAN_CHE_CHESS_COUNT> offset = { -2, 0, 2 };
        for (size_t i = 0; i < HAN_CHE_CHESS_COUNT; i++){
            pChess[Chess::Type::Che_Chess + i]->SetPos(pChess[Chess::Type::Jiang_Chess]->GetRow() + offset[i], pChess[Chess::Type::Jiang_Chess]->GetColumn() + 3);
        }
#endif
        pChess[Chess::Type::Pao_Chess]->SetPos(pChess[Chess::Type::Jiang_Chess]->GetRow(), pChess[Chess::Type::Jiang_Chess]->GetColumn() + 2);
    }
}
Chessboard::Chessboard(/* args */){
    mRecord.reserve(MAX_UNDO_STEP);
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
    mChess[WU_COUNTRY_INDEX][Chess::Type::Jiang_Chess] = new Wu();
    mChess[WEI_COUNTRY_INDEX][Chess::Type::Jiang_Chess] = new Wei();
    mChess[SHU_COUNTRY_INDEX][Chess::Type::Jiang_Chess] = new Shu();
    mChess[HAN_COUNTRY_INDEX][Chess::Type::Jiang_Chess] = new Han();
    if(isControllable){
        for (uint32_t uiCountry = 0; uiCountry < MAX_COUNTRY_INDEX; ++uiCountry){
            for (uint32_t uiChess = 1; uiChess < COUNTRY_CHESS_COUNT; ++uiChess){
                if(uiChess >= Chess::Type::Ma_Chess && uiChess < Chess::Type::Ma_Chess + MA_CHESS_COUNT){
                    mChess[uiCountry][uiChess] = new Ma(uiCountry);
                }
                else if(uiChess >= Chess::Type::Pao_Chess && uiChess < Chess::Type::Pao_Chess + PAO_CHESS_COUNT){
                    mChess[uiCountry][uiChess] = new Pao(uiCountry);
                }
                else if(uiChess >= Chess::Type::Che_Chess && uiChess < Chess::Type::Che_Chess + CHE_CHESS_COUNT){
                    mChess[uiCountry][uiChess] = new Che(uiCountry);
                }
                else if(uiChess >= Chess::Type::Shi_Chess && uiChess < Chess::Type::Shi_Chess + SHI_CHESS_COUNT){
                    mChess[uiCountry][uiChess] = new Shi(uiCountry);
                }
                else if(uiChess >= Chess::Type::Xiang_Chess && uiChess < Chess::Type::Xiang_Chess + XIANG_CHESS_COUNT){
                    mChess[uiCountry][uiChess] = new Xiang(uiCountry);
                }
                else if(uiChess >= Chess::Type::Bing_Chess&& uiChess < Chess::Type::Bing_Chess + BING_CHESS_COUNT){
                    mChess[uiCountry][uiChess] = new Bing(uiCountry);
                }
                mChess[uiCountry][uiChess]->SetChessOffset(uiChess);
            }
        }
    }
    else{
        for (uint32_t uiCountry = 0; uiCountry < MAX_COUNTRY_INDEX; ++uiCountry){
            if(uiCountry != HAN_COUNTRY_INDEX){
                for (uint32_t offset = 1; offset < COUNTRY_CHESS_COUNT; ++offset){
                    if(offset >= Chess::Type::Ma_Chess && offset < Chess::Type::Ma_Chess + MA_CHESS_COUNT){
                        mChess[uiCountry][offset] = new Ma(uiCountry);
                    }
                    else if(offset >= Chess::Type::Pao_Chess && offset < Chess::Type::Pao_Chess + PAO_CHESS_COUNT){
                        mChess[uiCountry][offset] = new Pao(uiCountry);
                    }
                    else if(offset >= Chess::Type::Che_Chess && offset < Chess::Type::Che_Chess + CHE_CHESS_COUNT){
                        mChess[uiCountry][offset] = new Che(uiCountry);
                    }
                    else if(offset >= Chess::Type::Shi_Chess && offset < Chess::Type::Shi_Chess + SHI_CHESS_COUNT){
                        mChess[uiCountry][offset] = new Shi(uiCountry);
                    }
                    else if(offset >= Chess::Type::Xiang_Chess && offset < Chess::Type::Xiang_Chess + XIANG_CHESS_COUNT){
                        mChess[uiCountry][offset] = new Xiang(uiCountry);
                    }
                    else if(offset >= Chess::Type::Bing_Chess && offset < Chess::Type::Bing_Chess + BING_CHESS_COUNT){
                        mChess[uiCountry][offset] = new Bing(uiCountry);
                    }
                    mChess[uiCountry][offset]->SetChessOffset(offset);
                }
            }
            else{
                    mChess[uiCountry][Chess::Type::Pao_Chess] = new Pao(uiCountry);
                    for (uint32_t uiChess = Chess::Type::Che_Chess; uiChess < Chess::Type::Che_Chess + HAN_CHE_CHESS_COUNT; ++uiChess){
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
uint32_t Chessboard::IsPalaceCenter(uint32_t row, uint32_t column)const{
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
    //在移动棋子前调用，保存移动前的位置。
    //但如果存在吃子情况，则需要同时记录被吃棋子信息，也就是，极有可能需要记录2种棋子信息
    if(mRecord.size() >= MAX_UNDO_STEP){
        mRecord.erase(mRecord.begin());
    }
    std::array<Chess, 2>chess;
    chess[1].SetCountry(MAX_COUNTRY_INDEX);
    const Chess *pSelect = GetChess(srcRow, srcColumn), *pTarget = GetChess(dstRow, dstColumn);
    chess[0] = *pSelect;
    if(pTarget)chess[1] = *pTarget;
    mRecord.push_back(chess);
}
void Chessboard::UndoStep(uint32_t step){
    //撤回的步数等于sterp
    if(mRecord.empty())return;
    for (size_t i = 0; i < step; i++){
        auto last = mRecord.back();
        mChess[last[0].GetCountry()][last[0].GetChessOffset()]->SetPos(last[0].GetRow(), last[0].GetColumn());
        const uint32_t country = last[1].GetCountry();
        if(country < MAX_COUNTRY_INDEX){
            //说明这一步吃子了
            const uint32_t offset = last[1].GetChessOffset();
            mChess[country][offset] = CreateChess(country, last[1].GetChess(), last[1].GetRow(), last[1].GetColumn());
            mChess[country][offset]->SetChessOffset(offset);
        }
        mRecord.pop_back();
    }    
}
uint32_t Chessboard::IsInPalace(uint32_t row, uint32_t column) const{
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
    if(dstChess->GetChess() == Chess::Type::Jiang_Chess){
        GetCountryChess(srcCountry, dstCountry);
        DestroyCountry(dstCountry);
    }
    else{
        delete mChess[dstCountry][dstChess->GetChessOffset()];
        mChess[dstCountry][dstChess->GetChessOffset()] = nullptr;    
    }
}
void Chessboard::GetCountryChess(uint32_t srcCountry, uint32_t dstCountry){
    for (uint32_t i = 1; i < DRAW_CHESS_COUNT; ++i){
        const Chess *pChess = mChess[dstCountry][i];
        if(!pChess)continue;
        const Chess::Type chess = pChess->GetChess();
        const uint32_t offset = pChess->GetChessOffset();
        if(mChess[srcCountry][offset + COUNTRY_CHESS_COUNT]){
            mChess[srcCountry][offset + COUNTRY_CHESS_COUNT * 2] = CreateChess(srcCountry, chess, pChess->GetRow(), pChess->GetColumn());
            mChess[srcCountry][offset + COUNTRY_CHESS_COUNT * 2]->SetChessOffset(offset + COUNTRY_CHESS_COUNT * 2);
        }
        else{
            mChess[srcCountry][offset + COUNTRY_CHESS_COUNT] = CreateChess(srcCountry, chess, pChess->GetRow(), pChess->GetColumn());
            mChess[srcCountry][offset + COUNTRY_CHESS_COUNT]->SetChessOffset(offset + COUNTRY_CHESS_COUNT);
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