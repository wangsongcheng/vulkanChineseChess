#include "Chess.h"
#include "VulkanChess.h"
#include "VulkanChessboard.h"
void Chess::SwapCenter(ChessInfo *src, ChessInfo *dst){
    ChessInfo temp = *src;
    *src = *dst;
    *dst = temp;
}
uint32_t Chess::GetTerritoryIndex(uint32_t row, uint32_t column) const{
    if(mInfo.row < 13 && mInfo.row > 3){
        if(mInfo.column > 4){
            return WU_TERRITORY_INDEX;
        }
        else{
            return HAN_TERRITORY_INDEX;
        }
    }
    else if(mInfo.column > 3 && mInfo.column < 13){
        //魏或蜀
        if(mInfo.row > 4){
            return SHU_TERRITORY_INDEX;
        }
        else{
            return WEI_TERRITORY_INDEX;
        }
    }
    return INVALID_TERRITORY_INDEX;
}
uint32_t Chess::GetPlayerCountry(uint32_t country, uint32_t territory){
    //以蜀为参考视角
    uint32_t playerCountry = INVALID_COUNTRY_INDEX;
    if(territory == SHU_TERRITORY_INDEX){
        playerCountry = country;
    }
    else if(country == territory){
        playerCountry = SHU_COUNTRY_INDEX;
    }
#ifdef HAN_CAN_PLAY
    else if((country == WU_COUNTRY_INDEX && territory == WEI_COUNTRY_INDEX) || (country == WEI_COUNTRY_INDEX && territory == HAN_TERRITORY_INDEX) || (country == SHU_COUNTRY_INDEX && territory == WU_COUNTRY_INDEX)){
        playerCountry = HAN_COUNTRY_INDEX;
    }
#endif
    else{
        if(country == WU_COUNTRY_INDEX){
            if(territory == HAN_TERRITORY_INDEX){
                playerCountry = WEI_COUNTRY_INDEX;
            }
        }
        else if(country == WEI_COUNTRY_INDEX){
            if(territory == WU_TERRITORY_INDEX){
                playerCountry = WU_COUNTRY_INDEX;
            }      
        }
        else if(country == SHU_COUNTRY_INDEX){
            if(territory == WEI_COUNTRY_INDEX){
                playerCountry == WEI_COUNTRY_INDEX;
            }
            else if(territory == HAN_TERRITORY_INDEX){
                playerCountry = WU_COUNTRY_INDEX;
            }
        }
#ifdef HAN_CAN_PLAY
        else if(country == HAN_COUNTRY_INDEX){
            if(territory == WU_COUNTRY_INDEX){
                playerCountry = WEI_COUNTRY_INDEX;
            }
            else if(territory == WEI_COUNTRY_INDEX){
                playerCountry = WU_COUNTRY_INDEX;
            }
        }
#endif
    }
    return playerCountry;
}
const ChessInfo *Chess::GetChessInfo(uint32_t row, uint32_t column, const Chess *pChess[4][COUNTRY_CHESS_COUNT]) const{
    const ChessInfo *pChessInfo = nullptr;
    for (uint32_t uiCountry = 0; uiCountry < 4; ++uiCountry){
        pChessInfo = GetChessInfo(uiCountry, row, column, pChess);
        if(pChessInfo)break;
    }
    return pChessInfo;
}
const ChessInfo *Chess::GetChessInfo(uint32_t country, uint32_t row, uint32_t column, const Chess *pChess[4][COUNTRY_CHESS_COUNT])const{
    const ChessInfo *pChessInfo = nullptr;
    for (uint32_t uiChess = 0; uiChess < COUNTRY_CHESS_COUNT; ++uiChess){
        const Chess *pC = pChess[country][uiChess];
        if(pC){
            pChessInfo = pC->GetInfo();
            if(pChessInfo->row == row && pChessInfo->column == column){
                break;
            }
            else{
                pChessInfo = nullptr;
            }
        }
    }
    // printf("-------------\n");
    return pChessInfo;    
}
// #include "Chessboard.h"
Chess::Chess(const ChessInfo&info){
    mInfo = info;
    ResetPos(info.row, info.column);
}

