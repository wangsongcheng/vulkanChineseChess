#include "Chessboard.h"
void Chessboard::InitWuChessRowAndColumn(Chess *pChess[DRAW_COUNTRY_CHESS_COUNT]){
    pChess[JIANG_CHESS_INDEX]->SetPos(8, 16);

    if(pChess[MA_CHESS_INDEX])pChess[MA_CHESS_INDEX]->SetPos(pChess[JIANG_CHESS_INDEX]->GetRow() + 3, pChess[JIANG_CHESS_INDEX]->GetColumn());
    if(pChess[MA_CHESS_INDEX + 1])pChess[MA_CHESS_INDEX + 1]->SetPos(pChess[JIANG_CHESS_INDEX]->GetRow() - 3, pChess[JIANG_CHESS_INDEX]->GetColumn());

    if(pChess[SHI_CHESS_INDEX])pChess[SHI_CHESS_INDEX]->SetPos(pChess[JIANG_CHESS_INDEX]->GetRow() + 1, pChess[JIANG_CHESS_INDEX]->GetColumn());
    if(pChess[SHI_CHESS_INDEX + 1])pChess[SHI_CHESS_INDEX + 1]->SetPos(pChess[JIANG_CHESS_INDEX]->GetRow() - 1, pChess[JIANG_CHESS_INDEX]->GetColumn());
    
    if(pChess[CHE_CHESS_INDEX])pChess[CHE_CHESS_INDEX + 1]->SetPos(4, pChess[JIANG_CHESS_INDEX]->GetColumn());
    if(pChess[CHE_CHESS_INDEX + 1])pChess[CHE_CHESS_INDEX]->SetPos(pChess[JIANG_CHESS_INDEX]->GetRow() + 4, pChess[JIANG_CHESS_INDEX]->GetColumn());

    if(pChess[PAO_CHESS_INDEX])pChess[PAO_CHESS_INDEX + 1]->SetPos(5, 14);
    if(pChess[PAO_CHESS_INDEX + 1])pChess[PAO_CHESS_INDEX]->SetPos(pChess[PAO_CHESS_INDEX + 1]->GetRow() + 6, pChess[PAO_CHESS_INDEX + 1]->GetColumn());

    if(pChess[XIANG_CHESS_INDEX])pChess[XIANG_CHESS_INDEX]->SetPos(pChess[JIANG_CHESS_INDEX]->GetRow() + 2, pChess[JIANG_CHESS_INDEX]->GetColumn());
    if(pChess[XIANG_CHESS_INDEX + 1])pChess[XIANG_CHESS_INDEX + 1]->SetPos(pChess[JIANG_CHESS_INDEX]->GetRow() - 2, pChess[JIANG_CHESS_INDEX]->GetColumn());

    if(pChess[BING_CHESS_INDEX])pChess[BING_CHESS_INDEX]->SetPos(12, 13);
    for (size_t i = 1; i < BING_CHESS_COUNT; ++i){
        if(pChess[i + BING_CHESS_INDEX])pChess[i + BING_CHESS_INDEX]->SetPos(pChess[i + BING_CHESS_INDEX - 1]->GetRow() - 2, pChess[BING_CHESS_INDEX]->GetColumn());
    }
}
void Chessboard::InitShuChessRowAndColumn(Chess *pChess[DRAW_COUNTRY_CHESS_COUNT]){
    pChess[JIANG_CHESS_INDEX]->SetPos(16, 8);
    
    if(pChess[MA_CHESS_INDEX])pChess[MA_CHESS_INDEX]->SetPos(pChess[JIANG_CHESS_INDEX]->GetRow(), 5);
    if(pChess[MA_CHESS_INDEX + 1])pChess[MA_CHESS_INDEX + 1]->SetPos(pChess[JIANG_CHESS_INDEX]->GetRow(), pChess[JIANG_CHESS_INDEX]->GetColumn() + 3);

    if(pChess[SHI_CHESS_INDEX])pChess[SHI_CHESS_INDEX]->SetPos(pChess[JIANG_CHESS_INDEX]->GetRow(), pChess[JIANG_CHESS_INDEX]->GetColumn() - 1);
    if(pChess[SHI_CHESS_INDEX + 1])pChess[SHI_CHESS_INDEX + 1]->SetPos(pChess[JIANG_CHESS_INDEX]->GetRow(), pChess[JIANG_CHESS_INDEX]->GetColumn() + 1);
    
    if(pChess[CHE_CHESS_INDEX])pChess[CHE_CHESS_INDEX]->SetPos(pChess[JIANG_CHESS_INDEX]->GetRow(), 4);
    if(pChess[CHE_CHESS_INDEX + 1])pChess[CHE_CHESS_INDEX + 1]->SetPos(pChess[JIANG_CHESS_INDEX]->GetRow(), pChess[JIANG_CHESS_INDEX]->GetColumn() + 4);
    
    if(pChess[PAO_CHESS_INDEX])pChess[PAO_CHESS_INDEX]->SetPos(14, pChess[MA_CHESS_INDEX]->GetColumn());
    if(pChess[PAO_CHESS_INDEX + 1])pChess[PAO_CHESS_INDEX + 1]->SetPos(pChess[PAO_CHESS_INDEX]->GetRow(), pChess[MA_CHESS_INDEX + 1]->GetColumn());

    if(pChess[XIANG_CHESS_INDEX])pChess[XIANG_CHESS_INDEX]->SetPos(pChess[JIANG_CHESS_INDEX]->GetRow(), pChess[MA_CHESS_INDEX]->GetColumn() + 1);
    if(pChess[XIANG_CHESS_INDEX + 1])pChess[XIANG_CHESS_INDEX + 1]->SetPos(pChess[JIANG_CHESS_INDEX]->GetRow(), pChess[JIANG_CHESS_INDEX]->GetColumn() + 2);

    if(pChess[BING_CHESS_INDEX])pChess[BING_CHESS_INDEX]->SetPos(13, 4);
    for (size_t i = 1; i < BING_CHESS_COUNT; ++i){
        if(pChess[i + BING_CHESS_INDEX])pChess[i + BING_CHESS_INDEX]->SetPos(pChess[BING_CHESS_INDEX]->GetRow(), pChess[i + BING_CHESS_INDEX - 1]->GetColumn() + 2);
    }
}
void Chessboard::InitWeiChessRowAndColumn(Chess *pChess[DRAW_COUNTRY_CHESS_COUNT]){
    pChess[JIANG_CHESS_INDEX]->SetPos(0, 8);

    pChess[MA_CHESS_INDEX]->SetPos(pChess[JIANG_CHESS_INDEX]->GetRow(), pChess[JIANG_CHESS_INDEX]->GetColumn() + 3);
    pChess[MA_CHESS_INDEX + 1]->SetPos(pChess[JIANG_CHESS_INDEX]->GetRow(), 5);

    pChess[SHI_CHESS_INDEX]->SetPos(pChess[JIANG_CHESS_INDEX]->GetRow(), pChess[JIANG_CHESS_INDEX]->GetColumn() + 1);
    pChess[SHI_CHESS_INDEX + 1]->SetPos(pChess[JIANG_CHESS_INDEX]->GetRow(), pChess[JIANG_CHESS_INDEX]->GetColumn() - 1);
    
    pChess[CHE_CHESS_INDEX]->SetPos(pChess[JIANG_CHESS_INDEX]->GetRow(), pChess[JIANG_CHESS_INDEX]->GetColumn() + 4);
    pChess[CHE_CHESS_INDEX + 1]->SetPos(pChess[JIANG_CHESS_INDEX]->GetRow(), 4);
    
    pChess[PAO_CHESS_INDEX]->SetPos(2, pChess[MA_CHESS_INDEX]->GetColumn());
    pChess[PAO_CHESS_INDEX + 1]->SetPos(pChess[PAO_CHESS_INDEX]->GetRow(), pChess[MA_CHESS_INDEX + 1]->GetColumn());

    pChess[XIANG_CHESS_INDEX]->SetPos(pChess[JIANG_CHESS_INDEX]->GetRow(), pChess[MA_CHESS_INDEX]->GetColumn() - 1);
    pChess[XIANG_CHESS_INDEX + 1]->SetPos(pChess[JIANG_CHESS_INDEX]->GetRow(), pChess[JIANG_CHESS_INDEX]->GetColumn() - 2);

    pChess[BING_CHESS_INDEX]->SetPos(3, 12);
    for (size_t i = 1; i < BING_CHESS_COUNT; ++i){
        pChess[i + BING_CHESS_INDEX]->SetPos(pChess[BING_CHESS_INDEX]->GetRow(), pChess[i + BING_CHESS_INDEX - 1]->GetColumn() - 2);
    }
}
void Chessboard::InitHanChessRowAndColumn(Chess *pChess[DRAW_COUNTRY_CHESS_COUNT]){
    pChess[JIANG_CHESS_INDEX]->SetPos(8, 0);

    pChess[MA_CHESS_INDEX]->SetPos(pChess[JIANG_CHESS_INDEX]->GetRow() - 3, pChess[JIANG_CHESS_INDEX]->GetColumn());
    pChess[MA_CHESS_INDEX + 1]->SetPos(pChess[JIANG_CHESS_INDEX]->GetRow() + 3, pChess[JIANG_CHESS_INDEX]->GetColumn());

    pChess[SHI_CHESS_INDEX]->SetPos(pChess[JIANG_CHESS_INDEX]->GetRow() - 1, pChess[JIANG_CHESS_INDEX]->GetColumn());
    pChess[SHI_CHESS_INDEX + 1]->SetPos(pChess[JIANG_CHESS_INDEX]->GetRow() + 1, pChess[JIANG_CHESS_INDEX]->GetColumn());
    
    pChess[CHE_CHESS_INDEX]->SetPos(4, pChess[JIANG_CHESS_INDEX]->GetColumn());
    pChess[CHE_CHESS_INDEX + 1]->SetPos(pChess[JIANG_CHESS_INDEX]->GetRow() + 4, pChess[JIANG_CHESS_INDEX]->GetColumn());
    
    pChess[PAO_CHESS_INDEX]->SetPos(5, 2);
    pChess[PAO_CHESS_INDEX + 1]->SetPos(pChess[PAO_CHESS_INDEX]->GetRow() + 6, pChess[PAO_CHESS_INDEX]->GetColumn());

    pChess[XIANG_CHESS_INDEX]->SetPos(pChess[JIANG_CHESS_INDEX]->GetRow() - 2, pChess[JIANG_CHESS_INDEX]->GetColumn());
    pChess[XIANG_CHESS_INDEX + 1]->SetPos(pChess[JIANG_CHESS_INDEX]->GetRow() + 2, pChess[JIANG_CHESS_INDEX]->GetColumn());

    pChess[BING_CHESS_INDEX]->SetPos(4, 3);
    for (size_t i = 1; i < BING_CHESS_COUNT; ++i){
        pChess[i + BING_CHESS_INDEX]->SetPos(pChess[i + BING_CHESS_INDEX - 1]->GetRow() + 2, pChess[BING_CHESS_INDEX]->GetColumn());
    }
}
void Chessboard::InitHanChessRowAndColumn(uint32_t country, Chess *pChess[DRAW_COUNTRY_CHESS_COUNT]){
    if(country == WU_COUNTRY_INDEX){
        pChess[JIANG_CHESS_INDEX]->SetPos(0, 8);

        pChess[HAN_CHE_CHESS_INDEX]->SetPos(pChess[JIANG_CHESS_INDEX]->GetRow() + 3, pChess[JIANG_CHESS_INDEX]->GetColumn() + 2);
        pChess[HAN_CHE_CHESS_INDEX + 1]->SetPos(pChess[HAN_CHE_CHESS_INDEX]->GetRow(), pChess[JIANG_CHESS_INDEX]->GetColumn());
        pChess[HAN_CHE_CHESS_INDEX + 2]->SetPos(pChess[HAN_CHE_CHESS_INDEX]->GetRow(), pChess[HAN_CHE_CHESS_INDEX + 1]->GetColumn() - 2);

        pChess[HAN_PAO_CHESS_INDEX]->SetPos(pChess[HAN_CHE_CHESS_INDEX]->GetRow() - 1, pChess[JIANG_CHESS_INDEX]->GetColumn());
    }
    else if(country == WEI_COUNTRY_INDEX){
        pChess[JIANG_CHESS_INDEX]->SetPos(8, CHESSBOARD_COLUMN);

        pChess[HAN_CHE_CHESS_INDEX]->SetPos(10, CHESSBOARD_COLUMN - 3);
        pChess[HAN_CHE_CHESS_INDEX + 1]->SetPos(pChess[JIANG_CHESS_INDEX]->GetRow(), pChess[HAN_CHE_CHESS_INDEX]->GetColumn());
        pChess[HAN_CHE_CHESS_INDEX + 2]->SetPos(pChess[HAN_CHE_CHESS_INDEX + 1]->GetRow() - 2, pChess[HAN_CHE_CHESS_INDEX]->GetColumn());

        pChess[HAN_PAO_CHESS_INDEX]->SetPos(pChess[JIANG_CHESS_INDEX]->GetRow(), pChess[HAN_CHE_CHESS_INDEX]->GetColumn() + 1);
    }
    else if(country == HAN_COUNTRY_INDEX){
        pChess[JIANG_CHESS_INDEX]->SetPos(16, 8);

        pChess[HAN_CHE_CHESS_INDEX]->SetPos(13, 6);
        pChess[HAN_CHE_CHESS_INDEX + 1]->SetPos(pChess[HAN_CHE_CHESS_INDEX]->GetRow(), pChess[JIANG_CHESS_INDEX]->GetColumn());
        pChess[HAN_CHE_CHESS_INDEX + 2]->SetPos(pChess[HAN_CHE_CHESS_INDEX + 1]->GetRow(), pChess[HAN_CHE_CHESS_INDEX + 1]->GetColumn() + 2);

        pChess[HAN_PAO_CHESS_INDEX]->SetPos(pChess[HAN_CHE_CHESS_INDEX + 1]->GetRow() + 1, pChess[JIANG_CHESS_INDEX]->GetColumn());
    }
    else{
        //那就是蜀了
        pChess[JIANG_CHESS_INDEX]->SetPos(8, 0);

        pChess[HAN_CHE_CHESS_INDEX]->SetPos(6, 3);
        pChess[HAN_CHE_CHESS_INDEX + 1]->SetPos(pChess[JIANG_CHESS_INDEX]->GetRow(), pChess[HAN_CHE_CHESS_INDEX]->GetColumn());
        pChess[HAN_CHE_CHESS_INDEX + 2]->SetPos(pChess[HAN_CHE_CHESS_INDEX + 1]->GetRow() + 2, pChess[HAN_CHE_CHESS_INDEX + 1]->GetColumn());

        pChess[HAN_PAO_CHESS_INDEX]->SetPos(pChess[JIANG_CHESS_INDEX]->GetRow(), pChess[HAN_CHE_CHESS_INDEX + 1]->GetColumn() - 1) ;
    }
}
void Chessboard::InitHanChessinfo(Chess *pChess[DRAW_COUNTRY_CHESS_COUNT]){
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
void Chessboard::InitChessInfo(uint32_t country, Chess *pChess[DRAW_COUNTRY_CHESS_COUNT]){
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
void Chessboard::InitializeChess(uint32_t playerCountry){
    for (uint32_t uiCountry = 0; uiCountry < MAX_COUNTRY_INDEX; ++uiCountry){
        DestroyCountry(uiCountry);
    }
    mChess[WU_COUNTRY_INDEX][JIANG_CHESS_INDEX] = new Wu;
    mChess[WEI_COUNTRY_INDEX][JIANG_CHESS_INDEX] = new Wei();
    mChess[SHU_COUNTRY_INDEX][JIANG_CHESS_INDEX] = new Shu();
    mChess[HAN_COUNTRY_INDEX][JIANG_CHESS_INDEX] = new Han();
    if(mHanCanPlay){
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
            if(mHanCanPlay){
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
    //以蜀为参照//把函数名中的国家坐标复制给chessInfo[playerCountry]
    InitShuChessRowAndColumn(mChess[playerCountry]);
    if(!mHanCanPlay)InitHanChessRowAndColumn(playerCountry, mChess[HAN_COUNTRY_INDEX]);
    if(WU_COUNTRY_INDEX == playerCountry){
        InitWuChessRowAndColumn(mChess[WEI_COUNTRY_INDEX]);
        InitHanChessRowAndColumn(mChess[SHU_COUNTRY_INDEX]);
        if(mHanCanPlay)InitWeiChessRowAndColumn(mChess[HAN_COUNTRY_INDEX]);
    }
    else if(WEI_COUNTRY_INDEX == playerCountry){
        InitHanChessRowAndColumn(mChess[WU_COUNTRY_INDEX]);
        InitWeiChessRowAndColumn(mChess[SHU_COUNTRY_INDEX]);
        if(mHanCanPlay)InitWuChessRowAndColumn(mChess[HAN_COUNTRY_INDEX]);
    }
    else if(SHU_COUNTRY_INDEX == playerCountry){
        InitWuChessRowAndColumn(mChess[WU_COUNTRY_INDEX]);
        InitWeiChessRowAndColumn(mChess[WEI_COUNTRY_INDEX]);
        if(mHanCanPlay)InitHanChessRowAndColumn(mChess[HAN_COUNTRY_INDEX]);
    }
    else if(HAN_COUNTRY_INDEX == playerCountry){
        if(mHanCanPlay){
            InitWeiChessRowAndColumn(mChess[WU_COUNTRY_INDEX]);
            InitHanChessRowAndColumn(mChess[WEI_COUNTRY_INDEX]);
            InitWuChessRowAndColumn(mChess[SHU_COUNTRY_INDEX]);
        }
    }
}
void Chessboard::CaptureChess(const Chess *play, const Chess *target){
    if(target){
        const uint32_t dstCountry = target->GetCountry(), dstChess = target->GetChess();
        if(!mHanCanPlay){
            if(target->GetChess() == JIANG_CHESS_INDEX){
                GetCountryChess(play->GetCountry(), dstCountry);
            }
        }
        delete mChess[dstCountry][dstChess];
        mChess[dstCountry][dstChess] = nullptr;
    }
}
void Chessboard::GetCountryChess(uint32_t srcCountry, uint32_t dstCountry){
    if(!mHanCanPlay){
        if(dstCountry == HAN_COUNTRY_INDEX){
            for (uint32_t uiChess = HAN_PAO_CHESS_INDEX; uiChess < HAN_CHESS_COUNT; ++uiChess){
                Chess *pChess = mChess[dstCountry][uiChess];
                if(pChess){
                    uint32_t fontIndex;
                    if(uiChess >= HAN_PAO_CHESS_INDEX && uiChess < HAN_PAO_CHESS_INDEX + HAN_PAO_CHESS_COUNT){
                        fontIndex = FONT_INDEX_PAO;
                        mChess[srcCountry][uiChess + COUNTRY_CHESS_COUNT] = new Pao;
                    }
                    else if(uiChess >= HAN_CHE_CHESS_INDEX && uiChess < HAN_CHE_CHESS_INDEX + HAN_CHE_CHESS_COUNT){
                        fontIndex = FONT_INDEX_CHE;
                        mChess[srcCountry][uiChess + COUNTRY_CHESS_COUNT] = new Che;
                    }
                    mChess[srcCountry][uiChess + COUNTRY_CHESS_COUNT]->SetCountry(srcCountry);
                    mChess[srcCountry][uiChess + COUNTRY_CHESS_COUNT]->SetFontIndex(fontIndex);
                    mChess[srcCountry][uiChess + COUNTRY_CHESS_COUNT]->SetChess(uiChess + COUNTRY_CHESS_COUNT);
                    mChess[srcCountry][uiChess + COUNTRY_CHESS_COUNT]->SetPos(pChess->GetRow(), pChess->GetColumn());

                    delete mChess[dstCountry][uiChess];
                    mChess[dstCountry][uiChess] = nullptr;
                }
            }
        }
    }
}
uint32_t Chessboard::GetChessCount(uint32_t country){
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
    if(country != MAX_COUNTRY_INDEX){
        for (uint32_t uiChess = 0; uiChess < DRAW_COUNTRY_CHESS_COUNT; ++uiChess){
            if(mChess[country][uiChess] && mChess[country][uiChess]->GetRow() == row && mChess[country][uiChess]->GetColumn() == column){
                pChess = mChess[country][uiChess];
                break;
            }
        }
    }
    return pChess;
}
void Chessboard::DestroyCountry(uint32_t country){
    for (size_t uiChess = 0; uiChess < COUNTRY_CHESS_COUNT; ++uiChess){
        if(mChess[country][uiChess]){
            delete mChess[country][uiChess];
            mChess[country][uiChess] = nullptr;
        }
    }
}