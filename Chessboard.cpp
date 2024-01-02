#include "Chessboard.h"
void Chessboard::GetCountryChess(uint32_t srcCountry, uint32_t dstCountry){
    //想获得并使用其他国家的棋子就必须多显示几个棋子
    if(dstCountry == HAN_COUNTRY_INDEX){
        ChessInfo chessInfo;
        for (uint32_t uiChess = 1; uiChess < HAN_CHESS_COUNT; ++uiChess){
            if(mChess[dstCountry][uiChess]){
                chessInfo = *mChess[dstCountry][uiChess]->GetInfo();
                chessInfo.country = srcCountry;
                chessInfo.chess = MAX_CHESS_INDEX + uiChess;
                if(uiChess == PAO_CHESS_INDEX_3){
                    mChess[srcCountry][chessInfo.chess] = new Pao(chessInfo);
                }
                else if(uiChess == CHE_CHESS_INDEX_3 || uiChess == CHE_CHESS_INDEX_4 || uiChess == CHE_CHESS_INDEX_5){
                    mChess[srcCountry][chessInfo.chess] = new Che(chessInfo);
                }
                mChess[srcCountry][chessInfo.chess]->ResetInfo(chessInfo);
                //下面是获得其他势力的牌
                // if(uiChess == MA_CHESS_INDEX_1 || uiChess == MA_CHESS_INDEX_2){
                //     mChess[srcCountry][chessInfo.chess] = new Ma(chessInfo);
                // }
                // else if(uiChess == SHI_CHESS_INDEX_1 || uiChess == SHI_CHESS_INDEX_2){
                //     mChess[srcCountry][chessInfo.chess] = new Shi(chessInfo);
                // }
                // else if(uiChess == XIANG_CHESS_INDEX_1 || uiChess == XIANG_CHESS_INDEX_2){
                //     mChess[srcCountry][chessInfo.chess] = new Xiang(chessInfo);
                // }
                // else if(uiChess == PAO_CHESS_INDEX_1 || uiChess == PAO_CHESS_INDEX_2 || uiChess == PAO_CHESS_INDEX_3){
                //     mChess[srcCountry][chessInfo.chess] = new Pao(chessInfo);
                // }
                // else if(uiChess == CHE_CHESS_INDEX_1 || uiChess == CHE_CHESS_INDEX_2 || uiChess == CHE_CHESS_INDEX_3 || uiChess == CHE_CHESS_INDEX_4 || uiChess == CHE_CHESS_INDEX_5){
                //     mChess[srcCountry][chessInfo.chess] = new Che(chessInfo);
                // }
                // else if(uiChess == BING_CHESS_INDEX_1 || uiChess == BING_CHESS_INDEX_2 || uiChess == BING_CHESS_INDEX_3 || uiChess == BING_CHESS_INDEX_4 || uiChess == BING_CHESS_INDEX_5){
                //     mChess[srcCountry][chessInfo.chess] = new Bing(chessInfo);
                // }
                delete mChess[dstCountry][uiChess];
                mChess[dstCountry][uiChess] = nullptr;
            }
        }
    }
}
void Chessboard::InitWuChessRowAndColumn(ChessInfo chessInfo[4][COUNTRY_CHESS_COUNT]){
    chessInfo[WU_COUNTRY_INDEX][JIANG_CHESS_INDEX].row = 8;
    chessInfo[WU_COUNTRY_INDEX][JIANG_CHESS_INDEX].column = 16;

    chessInfo[WU_COUNTRY_INDEX][MA_CHESS_INDEX_1].row = chessInfo[WU_COUNTRY_INDEX][JIANG_CHESS_INDEX].row - 3;
    chessInfo[WU_COUNTRY_INDEX][MA_CHESS_INDEX_1].column = chessInfo[WU_COUNTRY_INDEX][JIANG_CHESS_INDEX].column;

    chessInfo[WU_COUNTRY_INDEX][MA_CHESS_INDEX_2].row = chessInfo[WU_COUNTRY_INDEX][JIANG_CHESS_INDEX].row + 3;
    chessInfo[WU_COUNTRY_INDEX][MA_CHESS_INDEX_2].column = chessInfo[WU_COUNTRY_INDEX][JIANG_CHESS_INDEX].column;

    chessInfo[WU_COUNTRY_INDEX][SHI_CHESS_INDEX_1].row = chessInfo[WU_COUNTRY_INDEX][JIANG_CHESS_INDEX].row - 1;
    chessInfo[WU_COUNTRY_INDEX][SHI_CHESS_INDEX_1].column = chessInfo[WU_COUNTRY_INDEX][JIANG_CHESS_INDEX].column;
    
    chessInfo[WU_COUNTRY_INDEX][SHI_CHESS_INDEX_2].row = chessInfo[WU_COUNTRY_INDEX][JIANG_CHESS_INDEX].row + 1;
    chessInfo[WU_COUNTRY_INDEX][SHI_CHESS_INDEX_2].column = chessInfo[WU_COUNTRY_INDEX][JIANG_CHESS_INDEX].column;
    
    chessInfo[WU_COUNTRY_INDEX][CHE_CHESS_INDEX_1].row = 4;
    chessInfo[WU_COUNTRY_INDEX][CHE_CHESS_INDEX_1].column = chessInfo[WU_COUNTRY_INDEX][JIANG_CHESS_INDEX].column;
    
    chessInfo[WU_COUNTRY_INDEX][CHE_CHESS_INDEX_2].row = chessInfo[WU_COUNTRY_INDEX][JIANG_CHESS_INDEX].row + 4;
    chessInfo[WU_COUNTRY_INDEX][CHE_CHESS_INDEX_2].column = chessInfo[WU_COUNTRY_INDEX][JIANG_CHESS_INDEX].column;
    
    chessInfo[WU_COUNTRY_INDEX][PAO_CHESS_INDEX_1].row = 5;
    chessInfo[WU_COUNTRY_INDEX][PAO_CHESS_INDEX_1].column = 14;
    
    chessInfo[WU_COUNTRY_INDEX][PAO_CHESS_INDEX_2].row = chessInfo[WU_COUNTRY_INDEX][PAO_CHESS_INDEX_1].row + 6;
    chessInfo[WU_COUNTRY_INDEX][PAO_CHESS_INDEX_2].column = chessInfo[WU_COUNTRY_INDEX][PAO_CHESS_INDEX_1].column;
    
    chessInfo[WU_COUNTRY_INDEX][BING_CHESS_INDEX_1].row = 4;
    chessInfo[WU_COUNTRY_INDEX][BING_CHESS_INDEX_1].column = 13;
    for (size_t i = 1; i < BING_CHESS_COUNT; ++i){
        chessInfo[WU_COUNTRY_INDEX][i + BING_CHESS_INDEX_1].row = chessInfo[WU_COUNTRY_INDEX][i + BING_CHESS_INDEX_1 - 1].row + 2;;
        chessInfo[WU_COUNTRY_INDEX][i + BING_CHESS_INDEX_1].column = chessInfo[WU_COUNTRY_INDEX][BING_CHESS_INDEX_1].column;
    }

    chessInfo[WU_COUNTRY_INDEX][XIANG_CHESS_INDEX_1].row = chessInfo[WU_COUNTRY_INDEX][JIANG_CHESS_INDEX].row - 2;
    chessInfo[WU_COUNTRY_INDEX][XIANG_CHESS_INDEX_1].column = chessInfo[WU_COUNTRY_INDEX][JIANG_CHESS_INDEX].column;

    chessInfo[WU_COUNTRY_INDEX][XIANG_CHESS_INDEX_2].row = chessInfo[WU_COUNTRY_INDEX][JIANG_CHESS_INDEX].row + 2;
    chessInfo[WU_COUNTRY_INDEX][XIANG_CHESS_INDEX_2].column = chessInfo[WU_COUNTRY_INDEX][JIANG_CHESS_INDEX].column;
}
void Chessboard::InitShuChessRowAndColumn(ChessInfo chessInfo[4][COUNTRY_CHESS_COUNT]){
    chessInfo[SHU_COUNTRY_INDEX][JIANG_CHESS_INDEX].row = 16;
    chessInfo[SHU_COUNTRY_INDEX][JIANG_CHESS_INDEX].column = 8;

    chessInfo[SHU_COUNTRY_INDEX][MA_CHESS_INDEX_1].row = chessInfo[SHU_COUNTRY_INDEX][JIANG_CHESS_INDEX].row;
    chessInfo[SHU_COUNTRY_INDEX][MA_CHESS_INDEX_1].column = 5;

    chessInfo[SHU_COUNTRY_INDEX][MA_CHESS_INDEX_2].row = chessInfo[SHU_COUNTRY_INDEX][JIANG_CHESS_INDEX].row;
    chessInfo[SHU_COUNTRY_INDEX][MA_CHESS_INDEX_2].column = chessInfo[SHU_COUNTRY_INDEX][JIANG_CHESS_INDEX].column + 3;

    chessInfo[SHU_COUNTRY_INDEX][SHI_CHESS_INDEX_1].row = chessInfo[SHU_COUNTRY_INDEX][JIANG_CHESS_INDEX].row;
    chessInfo[SHU_COUNTRY_INDEX][SHI_CHESS_INDEX_1].column = chessInfo[SHU_COUNTRY_INDEX][MA_CHESS_INDEX_1].column + 2;
    
    chessInfo[SHU_COUNTRY_INDEX][SHI_CHESS_INDEX_2].row = chessInfo[SHU_COUNTRY_INDEX][JIANG_CHESS_INDEX].row;
    chessInfo[SHU_COUNTRY_INDEX][SHI_CHESS_INDEX_2].column = chessInfo[SHU_COUNTRY_INDEX][JIANG_CHESS_INDEX].column + 1;
    
    chessInfo[SHU_COUNTRY_INDEX][CHE_CHESS_INDEX_1].row = chessInfo[SHU_COUNTRY_INDEX][JIANG_CHESS_INDEX].row;
    chessInfo[SHU_COUNTRY_INDEX][CHE_CHESS_INDEX_1].column = 4;
    
    chessInfo[SHU_COUNTRY_INDEX][CHE_CHESS_INDEX_2].row = chessInfo[SHU_COUNTRY_INDEX][JIANG_CHESS_INDEX].row;
    chessInfo[SHU_COUNTRY_INDEX][CHE_CHESS_INDEX_2].column = chessInfo[SHU_COUNTRY_INDEX][JIANG_CHESS_INDEX].column + 4;
    
    chessInfo[SHU_COUNTRY_INDEX][PAO_CHESS_INDEX_1].row = 14;
    chessInfo[SHU_COUNTRY_INDEX][PAO_CHESS_INDEX_1].column = chessInfo[SHU_COUNTRY_INDEX][MA_CHESS_INDEX_1].column;
    
    chessInfo[SHU_COUNTRY_INDEX][PAO_CHESS_INDEX_2].row = chessInfo[SHU_COUNTRY_INDEX][PAO_CHESS_INDEX_1].row;
    chessInfo[SHU_COUNTRY_INDEX][PAO_CHESS_INDEX_2].column = chessInfo[SHU_COUNTRY_INDEX][MA_CHESS_INDEX_2].column;

    chessInfo[SHU_COUNTRY_INDEX][BING_CHESS_INDEX_1].row = 13;
    chessInfo[SHU_COUNTRY_INDEX][BING_CHESS_INDEX_1].column = 4;
    for (size_t i = 1; i < BING_CHESS_COUNT; ++i){
        chessInfo[SHU_COUNTRY_INDEX][i + BING_CHESS_INDEX_1].row = chessInfo[SHU_COUNTRY_INDEX][BING_CHESS_INDEX_1].row;
        chessInfo[SHU_COUNTRY_INDEX][i + BING_CHESS_INDEX_1].column = chessInfo[SHU_COUNTRY_INDEX][i + BING_CHESS_INDEX_1 - 1].column + 2;
    }

    chessInfo[SHU_COUNTRY_INDEX][XIANG_CHESS_INDEX_1].row = chessInfo[SHU_COUNTRY_INDEX][JIANG_CHESS_INDEX].row;
    chessInfo[SHU_COUNTRY_INDEX][XIANG_CHESS_INDEX_1].column = chessInfo[SHU_COUNTRY_INDEX][MA_CHESS_INDEX_1].column + 1;

    chessInfo[SHU_COUNTRY_INDEX][XIANG_CHESS_INDEX_2].row = chessInfo[SHU_COUNTRY_INDEX][JIANG_CHESS_INDEX].row;
    chessInfo[SHU_COUNTRY_INDEX][XIANG_CHESS_INDEX_2].column = chessInfo[SHU_COUNTRY_INDEX][JIANG_CHESS_INDEX].column + 2;
}
void Chessboard::InitWeiChessRowAndColumn(ChessInfo chessInfo[4][COUNTRY_CHESS_COUNT]){
    chessInfo[WEI_COUNTRY_INDEX][JIANG_CHESS_INDEX].row = 0;
    chessInfo[WEI_COUNTRY_INDEX][JIANG_CHESS_INDEX].column = 8;

    chessInfo[WEI_COUNTRY_INDEX][MA_CHESS_INDEX_1].row = chessInfo[WEI_COUNTRY_INDEX][JIANG_CHESS_INDEX].row;
    chessInfo[WEI_COUNTRY_INDEX][MA_CHESS_INDEX_1].column = 5;

    chessInfo[WEI_COUNTRY_INDEX][MA_CHESS_INDEX_2].row = chessInfo[WEI_COUNTRY_INDEX][JIANG_CHESS_INDEX].row;
    chessInfo[WEI_COUNTRY_INDEX][MA_CHESS_INDEX_2].column = chessInfo[WEI_COUNTRY_INDEX][JIANG_CHESS_INDEX].column + 3;

    chessInfo[WEI_COUNTRY_INDEX][SHI_CHESS_INDEX_1].row = chessInfo[WEI_COUNTRY_INDEX][JIANG_CHESS_INDEX].row;
    chessInfo[WEI_COUNTRY_INDEX][SHI_CHESS_INDEX_1].column = chessInfo[WEI_COUNTRY_INDEX][JIANG_CHESS_INDEX].column - 1;
    
    chessInfo[WEI_COUNTRY_INDEX][SHI_CHESS_INDEX_2].row = chessInfo[WEI_COUNTRY_INDEX][JIANG_CHESS_INDEX].row;
    chessInfo[WEI_COUNTRY_INDEX][SHI_CHESS_INDEX_2].column = chessInfo[WEI_COUNTRY_INDEX][JIANG_CHESS_INDEX].column + 1;
    
    chessInfo[WEI_COUNTRY_INDEX][CHE_CHESS_INDEX_1].row = chessInfo[WEI_COUNTRY_INDEX][JIANG_CHESS_INDEX].row;
    chessInfo[WEI_COUNTRY_INDEX][CHE_CHESS_INDEX_1].column = 4;
    
    chessInfo[WEI_COUNTRY_INDEX][CHE_CHESS_INDEX_2].row = chessInfo[WEI_COUNTRY_INDEX][JIANG_CHESS_INDEX].row;
    chessInfo[WEI_COUNTRY_INDEX][CHE_CHESS_INDEX_2].column = chessInfo[WEI_COUNTRY_INDEX][JIANG_CHESS_INDEX].column + 4;
    
    chessInfo[WEI_COUNTRY_INDEX][PAO_CHESS_INDEX_1].row = 2;
    chessInfo[WEI_COUNTRY_INDEX][PAO_CHESS_INDEX_1].column = chessInfo[WEI_COUNTRY_INDEX][MA_CHESS_INDEX_1].column;
    
    chessInfo[WEI_COUNTRY_INDEX][PAO_CHESS_INDEX_2].row = chessInfo[WEI_COUNTRY_INDEX][PAO_CHESS_INDEX_1].row;
    chessInfo[WEI_COUNTRY_INDEX][PAO_CHESS_INDEX_2].column = chessInfo[WEI_COUNTRY_INDEX][MA_CHESS_INDEX_2].column;
    
    chessInfo[WEI_COUNTRY_INDEX][BING_CHESS_INDEX_1].row = 3;
    chessInfo[WEI_COUNTRY_INDEX][BING_CHESS_INDEX_1].column = 4;
    for (size_t i = 1; i < BING_CHESS_COUNT; ++i){
        chessInfo[WEI_COUNTRY_INDEX][i + BING_CHESS_INDEX_1].row = chessInfo[WEI_COUNTRY_INDEX][BING_CHESS_INDEX_1].row;
        chessInfo[WEI_COUNTRY_INDEX][i + BING_CHESS_INDEX_1].column = chessInfo[WEI_COUNTRY_INDEX][i + BING_CHESS_INDEX_1 - 1].column + 2;
    }

    chessInfo[WEI_COUNTRY_INDEX][XIANG_CHESS_INDEX_1].row = chessInfo[WEI_COUNTRY_INDEX][JIANG_CHESS_INDEX].row;
    chessInfo[WEI_COUNTRY_INDEX][XIANG_CHESS_INDEX_1].column = chessInfo[WEI_COUNTRY_INDEX][MA_CHESS_INDEX_1].column + 1;

    chessInfo[WEI_COUNTRY_INDEX][XIANG_CHESS_INDEX_2].row = chessInfo[WEI_COUNTRY_INDEX][JIANG_CHESS_INDEX].row;
    chessInfo[WEI_COUNTRY_INDEX][XIANG_CHESS_INDEX_2].column = chessInfo[WEI_COUNTRY_INDEX][JIANG_CHESS_INDEX].column + 2;
}
void Chessboard::InitChessInfo(uint32_t country, ChessInfo chessInfo[4][COUNTRY_CHESS_COUNT]){
    for (size_t uiChess = 0; uiChess < COUNTRY_CHESS_COUNT; ++uiChess){
        chessInfo[country][uiChess].row = CHESSBOARD_ROW + 100;
        chessInfo[country][uiChess].column = CHESSBOARD_COLUMN + 100;
    }
    uint32_t fontIndex[] = { FONT_INDEX_WEI, FONT_INDEX_SHU, FONT_INDEX_WU, FONT_INDEX_HAN };
    chessInfo[country][JIANG_CHESS_INDEX].country = country;
    chessInfo[country][JIANG_CHESS_INDEX].chess = JIANG_CHESS_INDEX;
    chessInfo[country][JIANG_CHESS_INDEX].fontIndex = fontIndex[country];
    if(country != HAN_COUNTRY_INDEX){
        chessInfo[country][MA_CHESS_INDEX_1].chess = MA_CHESS_INDEX_1;
        chessInfo[country][MA_CHESS_INDEX_2].chess = MA_CHESS_INDEX_2;
        chessInfo[country][SHI_CHESS_INDEX_1].chess = SHI_CHESS_INDEX_1;
        chessInfo[country][SHI_CHESS_INDEX_2].chess = SHI_CHESS_INDEX_2;
        chessInfo[country][CHE_CHESS_INDEX_1].chess = CHE_CHESS_INDEX_1;
        chessInfo[country][CHE_CHESS_INDEX_2].chess = CHE_CHESS_INDEX_2;
        chessInfo[country][PAO_CHESS_INDEX_1].chess = PAO_CHESS_INDEX_1;
        chessInfo[country][PAO_CHESS_INDEX_2].chess = PAO_CHESS_INDEX_2;
        chessInfo[country][BING_CHESS_INDEX_1].chess = BING_CHESS_INDEX_1;
        chessInfo[country][BING_CHESS_INDEX_2].chess = BING_CHESS_INDEX_2;
        chessInfo[country][BING_CHESS_INDEX_3].chess = BING_CHESS_INDEX_3;
        chessInfo[country][BING_CHESS_INDEX_4].chess = BING_CHESS_INDEX_4;
        chessInfo[country][BING_CHESS_INDEX_5].chess = BING_CHESS_INDEX_5;
        chessInfo[country][XIANG_CHESS_INDEX_1].chess = XIANG_CHESS_INDEX_1;
        chessInfo[country][XIANG_CHESS_INDEX_2].chess = XIANG_CHESS_INDEX_2;
        chessInfo[country][MA_CHESS_INDEX_1].country = country;
        chessInfo[country][MA_CHESS_INDEX_2].country = country;
        chessInfo[country][SHI_CHESS_INDEX_1].country = country;
        chessInfo[country][SHI_CHESS_INDEX_2].country = country;
        chessInfo[country][CHE_CHESS_INDEX_1].country = country;
        chessInfo[country][CHE_CHESS_INDEX_2].country = country;
        chessInfo[country][PAO_CHESS_INDEX_1].country = country;
        chessInfo[country][PAO_CHESS_INDEX_2].country = country;
        chessInfo[country][BING_CHESS_INDEX_1].country = country;
        chessInfo[country][BING_CHESS_INDEX_2].country = country;
        chessInfo[country][BING_CHESS_INDEX_3].country = country;
        chessInfo[country][BING_CHESS_INDEX_4].country = country;
        chessInfo[country][BING_CHESS_INDEX_5].country = country;
        chessInfo[country][XIANG_CHESS_INDEX_1].country = country;
        chessInfo[country][XIANG_CHESS_INDEX_2].country = country;

        chessInfo[country][MA_CHESS_INDEX_1].fontIndex = FONT_INDEX_MA;
        chessInfo[country][MA_CHESS_INDEX_2].fontIndex = FONT_INDEX_MA;
        chessInfo[country][SHI_CHESS_INDEX_1].fontIndex = FONT_INDEX_SHI;
        chessInfo[country][SHI_CHESS_INDEX_2].fontIndex = FONT_INDEX_SHI;
        chessInfo[country][CHE_CHESS_INDEX_1].fontIndex = FONT_INDEX_CHE;
        chessInfo[country][CHE_CHESS_INDEX_2].fontIndex = FONT_INDEX_CHE;
        chessInfo[country][PAO_CHESS_INDEX_1].fontIndex = FONT_INDEX_PAO;
        chessInfo[country][PAO_CHESS_INDEX_2].fontIndex = FONT_INDEX_PAO;
        chessInfo[country][BING_CHESS_INDEX_1].fontIndex = FONT_INDEX_BING;
        chessInfo[country][BING_CHESS_INDEX_2].fontIndex = FONT_INDEX_BING;
        chessInfo[country][BING_CHESS_INDEX_3].fontIndex = FONT_INDEX_BING;
        chessInfo[country][BING_CHESS_INDEX_4].fontIndex = FONT_INDEX_BING;
        chessInfo[country][BING_CHESS_INDEX_5].fontIndex = FONT_INDEX_BING;
        chessInfo[country][XIANG_CHESS_INDEX_1].fontIndex = FONT_INDEX_XIANG;
        chessInfo[country][XIANG_CHESS_INDEX_2].fontIndex = FONT_INDEX_XIANG;
        if(country == WU_COUNTRY_INDEX){
            InitWuChessRowAndColumn(chessInfo);
        }
        else if(country == WEI_COUNTRY_INDEX){
            InitWeiChessRowAndColumn(chessInfo);
        }
        else{
            InitShuChessRowAndColumn(chessInfo);
        }
    }
    else{
        chessInfo[country][CHE_CHESS_INDEX_3].chess = CHE_CHESS_INDEX_3;
        chessInfo[country][CHE_CHESS_INDEX_4].chess = CHE_CHESS_INDEX_4;
        chessInfo[country][CHE_CHESS_INDEX_5].chess = CHE_CHESS_INDEX_5;
        chessInfo[country][PAO_CHESS_INDEX_3].chess = PAO_CHESS_INDEX_3;

        chessInfo[country][CHE_CHESS_INDEX_3].country = HAN_COUNTRY_INDEX;
        chessInfo[country][CHE_CHESS_INDEX_4].country = HAN_COUNTRY_INDEX;
        chessInfo[country][CHE_CHESS_INDEX_5].country = HAN_COUNTRY_INDEX;
        chessInfo[country][PAO_CHESS_INDEX_3].country = HAN_COUNTRY_INDEX;

        chessInfo[country][CHE_CHESS_INDEX_3].fontIndex = FONT_INDEX_CHE;
        chessInfo[country][CHE_CHESS_INDEX_4].fontIndex = FONT_INDEX_CHE;
        chessInfo[country][CHE_CHESS_INDEX_5].fontIndex = FONT_INDEX_CHE;
        chessInfo[country][PAO_CHESS_INDEX_3].fontIndex = FONT_INDEX_PAO;

        chessInfo[country][JIANG_CHESS_INDEX].row = 8;
        chessInfo[country][JIANG_CHESS_INDEX].column = 0;

        chessInfo[country][CHE_CHESS_INDEX_3].row = 6;
        chessInfo[country][CHE_CHESS_INDEX_3].column = 3;

        chessInfo[country][CHE_CHESS_INDEX_4].row = chessInfo[country][JIANG_CHESS_INDEX].row;
        chessInfo[country][CHE_CHESS_INDEX_4].column = chessInfo[country][CHE_CHESS_INDEX_3].column;

        chessInfo[country][CHE_CHESS_INDEX_5].row = chessInfo[country][CHE_CHESS_INDEX_4].row + 2;
        chessInfo[country][CHE_CHESS_INDEX_5].column = chessInfo[country][CHE_CHESS_INDEX_3].column;

        chessInfo[country][PAO_CHESS_INDEX_3].row = chessInfo[country][JIANG_CHESS_INDEX].row;
        chessInfo[country][PAO_CHESS_INDEX_3].column = chessInfo[country][CHE_CHESS_INDEX_3].column - 1;
    }
}
Chessboard::Chessboard(/* args */){
    for (uint32_t uiCountry = 0; uiCountry < 4; ++uiCountry){
        for (uint32_t uiChess = 0; uiChess < COUNTRY_CHESS_COUNT; ++uiChess){
            mChess[uiCountry][uiChess] = nullptr;
        }
    }
}

