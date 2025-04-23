#include "Chessboard.h"
#include "VulkanChess.h"
void Chessboard::InitWuChessRowAndColumn(std::array<Chess *, DRAW_COUNTRY_CHESS_COUNT>&pChess){
    pChess[JIANG_CHESS_INDEX]->SetPos(CHESSBOARD_ROW / 2, CHESSBOARD_COLUMN);
    std::array<int32_t, 2> offset = { -3, 3 };
    for (size_t i = 0; i < MA_CHESS_COUNT; i++){
        if(pChess[MA_CHESS_INDEX + i])pChess[MA_CHESS_INDEX + i]->SetPos(pChess[JIANG_CHESS_INDEX]->GetRow() - offset[i], pChess[JIANG_CHESS_INDEX]->GetColumn());
    }
    offset = { -1, 1 };
    for (size_t i = 0; i < SHI_CHESS_COUNT; i++){
        if(pChess[SHI_CHESS_INDEX + i])pChess[SHI_CHESS_INDEX + i]->SetPos(pChess[JIANG_CHESS_INDEX]->GetRow() - offset[i], pChess[JIANG_CHESS_INDEX]->GetColumn());
    }
    offset = { -4, 4 };
    for (size_t i = 0; i < CHE_CHESS_COUNT; i++){
        if(pChess[CHE_CHESS_INDEX + i])pChess[CHE_CHESS_INDEX + i]->SetPos(pChess[JIANG_CHESS_INDEX]->GetRow() - offset[i], pChess[JIANG_CHESS_INDEX]->GetColumn());
    }
    offset = { -2, 2 };
    for (size_t i = 0; i < XIANG_CHESS_COUNT; i++){
        if(pChess[XIANG_CHESS_INDEX + i])pChess[XIANG_CHESS_INDEX + i]->SetPos(pChess[JIANG_CHESS_INDEX]->GetRow() + offset[i], pChess[JIANG_CHESS_INDEX]->GetColumn());
    }
#ifdef ENABLE_BATTLE_FAN_GUANYU
    for (size_t i = 0; i < PAO_CHESS_COUNT; i++){
        if(pChess[PAO_CHESS_INDEX + i])pChess[PAO_CHESS_INDEX + i]->SetPos(pChess[XIANG_CHESS_INDEX + i * 1]->GetRow(), CHESSBOARD_COLUMN - 2);
    }
    if(pChess[BING_CHESS_INDEX])pChess[BING_CHESS_INDEX]->SetPos(CHESSBOARD_BING_GRID_DENSITY + 1, CHESSBOARD_COLUMN - 3);
    for (size_t i = 1; i < BING_CHESS_COUNT; ++i){
        if(pChess[i + BING_CHESS_INDEX])pChess[i + BING_CHESS_INDEX]->SetPos(pChess[i + BING_CHESS_INDEX - 1]->GetRow() + 2, pChess[BING_CHESS_INDEX]->GetColumn());
    }

#else
    for (size_t i = 0; i < PAO_CHESS_COUNT; i++){
        if(pChess[PAO_CHESS_INDEX + i])pChess[PAO_CHESS_INDEX + i]->SetPos(pChess[MA_CHESS_INDEX + i * 1]->GetRow(), CHESSBOARD_COLUMN - 2);
    }
    if(pChess[BING_CHESS_INDEX])pChess[BING_CHESS_INDEX]->SetPos(CHESSBOARD_BING_GRID_DENSITY, CHESSBOARD_COLUMN - 3);
    for (size_t i = 1; i < BING_CHESS_COUNT; ++i){
        if(pChess[i + BING_CHESS_INDEX])pChess[i + BING_CHESS_INDEX]->SetPos(pChess[i + BING_CHESS_INDEX - 1]->GetRow() + 2, pChess[BING_CHESS_INDEX]->GetColumn());
    }
#endif    
}
void Chessboard::InitShuChessRowAndColumn(std::array<Chess *, DRAW_COUNTRY_CHESS_COUNT>&pChess){
    pChess[JIANG_CHESS_INDEX]->SetPos(CHESSBOARD_ROW, CHESSBOARD_COLUMN / 2);
    
    std::array<int32_t, 2> offset = { -3, 3 };
    for (size_t i = 0; i < MA_CHESS_COUNT; i++){
        if(pChess[MA_CHESS_INDEX + i])pChess[MA_CHESS_INDEX + i]->SetPos(pChess[JIANG_CHESS_INDEX]->GetRow(), pChess[JIANG_CHESS_INDEX]->GetColumn() - offset[i]);
    }
    offset = { -1, 1 };
    for (size_t i = 0; i < SHI_CHESS_COUNT; i++){
        if(pChess[SHI_CHESS_INDEX + i])pChess[SHI_CHESS_INDEX + i]->SetPos(pChess[JIANG_CHESS_INDEX]->GetRow(), pChess[JIANG_CHESS_INDEX]->GetColumn() - offset[i]);        
    }
    offset = { -4, 4 };
    for (size_t i = 0; i < CHE_CHESS_COUNT; i++){
        if(pChess[CHE_CHESS_INDEX + i])pChess[CHE_CHESS_INDEX + i]->SetPos(pChess[JIANG_CHESS_INDEX]->GetRow(), pChess[JIANG_CHESS_INDEX]->GetColumn() - offset[i]);
    }
    offset = { -2, 2 };
    for (size_t i = 0; i < XIANG_CHESS_COUNT; i++){
        if(pChess[XIANG_CHESS_INDEX + i])pChess[XIANG_CHESS_INDEX + i]->SetPos(pChess[JIANG_CHESS_INDEX]->GetRow(), pChess[JIANG_CHESS_INDEX]->GetColumn() + offset[i]);
    }
#ifdef ENABLE_BATTLE_FAN_GUANYU
    for (size_t i = 0; i < PAO_CHESS_COUNT; i++){
        if(pChess[PAO_CHESS_INDEX + i])pChess[PAO_CHESS_INDEX + i]->SetPos(CHESSBOARD_ROW - 2, pChess[XIANG_CHESS_INDEX + i * 1]->GetColumn());
    }
    if(pChess[BING_CHESS_INDEX])pChess[BING_CHESS_INDEX]->SetPos(CHESSBOARD_ROW - 3, CHESSBOARD_BING_GRID_DENSITY + 1);
    for (size_t i = 1; i < BING_CHESS_COUNT; ++i){
        if(pChess[i + BING_CHESS_INDEX])pChess[i + BING_CHESS_INDEX]->SetPos(pChess[BING_CHESS_INDEX]->GetRow(), pChess[i + BING_CHESS_INDEX - 1]->GetColumn() + 2);
    }
#else
    for (size_t i = 0; i < PAO_CHESS_COUNT; i++){
        if(pChess[PAO_CHESS_INDEX + i])pChess[PAO_CHESS_INDEX + i]->SetPos(CHESSBOARD_ROW - 2, pChess[MA_CHESS_INDEX + i * 1]->GetColumn());
    }
    if(pChess[BING_CHESS_INDEX])pChess[BING_CHESS_INDEX]->SetPos(CHESSBOARD_ROW - 3, CHESSBOARD_BING_GRID_DENSITY);
    for (size_t i = 1; i < BING_CHESS_COUNT; ++i){
        if(pChess[i + BING_CHESS_INDEX])pChess[i + BING_CHESS_INDEX]->SetPos(pChess[BING_CHESS_INDEX]->GetRow(), pChess[i + BING_CHESS_INDEX - 1]->GetColumn() + 2);
    }
#endif
}
void Chessboard::InitWeiChessRowAndColumn(std::array<Chess *, DRAW_COUNTRY_CHESS_COUNT>&pChess){
    pChess[JIANG_CHESS_INDEX]->SetPos(0, CHESSBOARD_COLUMN / 2);

    std::array<int32_t, 2> offset = { -3, 3 };
    for (size_t i = 0; i < MA_CHESS_COUNT; i++){
        if(pChess[MA_CHESS_INDEX + i])pChess[MA_CHESS_INDEX + i]->SetPos(pChess[JIANG_CHESS_INDEX]->GetRow(), pChess[JIANG_CHESS_INDEX]->GetColumn() - offset[i]);
    }
    offset = { -1, 1 };
    for (size_t i = 0; i < SHI_CHESS_COUNT; i++){
        if(pChess[SHI_CHESS_INDEX + i])pChess[SHI_CHESS_INDEX + i]->SetPos(pChess[JIANG_CHESS_INDEX]->GetRow(), pChess[JIANG_CHESS_INDEX]->GetColumn() - offset[i]);        
    }
    offset = { -4, 4 };
    for (size_t i = 0; i < CHE_CHESS_COUNT; i++){
        if(pChess[CHE_CHESS_INDEX + i])pChess[CHE_CHESS_INDEX + i]->SetPos(pChess[JIANG_CHESS_INDEX]->GetRow(), pChess[JIANG_CHESS_INDEX]->GetColumn() - offset[i]);
    }
    offset = { -2, 2 };
    for (size_t i = 0; i < XIANG_CHESS_COUNT; i++){
        if(pChess[XIANG_CHESS_INDEX + i])pChess[XIANG_CHESS_INDEX + i]->SetPos(pChess[JIANG_CHESS_INDEX]->GetRow(), pChess[JIANG_CHESS_INDEX]->GetColumn() + offset[i]);
    }
#ifdef ENABLE_BATTLE_FAN_GUANYU
    for (size_t i = 0; i < PAO_CHESS_COUNT; i++){
        if(pChess[PAO_CHESS_INDEX + i])pChess[PAO_CHESS_INDEX + i]->SetPos(2, pChess[XIANG_CHESS_INDEX + i * 1]->GetColumn());
    }
    pChess[BING_CHESS_INDEX]->SetPos(3, CHESSBOARD_BING_GRID_DENSITY + 1);
    for (size_t i = 1; i < BING_CHESS_COUNT; ++i){
        pChess[i + BING_CHESS_INDEX]->SetPos(pChess[BING_CHESS_INDEX]->GetRow(), pChess[i + BING_CHESS_INDEX - 1]->GetColumn() + 2);
    }
#else
    for (size_t i = 0; i < PAO_CHESS_COUNT; i++){
        if(pChess[PAO_CHESS_INDEX + i])pChess[PAO_CHESS_INDEX + i]->SetPos(2, pChess[MA_CHESS_INDEX + i * 1]->GetColumn());
    }
    pChess[BING_CHESS_INDEX]->SetPos(3, CHESSBOARD_BING_GRID_DENSITY);
    for (size_t i = 1; i < BING_CHESS_COUNT; ++i){
        pChess[i + BING_CHESS_INDEX]->SetPos(pChess[BING_CHESS_INDEX]->GetRow(), pChess[i + BING_CHESS_INDEX - 1]->GetColumn() + 2);
    }

#endif
}
void Chessboard::InitHanChessRowAndColumn(std::array<Chess *, DRAW_COUNTRY_CHESS_COUNT>&pChess){
    pChess[JIANG_CHESS_INDEX]->SetPos(CHESSBOARD_ROW / 2, 0);

    std::array<int32_t, 2> offset = { -3, 3 };
    for (size_t i = 0; i < MA_CHESS_COUNT; i++){
        if(pChess[MA_CHESS_INDEX + i])pChess[MA_CHESS_INDEX + i]->SetPos(pChess[JIANG_CHESS_INDEX]->GetRow() - offset[i], pChess[JIANG_CHESS_INDEX]->GetColumn());
    }
    offset = { -1, 1 };
    for (size_t i = 0; i < SHI_CHESS_COUNT; i++){
        if(pChess[SHI_CHESS_INDEX + i])pChess[SHI_CHESS_INDEX + i]->SetPos(pChess[JIANG_CHESS_INDEX]->GetRow() - offset[i], pChess[JIANG_CHESS_INDEX]->GetColumn());
    }
    offset = { -4, 4 };
    for (size_t i = 0; i < CHE_CHESS_COUNT; i++){
        if(pChess[CHE_CHESS_INDEX + i])pChess[CHE_CHESS_INDEX + i]->SetPos(pChess[JIANG_CHESS_INDEX]->GetRow() - offset[i], pChess[JIANG_CHESS_INDEX]->GetColumn());
    }
    offset = { -2, 2 };
    for (size_t i = 0; i < XIANG_CHESS_COUNT; i++){
        if(pChess[XIANG_CHESS_INDEX + i])pChess[XIANG_CHESS_INDEX + i]->SetPos(pChess[JIANG_CHESS_INDEX]->GetRow() + offset[i], pChess[JIANG_CHESS_INDEX]->GetColumn());
    }
#ifdef ENABLE_BATTLE_FAN_GUANYU
    for (size_t i = 0; i < PAO_CHESS_COUNT; i++){
        if(pChess[PAO_CHESS_INDEX + i])pChess[PAO_CHESS_INDEX + i]->SetPos(pChess[XIANG_CHESS_INDEX + i * 1]->GetRow(), 2);
    }
    pChess[BING_CHESS_INDEX]->SetPos(CHESSBOARD_BING_GRID_DENSITY + 1, 3);
    for (size_t i = 1; i < BING_CHESS_COUNT; ++i){
        pChess[i + BING_CHESS_INDEX]->SetPos(pChess[i + BING_CHESS_INDEX - 1]->GetRow() + 2, pChess[BING_CHESS_INDEX]->GetColumn());
    }

#else
    for (size_t i = 0; i < PAO_CHESS_COUNT; i++){
        if(pChess[PAO_CHESS_INDEX + i])pChess[PAO_CHESS_INDEX + i]->SetPos(pChess[MA_CHESS_INDEX + i * 1]->GetRow(), 2);
    }
    pChess[BING_CHESS_INDEX]->SetPos(CHESSBOARD_BING_GRID_DENSITY, 3);
    for (size_t i = 1; i < BING_CHESS_COUNT; ++i){
        pChess[i + BING_CHESS_INDEX]->SetPos(pChess[i + BING_CHESS_INDEX - 1]->GetRow() + 2, pChess[BING_CHESS_INDEX]->GetColumn());
    }

#endif
}
void Chessboard::InitHanChessRowAndColumn(uint32_t country, std::array<Chess *, DRAW_COUNTRY_CHESS_COUNT>&pChess){
    if(country == WU_COUNTRY_INDEX){
        pChess[JIANG_CHESS_INDEX]->SetPos(0, CHESSBOARD_COLUMN / 2);

        pChess[HAN_CHE_CHESS_INDEX]->SetPos(pChess[JIANG_CHESS_INDEX]->GetRow() + 3, pChess[JIANG_CHESS_INDEX]->GetColumn());
        pChess[HAN_CHE_CHESS_INDEX + 1]->SetPos(pChess[HAN_CHE_CHESS_INDEX]->GetRow(), pChess[JIANG_CHESS_INDEX]->GetColumn() + 2);
        pChess[HAN_CHE_CHESS_INDEX + 2]->SetPos(pChess[HAN_CHE_CHESS_INDEX]->GetRow(), pChess[HAN_CHE_CHESS_INDEX + 1]->GetColumn() - 2);

        pChess[HAN_PAO_CHESS_INDEX]->SetPos(pChess[HAN_CHE_CHESS_INDEX]->GetRow() - 1, pChess[JIANG_CHESS_INDEX]->GetColumn());
    }
    else if(country == WEI_COUNTRY_INDEX){
        pChess[JIANG_CHESS_INDEX]->SetPos(CHESSBOARD_ROW / 2, CHESSBOARD_COLUMN);

        pChess[HAN_CHE_CHESS_INDEX]->SetPos(pChess[JIANG_CHESS_INDEX]->GetRow(), CHESSBOARD_COLUMN - 3);
        pChess[HAN_CHE_CHESS_INDEX + 1]->SetPos(pChess[HAN_CHE_CHESS_INDEX]->GetRow() + 2, pChess[HAN_CHE_CHESS_INDEX]->GetColumn());
        pChess[HAN_CHE_CHESS_INDEX + 2]->SetPos(pChess[HAN_CHE_CHESS_INDEX]->GetRow() - 2, pChess[HAN_CHE_CHESS_INDEX]->GetColumn());

        pChess[HAN_PAO_CHESS_INDEX]->SetPos(pChess[JIANG_CHESS_INDEX]->GetRow(), pChess[HAN_CHE_CHESS_INDEX]->GetColumn() + 1);
    }
    else if(country == HAN_COUNTRY_INDEX){
        pChess[JIANG_CHESS_INDEX]->SetPos(CHESSBOARD_ROW, CHESSBOARD_COLUMN / 2);

        pChess[HAN_CHE_CHESS_INDEX]->SetPos(CHESSBOARD_ROW, pChess[JIANG_CHESS_INDEX]->GetColumn());
        pChess[HAN_CHE_CHESS_INDEX + 1]->SetPos(pChess[HAN_CHE_CHESS_INDEX]->GetRow(), pChess[JIANG_CHESS_INDEX]->GetColumn() + 2);
        pChess[HAN_CHE_CHESS_INDEX + 2]->SetPos(pChess[HAN_CHE_CHESS_INDEX + 1]->GetRow(), pChess[HAN_CHE_CHESS_INDEX + 1]->GetColumn() + 2);

        pChess[HAN_PAO_CHESS_INDEX]->SetPos(pChess[HAN_CHE_CHESS_INDEX + 1]->GetRow() + 1, pChess[JIANG_CHESS_INDEX]->GetColumn());
    }
    else{
        //那就是蜀了
        pChess[JIANG_CHESS_INDEX]->SetPos(CHESSBOARD_ROW / 2, 0);

        pChess[HAN_CHE_CHESS_INDEX]->SetPos(6, 3);
        pChess[HAN_CHE_CHESS_INDEX + 1]->SetPos(pChess[JIANG_CHESS_INDEX]->GetRow(), pChess[HAN_CHE_CHESS_INDEX]->GetColumn());
        pChess[HAN_CHE_CHESS_INDEX + 2]->SetPos(pChess[HAN_CHE_CHESS_INDEX + 1]->GetRow() + 2, pChess[HAN_CHE_CHESS_INDEX + 1]->GetColumn());

        pChess[HAN_PAO_CHESS_INDEX]->SetPos(pChess[JIANG_CHESS_INDEX]->GetRow(), pChess[HAN_CHE_CHESS_INDEX + 1]->GetColumn() - 1) ;
    }
}
void Chessboard::InitHanChessinfo(std::array<Chess *, DRAW_COUNTRY_CHESS_COUNT>&pChess){
    pChess[JIANG_CHESS_INDEX]->SetChess(JIANG_CHESS_INDEX);
    pChess[JIANG_CHESS_INDEX]->SetFontIndex(FONT_INDEX_HAN);
    pChess[JIANG_CHESS_INDEX]->SetCountry(HAN_COUNTRY_INDEX);
    for (size_t i = 0; i < HAN_CHE_CHESS_COUNT; i++){
        pChess[HAN_CHE_CHESS_INDEX + i]->SetFontIndex(FONT_INDEX_CHE);
        pChess[HAN_CHE_CHESS_INDEX + i]->SetChess(HAN_CHE_CHESS_INDEX + i);
        pChess[HAN_CHE_CHESS_INDEX + i]->SetCountry(HAN_COUNTRY_INDEX);
    }
    for (size_t i = 0; i < HAN_PAO_CHESS_COUNT; i++){        
        pChess[HAN_PAO_CHESS_INDEX + i]->SetChess(HAN_PAO_CHESS_INDEX + i);
        pChess[HAN_PAO_CHESS_INDEX + i]->SetFontIndex(FONT_INDEX_PAO);
        pChess[HAN_PAO_CHESS_INDEX + i]->SetCountry(HAN_COUNTRY_INDEX);
    }
}
void Chessboard::InitChessInfo(uint32_t country, std::array<Chess *, DRAW_COUNTRY_CHESS_COUNT>&pChess){
    uint32_t fontIndex[4];
    fontIndex[WU_COUNTRY_INDEX] = FONT_INDEX_WU;
    fontIndex[WEI_COUNTRY_INDEX] = FONT_INDEX_WEI;
    fontIndex[SHU_COUNTRY_INDEX] = FONT_INDEX_SHU;
    fontIndex[HAN_COUNTRY_INDEX] = FONT_INDEX_HAN;
    pChess[JIANG_CHESS_INDEX]->SetCountry(country);
    pChess[JIANG_CHESS_INDEX]->SetChess(JIANG_CHESS_INDEX);
    pChess[JIANG_CHESS_INDEX]->SetFontIndex(fontIndex[country]);
    for (size_t i = 0; i < MA_CHESS_COUNT; i++){
        pChess[MA_CHESS_INDEX + i]->SetCountry(country);
        pChess[MA_CHESS_INDEX + i]->SetChess(MA_CHESS_INDEX + i);
        pChess[MA_CHESS_INDEX + i]->SetFontIndex(FONT_INDEX_MA);
    }
    for (size_t i = 0; i < SHI_CHESS_COUNT; i++){
        pChess[SHI_CHESS_INDEX + i]->SetCountry(country);
        pChess[SHI_CHESS_INDEX + i]->SetChess(SHI_CHESS_INDEX + i);
        pChess[SHI_CHESS_INDEX + i]->SetFontIndex(FONT_INDEX_SHI);
    }
    for (size_t i = 0; i < CHE_CHESS_COUNT; i++){
        pChess[CHE_CHESS_INDEX + i]->SetCountry(country);
        pChess[CHE_CHESS_INDEX + i]->SetChess(CHE_CHESS_INDEX + i);
        pChess[CHE_CHESS_INDEX + i]->SetFontIndex(FONT_INDEX_CHE);
    }
    for (size_t i = 0; i < PAO_CHESS_COUNT; i++){
        pChess[PAO_CHESS_INDEX + i]->SetCountry(country);
        pChess[PAO_CHESS_INDEX + i]->SetChess(PAO_CHESS_INDEX + i);
        pChess[PAO_CHESS_INDEX + i]->SetFontIndex(FONT_INDEX_PAO);
    }
    for (size_t i = 0; i < BING_CHESS_COUNT; i++){
        pChess[BING_CHESS_INDEX + i]->SetCountry(country);
        pChess[BING_CHESS_INDEX + i]->SetChess(BING_CHESS_INDEX + i);
        pChess[BING_CHESS_INDEX + i]->SetFontIndex(FONT_INDEX_BING);
    }
    for (size_t i = 0; i < XIANG_CHESS_COUNT; i++){
        pChess[XIANG_CHESS_INDEX + i]->SetCountry(country);
        pChess[XIANG_CHESS_INDEX + i]->SetChess(XIANG_CHESS_INDEX + i);
        pChess[XIANG_CHESS_INDEX + i]->SetFontIndex(FONT_INDEX_XIANG);
    }
}
Chessboard::Chessboard(/* args */){
}