Chess::~Chess(){

}
void Chess::ResetPos(uint32_t row, uint32_t column){
    mInfo.row = row;
    mInfo.column = column;
    mPos.y = ROW_TO_Y(row);
    mPos.x = COLUMN_TO_X(column);
}
bool Chess::IsBoundary(uint32_t row, uint32_t column) const{
    if(row > CHESSBOARD_ROW || column > CHESSBOARD_COLUMN){
        // printf("到达边界, row:%u, column:%u\n", row, column);
        return true;
    }
    else if ((row < 4 || row > 12) && (row < 0 || column < 4 || column > 12)){
        // printf("到达边界, row:%u, column:%u\n", row, column);
        return true;
    }
    return false;
}
bool Chess::IsPalaceCenter(uint32_t centerCount, const ChessInfo *center)const{
    bool bIsPalaceCenter = false;
    for (size_t i = 0; i < centerCount; ++i){
        if(mInfo.row == center[i].row && mInfo.column == center[i].column){
            // printf("%d国,%d行%d列的棋子:%d, 在中心为%d行%d列的九宫格内\n", mInfo.country, mInfo.row, mInfo.column, mInfo.chess, center[i].row, center[i].column);
            bIsPalaceCenter = true;
            break;
        }   
    }
    return bIsPalaceCenter;
}
bool Chess::IsInPalace(uint32_t row, uint32_t column, const ChessInfo *pCenter) const{
    bool bInIsPalace = false;
    if(IsPalaceCenter(1, pCenter))return true;
    if(abs((float)row - pCenter->row) < 2 && abs((float)column - pCenter->column) < 2){
    // if(abs((float)row - center[i].row) < 2 && abs((float)column - center[i].column) < 2){
        // printf("%d国, %d行%d列的棋子:%d, 行绝对值:%.0f, 列绝对值:%.0f, 属于中心位置为%d行, %d列九宫格内\n", mInfo.country, row, column, mInfo.chess, uiRow, uiColumn, center[i].row, center[i].column);
        bInIsPalace = true;
    }
    return bInIsPalace;
}
void Chess::SelectChessInPalace(const Chess *pChess[4][COUNTRY_CHESS_COUNT], uint32_t centerCount, const ChessInfo *center, std::vector<ChessInfo>&canplays)const{
    //只需要管斜线，其他位置归棋子子类管
    //因为我们确实在自己的九宫格内，所以该条件直接通过，后面就是想去谁的九宫格都行
    const ChessInfo *pChessInfo = nullptr;
    for (size_t i = 0; i < centerCount; ++i){
        if(IsInPalace(mInfo.row, mInfo.column, &center[i])){
            //上面的if已经排除了不在九宫格内的情况
            if(center[i].row == mInfo.row && center[i].column == mInfo.column){
                //除馬、相外, 其他棋子在中心点时，能走的肯定就是斜线边的点
                const glm::vec2 pos[] = {
                    glm::vec2(center[i].column - 1, center[i].row - 1),
                    glm::vec2(center[i].column + 1, center[i].row - 1),
                    glm::vec2(center[i].column - 1, center[i].row + 1),
                    glm::vec2(center[i].column + 1, center[i].row + 1)
                };
                for (size_t i = 0; i < sizeof(pos) / sizeof(glm::vec2); ++i){
                    pChessInfo = GetChessInfo(pos[i].y, pos[i].x, pChess);
                    if(!pChessInfo || pChessInfo->country != mInfo.country){
                        canplays.push_back(ChessInfo(pos[i].y, pos[i].x));
                    }
                }
                break;
            }
            else if((mInfo.row == center[i].row - 1 || mInfo.row == center[i].row + 1) && (mInfo.column == center[i].column - 1 || mInfo.column == center[i].column + 1)){
                //在斜线边的点上时, 能走的只有中心点
                pChessInfo = GetChessInfo(center[i].row, center[i].column, pChess);
                if(!pChessInfo || pChessInfo->country != mInfo.country)canplays.push_back(ChessInfo(center[i].row, center[i].column));
                break;
            }
        }
    }
}
Wei::Wei(const ChessInfo &info) : Chess(info){
    uint32_t territory = GetTerritoryIndex(info.row, info.column);
    if(territory == WU_TERRITORY_INDEX){
        mCenter.row = info.row - 1;
        mCenter.column = info.column;
    }
    else if(territory == WEI_TERRITORY_INDEX){
        mCenter.row = info.row + 1;
        mCenter.column = info.column;
    }
    else if(territory == SHU_TERRITORY_INDEX){
        mCenter.row = info.row - 1;
        mCenter.column = info.column;
    }
    else if(territory == HAN_TERRITORY_INDEX){
        mCenter.row = info.row;
        mCenter.column = info.column + 1;
    }
}
Wei::~Wei(){

}
void Wei::Selected(const Chess *pChess[4][COUNTRY_CHESS_COUNT], std::vector<ChessInfo>&canplays)const{
    //需要知道某个地方有没有棋子，该棋子的信息
    const ChessInfo cInfo[] = { ChessInfo(mInfo.row + 1, mInfo.column), ChessInfo(mInfo.row - 1, mInfo.column), ChessInfo(mInfo.row, mInfo.column + 1), ChessInfo(mInfo.row, mInfo.column - 1) };
    for(uint32_t i = 0; i < sizeof(cInfo) / sizeof(ChessInfo); ++i){
        if(!IsBoundary(cInfo[i].row, cInfo[i].column)){
            if(IsInPalace(cInfo[i].row, cInfo[i].column, &mCenter)){//将不能出九宫格
                const ChessInfo *pChessInfo = GetChessInfo(cInfo[i].row, cInfo[i].column, pChess);
                if(!pChessInfo || pChessInfo->country != mInfo.country){
                    canplays.push_back(ChessInfo(cInfo[i].row, cInfo[i].column));
                }
            }
        }
    }
    SelectChessInPalace(pChess, 1, &mCenter, canplays);
}

