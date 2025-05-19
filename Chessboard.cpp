#include "Chessboard.h"
Chess *Chessboard::CreateChess(Country country, Chess::Type chess, uint32_t row, uint32_t column){
    switch (chess){
    case Chess::Type::Jiang_Chess:{
        if(country == Wu_Country)return new Wu(mTerritory[country], row, column);
        else if(country == Wei_Country)return new Wei(mTerritory[country], row, column);
        else if(country == Shu_Country)return new Shu(mTerritory[country], row, column);
        else if(country == Han_Country)return new Han(mTerritory[country], row, column);
    }
    case Chess::Type::Ma_Chess:return new Ma(country, mTerritory[country], row, column);
    case Chess::Type::Pao_Chess:return new Pao(country, mTerritory[country], row, column);
    case Chess::Type::Che_Chess:return new Che(country, mTerritory[country], row, column);
    case Chess::Type::Xiang_Chess:return new Xiang(country, mTerritory[country], row, column);
    case Chess::Type::Shi_Chess:return new Shi(country, mTerritory[country], row, column);
    case Chess::Type::Bing_Chess:return new Bing(country, mTerritory[country], row, column);
    }
    return nullptr;
}
uint32_t Chessboard::GetCountryCount(){
    uint32_t count = 0;
    for (size_t i = 0; i < MAX_COUNTRY_INDEX; i++){
        if(!IsDeath((Country)i))++count;
    }
    return count;
}
void Chessboard::DeleteFollowingStep(uint32_t number){
    //处理回放时的if路线, 也就是直接删除pChess后面的步骤
    auto it = mRecord.begin();
    while(it  != mRecord.end()){
        if(it->number == number){
            break;
        }
        ++it;
    }
    while (it != mRecord.end()){
        it = mRecord.erase(it);
    }
}
void Chessboard::DestoryChess(Country country, uint32_t offset){
    if(mChess[country][offset]){
        delete mChess[country][offset];
        mChess[country][offset] = nullptr;
    }
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
    std::array<int32_t, PAO_CHESS_COUNT>paoOffset = { 2, -2 };
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
    //棋子的位置也需要一致, 蜀炮0号的索引小，吴炮0号的索引大，以此类推，除非不用网络模式
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
    // std::array<int32_t, BING_CHESS_OFFSET>bingOffset = { -2, 2 };
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
    // std::array<int32_t, BING_CHESS_OFFSET>bingOffset = { 2, -2 };
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
    if(country == Wu_Country){
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
    else if(country == Wei_Country){
        pChess[JIANG_CHESS_OFFSET]->SetPos(CHESSBOARD_ROW / 2, CHESSBOARD_COLUMN);
        std::array<int32_t, HAN_CHE_CHESS_COUNT> offset = { 2, 0, -2 };
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
    else if(country == Shu_Country){
        pChess[JIANG_CHESS_OFFSET]->SetPos(CHESSBOARD_ROW / 2, 0);
        std::array<int32_t, HAN_CHE_CHESS_COUNT> offset = { -2, 0, 2 };
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
    else{
        //那就是汉了
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
}
void Chessboard::UndoStep(){
    while (!mRecord.empty() && IsBoundary(mRecord.back().chess.GetRow(), mRecord.back().chess.GetColumn())){
        mRecord.pop_back();
    }
    
    if(mRecord.empty())return;

    auto last = mRecord.back();
    //走的棋子也可能被吃
    const Country selectCountry = last.chess.GetCountry(), captureCountry = last.captured.GetCountry();
    auto pChess = mChess[selectCountry][last.chess.GetChessOffset()];
    if(pChess){
        pChess->SetPos(last.chess.GetRow(), last.chess.GetColumn());
    }
    else{
        mChess[selectCountry][last.chess.GetChessOffset()] = CreateChess(selectCountry, last.chess.GetChess(), last.chess.GetRow(), last.chess.GetColumn());
        mChess[selectCountry][last.chess.GetChessOffset()]->SetChessOffset(last.chess.GetChessOffset());
    }
    if(last.is_death){
        //灭了其他势力后，获得他的棋子，但撤回的时候没删除这些获得的棋子
        const Country country = last.death.country;
        for (auto&it:last.death.chess){
            const uint32_t offset = it.GetChessOffset();
            mChess[country][offset] = CreateChess(country, it.GetChess(), it.GetRow(), it.GetColumn());
            mChess[country][offset]->SetChessOffset(offset);                
        }
        //得删除和被灭势力同位置的棋子
        for (size_t i = COUNTRY_CHESS_COUNT; i < DRAW_CHESS_COUNT; i++){
            auto ps = mChess[selectCountry][i], pc = mChess[country][i - COUNTRY_CHESS_COUNT];
            if(ps && pc && ps->GetRow() == pc->GetRow() && ps->GetColumn() == pc->GetColumn()){
                DestoryChess(selectCountry, i);
            }
        }        
    }
    else if(last.is_capture){
        //说明这一步吃子了
        const uint32_t offset = last.captured.GetChessOffset();
        mChess[captureCountry][offset] = CreateChess(captureCountry, last.captured.GetChess(), last.captured.GetRow(), last.captured.GetColumn());
        mChess[captureCountry][offset]->SetChessOffset(offset);
    }
    if(last.is_facing){
        for (auto&it:last.facing){
            const Country country = it.country;
            for (auto&cit:it.chess){
                pChess = mChess[country][cit.GetChessOffset()];
                if(pChess){
                    pChess->SetPos(cit.GetRow(), cit.GetColumn());
                }
                else{
                    mChess[country][cit.GetChessOffset()] = CreateChess(country, cit.GetChess(), cit.GetRow(), cit.GetColumn());
                    mChess[country][cit.GetChessOffset()]->SetChessOffset(cit.GetChessOffset());
                }                                
            }
        }
    }
    mRecord.pop_back();
}
Chessboard::Chessboard(/* args */){
    mRecord.reserve(MAX_UNDO_STEP);
}

Chessboard::~Chessboard(){
    DestroyCountry();
    mRecord.clear();
}

bool Chessboard::areKingsFacing(Country srcCountry, Country dstCountry)const{
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
bool Chessboard::IsBoundary(int32_t row, int32_t column)const{
    if(row < 0 || column < 0)return true;
    if(row > CHESSBOARD_ROW || column > CHESSBOARD_COLUMN){
        // printf("到达边界, row:%u, column:%u\n", row, column);
        return true;
    }
    else if ((row < CHESSBOARD_BING_GRID_DENSITY || row > CHESSBOARD_BOUNDARY_CENTER_RECT_COUNT + CHESSBOARD_BING_GRID_DENSITY) && (column < CHESSBOARD_BING_GRID_DENSITY || column > CHESSBOARD_BOUNDARY_CENTER_RECT_COUNT + CHESSBOARD_BING_GRID_DENSITY)){
        // printf("到达边界, row:%u, column:%u\n", row, column);
        return true;
    }
    return false;
}
// void Chessboard::ImportRecord(const std::vector<ChessMove> &record){
//     mRecord = record;
// }
void Chessboard::InitializeChess(Country player, bool isControllable){
    mRecord.clear();
    mCountryCount = isControllable?MAX_COUNTRY_INDEX:MAX_COUNTRY_INDEX - 1;
    for (uint32_t uiCountry = 0; uiCountry < MAX_COUNTRY_INDEX; ++uiCountry){
        mTerritory[uiCountry] = (Territory)GET_TERRITORY_INDEX(uiCountry, player);
    }
    mPalacesCenter[Han_Territory] = glm::vec2(1, CHESSBOARD_ROW / 2);
    mPalacesCenter[Wei_Territory] = glm::vec2(CHESSBOARD_COLUMN / 2, 1);
    mPalacesCenter[Wu_Territory] = glm::vec2(CHESSBOARD_COLUMN - 1, CHESSBOARD_ROW / 2);
    mPalacesCenter[Shu_Territory] = glm::vec2(CHESSBOARD_COLUMN / 2, CHESSBOARD_ROW - 1);
    for (uint32_t uiCountry = 0; uiCountry < MAX_COUNTRY_INDEX; ++uiCountry){
        DestroyCountry((Country)uiCountry);
    }
    //初始化棋子信息
    mChess[Wu_Country][JIANG_CHESS_OFFSET] = new Wu(mTerritory[Wu_Country]);
    mChess[Wei_Country][JIANG_CHESS_OFFSET] = new Wei(mTerritory[Wei_Country]);
    mChess[Shu_Country][JIANG_CHESS_OFFSET] = new Shu(mTerritory[Shu_Country]);
    mChess[Han_Country][JIANG_CHESS_OFFSET] = new Han(mTerritory[Han_Country]);
    //因为需要要到表达式外的变量才需要[&]
    auto CrerateCountryChess = [this](Country country, std::array<Chess *, DRAW_CHESS_COUNT>&pChess){
        for (uint32_t uiChess = 1; uiChess < COUNTRY_CHESS_COUNT; ++uiChess){
            if(uiChess >= MA_CHESS_OFFSET && uiChess < MA_CHESS_OFFSET + MA_CHESS_COUNT){
                pChess[uiChess] = new Ma(country, mTerritory[country]);
            }
            else if(uiChess >= PAO_CHESS_OFFSET && uiChess < PAO_CHESS_OFFSET + PAO_CHESS_COUNT){
                pChess[uiChess] = new Pao(country, mTerritory[country]);
            }
            else if(uiChess >= CHE_CHESS_OFFSET && uiChess < CHE_CHESS_OFFSET + CHE_CHESS_COUNT){
                pChess[uiChess] = new Che(country, mTerritory[country]);
            }
            else if(uiChess >= SHI_CHESS_OFFSET && uiChess < SHI_CHESS_OFFSET + SHI_CHESS_COUNT){
                pChess[uiChess] = new Shi(country, mTerritory[country]);
            }
            else if(uiChess >= XIANG_CHESS_OFFSET && uiChess < XIANG_CHESS_OFFSET + XIANG_CHESS_COUNT){
                pChess[uiChess] = new Xiang(country, mTerritory[country]);
            }
            else if(uiChess >= BING_CHESS_OFFSET && uiChess < BING_CHESS_OFFSET + BING_CHESS_COUNT){
                pChess[uiChess] = new Bing(country, mTerritory[country]);
            }
            pChess[uiChess]->SetChessOffset(uiChess);
        }
    };
    if(isControllable){
        for (uint32_t uiCountry = 0; uiCountry < MAX_COUNTRY_INDEX; ++uiCountry){
            CrerateCountryChess((Country)uiCountry, mChess[uiCountry]);
        }
    }
    else{
        // for (size_t i = 1; i < COUNTRY_CHESS_COUNT; i++){
        //     mChess[Wei_Country][i] = new Ma(Wei_Country, (Territory)GET_TERRITORY_INDEX(Wei_Country, player));
        //     mChess[Wei_Country][i]->SetChessOffset(i);
        // }
        // for (size_t i = 1; i < COUNTRY_CHESS_COUNT; i++){
        //     mChess[Shu_Country][i] = new ChaoJiBing(Shu_Country, (Territory)GET_TERRITORY_INDEX(Shu_Country, player));
        //     mChess[Shu_Country][i]->SetChessOffset(i);
        // }
        // for (size_t i = 1; i < COUNTRY_CHESS_COUNT; i++){
        //     mChess[Wu_Country][i] = new Pao(Wu_Country, (Territory)GET_TERRITORY_INDEX(Wu_Country, player));
        //     mChess[Wu_Country][i]->SetChessOffset(i);
        // }
        // for (size_t uiChess = HAN_PAO_CHESS_OFFSET; uiChess < HAN_PAO_CHESS_OFFSET + HAN_PAO_CHESS_COUNT; uiChess++){
        //     mChess[Han_Country][uiChess] = new Pao((Country)Han_Country, (Territory)GET_TERRITORY_INDEX(Han_Country, player));
        //     mChess[Han_Country][uiChess]->SetChessOffset(uiChess);
        // }
        // for (uint32_t uiChess = HAN_CHE_CHESS_OFFSET; uiChess < HAN_CHE_CHESS_OFFSET + HAN_CHE_CHESS_COUNT; ++uiChess){
        //     mChess[Han_Country][uiChess] = new Che((Country)Han_Country, (Territory)GET_TERRITORY_INDEX(Han_Country, player));
        //     mChess[Han_Country][uiChess]->SetChessOffset(uiChess);
        // }
        for (uint32_t uiCountry = 0; uiCountry < MAX_COUNTRY_INDEX; ++uiCountry){
            if(uiCountry != Han_Country){
                CrerateCountryChess((Country)uiCountry, mChess[uiCountry]);
            }
            else{
                    for (size_t uiChess = HAN_PAO_CHESS_OFFSET; uiChess < HAN_PAO_CHESS_OFFSET + HAN_PAO_CHESS_COUNT; uiChess++){
                        mChess[uiCountry][uiChess] = new Pao((Country)uiCountry, (Territory)GET_TERRITORY_INDEX(uiCountry, player));
                        mChess[uiCountry][uiChess]->SetChessOffset(uiChess);
                    }
                    for (uint32_t uiChess = HAN_CHE_CHESS_OFFSET; uiChess < HAN_CHE_CHESS_OFFSET + HAN_CHE_CHESS_COUNT; ++uiChess){
                        mChess[uiCountry][uiChess] = new Che((Country)uiCountry, (Territory)GET_TERRITORY_INDEX(uiCountry, player));
                        mChess[uiCountry][uiChess]->SetChessOffset(uiChess);
                    }
            }
        }
    }
    /*
        上魏下蜀，左汉右吴
        势力地盘是固定的，因此, 无论玩家什么势力，都固定在蜀的地盘，其他的类似
    */
    //把函数名中的国家坐标复制给chessInfo[player]
    InitShuChessRowAndColumn(mChess[player]);
    if(!isControllable)InitHanChessRowAndColumn(player, mChess[Han_Country]);
    if(Wu_Country == player){
        //如果玩家的势力是吴，那么魏在吴的地盘上，蜀在汉的地盘上, 下边同理
        InitWuChessRowAndColumn(mChess[Wei_Country]);
        InitHanChessRowAndColumn(mChess[Shu_Country]);
        if(isControllable)InitWeiChessRowAndColumn(mChess[Han_Country]);
    }
    else if(Wei_Country == player){
        InitHanChessRowAndColumn(mChess[Wu_Country]);
        InitWeiChessRowAndColumn(mChess[Shu_Country]);
        if(isControllable)InitWuChessRowAndColumn(mChess[Han_Country]);
    }
    else if(Shu_Country == player){
        InitWuChessRowAndColumn(mChess[Wu_Country]);
        InitWeiChessRowAndColumn(mChess[Wei_Country]);
        if(isControllable)InitHanChessRowAndColumn(mChess[Han_Country]);
    }
    else if(Han_Country == player){
        if(isControllable){
            InitWeiChessRowAndColumn(mChess[Wu_Country]);
            InitHanChessRowAndColumn(mChess[Wei_Country]);
            InitWuChessRowAndColumn(mChess[Shu_Country]);
        }
    }
}
Territory Chessboard::IsPalaceCenter(uint32_t row, uint32_t column)const{
    Territory country = Invald_Territory;
    for (size_t i = 0; i < MAX_COUNTRY_INDEX; ++i){
        if(row == mPalacesCenter[i].y && column == mPalacesCenter[i].x){
            // printf("%d国,%d行%d列的棋子:%d, 在中心为%d行%d列的九宫格内\n", mInfo.country, row, column, mInfo.chess, center[i].GetRow(), center[i].GetColumn());
            country = (Territory)i;
            break;
        }   
    }
    return country;
}
bool Chessboard::IsHasExitPermission(Country country){
    bool has = false;
    for (uint32_t uiChess = 0; uiChess < DRAW_CHESS_COUNT; ++uiChess){
        if(mChess[country][uiChess] && mChess[country][uiChess]->GetHasExitPermission()){
            has = true;
            break;
        }
    }
    return has;
}
Country Chessboard::PlayChess(Chess *pChess, uint32_t dstRow, uint32_t dstColumn){    
    Country targetCountry = Invald_Country;
    const Chess *pTarget = GetChess(dstRow, dstColumn);
    if(pTarget){
        targetCountry = pTarget->GetCountry();
        CaptureChess(pChess, pTarget);
        if(IsDeath(targetCountry)){
            DestroyCountry(targetCountry);
        }
        else{
            targetCountry = Invald_Country;
        }
    }
    pChess->SetPos(dstRow, dstColumn);
    return targetCountry;
}
void Chessboard::SaveStep(const ChessMove &dStep){
    if(mRecord.size() >= MAX_UNDO_STEP){
        mRecord.erase(mRecord.begin());
    }
    mRecord.push_back(dStep);
}
// void Chessboard::SetStepNumber(uint32_t number, Country player, bool isControllable){
//     auto record = mRecord;
//     InitializeChess(player, isControllable);
//     mRecord = record;
//     uint32_t n = 0;
//     for (auto it = mRecord.begin(); it != mRecord.end() && n <= number; ++it, ++n){
//         Chess *pStart = GetChess(it->chess.GetRow(), it->chess.GetColumn());
//         if(!IsBoundary(it->chess.GetRow(), it->chess.GetColumn())){
//             PlayChess(pStart, it->captured.GetRow(), it->captured.GetColumn());
//         }
//     }
// }
void Chessboard::UndoStep(uint32_t step){
    //撤回的步数等于sterp
    if(mRecord.empty())return;
    for (size_t i = 0; i < step; i++){
        UndoStep();
    }
}
Territory Chessboard::IsInPalace(uint32_t row, uint32_t column) const{
    Territory country = IsPalaceCenter(row, column);
    if(country != Invald_Territory)return country;
    for (size_t i = 0; i < MAX_COUNTRY_INDEX; ++i){
        if(abs((float)row - mPalacesCenter[i].y) < 2 && abs((float)column - mPalacesCenter[i].x) < 2){
            country = (Territory)i;
            break;
        }
    }
    return country;
}
void Chessboard::CaptureChess(const Chess *srcChess, const Chess *dstChess){
    const Country srcCountry = srcChess->GetCountry(), dstCountry = dstChess->GetCountry();
    if(dstChess->GetChess() == Chess::Type::Jiang_Chess){
        auto count = GetCountryCount() - 1;
        if(count > 1)GetCountryChess(srcCountry, dstCountry);
        DestroyCountry(dstCountry);
    }
    else{
        DestoryChess(dstCountry, dstChess->GetChessOffset());
    }
}
void Chessboard::GetCountryChess(Country srcCountry, Country dstCountry){
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
        pChess = GetChess((Country)uiCountry, row, column);
        if(pChess)break;
    }
    return pChess;
}
Chess *Chessboard::GetChess(Country country, Chess::Type chess) const{
    Chess *pChess = nullptr;
    for (size_t uiChess = 0; uiChess < DRAW_CHESS_COUNT; ++uiChess){
        if(mChess[country][uiChess] && mChess[country][uiChess]->GetChess() == chess){
            pChess = mChess[country][uiChess];
            break;
        }
    }
    return pChess;
}
Chess *Chessboard::GetChess(Country country, const glm::vec2 &mousePos) const{
    Chess *pChess = nullptr;
    for (size_t uiChess = 0; uiChess < DRAW_CHESS_COUNT; ++uiChess){
        if(mChess[country][uiChess] && mChess[country][uiChess]->IsSelect(mousePos)){
            pChess = mChess[country][uiChess];
            break;
        }
    }
    return pChess;
}

Chess *Chessboard::GetChess(Country country, uint32_t row, uint32_t column)const{
    Chess *pChess = nullptr;
    if(country != Invald_Country){
        for (uint32_t uiChess = 0; uiChess < DRAW_CHESS_COUNT; ++uiChess){
            if(mChess[country][uiChess] && mChess[country][uiChess]->GetRow() == row && mChess[country][uiChess]->GetColumn() == column){
                pChess = mChess[country][uiChess];
                break;
            }
        }
    }
    return pChess;
}
const Chess *Chessboard::Check(Country country, uint32_t dstRow, uint32_t dstColumn) const{
    for (size_t uiChess = 0; uiChess < DRAW_CHESS_COUNT; ++uiChess){
        std::vector<glm::vec2>canplays;
        const Chess *pChess = mChess[country][uiChess];
        if(pChess){
            pChess->Select(this, canplays);
            for(auto it:canplays){
                if(it.x == dstColumn && dstRow == it.y){
                    return pChess;
                }
            }
        }             
    }
    return nullptr;
}
void Chessboard::DestroyCountry(){
    for (size_t i = 0; i < MAX_COUNTRY_INDEX; ++i){
        DestroyCountry((Country)i);
    }
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
void Chessboard::DestroyCountry(Country country){
    for (size_t uiChess = 0; uiChess < DRAW_CHESS_COUNT; ++uiChess){
        DestoryChess(country, uiChess);
    }
}