Chessboard::~Chessboard(){
}
void Chessboard::InitializeChess(uint32_t playerCountry, bool bHanCanPlay, uint32_t countryCount){
    mCountryCount = countryCount;
    if(playerCountry == WU_COUNTRY_INDEX){
        mPalacesCenter = { glm::vec2(CHESSBOARD_COLUMN / 2, CHESSBOARD_ROW - 1), glm::vec2(CHESSBOARD_COLUMN - 1, CHESSBOARD_ROW / 2), glm::vec2(1, CHESSBOARD_ROW / 2), glm::vec2(CHESSBOARD_COLUMN / 2, 1) };
    }
    else if(playerCountry == WEI_COUNTRY_INDEX){
        mPalacesCenter = { glm::vec2(1, CHESSBOARD_ROW / 2), glm::vec2(CHESSBOARD_COLUMN / 2, CHESSBOARD_ROW - 1), glm::vec2(CHESSBOARD_COLUMN / 2, 1), glm::vec2(CHESSBOARD_COLUMN - 1, CHESSBOARD_ROW / 2) };
    }
    else if(playerCountry == SHU_COUNTRY_INDEX){
        mPalacesCenter = { glm::vec2(CHESSBOARD_COLUMN - 1, CHESSBOARD_ROW / 2), glm::vec2(CHESSBOARD_COLUMN / 2, 1), glm::vec2(CHESSBOARD_COLUMN / 2, CHESSBOARD_ROW - 1), glm::vec2(1, CHESSBOARD_ROW / 2) };
    }
    else if(playerCountry == HAN_CHE_CHESS_INDEX){
        mPalacesCenter = { glm::vec2(CHESSBOARD_COLUMN / 2, 1), glm::vec2(1, CHESSBOARD_ROW / 2), glm::vec2(CHESSBOARD_COLUMN - 1, CHESSBOARD_ROW / 2), glm::vec2(CHESSBOARD_COLUMN / 2, CHESSBOARD_ROW - 1) };
    }
    for (uint32_t uiCountry = 0; uiCountry < MAX_COUNTRY_INDEX; ++uiCountry){
        DestroyCountry(uiCountry);
    }
    mChess[WU_COUNTRY_INDEX][JIANG_CHESS_INDEX] = new Wu;
    mChess[WEI_COUNTRY_INDEX][JIANG_CHESS_INDEX] = new Wei();
    mChess[SHU_COUNTRY_INDEX][JIANG_CHESS_INDEX] = new Shu();
    mChess[HAN_COUNTRY_INDEX][JIANG_CHESS_INDEX] = new Han();
    if(bHanCanPlay){
        for (uint32_t uiCountry = 0; uiCountry < MAX_COUNTRY_INDEX; ++uiCountry){
            for (uint32_t uiChess = 0; uiChess < COUNTRY_CHESS_COUNT; ++uiChess){
                if(uiChess >= MA_CHESS_INDEX && uiChess < MA_CHESS_INDEX + MA_CHESS_COUNT){
                    mChess[uiCountry][uiChess] = new Ma();
                }
                else if(uiChess >= PAO_CHESS_INDEX && uiChess < PAO_CHESS_INDEX + PAO_CHESS_COUNT){
                    mChess[uiCountry][uiChess] = new Pao();
                }
                else if(uiChess >= CHE_CHESS_INDEX && uiChess < CHE_CHESS_INDEX + CHE_CHESS_COUNT){
                    mChess[uiCountry][uiChess] = new Che();
                }
                else if(uiChess >= SHI_CHESS_INDEX && uiChess < SHI_CHESS_INDEX + SHI_CHESS_COUNT){
                    mChess[uiCountry][uiChess] = new Shi();
                }
                else if(uiChess >= XIANG_CHESS_INDEX && uiChess < XIANG_CHESS_INDEX + XIANG_CHESS_COUNT){
                    mChess[uiCountry][uiChess] = new Xiang();
                }
                else if(uiChess >= BING_CHESS_INDEX&& uiChess < BING_CHESS_INDEX + BING_CHESS_COUNT){
                    mChess[uiCountry][uiChess] = new Bing();
                }
            }
        }
    }
    else{
        for (uint32_t uiCountry = 0; uiCountry < MAX_COUNTRY_INDEX; ++uiCountry){
            if(uiCountry != HAN_COUNTRY_INDEX){
                for (uint32_t uiChess = MA_CHESS_INDEX; uiChess < COUNTRY_CHESS_COUNT; ++uiChess){
                    if(uiChess >= MA_CHESS_INDEX && uiChess < MA_CHESS_INDEX + MA_CHESS_COUNT){
                        mChess[uiCountry][uiChess] = new Ma();
                    }
                    else if(uiChess >= PAO_CHESS_INDEX && uiChess < PAO_CHESS_INDEX + PAO_CHESS_COUNT){
                        mChess[uiCountry][uiChess] = new Pao();
                    }
                    else if(uiChess >= CHE_CHESS_INDEX && uiChess < CHE_CHESS_INDEX + CHE_CHESS_COUNT){
                        mChess[uiCountry][uiChess] = new Che();
                    }
                    else if(uiChess >= SHI_CHESS_INDEX && uiChess < SHI_CHESS_INDEX + SHI_CHESS_COUNT){
                        mChess[uiCountry][uiChess] = new Shi();
                    }
                    else if(uiChess >= XIANG_CHESS_INDEX && uiChess < XIANG_CHESS_INDEX + XIANG_CHESS_COUNT){
                        mChess[uiCountry][uiChess] = new Xiang();
                    }
                    else if(uiChess >= BING_CHESS_INDEX&& uiChess < BING_CHESS_INDEX + BING_CHESS_COUNT){
                        mChess[uiCountry][uiChess] = new Bing();
                    }
                }
            }
            else{
                    mChess[uiCountry][HAN_PAO_CHESS_INDEX] = new Pao();
                    for (uint32_t uiChess = HAN_CHE_CHESS_INDEX; uiChess < HAN_CHE_CHESS_INDEX + HAN_CHE_CHESS_COUNT; ++uiChess){
                        mChess[uiCountry][uiChess] = new Che();
                    }
            }
        }
    }
    for (uint32_t uiCountry = 0; uiCountry < MAX_COUNTRY_INDEX; ++uiCountry){
        for (uint32_t uiChess = 0; uiChess < COUNTRY_CHESS_COUNT; ++uiChess){
            if(bHanCanPlay){
                InitChessInfo(uiCountry, mChess[uiCountry]);
            }
            else{
                if(uiCountry != HAN_COUNTRY_INDEX){
                    InitChessInfo(uiCountry, mChess[uiCountry]);
                }
                else{
                    InitHanChessinfo(mChess[uiCountry]);
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
    if(!bHanCanPlay)InitHanChessRowAndColumn(playerCountry, mChess[HAN_COUNTRY_INDEX]);
    if(WU_COUNTRY_INDEX == playerCountry){
        InitWuChessRowAndColumn(mChess[WEI_COUNTRY_INDEX]);
        InitHanChessRowAndColumn(mChess[SHU_COUNTRY_INDEX]);
        if(bHanCanPlay)InitWeiChessRowAndColumn(mChess[HAN_COUNTRY_INDEX]);
    }
    else if(WEI_COUNTRY_INDEX == playerCountry){
        InitHanChessRowAndColumn(mChess[WU_COUNTRY_INDEX]);
        InitWeiChessRowAndColumn(mChess[SHU_COUNTRY_INDEX]);
        if(bHanCanPlay)InitWuChessRowAndColumn(mChess[HAN_COUNTRY_INDEX]);
    }
    else if(SHU_COUNTRY_INDEX == playerCountry){
        InitWuChessRowAndColumn(mChess[WU_COUNTRY_INDEX]);
        InitWeiChessRowAndColumn(mChess[WEI_COUNTRY_INDEX]);
        if(bHanCanPlay)InitHanChessRowAndColumn(mChess[HAN_COUNTRY_INDEX]);
    }
    else if(HAN_COUNTRY_INDEX == playerCountry){
        if(bHanCanPlay){
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
uint32_t Chessboard::IsInPalace(uint32_t row, uint32_t column)const{
    uint32_t country = IsPalaceCenter(row, column);
    if(country != -1)return true;
    for (size_t i = 0; i < MAX_COUNTRY_INDEX; ++i){
        if(abs((float)row - mPalacesCenter[i].y) < 2 && abs((float)column - mPalacesCenter[i].x) < 2){
            country = i;
            break;
        }
    }
    return country;
}
void Chessboard::CaptureChess(uint32_t srcCountry, uint32_t dstCountry, uint32_t dstChess){
    if(dstChess == JIANG_CHESS_INDEX){
        GetCountryChess(srcCountry, dstCountry);
        DestroyCountry(dstCountry);
    }
    delete mChess[dstCountry][dstChess];
    mChess[dstCountry][dstChess] = nullptr;
}
void Chessboard::GetCountryChess(uint32_t srcCountry, uint32_t dstCountry){
    uint32_t fontIndex;
    for (uint32_t uiChess = JIANG_CHESS_INDEX + 1; uiChess < DRAW_COUNTRY_CHESS_COUNT; ++uiChess){
        Chess *pChess = mChess[dstCountry][uiChess];
        if(pChess){
            if(uiChess >= MA_CHESS_INDEX && uiChess < MA_CHESS_INDEX + MA_CHESS_COUNT){
                fontIndex = FONT_INDEX_MA;
                if(mChess[srcCountry][uiChess + COUNTRY_CHESS_COUNT]){
                    mChess[srcCountry][uiChess + COUNTRY_CHESS_COUNT * 2] = new Ma();
                }
                else{
                    mChess[srcCountry][uiChess + COUNTRY_CHESS_COUNT] = new Ma();
                }
            }
            else if(uiChess >= PAO_CHESS_INDEX && uiChess < PAO_CHESS_INDEX + PAO_CHESS_COUNT){
                fontIndex = FONT_INDEX_PAO;
                if(mChess[srcCountry][uiChess + COUNTRY_CHESS_COUNT]){
                    mChess[srcCountry][uiChess + COUNTRY_CHESS_COUNT * 2] = new Pao();
                }
                else{
                    mChess[srcCountry][uiChess + COUNTRY_CHESS_COUNT] = new Pao();
                }
            }
            else if(uiChess >= CHE_CHESS_INDEX && uiChess < CHE_CHESS_INDEX + CHE_CHESS_COUNT){
                fontIndex = FONT_INDEX_CHE;
                if(mChess[srcCountry][uiChess + COUNTRY_CHESS_COUNT]){
                    mChess[srcCountry][uiChess + COUNTRY_CHESS_COUNT * 2] = new Che();
                }
                else{
                    mChess[srcCountry][uiChess + COUNTRY_CHESS_COUNT] = new Che();
                }
            }
            else if(uiChess >= SHI_CHESS_INDEX && uiChess < SHI_CHESS_INDEX + SHI_CHESS_COUNT){
                fontIndex = FONT_INDEX_SHI;
                if(mChess[srcCountry][uiChess + COUNTRY_CHESS_COUNT]){
                    mChess[srcCountry][uiChess + COUNTRY_CHESS_COUNT * 2] = new Shi();
                }
                else{
                    mChess[srcCountry][uiChess + COUNTRY_CHESS_COUNT] = new Shi();
                }
            }
            else if(uiChess >= XIANG_CHESS_INDEX && uiChess < XIANG_CHESS_INDEX + XIANG_CHESS_COUNT){
                fontIndex = FONT_INDEX_XIANG;
                if(mChess[srcCountry][uiChess + COUNTRY_CHESS_COUNT]){
                    mChess[srcCountry][uiChess + COUNTRY_CHESS_COUNT * 2] = new Xiang();
                }
                else{
                    mChess[srcCountry][uiChess + COUNTRY_CHESS_COUNT] = new Xiang();
                }
            }
            else if(uiChess >= BING_CHESS_INDEX&& uiChess < BING_CHESS_INDEX + BING_CHESS_COUNT){
                fontIndex = FONT_INDEX_BING;
                if(mChess[srcCountry][uiChess + COUNTRY_CHESS_COUNT]){
                    mChess[srcCountry][uiChess + COUNTRY_CHESS_COUNT * 2] = new Bing();
                }
                else{
                    mChess[srcCountry][uiChess + COUNTRY_CHESS_COUNT] = new Bing();
                }
            }
            if(mChess[srcCountry][uiChess + COUNTRY_CHESS_COUNT * 2]){
                mChess[srcCountry][uiChess + COUNTRY_CHESS_COUNT * 2]->SetCountry(srcCountry);
                mChess[srcCountry][uiChess + COUNTRY_CHESS_COUNT * 2]->SetFontIndex(fontIndex);
                mChess[srcCountry][uiChess + COUNTRY_CHESS_COUNT * 2]->SetChess(uiChess + COUNTRY_CHESS_COUNT);
                mChess[srcCountry][uiChess + COUNTRY_CHESS_COUNT * 2]->SetPos(pChess->GetRow(), pChess->GetColumn());
            }
            else{
                mChess[srcCountry][uiChess + COUNTRY_CHESS_COUNT]->SetCountry(srcCountry);
                mChess[srcCountry][uiChess + COUNTRY_CHESS_COUNT]->SetFontIndex(fontIndex);
                mChess[srcCountry][uiChess + COUNTRY_CHESS_COUNT]->SetChess(uiChess + COUNTRY_CHESS_COUNT);
                mChess[srcCountry][uiChess + COUNTRY_CHESS_COUNT]->SetPos(pChess->GetRow(), pChess->GetColumn());
            }
        }
    }
}
const Chess *Chessboard::Check(uint32_t srcCountry, uint32_t dstCountry, uint32_t chess)const{
    if(srcCountry == dstCountry)return nullptr;
    for (size_t uiChess = 0; uiChess < DRAW_COUNTRY_CHESS_COUNT; ++uiChess){
        std::vector<glm::vec2>canplays;
        if(mChess[srcCountry][uiChess]){
            mChess[srcCountry][uiChess]->Select(this, canplays);
            for(auto it:canplays){
                const Chess *pChess = GetChess(it.y, it.x);
                if(pChess && pChess->GetChess() == chess && pChess->GetCountry() == dstCountry){
                    return mChess[srcCountry][uiChess];
                }
            }
        }             
    }
    return nullptr;
}
uint32_t Chessboard::GetChessCount(uint32_t country)const{
    uint32_t count = 0;
    for (size_t i = 0; i < DRAW_COUNTRY_CHESS_COUNT; i++){
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
    for (size_t uiChess = 0; uiChess < DRAW_COUNTRY_CHESS_COUNT; ++uiChess){
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
        for (uint32_t uiChess = 0; uiChess < DRAW_COUNTRY_CHESS_COUNT; ++uiChess){
            if(mChess[country][uiChess] && mChess[country][uiChess]->GetRow() == row && mChess[country][uiChess]->GetColumn() == column){
                pChess = mChess[country][uiChess];
                break;
            }
        }
    }
    return pChess;
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
    for (size_t uiChess = 0; uiChess < DRAW_COUNTRY_CHESS_COUNT; ++uiChess){
        if(mChess[country][uiChess]){
            delete mChess[country][uiChess];
            mChess[country][uiChess] = nullptr;
        }
    }
}