Shu::Shu(const ChessInfo&info):Chess(info){
    uint32_t territory = GetTerritoryIndex(info.row, info.column);
    if(territory == WU_TERRITORY_INDEX){
        mCenter.row = info.row - 1;
        mCenter.column = info.column;
    }
    else if(territory == WEI_TERRITORY_INDEX){
        mCenter.row = info.row + 1;
        mCenter.column = info.column;
    }
    else if(territory == SHU_TERRITORY_INDEX){
        mCenter.row = info.row - 1;
        mCenter.column = info.column;
    }
    else if(territory == HAN_TERRITORY_INDEX){
        mCenter.row = info.row;
        mCenter.column = info.column + 1;
    }
}
Shu::~Shu(){

}
void Shu::Selected(const Chess *pChess[4][COUNTRY_CHESS_COUNT], std::vector<ChessInfo>&canplays)const{
    const ChessInfo cInfo[] = { ChessInfo(mInfo.row + 1, mInfo.column), ChessInfo(mInfo.row - 1, mInfo.column), ChessInfo(mInfo.row, mInfo.column + 1), ChessInfo(mInfo.row, mInfo.column - 1) };
    for(uint32_t i = 0; i < sizeof(cInfo) / sizeof(ChessInfo); ++i){
        if(!IsBoundary(cInfo[i].row, cInfo[i].column)){
            if(IsInPalace(cInfo[i].row, cInfo[i].column, &mCenter)){//将不能出九宫格
                const ChessInfo *pChessInfo = GetChessInfo(cInfo[i].row, cInfo[i].column, pChess);
                if(!pChessInfo || pChessInfo->country != mInfo.country)
                    canplays.push_back(ChessInfo(cInfo[i].row, cInfo[i].column));
            }
        }
    }
    SelectChessInPalace(pChess, 1, &mCenter, canplays);
}
Wu::Wu(const ChessInfo&info):Chess(info){
    uint32_t territory = GetTerritoryIndex(info.row, info.column);
    if(territory == WU_TERRITORY_INDEX){
        mCenter.row = info.row - 1;
        mCenter.column = info.column;
    }
    else if(territory == WEI_TERRITORY_INDEX){
        mCenter.row = info.row + 1;
        mCenter.column = info.column;
    }
    else if(territory == SHU_TERRITORY_INDEX){
        mCenter.row = info.row - 1;
        mCenter.column = info.column;
    }
    else if(territory == HAN_TERRITORY_INDEX){
        mCenter.row = info.row;
        mCenter.column = info.column + 1;
    }
}
Wu::~Wu(){

}
void Wu::Selected(const Chess *pChess[4][COUNTRY_CHESS_COUNT], std::vector<ChessInfo>&canplays)const{
    const ChessInfo cInfo[] = { ChessInfo(mInfo.row + 1, mInfo.column), ChessInfo(mInfo.row - 1, mInfo.column), ChessInfo(mInfo.row, mInfo.column + 1), ChessInfo(mInfo.row, mInfo.column - 1) };
    for(uint32_t i = 0; i < sizeof(cInfo) / sizeof(ChessInfo); ++i){
        if(!IsBoundary(cInfo[i].row, cInfo[i].column)){
            if(IsInPalace(cInfo[i].row, cInfo[i].column, &mCenter)){//将不能出九宫格
                const ChessInfo *pChessInfo = GetChessInfo(cInfo[i].row, cInfo[i].column, pChess);
                if(!pChessInfo || pChessInfo->country != mInfo.country)
                    canplays.push_back(ChessInfo(cInfo[i].row, cInfo[i].column));
            }
        }
    }
    SelectChessInPalace(pChess, 1, &mCenter, canplays);
}
Han::Han(const ChessInfo&info):Chess(info){
#ifdef HAN_CAN_PLAY
    uint32_t territory = GetTerritoryIndex(info.row, info.column);
    if(territory == WU_TERRITORY_INDEX){
        mCenter.row = info.row - 1;
        mCenter.column = info.column;
    }
    else if(territory == WEI_TERRITORY_INDEX){
        mCenter.row = info.row + 1;
        mCenter.column = info.column;
    }
    else if(territory == SHU_TERRITORY_INDEX){
        mCenter.row = info.row - 1;
        mCenter.column = info.column;
    }
    else if(territory == HAN_TERRITORY_INDEX){
        mCenter.row = info.row;
        mCenter.column = info.column + 1;
    }
#endif
}
Han::~Han(){

}
void Han::Selected(const Chess *pChess[4][COUNTRY_CHESS_COUNT], std::vector<ChessInfo>&canplays)const{
#ifdef HAN_CAN_PLAY
    const ChessInfo cInfo[] = { ChessInfo(mInfo.row + 1, mInfo.column), ChessInfo(mInfo.row - 1, mInfo.column), ChessInfo(mInfo.row, mInfo.column + 1), ChessInfo(mInfo.row, mInfo.column - 1) };
    for(uint32_t i = 0; i < sizeof(cInfo) / sizeof(ChessInfo); ++i){
        if(!IsBoundary(cInfo[i].row, cInfo[i].column)){
            if(IsInPalace(cInfo[i].row, cInfo[i].column, &mCenter)){//将不能出九宫格
                const ChessInfo *pChessInfo = GetChessInfo(cInfo[i].row, cInfo[i].column, pChess);
                if(!pChessInfo || pChessInfo->country != mInfo.country)
                    canplays.push_back(ChessInfo(cInfo[i].row, cInfo[i].column));
            }
        }
    }
    SelectChessInPalace(pChess, 1, &mCenter, canplays);
#endif
}
glm::vec2 Bing::GetBingBack()const{
    glm::vec2 back = glm::vec2(0);
    if(mTerriory == WU_TERRITORY_INDEX){
        back.x = 1;
    }
    else if(mTerriory == WEI_TERRITORY_INDEX){
        back.y = -1;
    }
    else if(mTerriory == SHU_TERRITORY_INDEX){
        back.y = 1;
    }
    else if(mTerriory == HAN_TERRITORY_INDEX){
        back.x = -1;
    }
    return back;
}
bool Bing::IsAbroad(uint32_t row, uint32_t column)const{
    return mTerriory != GetTerritoryIndex(row, column);
}
Bing::Bing(const ChessInfo &info) : Chess(info){
    mTerriory = GetTerritoryIndex(info.row, info.column);

    mCenter[WU_COUNTRY_INDEX].row = 8;
    mCenter[WU_COUNTRY_INDEX].column = 15;

    mCenter[WEI_COUNTRY_INDEX].row = 1;
    mCenter[WEI_COUNTRY_INDEX].column = 8;

    mCenter[SHU_COUNTRY_INDEX].row = 15;
    mCenter[SHU_COUNTRY_INDEX].column = 8;
    //因为汉在最后面,如果没定义,后面没用到,所以不需要管
    mCenter[HAN_COUNTRY_INDEX].row = 8;
    mCenter[HAN_COUNTRY_INDEX].column = 1;
    
    SwapCenter(&mCenter[info.country], &mCenter[HAN_COUNTRY_INDEX]);
#ifndef HAN_CAN_PLAY
    uint32_t playerCountry = GetPlayerCountry(info.country, mTerriory);
    SwapCenter(&mCenter[info.country], &mCenter[HAN_COUNTRY_INDEX]);
    if(playerCountry == WU_COUNTRY_INDEX){
        SwapCenter(&mCenter[WEI_COUNTRY_INDEX], &mCenter[SHU_COUNTRY_INDEX]);
    }
    else if(playerCountry == WEI_COUNTRY_INDEX){
        SwapCenter(&mCenter[WU_COUNTRY_INDEX], &mCenter[SHU_COUNTRY_INDEX]);
    }
    // else if(playerCountry == SHU_COUNTRY_INDEX){
    //     SwapCenter(HAN_COUNTRY_INDEX, HAN_COUNTRY_INDEX);
    // }
#endif
}
Bing::~Bing(){

}
void Bing::Selected(const Chess *pChess[4][COUNTRY_CHESS_COUNT], std::vector<ChessInfo>&canplays)const{
    //兵在自身境内不能后退
    const glm::vec2 back = GetBingBack();
    const bool bAbroad = IsAbroad(mInfo.row, mInfo.column);
    // const const ChessInfo *pChessInfo[] = { ChessInfo(mRow + 1, mColumn), ChessInfo(mRow - 1, mColumn), ChessInfo(mRow, mColumn + 1), ChessInfo(mRow, mColumn - 1) };
    const glm::vec2 cInfo[] = { glm::vec2(0, 1), glm::vec2(0, -1), glm::vec2(1, 0), glm::vec2(-1, 0) };
    for(uint32_t i = 0; i < sizeof(cInfo) / sizeof(glm::vec2); ++i){
        if(!bAbroad && cInfo[i].x == back.x && cInfo[i].y == back.y)continue;
        const glm::vec2 currentPos = glm::vec2(mInfo.column, mInfo.row) + cInfo[i];
        if(!IsBoundary(currentPos.y, currentPos.x)){
            const ChessInfo *pChessInfo = GetChessInfo(currentPos.y, currentPos.x, pChess);
            if(!pChessInfo || pChessInfo->country != mInfo.country)
                canplays.push_back(ChessInfo(currentPos.y, currentPos.x));
        }
    }
#ifdef HAN_CAN_PLAY
    SelectChessInPalace(pChess, 3, mCenter, canplays);
#else
    SelectChessInPalace(pChess, 2, mCenter, canplays);
#endif
}

