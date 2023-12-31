#include "Chessboard.h"
void Chessboard::InitWuChessRowAndColumn(){
    mChessInfo[WU_COUNTRY_INDEX][JIANG_CHESS_INDEX].row = 8;
    mChessInfo[WU_COUNTRY_INDEX][JIANG_CHESS_INDEX].column = 16;

    mChessInfo[WU_COUNTRY_INDEX][MA_CHESS_INDEX_1].row = mChessInfo[WU_COUNTRY_INDEX][JIANG_CHESS_INDEX].row - 3;
    mChessInfo[WU_COUNTRY_INDEX][MA_CHESS_INDEX_1].column = mChessInfo[WU_COUNTRY_INDEX][JIANG_CHESS_INDEX].column;

    mChessInfo[WU_COUNTRY_INDEX][MA_CHESS_INDEX_2].row = mChessInfo[WU_COUNTRY_INDEX][JIANG_CHESS_INDEX].row + 3;
    mChessInfo[WU_COUNTRY_INDEX][MA_CHESS_INDEX_2].column = mChessInfo[WU_COUNTRY_INDEX][JIANG_CHESS_INDEX].column;

    mChessInfo[WU_COUNTRY_INDEX][SHI_CHESS_INDEX_1].row = mChessInfo[WU_COUNTRY_INDEX][JIANG_CHESS_INDEX].row - 1;
    mChessInfo[WU_COUNTRY_INDEX][SHI_CHESS_INDEX_1].column = mChessInfo[WU_COUNTRY_INDEX][JIANG_CHESS_INDEX].column;
    
    mChessInfo[WU_COUNTRY_INDEX][SHI_CHESS_INDEX_2].row = mChessInfo[WU_COUNTRY_INDEX][JIANG_CHESS_INDEX].row + 1;
    mChessInfo[WU_COUNTRY_INDEX][SHI_CHESS_INDEX_2].column = mChessInfo[WU_COUNTRY_INDEX][JIANG_CHESS_INDEX].column;
    
    mChessInfo[WU_COUNTRY_INDEX][CHE_CHESS_INDEX_1].row = 4;
    mChessInfo[WU_COUNTRY_INDEX][CHE_CHESS_INDEX_1].column = mChessInfo[WU_COUNTRY_INDEX][JIANG_CHESS_INDEX].column;
    
    mChessInfo[WU_COUNTRY_INDEX][CHE_CHESS_INDEX_2].row = mChessInfo[WU_COUNTRY_INDEX][JIANG_CHESS_INDEX].row + 4;
    mChessInfo[WU_COUNTRY_INDEX][CHE_CHESS_INDEX_2].column = mChessInfo[WU_COUNTRY_INDEX][JIANG_CHESS_INDEX].column;
    
    mChessInfo[WU_COUNTRY_INDEX][PAO_CHESS_INDEX_1].row = 5;
    mChessInfo[WU_COUNTRY_INDEX][PAO_CHESS_INDEX_1].column = 14;
    
    mChessInfo[WU_COUNTRY_INDEX][PAO_CHESS_INDEX_2].row = mChessInfo[WU_COUNTRY_INDEX][PAO_CHESS_INDEX_1].row + 6;
    mChessInfo[WU_COUNTRY_INDEX][PAO_CHESS_INDEX_2].column = mChessInfo[WU_COUNTRY_INDEX][PAO_CHESS_INDEX_1].column;
    
    mChessInfo[WU_COUNTRY_INDEX][BING_CHESS_INDEX_1].row = 4;
    mChessInfo[WU_COUNTRY_INDEX][BING_CHESS_INDEX_1].column = 13;
    for (size_t i = 1; i < BING_CHESS_COUNT; ++i){
        mChessInfo[WU_COUNTRY_INDEX][i + BING_CHESS_INDEX_1].row = mChessInfo[WU_COUNTRY_INDEX][i + BING_CHESS_INDEX_1 - 1].row + 2;;
        mChessInfo[WU_COUNTRY_INDEX][i + BING_CHESS_INDEX_1].column = mChessInfo[WU_COUNTRY_INDEX][BING_CHESS_INDEX_1].column;
    }

    mChessInfo[WU_COUNTRY_INDEX][XIANG_CHESS_INDEX_1].row = mChessInfo[WU_COUNTRY_INDEX][JIANG_CHESS_INDEX].row - 2;
    mChessInfo[WU_COUNTRY_INDEX][XIANG_CHESS_INDEX_1].column = mChessInfo[WU_COUNTRY_INDEX][JIANG_CHESS_INDEX].column;

    mChessInfo[WU_COUNTRY_INDEX][XIANG_CHESS_INDEX_2].row = mChessInfo[WU_COUNTRY_INDEX][JIANG_CHESS_INDEX].row + 2;
    mChessInfo[WU_COUNTRY_INDEX][XIANG_CHESS_INDEX_2].column = mChessInfo[WU_COUNTRY_INDEX][JIANG_CHESS_INDEX].column;
}
void Chessboard::InitShuChessRowAndColumn(){
    mChessInfo[SHU_COUNTRY_INDEX][JIANG_CHESS_INDEX].row = 16;
    mChessInfo[SHU_COUNTRY_INDEX][JIANG_CHESS_INDEX].column = 8;

    mChessInfo[SHU_COUNTRY_INDEX][MA_CHESS_INDEX_1].row = mChessInfo[SHU_COUNTRY_INDEX][JIANG_CHESS_INDEX].row;
    mChessInfo[SHU_COUNTRY_INDEX][MA_CHESS_INDEX_1].column = 5;

    mChessInfo[SHU_COUNTRY_INDEX][MA_CHESS_INDEX_2].row = mChessInfo[SHU_COUNTRY_INDEX][JIANG_CHESS_INDEX].row;
    mChessInfo[SHU_COUNTRY_INDEX][MA_CHESS_INDEX_2].column = mChessInfo[SHU_COUNTRY_INDEX][JIANG_CHESS_INDEX].column + 3;

    mChessInfo[SHU_COUNTRY_INDEX][SHI_CHESS_INDEX_1].row = mChessInfo[SHU_COUNTRY_INDEX][JIANG_CHESS_INDEX].row;
    mChessInfo[SHU_COUNTRY_INDEX][SHI_CHESS_INDEX_1].column = mChessInfo[SHU_COUNTRY_INDEX][MA_CHESS_INDEX_1].column + 2;
    
    mChessInfo[SHU_COUNTRY_INDEX][SHI_CHESS_INDEX_2].row = mChessInfo[SHU_COUNTRY_INDEX][JIANG_CHESS_INDEX].row;
    mChessInfo[SHU_COUNTRY_INDEX][SHI_CHESS_INDEX_2].column = mChessInfo[SHU_COUNTRY_INDEX][JIANG_CHESS_INDEX].column + 1;
    
    mChessInfo[SHU_COUNTRY_INDEX][CHE_CHESS_INDEX_1].row = mChessInfo[SHU_COUNTRY_INDEX][JIANG_CHESS_INDEX].row;
    mChessInfo[SHU_COUNTRY_INDEX][CHE_CHESS_INDEX_1].column = 4;
    
    mChessInfo[SHU_COUNTRY_INDEX][CHE_CHESS_INDEX_2].row = mChessInfo[SHU_COUNTRY_INDEX][JIANG_CHESS_INDEX].row;
    mChessInfo[SHU_COUNTRY_INDEX][CHE_CHESS_INDEX_2].column = mChessInfo[SHU_COUNTRY_INDEX][JIANG_CHESS_INDEX].column + 4;
    
    mChessInfo[SHU_COUNTRY_INDEX][PAO_CHESS_INDEX_1].row = 14;
    mChessInfo[SHU_COUNTRY_INDEX][PAO_CHESS_INDEX_1].column = mChessInfo[SHU_COUNTRY_INDEX][MA_CHESS_INDEX_1].column;
    
    mChessInfo[SHU_COUNTRY_INDEX][PAO_CHESS_INDEX_2].row = mChessInfo[SHU_COUNTRY_INDEX][PAO_CHESS_INDEX_1].row;
    mChessInfo[SHU_COUNTRY_INDEX][PAO_CHESS_INDEX_2].column = mChessInfo[SHU_COUNTRY_INDEX][MA_CHESS_INDEX_2].column;

    mChessInfo[SHU_COUNTRY_INDEX][BING_CHESS_INDEX_1].row = 13;
    mChessInfo[SHU_COUNTRY_INDEX][BING_CHESS_INDEX_1].column = 4;
    for (size_t i = 1; i < BING_CHESS_COUNT; ++i){
        mChessInfo[SHU_COUNTRY_INDEX][i + BING_CHESS_INDEX_1].row = mChessInfo[SHU_COUNTRY_INDEX][BING_CHESS_INDEX_1].row;
        mChessInfo[SHU_COUNTRY_INDEX][i + BING_CHESS_INDEX_1].column = mChessInfo[SHU_COUNTRY_INDEX][i + BING_CHESS_INDEX_1 - 1].column + 2;
    }

    mChessInfo[SHU_COUNTRY_INDEX][XIANG_CHESS_INDEX_1].row = mChessInfo[SHU_COUNTRY_INDEX][JIANG_CHESS_INDEX].row;
    mChessInfo[SHU_COUNTRY_INDEX][XIANG_CHESS_INDEX_1].column = mChessInfo[SHU_COUNTRY_INDEX][MA_CHESS_INDEX_1].column + 1;

    mChessInfo[SHU_COUNTRY_INDEX][XIANG_CHESS_INDEX_2].row = mChessInfo[SHU_COUNTRY_INDEX][JIANG_CHESS_INDEX].row;
    mChessInfo[SHU_COUNTRY_INDEX][XIANG_CHESS_INDEX_2].column = mChessInfo[SHU_COUNTRY_INDEX][JIANG_CHESS_INDEX].column + 2;
}
void Chessboard::InitWeiChessRowAndColumn(){
    mChessInfo[WEI_COUNTRY_INDEX][JIANG_CHESS_INDEX].row = 0;
    mChessInfo[WEI_COUNTRY_INDEX][JIANG_CHESS_INDEX].column = 8;

    mChessInfo[WEI_COUNTRY_INDEX][MA_CHESS_INDEX_1].row = mChessInfo[WEI_COUNTRY_INDEX][JIANG_CHESS_INDEX].row;
    mChessInfo[WEI_COUNTRY_INDEX][MA_CHESS_INDEX_1].column = 5;

    mChessInfo[WEI_COUNTRY_INDEX][MA_CHESS_INDEX_2].row = mChessInfo[WEI_COUNTRY_INDEX][JIANG_CHESS_INDEX].row;
    mChessInfo[WEI_COUNTRY_INDEX][MA_CHESS_INDEX_2].column = mChessInfo[WEI_COUNTRY_INDEX][JIANG_CHESS_INDEX].column + 3;

    mChessInfo[WEI_COUNTRY_INDEX][SHI_CHESS_INDEX_1].row = mChessInfo[WEI_COUNTRY_INDEX][JIANG_CHESS_INDEX].row;
    mChessInfo[WEI_COUNTRY_INDEX][SHI_CHESS_INDEX_1].column = mChessInfo[WEI_COUNTRY_INDEX][JIANG_CHESS_INDEX].column - 1;
    
    mChessInfo[WEI_COUNTRY_INDEX][SHI_CHESS_INDEX_2].row = mChessInfo[WEI_COUNTRY_INDEX][JIANG_CHESS_INDEX].row;
    mChessInfo[WEI_COUNTRY_INDEX][SHI_CHESS_INDEX_2].column = mChessInfo[WEI_COUNTRY_INDEX][JIANG_CHESS_INDEX].column + 1;
    
    mChessInfo[WEI_COUNTRY_INDEX][CHE_CHESS_INDEX_1].row = mChessInfo[WEI_COUNTRY_INDEX][JIANG_CHESS_INDEX].row;
    mChessInfo[WEI_COUNTRY_INDEX][CHE_CHESS_INDEX_1].column = 4;
    
    mChessInfo[WEI_COUNTRY_INDEX][CHE_CHESS_INDEX_2].row = mChessInfo[WEI_COUNTRY_INDEX][JIANG_CHESS_INDEX].row;
    mChessInfo[WEI_COUNTRY_INDEX][CHE_CHESS_INDEX_2].column = mChessInfo[WEI_COUNTRY_INDEX][JIANG_CHESS_INDEX].column + 4;
    
    mChessInfo[WEI_COUNTRY_INDEX][PAO_CHESS_INDEX_1].row = 2;
    mChessInfo[WEI_COUNTRY_INDEX][PAO_CHESS_INDEX_1].column = mChessInfo[WEI_COUNTRY_INDEX][MA_CHESS_INDEX_1].column;
    
    mChessInfo[WEI_COUNTRY_INDEX][PAO_CHESS_INDEX_2].row = mChessInfo[WEI_COUNTRY_INDEX][PAO_CHESS_INDEX_1].row;
    mChessInfo[WEI_COUNTRY_INDEX][PAO_CHESS_INDEX_2].column = mChessInfo[WEI_COUNTRY_INDEX][MA_CHESS_INDEX_2].column;
    
    mChessInfo[WEI_COUNTRY_INDEX][BING_CHESS_INDEX_1].row = 3;
    mChessInfo[WEI_COUNTRY_INDEX][BING_CHESS_INDEX_1].column = 4;
    for (size_t i = 1; i < BING_CHESS_COUNT; ++i){
        mChessInfo[WEI_COUNTRY_INDEX][i + BING_CHESS_INDEX_1].row = mChessInfo[WEI_COUNTRY_INDEX][BING_CHESS_INDEX_1].row;
        mChessInfo[WEI_COUNTRY_INDEX][i + BING_CHESS_INDEX_1].column = mChessInfo[WEI_COUNTRY_INDEX][i + BING_CHESS_INDEX_1 - 1].column + 2;
    }

    mChessInfo[WEI_COUNTRY_INDEX][XIANG_CHESS_INDEX_1].row = mChessInfo[WEI_COUNTRY_INDEX][JIANG_CHESS_INDEX].row;
    mChessInfo[WEI_COUNTRY_INDEX][XIANG_CHESS_INDEX_1].column = mChessInfo[WEI_COUNTRY_INDEX][MA_CHESS_INDEX_1].column + 1;

    mChessInfo[WEI_COUNTRY_INDEX][XIANG_CHESS_INDEX_2].row = mChessInfo[WEI_COUNTRY_INDEX][JIANG_CHESS_INDEX].row;
    mChessInfo[WEI_COUNTRY_INDEX][XIANG_CHESS_INDEX_2].column = mChessInfo[WEI_COUNTRY_INDEX][JIANG_CHESS_INDEX].column + 2;
}
void Chessboard::InitChessInfo(uint32_t country){
    for (size_t uiChess = 0; uiChess < COUNTRY_CHESS_COUNT; ++uiChess){
        mChessInfo[country][uiChess].row = CHESSBOARD_ROW + 100;
        mChessInfo[country][uiChess].column = CHESSBOARD_COLUMN + 100;
    }
    uint32_t fontIndex[] = { FONT_INDEX_WEI, FONT_INDEX_SHU, FONT_INDEX_WU, FONT_INDEX_HAN };
    mChessInfo[country][JIANG_CHESS_INDEX].country = country;
    mChessInfo[country][JIANG_CHESS_INDEX].chess = JIANG_CHESS_INDEX;
    mChessInfo[country][JIANG_CHESS_INDEX].fontIndex = fontIndex[country];
    if(country != HAN_COUNTRY_INDEX){
        mChessInfo[country][MA_CHESS_INDEX_1].chess = MA_CHESS_INDEX_1;
        mChessInfo[country][MA_CHESS_INDEX_2].chess = MA_CHESS_INDEX_2;
        mChessInfo[country][SHI_CHESS_INDEX_1].chess = SHI_CHESS_INDEX_1;
        mChessInfo[country][SHI_CHESS_INDEX_2].chess = SHI_CHESS_INDEX_2;
        mChessInfo[country][CHE_CHESS_INDEX_1].chess = CHE_CHESS_INDEX_1;
        mChessInfo[country][CHE_CHESS_INDEX_2].chess = CHE_CHESS_INDEX_2;
        mChessInfo[country][PAO_CHESS_INDEX_1].chess = PAO_CHESS_INDEX_1;
        mChessInfo[country][PAO_CHESS_INDEX_2].chess = PAO_CHESS_INDEX_2;
        mChessInfo[country][BING_CHESS_INDEX_1].chess = BING_CHESS_INDEX_1;
        mChessInfo[country][BING_CHESS_INDEX_2].chess = BING_CHESS_INDEX_2;
        mChessInfo[country][BING_CHESS_INDEX_3].chess = BING_CHESS_INDEX_3;
        mChessInfo[country][BING_CHESS_INDEX_4].chess = BING_CHESS_INDEX_4;
        mChessInfo[country][BING_CHESS_INDEX_5].chess = BING_CHESS_INDEX_5;
        mChessInfo[country][XIANG_CHESS_INDEX_1].chess = XIANG_CHESS_INDEX_1;
        mChessInfo[country][XIANG_CHESS_INDEX_2].chess = XIANG_CHESS_INDEX_2;
        mChessInfo[country][MA_CHESS_INDEX_1].country = country;
        mChessInfo[country][MA_CHESS_INDEX_2].country = country;
        mChessInfo[country][SHI_CHESS_INDEX_1].country = country;
        mChessInfo[country][SHI_CHESS_INDEX_2].country = country;
        mChessInfo[country][CHE_CHESS_INDEX_1].country = country;
        mChessInfo[country][CHE_CHESS_INDEX_2].country = country;
        mChessInfo[country][PAO_CHESS_INDEX_1].country = country;
        mChessInfo[country][PAO_CHESS_INDEX_2].country = country;
        mChessInfo[country][BING_CHESS_INDEX_1].country = country;
        mChessInfo[country][BING_CHESS_INDEX_2].country = country;
        mChessInfo[country][BING_CHESS_INDEX_3].country = country;
        mChessInfo[country][BING_CHESS_INDEX_4].country = country;
        mChessInfo[country][BING_CHESS_INDEX_5].country = country;
        mChessInfo[country][XIANG_CHESS_INDEX_1].country = country;
        mChessInfo[country][XIANG_CHESS_INDEX_2].country = country;

        mChessInfo[country][MA_CHESS_INDEX_1].fontIndex = FONT_INDEX_MA;
        mChessInfo[country][MA_CHESS_INDEX_2].fontIndex = FONT_INDEX_MA;
        mChessInfo[country][SHI_CHESS_INDEX_1].fontIndex = FONT_INDEX_SHI;
        mChessInfo[country][SHI_CHESS_INDEX_2].fontIndex = FONT_INDEX_SHI;
        mChessInfo[country][CHE_CHESS_INDEX_1].fontIndex = FONT_INDEX_CHE;
        mChessInfo[country][CHE_CHESS_INDEX_2].fontIndex = FONT_INDEX_CHE;
        mChessInfo[country][PAO_CHESS_INDEX_1].fontIndex = FONT_INDEX_PAO;
        mChessInfo[country][PAO_CHESS_INDEX_2].fontIndex = FONT_INDEX_PAO;
        mChessInfo[country][BING_CHESS_INDEX_1].fontIndex = FONT_INDEX_BING;
        mChessInfo[country][BING_CHESS_INDEX_2].fontIndex = FONT_INDEX_BING;
        mChessInfo[country][BING_CHESS_INDEX_3].fontIndex = FONT_INDEX_BING;
        mChessInfo[country][BING_CHESS_INDEX_4].fontIndex = FONT_INDEX_BING;
        mChessInfo[country][BING_CHESS_INDEX_5].fontIndex = FONT_INDEX_BING;
        mChessInfo[country][XIANG_CHESS_INDEX_1].fontIndex = FONT_INDEX_XIANG;
        mChessInfo[country][XIANG_CHESS_INDEX_2].fontIndex = FONT_INDEX_XIANG;
        if(country == WU_COUNTRY_INDEX){
            InitWuChessRowAndColumn();
        }
        else if(country == WEI_COUNTRY_INDEX){
            InitWeiChessRowAndColumn();
        }
        else{
            InitShuChessRowAndColumn();
        }
    }
    else{
        mChessInfo[country][CHE_CHESS_INDEX_3].chess = CHE_CHESS_INDEX_3;
        mChessInfo[country][CHE_CHESS_INDEX_4].chess = CHE_CHESS_INDEX_4;
        mChessInfo[country][CHE_CHESS_INDEX_5].chess = CHE_CHESS_INDEX_5;
        mChessInfo[country][PAO_CHESS_INDEX_3].chess = PAO_CHESS_INDEX_3;

        mChessInfo[country][CHE_CHESS_INDEX_3].country = HAN_COUNTRY_INDEX;
        mChessInfo[country][CHE_CHESS_INDEX_4].country = HAN_COUNTRY_INDEX;
        mChessInfo[country][CHE_CHESS_INDEX_5].country = HAN_COUNTRY_INDEX;
        mChessInfo[country][PAO_CHESS_INDEX_3].country = HAN_COUNTRY_INDEX;

        mChessInfo[country][CHE_CHESS_INDEX_3].fontIndex = FONT_INDEX_CHE;
        mChessInfo[country][CHE_CHESS_INDEX_4].fontIndex = FONT_INDEX_CHE;
        mChessInfo[country][CHE_CHESS_INDEX_5].fontIndex = FONT_INDEX_CHE;
        mChessInfo[country][PAO_CHESS_INDEX_3].fontIndex = FONT_INDEX_PAO;

        mChessInfo[country][JIANG_CHESS_INDEX].row = 8;
        mChessInfo[country][JIANG_CHESS_INDEX].column = 0;

        mChessInfo[country][CHE_CHESS_INDEX_3].row = 6;
        mChessInfo[country][CHE_CHESS_INDEX_3].column = 3;

        mChessInfo[country][CHE_CHESS_INDEX_4].row = mChessInfo[country][JIANG_CHESS_INDEX].row;
        mChessInfo[country][CHE_CHESS_INDEX_4].column = mChessInfo[country][CHE_CHESS_INDEX_3].column;

        mChessInfo[country][CHE_CHESS_INDEX_5].row = mChessInfo[country][CHE_CHESS_INDEX_4].row + 2;
        mChessInfo[country][CHE_CHESS_INDEX_5].column = mChessInfo[country][CHE_CHESS_INDEX_3].column;

        mChessInfo[country][PAO_CHESS_INDEX_3].row = mChessInfo[country][JIANG_CHESS_INDEX].row;
        mChessInfo[country][PAO_CHESS_INDEX_3].column = mChessInfo[country][CHE_CHESS_INDEX_3].column - 1;
    }
}
void Chessboard::GetCountryChess(uint32_t srcCountry, uint32_t dstCountry){
    //想获得并使用其他国家的棋子就必须多显示几个棋子
    if(dstCountry == HAN_COUNTRY_INDEX){
        for (uint32_t uiChess = 1; uiChess < HAN_CHESS_COUNT; ++uiChess){
        // for (uint32_t uiChess = 1; uiChess < WEI_CHESS_COUNT; ++uiChess){
            if(mChess[dstCountry][uiChess] && mChessInfo[dstCountry][uiChess].row <= CHESSBOARD_ROW){
                mChessInfo[dstCountry][uiChess].country = srcCountry;
                mChessInfo[dstCountry][uiChess].chess = WEI_CHESS_COUNT + uiChess - 1;
                mChessInfo[srcCountry][mChessInfo[dstCountry][uiChess].chess] = mChessInfo[dstCountry][uiChess];
                if(uiChess == PAO_CHESS_INDEX_3){
                    mChess[srcCountry][mChessInfo[dstCountry][uiChess].chess] = new Pao(mChessInfo[dstCountry][uiChess]);
                }
                else if(uiChess == CHE_CHESS_INDEX_3 || uiChess == CHE_CHESS_INDEX_4 || uiChess == CHE_CHESS_INDEX_5){
                    mChess[srcCountry][mChessInfo[dstCountry][uiChess].chess] = new Che(mChessInfo[dstCountry][uiChess]);
                }
                //下面是获得其他势力的牌
                // if(uiChess == MA_CHESS_INDEX_1 || uiChess == MA_CHESS_INDEX_2){
                //     mChess[srcCountry][mChessInfo[dstCountry][uiChess].chess] = new Ma(mChessInfo[dstCountry][uiChess]);
                // }
                // else if(uiChess == SHI_CHESS_INDEX_1 || uiChess == SHI_CHESS_INDEX_2){
                //     mChess[srcCountry][mChessInfo[dstCountry][uiChess].chess] = new Shi(mChessInfo[dstCountry][uiChess]);
                // }
                // else if(uiChess == XIANG_CHESS_INDEX_1 || uiChess == XIANG_CHESS_INDEX_2){
                //     mChess[srcCountry][mChessInfo[dstCountry][uiChess].chess] = new Xiang(mChessInfo[dstCountry][uiChess]);
                // }
                // else if(uiChess == PAO_CHESS_INDEX_1 || uiChess == PAO_CHESS_INDEX_2 || uiChess == PAO_CHESS_INDEX_3){
                //     mChess[srcCountry][mChessInfo[dstCountry][uiChess].chess] = new Pao(mChessInfo[dstCountry][uiChess]);
                // }
                // else if(uiChess == CHE_CHESS_INDEX_1 || uiChess == CHE_CHESS_INDEX_2 || uiChess == CHE_CHESS_INDEX_3 || uiChess == CHE_CHESS_INDEX_4 || uiChess == CHE_CHESS_INDEX_5){
                //     mChess[srcCountry][mChessInfo[dstCountry][uiChess].chess] = new Che(mChessInfo[dstCountry][uiChess]);
                // }
                // else if(uiChess == BING_CHESS_INDEX_1 || uiChess == BING_CHESS_INDEX_2 || uiChess == BING_CHESS_INDEX_3 || uiChess == BING_CHESS_INDEX_4 || uiChess == BING_CHESS_INDEX_5){
                //     mChess[srcCountry][mChessInfo[dstCountry][uiChess].chess] = new Bing(mChessInfo[dstCountry][uiChess]);
                // }
                delete mChess[dstCountry][uiChess];
                mChess[dstCountry][uiChess] = nullptr;
                mChessInfo[dstCountry][uiChess].row = CHESSBOARD_ROW + 100;
                mChessInfo[dstCountry][uiChess].column = CHESSBOARD_COLUMN + 100;
            }
        }
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
void Chessboard::Selected(VkDevice device){
    if(mSelected && !IsBoundary(mSelected->row, mSelected->column)){
        mCanplays.clear();
        if(device != VK_NULL_HANDLE)ClearSelectWireframeUnfirom(device);
        mChess[mSelected->country][mSelected->chess]->Selected((ChessInfo *)mChessInfo, 4 * COUNTRY_CHESS_COUNT, mCanplays);
    }
    if(device != VK_NULL_HANDLE){
        VkExtent2D size;
        size.width = CHESS_WIDTH * 1.05;
        size.height = CHESS_HEIGHT * 1.05;
        if(!mCanplays.empty())UpdateSelectWireframeUniform(device, mCanplays.data(), mCanplays.size());
        UpdateChessUniform(device, mSelected->country, mSelected->chess, mSelected->row, mSelected->column, mSelected->fontIndex, size);
    }
}
void Chessboard::UnSelect(VkDevice device){
    UpdateChess(device);
    ClearCanPlay(device);
}
void Chessboard::InitChess(VkDevice device){
    for (size_t i = 0; i < 4; i++){
        InitChessInfo(i);        
    }
    for (uint32_t uiCountry = 0; uiCountry < 4; ++uiCountry){
        for (uint32_t uiChess = 0; uiChess < COUNTRY_CHESS_COUNT; ++uiChess){
            if(mChess[uiCountry][uiChess]){
                delete mChess[uiCountry][uiChess];
                mChess[uiCountry][uiChess] = nullptr;
            }
        }   
    }
    mChess[WU_COUNTRY_INDEX][JIANG_CHESS_INDEX] = new Wu(mChessInfo[WU_COUNTRY_INDEX][JIANG_CHESS_INDEX]);
    mChess[SHU_COUNTRY_INDEX][JIANG_CHESS_INDEX] = new Shu(mChessInfo[SHU_COUNTRY_INDEX][JIANG_CHESS_INDEX]);
    mChess[WEI_COUNTRY_INDEX][JIANG_CHESS_INDEX] = new Wei(mChessInfo[WEI_COUNTRY_INDEX][JIANG_CHESS_INDEX]);
    mChess[HAN_COUNTRY_INDEX][JIANG_CHESS_INDEX] = new Han(mChessInfo[HAN_COUNTRY_INDEX][JIANG_CHESS_INDEX]);
    for (uint32_t uiCountry = 0; uiCountry < 4; ++uiCountry){
        if(uiCountry != HAN_COUNTRY_INDEX){
            for (uint32_t uiChess = 0; uiChess < COUNTRY_CHESS_COUNT; ++uiChess){
                if(uiChess == MA_CHESS_INDEX_1 || uiChess == MA_CHESS_INDEX_2){
                    mChess[uiCountry][uiChess] = new Ma(mChessInfo[uiCountry][uiChess]);
                }
                else if(uiChess == PAO_CHESS_INDEX_1 || uiChess == PAO_CHESS_INDEX_2){
                    mChess[uiCountry][uiChess] = new Pao(mChessInfo[uiCountry][uiChess]);
                }
                else if(uiChess == CHE_CHESS_INDEX_1 || uiChess == CHE_CHESS_INDEX_2){
                    mChess[uiCountry][uiChess] = new Che(mChessInfo[uiCountry][uiChess]);
                }
                else if(uiChess == SHI_CHESS_INDEX_1 || uiChess == SHI_CHESS_INDEX_2){
                    mChess[uiCountry][uiChess] = new Shi(mChessInfo[uiCountry][uiChess]);
                }
                else if(uiChess == XIANG_CHESS_INDEX_1 || uiChess == XIANG_CHESS_INDEX_2){
                    mChess[uiCountry][uiChess] = new Xiang(mChessInfo[uiCountry][uiChess]);
                }
                else if(uiChess == BING_CHESS_INDEX_1 || uiChess == BING_CHESS_INDEX_2 || uiChess == BING_CHESS_INDEX_3 || uiChess == BING_CHESS_INDEX_4 || uiChess == BING_CHESS_INDEX_5){
                    mChess[uiCountry][uiChess] = new Bing(mChessInfo[uiCountry][uiChess]);
                }   
            }
        }
        else{
                mChess[uiCountry][PAO_CHESS_INDEX_3] = new Pao(mChessInfo[uiCountry][PAO_CHESS_INDEX_3]);

                mChess[uiCountry][CHE_CHESS_INDEX_3] = new Che(mChessInfo[uiCountry][CHE_CHESS_INDEX_3]);
                mChess[uiCountry][CHE_CHESS_INDEX_4] = new Che(mChessInfo[uiCountry][CHE_CHESS_INDEX_4]);
                mChess[uiCountry][CHE_CHESS_INDEX_5] = new Che(mChessInfo[uiCountry][CHE_CHESS_INDEX_5]);
        }
    }
    UpdateChess(device);
}
void Chessboard::UpdateChess(VkDevice device){
    VkExtent2D chessSize;
    chessSize.width = CHESS_WIDTH;
    chessSize.height = CHESS_HEIGHT;
    //-1是因为要单独处理'汉'
    for (uint32_t uiCoutry = 0; uiCoutry < 4; ++uiCoutry){
        for(uint32_t uiChess = 0; uiChess < COUNTRY_CHESS_COUNT; ++uiChess){
            const ChessInfo&chessInfo = mChessInfo[uiCoutry][uiChess];
            // printf("第%d个国家, 第%d颗棋子, 行:%d, 列:%d\n", uiCoutry, uiChess, chessInfo.row, chessInfo.column);
            UpdateChessUniform(device, uiCoutry, uiChess, chessInfo.row, chessInfo.column, chessInfo.fontIndex, chessSize);
        }
    }
}
void Chessboard::ClearCanPlay(VkDevice device){
    mCanplays.clear();
    mSelected = nullptr;
    if(device != VK_NULL_HANDLE)ClearSelectWireframeUnfirom(device);
}
int32_t Chessboard::IsCanPlay(const glm::vec2 &mousePos){
    int32_t index = -1;
    for (size_t i = 0; i < mCanplays.size(); ++i){
        const uint32_t chessY = ROW_TO_Y(mCanplays[i].row) - CHESSBOARD_GRID_SIZE * .5;
        const uint32_t chessX = COLUMN_TO_X(mCanplays[i].column) - CHESSBOARD_GRID_SIZE * .5;
        if(mousePos.x > chessX && mousePos.y > chessY && mousePos.x < chessX + CHESSBOARD_GRID_SIZE && mousePos.y < chessY + CHESSBOARD_GRID_SIZE){
            index = i;
            break;
        }
    }
    return index;
}
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
const ChessInfo *Chessboard::GetSelectInfo(const glm::vec2 &pos){
    const ChessInfo *pInfo = nullptr;
    for (size_t uiCountry = 0; uiCountry < 4; ++uiCountry){
        for (size_t uiChess = 0; uiChess < COUNTRY_CHESS_COUNT; ++uiChess){
            if(mChess[uiCountry][uiChess] && mChess[uiCountry][uiChess]->IsSelect(pos)){
                pInfo = &mChessInfo[uiCountry][uiChess];
                break;
            }
        }
        if(pInfo){
            break;
        }
    }
    return pInfo;
}
void Chessboard::DestroyCountry(VkDevice device, uint32_t country){
    for (size_t uiChess = 0; uiChess < COUNTRY_CHESS_COUNT; ++uiChess){
        mChessInfo[country][uiChess].row = CHESSBOARD_ROW + 100;
        mChessInfo[country][uiChess].column = CHESSBOARD_COLUMN + 100;
    }
    if(country != HAN_COUNTRY_INDEX){
        for (size_t uiChess = 0; uiChess < COUNTRY_CHESS_COUNT; ++uiChess){
            if(mChess[country][uiChess]){
                delete mChess[country][uiChess];
                mChess[country][uiChess] = nullptr;
            }
        }
    }
    UpdateChess(device);
}
const ChessInfo *Chessboard::GetChessInfos(uint32_t row, uint32_t column){
    const ChessInfo *pInfor = nullptr;
    for (size_t uiCountry = 0; uiCountry < 4; ++uiCountry){
        if(pInfor = GetChessInfos(uiCountry, row, column)){
            break;
        }
    }
    return pInfor;
}
const ChessInfo *Chessboard::GetChessInfos(uint32_t country, uint32_t row, uint32_t column){
    ChessInfo *pInfor = nullptr;
    for (size_t uiChess = 0; uiChess < COUNTRY_CHESS_COUNT; ++uiChess){
        if(mChessInfo[country][uiChess].row == row && mChessInfo[country][uiChess].column == column){
            pInfor = &mChessInfo[country][uiChess];
            break;
        }
    }
    return pInfor;
}
bool Chessboard::Play(VkDevice device, const glm::vec2&mousePos, const ChessInfo *pRival){
    int32_t index = IsCanPlay(mousePos);
    if(index != -1){
        const char county[][MAX_BYTE] = { "魏", "蜀", "吴", "汉" };
        //这里用的字体顺序,修改需要注意
        const char chess[][MAX_BYTE] = { "魏", "蜀", "吴", "漢", "兵", "炮", "車", "馬", "相", "士" };
        if(pRival){
            printf("%s国的%s吃掉了%s国的%s\n", county[mSelected->country], chess[mSelected->fontIndex], county[pRival->country], chess[pRival->fontIndex]);

            ChessInfo info = *pRival;
            CaptureChess(mSelected->country, mSelected->chess, pRival->country, pRival->chess);
            mSelected->row = info.row;
            mSelected->column = info.column;
        }
        else{
            mSelected->row = mCanplays[index].row;
            mSelected->column = mCanplays[index].column;                
         
            // printf("%s国的%s走到了%d行%d列\n", county[mSelected->country], chess[mSelected->fontIndex], mSelected->row, mSelected->column); 
        }
        mChess[mSelected->country][mSelected->chess]->ResetPos(mSelected->row, mSelected->column);
    }
    UpdateChess(device);
    ClearCanPlay(device);
    return index != -1;
}
void Chessboard::CaptureChess(uint32_t srcCountry, uint32_t srcChess, uint32_t dstCountry, uint32_t dstChess){
    if(mChessInfo[dstCountry][dstChess].chess == JIANG_CHESS_INDEX){
        GetCountryChess(srcCountry, dstCountry);
    }
    delete mChess[dstCountry][dstChess];
    mChess[dstCountry][dstChess] = nullptr;
    mChessInfo[dstCountry][dstChess].row = CHESSBOARD_ROW + 100;
    mChessInfo[dstCountry][dstChess].column = CHESSBOARD_COLUMN + 100;
}