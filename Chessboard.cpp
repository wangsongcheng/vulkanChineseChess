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
    case Chess::Type::Xiang_Chess:return new Xiang(country, row, column);
    case Chess::Type::Shi_Chess:return new Shi(country, row, column);
    case Chess::Type::Bing_Chess:return new Bing(country, row, column);
    }
    return nullptr;
}
uint32_t Chessboard::GetCountryCount(){
    uint32_t count = 0;
    for (size_t i = 0; i < MAX_COUNTRY_INDEX; i++){
        if(!IsDeath(i))++count;
    }
    return count;
}
void Chessboard::InitWuChessRowAndColumn(std::array<Chess *, DRAW_CHESS_COUNT> &pChess){
    pChess[JIANG_CHESS_OFFSET]->SetPos(CHESSBOARD_ROW / 2, CHESSBOARD_COLUMN);
#ifdef ENABLE_BATTLE_FAN_GUANYU
    std::array<int32_t, MA_CHESS_COUNT>maOffset = { 3, -3, 0 };
    for (size_t i = 0; i < MA_CHESS_COUNT; i++){
        if(pChess[MA_CHESS_OFFSET + i])pChess[MA_CHESS_OFFSET + i]->SetPos(pChess[JIANG_CHESS_OFFSET]->GetRow() + maOffset[i], i == MA_CHESS_COUNT - 1?CHESSBOARD_COLUMN - 2:pChess[JIANG_CHESS_OFFSET]->GetColumn());
    }
#else
    std::array<int32_t, MA_CHESS_COUNT>maOffset = { 3, -3 };
    for (size_t i = 0; i < MA_CHESS_COUNT; i++){
        if(pChess[MA_CHESS_OFFSET + i])pChess[MA_CHESS_OFFSET + i]->SetPos(pChess[JIANG_CHESS_OFFSET]->GetRow() + maOffset[i], pChess[JIANG_CHESS_OFFSET]->GetColumn());
    }
#endif
    std::array<int32_t, SHI_CHESS_COUNT>shiOffset = { 1, -1 };
    for (size_t i = 0; i < SHI_CHESS_COUNT; i++){
        if(pChess[SHI_CHESS_OFFSET + i])pChess[SHI_CHESS_OFFSET + i]->SetPos(pChess[JIANG_CHESS_OFFSET]->GetRow() + shiOffset[i], pChess[JIANG_CHESS_OFFSET]->GetColumn());
    }
#ifdef ENABLE_BATTLE_FAN_GUANYU
    std::array<int32_t, PAO_CHESS_COUNT>paoOffset = { -2, 2 };
    for (size_t i = 0; i < PAO_CHESS_COUNT; i++){
        if(pChess[PAO_CHESS_OFFSET + i])pChess[PAO_CHESS_OFFSET + i]->SetPos(pChess[JIANG_CHESS_OFFSET]->GetRow() + paoOffset[i], pChess[JIANG_CHESS_OFFSET]->GetColumn());
    }
    // std::array<int32_t, Chess::Type::Bing_Chess>bingOffset = { -2, 2 };
    if(pChess[BING_CHESS_OFFSET])pChess[BING_CHESS_OFFSET]->SetPos(CHESSBOARD_ROW - CHESSBOARD_BING_GRID_DENSITY - 1, CHESSBOARD_COLUMN - 3);
    for (size_t i = 1; i < BING_CHESS_COUNT; ++i){
        if(pChess[i + BING_CHESS_OFFSET])pChess[i + BING_CHESS_OFFSET]->SetPos(pChess[i + BING_CHESS_OFFSET - 1]->GetRow() - 2, pChess[BING_CHESS_OFFSET]->GetColumn());
    }
#else
    std::array<int32_t, CHE_CHESS_COUNT>cheOffset = { -4, 4 };
    for (size_t i = 0; i < CHE_CHESS_COUNT; i++){
        if(pChess[CHE_CHESS_OFFSET + i])pChess[CHE_CHESS_OFFSET + i]->SetPos(pChess[JIANG_CHESS_OFFSET]->GetRow() + cheOffset[i], pChess[JIANG_CHESS_OFFSET]->GetColumn());
    }
    std::array<int32_t, XIANG_CHESS_COUNT>xiangOffset = { -2, 2 };
    for (size_t i = 0; i < XIANG_CHESS_COUNT; i++){
        if(pChess[XIANG_CHESS_OFFSET + i])pChess[XIANG_CHESS_OFFSET + i]->SetPos(pChess[JIANG_CHESS_OFFSET]->GetRow() + xiangOffset[i], pChess[JIANG_CHESS_OFFSET]->GetColumn());
    }
    // std::array<int32_t, PAO_CHESS_COUNT>paoOffset = { 2, -2 };
    for (size_t i = 0; i < PAO_CHESS_COUNT; i++){
        if(pChess[PAO_CHESS_OFFSET + i])pChess[PAO_CHESS_OFFSET + i]->SetPos(pChess[MA_CHESS_OFFSET + i]->GetRow(), CHESSBOARD_COLUMN - 2);
    }
    // std::array<int32_t, BING_CHESS_OFFSET>bingOffset = { 2, -2 };
    if(pChess[BING_CHESS_OFFSET])pChess[BING_CHESS_OFFSET]->SetPos(CHESSBOARD_ROW - CHESSBOARD_BING_GRID_DENSITY, CHESSBOARD_COLUMN - 3);
    for (size_t i = 1; i < BING_CHESS_COUNT; ++i){
        if(pChess[i + BING_CHESS_OFFSET])pChess[i + BING_CHESS_OFFSET]->SetPos(pChess[i + BING_CHESS_OFFSET - 1]->GetRow() - 2, pChess[BING_CHESS_OFFSET]->GetColumn());
    }
#endif
}
void Chessboard::InitShuChessRowAndColumn(std::array<Chess *, DRAW_CHESS_COUNT>&pChess){
    pChess[JIANG_CHESS_OFFSET]->SetPos(CHESSBOARD_ROW, CHESSBOARD_COLUMN / 2);
    // 偏移也偏移变量(offset)不能共用,因为每个棋子的数量可能不同，也可能被修改
#ifdef ENABLE_BATTLE_FAN_GUANYU
    std::array<int32_t, MA_CHESS_COUNT>maOffset = { -3, 3, 0 };
    for (size_t i = 0; i < MA_CHESS_COUNT; i++){
        if(pChess[MA_CHESS_OFFSET + i])pChess[MA_CHESS_OFFSET + i]->SetPos(i ==MA_CHESS_COUNT - 1?CHESSBOARD_ROW - 2:pChess[JIANG_CHESS_OFFSET]->GetRow(), pChess[JIANG_CHESS_OFFSET]->GetColumn() + maOffset[i]);
    }
#else
    std::array<int32_t, MA_CHESS_COUNT>maOffset = { -3, 3 };
    for (size_t i = 0; i < MA_CHESS_COUNT; i++){
        if(pChess[MA_CHESS_OFFSET + i])pChess[MA_CHESS_OFFSET + i]->SetPos(pChess[JIANG_CHESS_OFFSET]->GetRow(), pChess[JIANG_CHESS_OFFSET]->GetColumn() + maOffset[i]);
    }
#endif
    std::array<int32_t, SHI_CHESS_COUNT>shiOffset = { -1, 1 };
    for (size_t i = 0; i < SHI_CHESS_COUNT; i++){
        if(pChess[SHI_CHESS_OFFSET + i])pChess[SHI_CHESS_OFFSET + i]->SetPos(pChess[JIANG_CHESS_OFFSET]->GetRow(), pChess[JIANG_CHESS_OFFSET]->GetColumn() + shiOffset[i]);
    }
#ifdef ENABLE_BATTLE_FAN_GUANYU
    std::array<int32_t, PAO_CHESS_COUNT>paoOffset = { -2, 2 };
    for (size_t i = 0; i < PAO_CHESS_COUNT; i++){
        if(pChess[PAO_CHESS_OFFSET + i])pChess[PAO_CHESS_OFFSET + i]->SetPos(pChess[JIANG_CHESS_OFFSET]->GetRow(), pChess[JIANG_CHESS_OFFSET]->GetColumn() + paoOffset[i]);
    }
    std::array<int32_t, BING_CHESS_OFFSET>bingOffset = { -2, 2 };
    if(pChess[BING_CHESS_OFFSET])pChess[BING_CHESS_OFFSET]->SetPos(CHESSBOARD_ROW - 3, CHESSBOARD_BING_GRID_DENSITY + 1);
    for (size_t i = 1; i < BING_CHESS_COUNT; ++i){
        if(pChess[i + BING_CHESS_OFFSET])pChess[i + BING_CHESS_OFFSET]->SetPos(pChess[BING_CHESS_OFFSET]->GetRow(), pChess[i + BING_CHESS_OFFSET - 1]->GetColumn() + 2);
    }
#else
    std::array<int32_t, CHE_CHESS_COUNT>cheOffset = { -4, 4 };
    for (size_t i = 0; i < CHE_CHESS_COUNT; i++){
        if(pChess[CHE_CHESS_OFFSET + i])pChess[CHE_CHESS_OFFSET + i]->SetPos(pChess[JIANG_CHESS_OFFSET]->GetRow(), pChess[JIANG_CHESS_OFFSET]->GetColumn() + cheOffset[i]);
    }
    std::array<int32_t, XIANG_CHESS_COUNT>xiangOffset = { -2, 2 };
    for (size_t i = 0; i < XIANG_CHESS_COUNT; i++){
        if(pChess[XIANG_CHESS_OFFSET + i])pChess[XIANG_CHESS_OFFSET + i]->SetPos(pChess[JIANG_CHESS_OFFSET]->GetRow(), pChess[JIANG_CHESS_OFFSET]->GetColumn() + xiangOffset[i]);
    }
    // std::array<int32_t, PAO_CHESS_COUNT>paoOffset = { -2, 2 };
    for (size_t i = 0; i < PAO_CHESS_COUNT; i++){
        if(pChess[PAO_CHESS_OFFSET + i])pChess[PAO_CHESS_OFFSET + i]->SetPos(CHESSBOARD_ROW - 2, pChess[MA_CHESS_OFFSET + i]->GetColumn());
    }
    // std::array<int32_t, BING_CHESS_OFFSET>bingOffset = { -2, 2 };
    if(pChess[BING_CHESS_OFFSET])pChess[BING_CHESS_OFFSET]->SetPos(CHESSBOARD_ROW - 3, CHESSBOARD_BING_GRID_DENSITY);
    for (size_t i = 1; i < BING_CHESS_COUNT; ++i){
        if(pChess[i + BING_CHESS_OFFSET])pChess[i + BING_CHESS_OFFSET]->SetPos(pChess[BING_CHESS_OFFSET]->GetRow(), pChess[i + BING_CHESS_OFFSET - 1]->GetColumn() + 2);
    }
#endif
}
void Chessboard::InitWeiChessRowAndColumn(std::array<Chess *, DRAW_CHESS_COUNT>&pChess){
    pChess[JIANG_CHESS_OFFSET]->SetPos(0, CHESSBOARD_COLUMN / 2);
#ifdef ENABLE_BATTLE_FAN_GUANYU
    std::array<int32_t, MA_CHESS_COUNT>maOffset = { 3, -3, 0 };
    for (size_t i = 0; i < MA_CHESS_COUNT; i++){
        if(pChess[MA_CHESS_OFFSET + i])pChess[MA_CHESS_OFFSET + i]->SetPos(i == MA_CHESS_COUNT - 1?2:pChess[JIANG_CHESS_OFFSET]->GetRow(), pChess[JIANG_CHESS_OFFSET]->GetColumn() + maOffset[i]);
    }
#else
    std::array<int32_t, MA_CHESS_COUNT>maOffset = { 3, -3 };
    for (size_t i = 0; i < MA_CHESS_COUNT; i++){
        if(pChess[MA_CHESS_OFFSET + i])pChess[MA_CHESS_OFFSET + i]->SetPos(pChess[JIANG_CHESS_OFFSET]->GetRow(), pChess[JIANG_CHESS_OFFSET]->GetColumn() + maOffset[i]);
    }
#endif
    std::array<int32_t, SHI_CHESS_COUNT>shiOffset = { 1, -1 };
    for (size_t i = 0; i < SHI_CHESS_COUNT; i++){
        if(pChess[SHI_CHESS_OFFSET + i])pChess[SHI_CHESS_OFFSET + i]->SetPos(pChess[JIANG_CHESS_OFFSET]->GetRow(), pChess[JIANG_CHESS_OFFSET]->GetColumn() + shiOffset[i]);
    }
#ifdef ENABLE_BATTLE_FAN_GUANYU
    std::array<int32_t, PAO_CHESS_COUNT>paoOffset = { 2, -2 };
    for (size_t i = 0; i < PAO_CHESS_COUNT; i++){
        if(pChess[PAO_CHESS_OFFSET + i])pChess[PAO_CHESS_OFFSET + i]->SetPos(pChess[JIANG_CHESS_OFFSET]->GetRow(), pChess[JIANG_CHESS_OFFSET]->GetColumn() + paoOffset[i]);
    }
    std::array<int32_t, BING_CHESS_OFFSET>bingOffset = { -2, 2 };
    if(pChess[BING_CHESS_OFFSET])pChess[BING_CHESS_OFFSET]->SetPos(3, CHESSBOARD_COLUMN - CHESSBOARD_BING_GRID_DENSITY - 1);
    for (size_t i = 1; i < BING_CHESS_COUNT; ++i){
        if(pChess[i + BING_CHESS_OFFSET])pChess[i + BING_CHESS_OFFSET]->SetPos(pChess[BING_CHESS_OFFSET]->GetRow(), pChess[i + BING_CHESS_OFFSET - 1]->GetColumn() - 2);
    }
#else
    std::array<int32_t, CHE_CHESS_COUNT>cheOffset = { -4, 4 };
    for (size_t i = 0; i < CHE_CHESS_COUNT; i++){
        if(pChess[CHE_CHESS_OFFSET + i])pChess[CHE_CHESS_OFFSET + i]->SetPos(pChess[JIANG_CHESS_OFFSET]->GetRow(), pChess[JIANG_CHESS_OFFSET]->GetColumn() + cheOffset[i]);
    }
    std::array<int32_t, XIANG_CHESS_COUNT>xiangOffset = { -2, 2 };
    for (size_t i = 0; i < XIANG_CHESS_COUNT; i++){
        if(pChess[XIANG_CHESS_OFFSET + i])pChess[XIANG_CHESS_OFFSET + i]->SetPos(pChess[JIANG_CHESS_OFFSET]->GetRow(), pChess[JIANG_CHESS_OFFSET]->GetColumn() + xiangOffset[i]);
    }
    // std::array<int32_t, PAO_CHESS_COUNT>paoOffset = { -2, 2 };
    for (size_t i = 0; i < PAO_CHESS_COUNT; i++){
        if(pChess[PAO_CHESS_OFFSET + i])pChess[PAO_CHESS_OFFSET + i]->SetPos(2, pChess[MA_CHESS_OFFSET + i]->GetColumn());
    }
    // std::array<int32_t, BING_CHESS_OFFSET>bingOffset = { -2, 2 };
    if(pChess[BING_CHESS_OFFSET])pChess[BING_CHESS_OFFSET]->SetPos(3, CHESSBOARD_COLUMN - CHESSBOARD_BING_GRID_DENSITY);
    for (size_t i = 1; i < BING_CHESS_COUNT; ++i){
        if(pChess[i + BING_CHESS_OFFSET])pChess[i + BING_CHESS_OFFSET]->SetPos(pChess[BING_CHESS_OFFSET]->GetRow(), pChess[i + BING_CHESS_OFFSET - 1]->GetColumn() - 2);
    }
#endif
}
void Chessboard::InitHanChessRowAndColumn(std::array<Chess *, DRAW_CHESS_COUNT>&pChess){
    pChess[JIANG_CHESS_OFFSET]->SetPos(CHESSBOARD_ROW / 2, 0);
#ifdef ENABLE_BATTLE_FAN_GUANYU
    std::array<int32_t, MA_CHESS_COUNT>maOffset = { -3, 3, 0 };
    for (size_t i = 0; i < MA_CHESS_COUNT; i++){
        if(pChess[MA_CHESS_OFFSET + i])pChess[MA_CHESS_OFFSET + i]->SetPos(pChess[JIANG_CHESS_OFFSET]->GetRow() + maOffset[i], i == MA_CHESS_COUNT - 1?2:pChess[JIANG_CHESS_OFFSET]->GetColumn());
    }
#else
    std::array<int32_t, MA_CHESS_COUNT>maOffset = { -3, 3 };
    for (size_t i = 0; i < MA_CHESS_COUNT; i++){
        if(pChess[MA_CHESS_OFFSET + i])pChess[MA_CHESS_OFFSET + i]->SetPos(pChess[JIANG_CHESS_OFFSET]->GetRow() + maOffset[i], pChess[JIANG_CHESS_OFFSET]->GetColumn());
    }
#endif
    std::array<int32_t, SHI_CHESS_COUNT>shiOffset = { -1, 1 };
    for (size_t i = 0; i < SHI_CHESS_COUNT; i++){
        if(pChess[SHI_CHESS_OFFSET + i])pChess[SHI_CHESS_OFFSET + i]->SetPos(pChess[JIANG_CHESS_OFFSET]->GetRow() + shiOffset[i], pChess[JIANG_CHESS_OFFSET]->GetColumn());
    }
#ifdef ENABLE_BATTLE_FAN_GUANYU
    std::array<int32_t, PAO_CHESS_COUNT>paoOffset = { -2, 2 };
    for (size_t i = 0; i < PAO_CHESS_COUNT; i++){
        if(pChess[PAO_CHESS_OFFSET + i])pChess[PAO_CHESS_OFFSET + i]->SetPos(pChess[JIANG_CHESS_OFFSET]->GetRow() + paoOffset[i], pChess[JIANG_CHESS_OFFSET]->GetColumn());
    }
    if(pChess[BING_CHESS_OFFSET])pChess[BING_CHESS_OFFSET]->SetPos(CHESSBOARD_BING_GRID_DENSITY + 1, 3);
    for (size_t i = 1; i < BING_CHESS_COUNT; ++i){
        if(pChess[i + BING_CHESS_OFFSET])pChess[i + BING_CHESS_OFFSET]->SetPos(pChess[i + BING_CHESS_OFFSET - 1]->GetRow() + 2, pChess[BING_CHESS_OFFSET]->GetColumn());
    }
#else
    std::array<int32_t, CHE_CHESS_COUNT>cheOffset = { -4, 4 };
    for (size_t i = 0; i < CHE_CHESS_COUNT; i++){
        if(pChess[CHE_CHESS_OFFSET + i])pChess[CHE_CHESS_OFFSET + i]->SetPos(pChess[JIANG_CHESS_OFFSET]->GetRow() + cheOffset[i], pChess[JIANG_CHESS_OFFSET]->GetColumn());
    }
    std::array<int32_t, XIANG_CHESS_COUNT>xiangOffset = { -2, 2 };
    for (size_t i = 0; i < XIANG_CHESS_COUNT; i++){
        if(pChess[XIANG_CHESS_OFFSET + i])pChess[XIANG_CHESS_OFFSET + i]->SetPos(pChess[JIANG_CHESS_OFFSET]->GetRow() + xiangOffset[i], pChess[JIANG_CHESS_OFFSET]->GetColumn());
    }
    // std::array<int32_t, PAO_CHESS_COUNT>paoOffset = { 2, -2 };
    for (size_t i = 0; i < PAO_CHESS_COUNT; i++){
        if(pChess[PAO_CHESS_OFFSET + i])pChess[PAO_CHESS_OFFSET + i]->SetPos(pChess[MA_CHESS_OFFSET + i]->GetRow(), 2);
    }
    // std::array<int32_t, BING_CHESS_OFFSET>bingOffset = { 2, -2 };
    if(pChess[BING_CHESS_OFFSET])pChess[BING_CHESS_OFFSET]->SetPos(CHESSBOARD_BING_GRID_DENSITY, 3);
    for (size_t i = 1; i < BING_CHESS_COUNT; ++i){
        if(pChess[i + BING_CHESS_OFFSET])pChess[i + BING_CHESS_OFFSET]->SetPos(pChess[i + BING_CHESS_OFFSET - 1]->GetRow() + 2, pChess[BING_CHESS_OFFSET]->GetColumn());
    }
#endif
}
void Chessboard::InitHanChessRowAndColumn(uint32_t country, std::array<Chess *, DRAW_CHESS_COUNT>&pChess){
    if(country == WU_COUNTRY_INDEX){
        pChess[JIANG_CHESS_OFFSET]->SetPos(0, CHESSBOARD_COLUMN / 2);
        std::array<int32_t, HAN_CHE_CHESS_COUNT> offset = { 2, 0, -2 };
#ifdef ENABLE_BATTLE_FAN_GUANYU
        for (size_t i = 0; i < HAN_CHE_CHESS_COUNT; i++){
            pChess[HAN_CHE_CHESS_OFFSET + i]->SetPos(pChess[JIANG_CHESS_OFFSET]->GetRow() + 2, pChess[JIANG_CHESS_OFFSET]->GetColumn() + offset[i]);
        }
#else
        for (size_t i = 0; i < HAN_CHE_CHESS_COUNT; i++){
            pChess[HAN_CHE_CHESS_OFFSET + i]->SetPos(pChess[JIANG_CHESS_OFFSET]->GetRow() + 3, pChess[JIANG_CHESS_OFFSET]->GetColumn() + offset[i]);
        }
#endif
        for (size_t i = 0; i < HAN_PAO_CHESS_COUNT; i++){
            pChess[HAN_PAO_CHESS_OFFSET + i]->SetPos(pChess[JIANG_CHESS_OFFSET]->GetRow() + 2, pChess[JIANG_CHESS_OFFSET]->GetColumn());
        }
    }
    else if(country == WEI_COUNTRY_INDEX){
        pChess[JIANG_CHESS_OFFSET]->SetPos(CHESSBOARD_ROW / 2, CHESSBOARD_COLUMN);
        std::array<int32_t, HAN_CHE_CHESS_COUNT> offset = { -2, 0, 2 };
#ifdef ENABLE_BATTLE_FAN_GUANYU
        for (size_t i = 0; i < HAN_CHE_CHESS_COUNT; i++){
            pChess[HAN_CHE_CHESS_OFFSET + i]->SetPos(pChess[JIANG_CHESS_OFFSET]->GetRow() + offset[i], pChess[JIANG_CHESS_OFFSET]->GetColumn() - 2);
        }
#else
        for (size_t i = 0; i < HAN_CHE_CHESS_COUNT; i++){
            pChess[HAN_CHE_CHESS_OFFSET + i]->SetPos(pChess[JIANG_CHESS_OFFSET]->GetRow() + offset[i], pChess[JIANG_CHESS_OFFSET]->GetColumn() - 3);
        }
#endif
        for (size_t i = 0; i < HAN_PAO_CHESS_COUNT; i++){
            pChess[HAN_PAO_CHESS_OFFSET + i]->SetPos(pChess[JIANG_CHESS_OFFSET]->GetRow(), pChess[JIANG_CHESS_OFFSET]->GetColumn() - 2);
        }
    }
    else if(country == HAN_COUNTRY_INDEX){
        pChess[JIANG_CHESS_OFFSET]->SetPos(CHESSBOARD_ROW, CHESSBOARD_COLUMN / 2);
        std::array<int32_t, HAN_CHE_CHESS_COUNT> offset = { -2, 0, 2 };
#ifdef ENABLE_BATTLE_FAN_GUANYU
        for (size_t i = 0; i < HAN_CHE_CHESS_COUNT; i++){
            pChess[HAN_CHE_CHESS_OFFSET + i]->SetPos(pChess[JIANG_CHESS_OFFSET]->GetRow() - 2, pChess[JIANG_CHESS_OFFSET]->GetColumn() + offset[i]);
        }
#else
        for (size_t i = 0; i < HAN_CHE_CHESS_COUNT; i++){
            pChess[HAN_CHE_CHESS_OFFSET + i]->SetPos(pChess[JIANG_CHESS_OFFSET]->GetRow() - 3, pChess[JIANG_CHESS_OFFSET]->GetColumn() + offset[i]);
        }
#endif
        for (size_t i = 0; i < HAN_PAO_CHESS_COUNT; i++){
            pChess[HAN_PAO_CHESS_OFFSET + i]->SetPos(pChess[JIANG_CHESS_OFFSET]->GetRow() - 2, pChess[JIANG_CHESS_OFFSET]->GetColumn());
        }
    }
    else{
        //那就是蜀了
        pChess[JIANG_CHESS_OFFSET]->SetPos(CHESSBOARD_ROW / 2, 0);
        std::array<int32_t, HAN_CHE_CHESS_COUNT> offset = { 2, 0, -2 };
#ifdef ENABLE_BATTLE_FAN_GUANYU
        for (size_t i = 0; i < HAN_CHE_CHESS_COUNT; i++){
            pChess[HAN_CHE_CHESS_OFFSET + i]->SetPos(pChess[JIANG_CHESS_OFFSET]->GetRow() + offset[i], pChess[JIANG_CHESS_OFFSET]->GetColumn() + 2);
        }
#else
        for (size_t i = 0; i < HAN_CHE_CHESS_COUNT; i++){
            pChess[HAN_CHE_CHESS_OFFSET + i]->SetPos(pChess[JIANG_CHESS_OFFSET]->GetRow() + offset[i], pChess[JIANG_CHESS_OFFSET]->GetColumn() + 3);
        }
#endif
        for (size_t i = 0; i < HAN_PAO_CHESS_COUNT; i++){
            pChess[HAN_PAO_CHESS_OFFSET + i]->SetPos(pChess[JIANG_CHESS_OFFSET]->GetRow(), pChess[JIANG_CHESS_OFFSET]->GetColumn() + 2);
        }
    }
}
Chessboard::Chessboard(/* args */){
    mRecord.reserve(MAX_UNDO_STEP);
}