Pao::Pao(const ChessInfo&info):Chess(info){
    uint32_t territory = GetTerritoryIndex(info.row, info.column);
    mCenter[WU_COUNTRY_INDEX].row = 8;
    mCenter[WU_COUNTRY_INDEX].column = 15;

    mCenter[WEI_COUNTRY_INDEX].row = 1;
    mCenter[WEI_COUNTRY_INDEX].column = 8;

    mCenter[SHU_COUNTRY_INDEX].row = 15;
    mCenter[SHU_COUNTRY_INDEX].column = 8;

    mCenter[HAN_COUNTRY_INDEX].row = 8;
    mCenter[HAN_COUNTRY_INDEX].column = 1;
#ifndef HAN_CAN_PLAY
    uint32_t playerCountry = GetPlayerCountry(info.country, territory);
    if(playerCountry == WU_COUNTRY_INDEX){
        SwapCenter(&mCenter[WEI_COUNTRY_INDEX], &mCenter[HAN_COUNTRY_INDEX]);
    }
    else if(playerCountry == WEI_COUNTRY_INDEX){
        SwapCenter(&mCenter[WU_COUNTRY_INDEX], &mCenter[HAN_COUNTRY_INDEX]);
    }
    //蜀视角的汉仍然在原位不用管
#endif
}
Pao::~Pao(){
    
}
void Pao::Selected(const Chess *pChess[4][COUNTRY_CHESS_COUNT], std::vector<ChessInfo>&canplays)const{
    // bool bCapture = false;
    const glm::vec2 direction[] = { glm::vec2(1, 0), glm::vec2(-1, 0), glm::vec2(0, 1), glm::vec2(0, -1) };
    for(uint32_t i = 0; i < sizeof(direction) / sizeof(glm::vec2); ++i){
        const glm::vec2 currentPos = glm::vec2(mInfo.column, mInfo.row);
        glm::vec2 pos = currentPos + direction[i];
        while(!IsBoundary(pos.y, pos.x)){
            const ChessInfo *pChessInfo = GetChessInfo(pos.y, pos.x, pChess);
            if(pChessInfo){
                pos += direction[i];
                while(!IsBoundary(pos.y, pos.x)){
                    pChessInfo = GetChessInfo(pos.y, pos.x, pChess);
                    if(pChessInfo && pChessInfo->country != mInfo.country){
                        canplays.push_back(ChessInfo(pos.y, pos.x));
                        break;
                    }
                    pos += direction[i];
                }
                break;
            }
            else{
                canplays.push_back(ChessInfo(pos.y, pos.x));
            }
            pos += direction[i];
        }
    }
#ifdef HAN_CAN_PLAY
    SelectChessInPalace(pChess, 4, mCenter, canplays);
#else
    SelectChessInPalace(pChess, 3, mCenter, canplays);
#endif
}