Chessboard::~Chessboard(){
}
void Chessboard::Cleanup(VkDevice device){
    for (size_t uiCountry = 0; uiCountry < 4; ++uiCountry){
        for (size_t uiChess = 0; uiChess < COUNTRY_CHESS_COUNT; ++uiChess){
            if(mChess[uiCountry][uiChess]){
                delete mChess[uiCountry][uiChess];
                mChess[uiCountry][uiChess] = nullptr;
            }
        }
    }
    VulkanChess::Cleanup(device);
    VulkanChessboard::Cleanup(device);
}
void Chessboard::InitChess(VkDevice device){
    ChessInfo chessInfo[4][COUNTRY_CHESS_COUNT];
    for (size_t i = 0; i < 4; i++){
        InitChessInfo(i, chessInfo);
    }
    for (uint32_t uiCountry = 0; uiCountry < 4; ++uiCountry){
        for (uint32_t uiChess = 0; uiChess < COUNTRY_CHESS_COUNT; ++uiChess){
            if(mChess[uiCountry][uiChess]){
                delete mChess[uiCountry][uiChess];
                mChess[uiCountry][uiChess] = nullptr;
            }
        }   
    }
    mChess[WU_COUNTRY_INDEX][JIANG_CHESS_INDEX] = new Wu(chessInfo[WU_COUNTRY_INDEX][JIANG_CHESS_INDEX]);
    mChess[SHU_COUNTRY_INDEX][JIANG_CHESS_INDEX] = new Shu(chessInfo[SHU_COUNTRY_INDEX][JIANG_CHESS_INDEX]);
    mChess[WEI_COUNTRY_INDEX][JIANG_CHESS_INDEX] = new Wei(chessInfo[WEI_COUNTRY_INDEX][JIANG_CHESS_INDEX]);
    mChess[HAN_COUNTRY_INDEX][JIANG_CHESS_INDEX] = new Han(chessInfo[HAN_COUNTRY_INDEX][JIANG_CHESS_INDEX]);
    for (uint32_t uiCountry = 0; uiCountry < 4; ++uiCountry){
        if(uiCountry != HAN_COUNTRY_INDEX){
            for (uint32_t uiChess = 0; uiChess < COUNTRY_CHESS_COUNT; ++uiChess){
                if(uiChess == MA_CHESS_INDEX_1 || uiChess == MA_CHESS_INDEX_2){
                    mChess[uiCountry][uiChess] = new Ma(chessInfo[uiCountry][uiChess]);
                }
                else if(uiChess == PAO_CHESS_INDEX_1 || uiChess == PAO_CHESS_INDEX_2){
                    mChess[uiCountry][uiChess] = new Pao(chessInfo[uiCountry][uiChess]);
                }
                else if(uiChess == CHE_CHESS_INDEX_1 || uiChess == CHE_CHESS_INDEX_2){
                    mChess[uiCountry][uiChess] = new Che(chessInfo[uiCountry][uiChess]);
                }
                else if(uiChess == SHI_CHESS_INDEX_1 || uiChess == SHI_CHESS_INDEX_2){
                    mChess[uiCountry][uiChess] = new Shi(chessInfo[uiCountry][uiChess]);
                }
                else if(uiChess == XIANG_CHESS_INDEX_1 || uiChess == XIANG_CHESS_INDEX_2){
                    mChess[uiCountry][uiChess] = new Xiang(chessInfo[uiCountry][uiChess]);
                }
                else if(uiChess == BING_CHESS_INDEX_1 || uiChess == BING_CHESS_INDEX_2 || uiChess == BING_CHESS_INDEX_3 || uiChess == BING_CHESS_INDEX_4 || uiChess == BING_CHESS_INDEX_5){
                    mChess[uiCountry][uiChess] = new Bing(chessInfo[uiCountry][uiChess]);
                }   
            }
        }
        else{
                mChess[uiCountry][PAO_CHESS_INDEX_3] = new Pao(chessInfo[uiCountry][PAO_CHESS_INDEX_3]);

                mChess[uiCountry][CHE_CHESS_INDEX_3] = new Che(chessInfo[uiCountry][CHE_CHESS_INDEX_3]);
                mChess[uiCountry][CHE_CHESS_INDEX_4] = new Che(chessInfo[uiCountry][CHE_CHESS_INDEX_4]);
                mChess[uiCountry][CHE_CHESS_INDEX_5] = new Che(chessInfo[uiCountry][CHE_CHESS_INDEX_5]);
        }
    }
    UpdateChess(device);
}
void Chessboard::UpdateChess(VkDevice device){
    VkExtent2D chessSize;
    chessSize.width = CHESS_WIDTH;
    chessSize.height = CHESS_HEIGHT;
    const ChessInfo *pChessInfo = nullptr;
    for (uint32_t uiCoutry = 0; uiCoutry < 4; ++uiCoutry){
        for(uint32_t uiChess = 0; uiChess < COUNTRY_CHESS_COUNT; ++uiChess){
            if(mChess[uiCoutry][uiChess]){
                pChessInfo = mChess[uiCoutry][uiChess]->GetInfo();
                UpdateChessUniform(device, uiCoutry, uiChess, pChessInfo->row, pChessInfo->column, pChessInfo->fontIndex, chessSize);
            }
            else{
                UpdateChessUniform(device, uiCoutry, uiChess, CHESSBOARD_ROW + 100, CHESSBOARD_COLUMN + 100, 0, chessSize);
            }
            // printf("第%d个国家, 第%d颗棋子, 行:%d, 列:%d\n", uiCoutry, uiChess, chessInfo.row, chessInfo.column);
        }
    }
}
void Chessboard::ClearCanPlay(VkDevice device){
    // mCanplays.clear();
    // mSelected = nullptr;
    if(device != VK_NULL_HANDLE)ClearSelectWireframeUnfirom(device);
}
// int32_t Chessboard::GetCanPlay(const glm::vec2 &mousePos){
//     int32_t index = -1;
//     for (size_t i = 0; i < mCanplays.size(); ++i){
//         const uint32_t chessY = ROW_TO_Y(mCanplays[i].row) - CHESSBOARD_GRID_SIZE * .5;
//         const uint32_t chessX = COLUMN_TO_X(mCanplays[i].column) - CHESSBOARD_GRID_SIZE * .5;
//         if(mousePos.x > chessX && mousePos.y > chessY && mousePos.x < chessX + CHESSBOARD_GRID_SIZE && mousePos.y < chessY + CHESSBOARD_GRID_SIZE){
//             index = i;
//             break;
//         }
//     }
//     return index;
// }
// int32_t Chessboard::GetCanPlay(uint32_t row, uint32_t column){
//     int32_t index = -1;
//     for (size_t i = 0; i < mCanplays.size(); ++i){
//         if(row == mCanplays[i].row && column == mCanplays[i].column){
//             index = i;
//             break;
//         }
//     }
//     return index;
// }
bool Chessboard::IsBoundary(uint32_t row, uint32_t column) const{
    if(row > 16 || column > 16){
        // printf("到达边界, row:%u, column:%u\n", row, column);
        return true;
    }
    else if ((row < 4 || row > 12) && (row < 0 || column < 4 || column > 12)){
        // printf("到达边界, row:%u, column:%u\n", row, column);
        return true;
    }
    return false;
}
// const ChessInfo *Chessboard::GetSelectInfo(const glm::vec2 &pos){
//     const ChessInfo *pInfo = nullptr;
//     for (size_t uiCountry = 0; uiCountry < 4; ++uiCountry){
//         for (size_t uiChess = 0; uiChess < COUNTRY_CHESS_COUNT; ++uiChess){
//             if(mChess[uiCountry][uiChess] && mChess[uiCountry][uiChess]->IsSelect(pos)){
//                 pInfo = mChess[uiCountry][uiChess]->GetInfo();
//                 break;
//             }
//         }
//         if(pInfo){
//             break;
//         }
//     }
//     return pInfo;
// }
void Chessboard::DestroyCountry(VkDevice device, uint32_t country){
    for (size_t uiChess = 0; uiChess < COUNTRY_CHESS_COUNT; ++uiChess){
        if(mChess[country][uiChess]){
            delete mChess[country][uiChess];
            mChess[country][uiChess] = nullptr;
        }
    }
    UpdateChess(device);
}
const ChessInfo *Chessboard::GetChessInfos(const glm::vec2 &mousePos){
    const ChessInfo *pInfor = nullptr;
    for (uint32_t uiCountry = 0; uiCountry < MAX_COUNTRY_INDEX; ++uiCountry){
        pInfor = GetChessInfos(uiCountry, mousePos);
        if(pInfor){
            break;
        }
    }
    return pInfor;
}
const ChessInfo *Chessboard::GetChessInfos(uint32_t row, uint32_t column){
    const ChessInfo *pInfor = nullptr;
    for (size_t uiCountry = 0; uiCountry < MAX_COUNTRY_INDEX; ++uiCountry){
        if(pInfor = GetChessInfos(uiCountry, row, column)){
            break;
        }
    }
    return pInfor;
}
void Chessboard::Select(VkDevice device, uint32_t country, uint32_t chess){
    std::vector<ChessInfo>canplays;
    const ChessInfo *pChessInfo = GetChessInfo(country, chess);
    if(!IsBoundary(pChessInfo->row, pChessInfo->column)){
        if(device != VK_NULL_HANDLE)ClearSelectWireframeUnfirom(device);
        mChess[pChessInfo->country][pChessInfo->chess]->Selected((const Chess *(*)[COUNTRY_CHESS_COUNT])mChess, canplays);
    }
    if(device != VK_NULL_HANDLE){
        VkExtent2D size;
        size.width = CHESS_WIDTH * 1.05;
        size.height = CHESS_HEIGHT * 1.05;
        if(!canplays.empty())UpdateSelectWireframeUniform(device, canplays.data(), canplays.size());
        UpdateChessUniform(device, pChessInfo->country, pChessInfo->chess, pChessInfo->row, pChessInfo->column, pChessInfo->fontIndex, size);
    }
}
void Chessboard::UnSelect(VkDevice device, uint32_t country, uint32_t chess){
    UpdateChess(device);
    ClearCanPlay(device);
}
const ChessInfo *Chessboard::GetChessInfos(uint32_t country, const glm::vec2 &mousePos){
    const ChessInfo *pInfor = nullptr;
    for (size_t uiChess = 0; uiChess < COUNTRY_CHESS_COUNT; ++uiChess){
        if(mChess[country][uiChess] && mChess[country][uiChess]->IsSelect(mousePos)){
            pInfor = mChess[country][uiChess]->GetInfo();
            break;
        }
    }
    return pInfor;
}
const ChessInfo *Chessboard::GetChessInfos(uint32_t country, uint32_t row, uint32_t column){
    const ChessInfo *pInfor = nullptr;
    for (size_t uiChess = 0; uiChess < COUNTRY_CHESS_COUNT; ++uiChess){
        if(mChess[country][uiChess]){
            pInfor = mChess[country][uiChess]->GetInfo();
            if(pInfor->row == row && pInfor->column == column){
                break;
            }
            else{
                pInfor = nullptr;
            }
        }
    }
    return pInfor;
}
void Chessboard::Move(VkDevice device, uint32_t country, uint32_t chess, uint32_t row, uint32_t column){
    mChess[country][chess]->ResetPos(row, column);
}
void Chessboard::CaptureChess(uint32_t srcCountry, uint32_t srcChess, uint32_t dstCountry, uint32_t dstChess){
    if(mChess[dstCountry][dstChess]){
        const ChessInfo *pInfo = mChess[dstCountry][dstChess]->GetInfo();
        if(pInfo->chess == JIANG_CHESS_INDEX){
            GetCountryChess(srcCountry, dstCountry);
        }
        delete mChess[dstCountry][dstChess];
        mChess[dstCountry][dstChess] = nullptr;
    }
}
void Chessboard::Play(VkDevice device, uint32_t srcCountry, uint32_t srcChess, uint32_t dstCountry, uint32_t dstChess){
    const char county[][MAX_BYTE] = { "魏", "蜀", "吴", "汉" };
    //这里用的字体顺序,修改需要注意
    const char chess[][MAX_BYTE] = { "魏", "蜀", "吴", "漢", "兵", "炮", "車", "馬", "相", "士" };
    Chess *pChess = mChess[srcCountry][srcChess], *pRival = mChess[dstCountry][dstChess];
    const ChessInfo *pChessInfo = pChess->GetInfo(), *pRivalInfo = pRival->GetInfo();
    printf("%s国的%s吃掉了%s国的%s\n", county[pChessInfo->country], chess[pChessInfo->fontIndex], county[pRivalInfo->country], chess[pRivalInfo->fontIndex]);
    pChess->ResetPos(pRivalInfo->row, pRivalInfo->column);
    CaptureChess(srcCountry, srcChess, dstCountry, dstChess);
    //也可以考虑类外更新, 不过目前先这样
    UpdateChess(device);
    ClearCanPlay(device);
}