Chessboard::~Chessboard(){
}

bool Chessboard::areKingsFacing(uint32_t srcCountry, uint32_t dstCountry){
    bool are = true;
    const Chess *pSrc = mChess[srcCountry][JIANG_CHESS_OFFSET], *pDst = mChess[dstCountry][JIANG_CHESS_OFFSET];
    if(!pSrc || !pDst)return false;
    if(pSrc->GetRow() == pDst->GetRow()){
        const uint32_t row = pSrc->GetRow();
        for (size_t i = std::min(pDst->GetColumn(), pSrc->GetColumn()) + 1; i < std::max(pDst->GetColumn(), pSrc->GetColumn()); i++){
            if(GetChess(row, i)){
                are = false;
                break;
            }
        }
    }
    else if(pSrc->GetColumn() == pDst->GetColumn()){
        const uint32_t column = pSrc->GetColumn();
        for (size_t i = std::min(pDst->GetRow(), pSrc->GetRow()) + 1; i < std::max(pDst->GetRow(), pSrc->GetRow()); i++){
            if(GetChess(i, column)){
                are = false;
                break;
            }
        }
    }
    else{
        are = false;
    }
    return are;
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
    //初始化棋子信息
    mChess[WU_COUNTRY_INDEX][JIANG_CHESS_OFFSET] = new Wu();
    mChess[WEI_COUNTRY_INDEX][JIANG_CHESS_OFFSET] = new Wei();
    mChess[SHU_COUNTRY_INDEX][JIANG_CHESS_OFFSET] = new Shu();
    mChess[HAN_COUNTRY_INDEX][JIANG_CHESS_OFFSET] = new Han();
    auto CrerateCountryChess = [](uint32_t country, std::array<Chess *, DRAW_CHESS_COUNT>&pChess){
        for (uint32_t uiChess = 1; uiChess < COUNTRY_CHESS_COUNT; ++uiChess){
            if(uiChess >= MA_CHESS_OFFSET && uiChess < MA_CHESS_OFFSET + MA_CHESS_COUNT){
                pChess[uiChess] = new Ma(country);
            }
            else if(uiChess >= PAO_CHESS_OFFSET && uiChess < PAO_CHESS_OFFSET + PAO_CHESS_COUNT){
                pChess[uiChess] = new Pao(country);
            }
            else if(uiChess >= CHE_CHESS_OFFSET && uiChess < CHE_CHESS_OFFSET + CHE_CHESS_COUNT){
                pChess[uiChess] = new Che(country);
            }
            else if(uiChess >= SHI_CHESS_OFFSET && uiChess < SHI_CHESS_OFFSET + SHI_CHESS_COUNT){
                pChess[uiChess] = new Shi(country);
            }
            else if(uiChess >= XIANG_CHESS_OFFSET && uiChess < XIANG_CHESS_OFFSET + XIANG_CHESS_COUNT){
                pChess[uiChess] = new Xiang(country);
            }
            else if(uiChess >= BING_CHESS_OFFSET && uiChess < BING_CHESS_OFFSET + BING_CHESS_COUNT){
                pChess[uiChess] = new Bing(country);
            }
            pChess[uiChess]->SetChessOffset(uiChess);
        }
    };
    if(isControllable){
        for (uint32_t uiCountry = 0; uiCountry < MAX_COUNTRY_INDEX; ++uiCountry){
            CrerateCountryChess(uiCountry, mChess[uiCountry]);
        }
    }
    else{
        for (uint32_t uiCountry = 0; uiCountry < MAX_COUNTRY_INDEX; ++uiCountry){
            if(uiCountry != HAN_COUNTRY_INDEX){
                CrerateCountryChess(uiCountry, mChess[uiCountry]);
            }
            else{
                    for (size_t uiChess = HAN_PAO_CHESS_OFFSET; uiChess < HAN_PAO_CHESS_OFFSET + HAN_PAO_CHESS_COUNT; uiChess++){
                        mChess[uiCountry][uiChess] = new Pao(uiCountry);
                        mChess[uiCountry][uiChess]->SetChessOffset(uiChess);
                    }
                    for (uint32_t uiChess = HAN_CHE_CHESS_OFFSET; uiChess < HAN_CHE_CHESS_OFFSET + HAN_CHE_CHESS_COUNT; ++uiChess){
                        mChess[uiCountry][uiChess] = new Che(uiCountry);
                        mChess[uiCountry][uiChess]->SetChessOffset(uiChess);
                    }
            }
        }
    }
    /*
        上魏下蜀，左汉右吴
        势力地盘是固定的，因此, 无论玩家什么势力，都固定在蜀的地盘，其他的类似
    */
    //把函数名中的国家坐标复制给chessInfo[playerCountry]
    InitShuChessRowAndColumn(mChess[playerCountry]);
    if(!isControllable)InitHanChessRowAndColumn(playerCountry, mChess[HAN_COUNTRY_INDEX]);
    if(WU_COUNTRY_INDEX == playerCountry){
        //如果玩家的势力是吴，那么魏在吴的地盘上，蜀在汉的地盘上, 下边同理
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
    //如果实在没办法，干脆直接保存整个棋盘
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
        auto count = GetCountryCount() - 1;
        if(count > 1)GetCountryChess(srcCountry, dstCountry);
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
Chess *Chessboard::GetChess(uint32_t country, Chess::Type chess) const{
    Chess *pChess = nullptr;
    for (size_t uiChess = 0; uiChess < DRAW_CHESS_COUNT; ++uiChess){
        if(mChess[country][uiChess] && mChess[country][uiChess]->GetChess() == chess){
            pChess = mChess[country][uiChess];
            break;
        }
    }
    return pChess;
}
Chess *Chessboard::GetChess(uint32_t country, const glm::vec2 &mousePos) const{
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