Che::Che(const ChessInfo&info):Chess(info){
    uint32_t territory = GetTerritoryIndex(info.row, info.column);
    mCenter[WU_COUNTRY_INDEX].row = 8;
    mCenter[WU_COUNTRY_INDEX].column = 15;

    mCenter[WEI_COUNTRY_INDEX].row = 1;
    mCenter[WEI_COUNTRY_INDEX].column = 8;

    mCenter[SHU_COUNTRY_INDEX].row = 15;
    mCenter[SHU_COUNTRY_INDEX].column = 8;

    mCenter[HAN_COUNTRY_INDEX].row = 8;
    mCenter[HAN_COUNTRY_INDEX].column = 1;
#ifndef HAN_CAN_PLAY
    uint32_t playerCountry = GetPlayerCountry(info.country, territory);
    if(playerCountry == WU_COUNTRY_INDEX){
        SwapCenter(&mCenter[WEI_COUNTRY_INDEX], &mCenter[HAN_COUNTRY_INDEX]);
    }
    else if(playerCountry == WEI_COUNTRY_INDEX){
        SwapCenter(&mCenter[WU_COUNTRY_INDEX], &mCenter[HAN_COUNTRY_INDEX]);
    }
    //蜀视角的汉仍然在原位不用管
#endif
}
Che::~Che(){
    
}
void Che::Selected(const Chess *pChess[4][COUNTRY_CHESS_COUNT], std::vector<ChessInfo>&canplays)const{
    const glm::vec2 direction[] = { glm::vec2(1, 0), glm::vec2(-1, 0), glm::vec2(0, 1), glm::vec2(0, -1) };
    for(uint32_t i = 0; i < sizeof(direction) / sizeof(glm::vec2); ++i){
        const glm::vec2 currentPos = glm::vec2(mInfo.column, mInfo.row);
        glm::vec2 pos = currentPos + direction[i];
        while(!IsBoundary(pos.y, pos.x)){
            const ChessInfo *pChessInfo = GetChessInfo(pos.y, pos.x, pChess);
            if(pChessInfo){
                if(pChessInfo->country != mInfo.country){
                    canplays.push_back(ChessInfo(pos.y, pos.x));
                }
                break;
            }
            else{
                canplays.push_back(ChessInfo(pos.y, pos.x));
            }
            pos += direction[i];
        }
    }
#ifdef HAN_CAN_PLAY
    SelectChessInPalace(pChess, 4, mCenter, canplays);
#else
    SelectChessInPalace(pChess, 3, mCenter, canplays);
#endif
}

Ma::Ma(const ChessInfo&info):Chess(info){
}
Ma::~Ma(){
    
}
void Ma::Selected(const Chess *pChess[4][COUNTRY_CHESS_COUNT], std::vector<ChessInfo>&canplays)const{
    const ChessInfo cInfo[] = {
        ChessInfo(mInfo.row - 1, mInfo.column - 2), ChessInfo(mInfo.row - 2, mInfo.column - 1), ChessInfo(mInfo.row - 2, mInfo.column + 1), ChessInfo(mInfo.row - 1, mInfo.column + 2),
        ChessInfo(mInfo.row + 1, mInfo.column + 2), ChessInfo(mInfo.row + 2, mInfo.column + 1), ChessInfo(mInfo.row + 2, mInfo.column - 1), ChessInfo(mInfo.row + 1, mInfo.column - 2)
    };
    for(uint32_t i = 0; i < sizeof(cInfo) / sizeof(ChessInfo); ++i){
        if(!IsBoundary(cInfo[i].row, cInfo[i].column)){//这里的马和相不会被"蹩马腿",所以不需要判断
            const ChessInfo *pChessInfo = GetChessInfo(cInfo[i].row, cInfo[i].column, pChess);
            if(!pChessInfo || pChessInfo->country != mInfo.country)
                canplays.push_back(ChessInfo(cInfo[i].row, cInfo[i].column));
        }
    }
}
bool Xiang::IsAbroad(uint32_t row, uint32_t column)const{
    return mTerriory != GetTerritoryIndex(row, column);
    // uint32_t terriory = GetTerritoryIndex(row, column);
    // if(terriory == WU_TERRITORY_INDEX){
    //     if(column < 12 || row < 4 || row > 12)return true;
    // }
    // else if(terriory == HAN_TERRITORY_INDEX){
    //     if(row < 4 || row > 12 || column > 12)return true;
    // }
    // else if(terriory == WEI_TERRITORY_INDEX){
    //     if(row > 4 || column < 4 || column > 12)return true;
    // }
    // else if(terriory == SHU_TERRITORY_INDEX){
    //     if(row < 12 || column < 4 || column > 12)return true;
    // }
    // return false;
}
Xiang::Xiang(const ChessInfo&info):Chess(info){
    mTerriory = GetTerritoryIndex(info.row, info.column);
}
Xiang::~Xiang(){
    
}
void Xiang::Selected(const Chess *pChess[4][COUNTRY_CHESS_COUNT], std::vector<ChessInfo>&canplays)const{
    const ChessInfo cInfo[] = { ChessInfo(mInfo.row + 2, mInfo.column + 2), ChessInfo(mInfo.row - 2, mInfo.column - 2), ChessInfo(mInfo.row - 2, mInfo.column + 2), ChessInfo(mInfo.row + 2, mInfo.column - 2) };
    for(uint32_t i = 0; i < sizeof(cInfo) / sizeof(ChessInfo); ++i){
        if(!IsBoundary(cInfo[i].row, cInfo[i].column) && !IsAbroad(cInfo[i].row, cInfo[i].column)){
            const ChessInfo *pChessInfo = GetChessInfo(cInfo[i].row, cInfo[i].column, pChess);
            if(!pChessInfo || pChessInfo->country != mInfo.country)
                canplays.push_back(ChessInfo(cInfo[i].row, cInfo[i].column));
        }
    }
}
Shi::Shi(const ChessInfo&info):Chess(info){
    uint32_t territory = GetTerritoryIndex(info.row, info.column);
    if(territory == WU_TERRITORY_INDEX){
        mCenter.row = 8;
        mCenter.column = CHESSBOARD_COLUMN - 1;
    }
    else if(territory == WEI_TERRITORY_INDEX){
        mCenter.row = 1;
        mCenter.column = 8;
    }
    else if(territory == SHU_TERRITORY_INDEX){
        mCenter.row = CHESSBOARD_ROW - 1;
        mCenter.column = 8;
    }
    else if(territory == HAN_TERRITORY_INDEX){
        mCenter.row = 8;
        mCenter.column = 1;
    }
}
Shi::~Shi(){
    
}
void Shi::Selected(const Chess *pChess[4][COUNTRY_CHESS_COUNT], std::vector<ChessInfo>&canplays)const{
    SelectChessInPalace(pChess, 1, &mCenter, canplays);
}