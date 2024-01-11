#include "Chess.h"
#include "VulkanChess.h"
#include "VulkanChessboard.h"
//除了将。视角不同, 九宫格位置也不同
void GetCenterPos(uint32_t playerIndex, ChessInfo *out){
#ifdef HAN_CAN_PLAY
    out[0].row = 1;
    out[0].column = 8;

    out[1].row = 8;
    out[1].column = 15;

    out[2].row = 8;
    out[2].column = 1;

    out[3].row = 15;
    out[3].column = 8;
#else
    if(playerIndex == WU_COUNTRY_INDEX){
        out[2].row = 8;
        out[2].column = 0;

        out[1].row = 8;
        out[1].column = 15;
    }
    else if(playerIndex == WEI_COUNTRY_INDEX){
        out[2].row = 8;
        out[2].column = 1;

        out[0].row = 1;
        out[0].column = 8;
    }
    else if(playerIndex == SHU_COUNTRY_INDEX){
        out[0].row = 1;
        out[0].column = 8;

        out[1].row = 8;
        out[1].column = 15;
    }
#endif
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
    // mCenter.row = info.row + 1;
    // mCenter.column = info.column;
    //不管那个参照, 自身的中心点肯定在蜀
    mCenter.row = info.row - 1;
    mCenter.column = info.column;
}
Wei::~Wei(){

}
void Wei::Selected(uint32_t playerIndex, const Chess *pChess[4][COUNTRY_CHESS_COUNT], std::vector<ChessInfo>&canplays)const{
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
    mCenter.row = info.row - 1;
    mCenter.column = info.column;
}
Shu::~Shu(){

}
void Shu::Selected(uint32_t playerIndex, const Chess *pChess[4][COUNTRY_CHESS_COUNT], std::vector<ChessInfo>&canplays)const{
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
    // mCenter.row = info.row;
    // mCenter.column = info.column - 1;
    mCenter.row = info.row - 1;
    mCenter.column = info.column;
}
Wu::~Wu(){

}
void Wu::Selected(uint32_t playerIndex, const Chess *pChess[4][COUNTRY_CHESS_COUNT], std::vector<ChessInfo>&canplays)const{
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
    // mCenter.row = info.row;
    // mCenter.column = info.column + 1;
    mCenter.row = info.row - 1;
    mCenter.column = info.column;
#endif
}
Han::~Han(){

}
void Han::Selected(uint32_t playerIndex, const Chess *pChess[4][COUNTRY_CHESS_COUNT], std::vector<ChessInfo>&canplays)const{
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
glm::vec2 Bing::GetCountryBack(uint32_t country)const{
    glm::vec2 back = glm::vec2(0);
    if(country == WU_COUNTRY_INDEX){
        back.x = 1;
    }
    else if(country == WEI_COUNTRY_INDEX){
        back.y = -1;
    }
    else if(country == SHU_COUNTRY_INDEX){
        back.y = 1;
    }
    else if(country == HAN_COUNTRY_INDEX){
        back.x = -1;
    }
    else{
        printf("无效的国家:%d\n", country);
    }
    return back;
}
bool Bing::IsAbroad(uint32_t row, uint32_t column)const{
    if(mInfo.country == WEI_COUNTRY_INDEX){
        if(row > 4 || column < 4 || column > 12)return true;
    }
    else if(mInfo.country == SHU_COUNTRY_INDEX){
        if(row < 12 || column < 4 || column > 12)return true;
    }
    else{
        if(column < 12 || row < 4 || row > 12)return true;
    }
    return false;
}
Bing::Bing(const ChessInfo&info):Chess(info){
    // if(mInfo.country == WEI_COUNTRY_INDEX){
    //     //重新设定索引是为了不越界, 因为WU_COUNTRY_INDEX是2而center只有2个
    //     const uint32_t shuIndex = 0, wuIndex = 1;
    //     center[shuIndex].row = 15;
    //     center[shuIndex].column = 8;

    //     center[wuIndex].row = 8;
    //     center[wuIndex].column = 15;
    // }
    // else if(mInfo.country == SHU_COUNTRY_INDEX){
    //     const uint32_t weiIndex = 0, wuIndex = 1;
    //     center[weiIndex].row = 1;
    //     center[weiIndex].column = 8;

    //     center[wuIndex].row = 8;
    //     center[wuIndex].column = 15;
    // }
    // else if(mInfo.country == WU_COUNTRY_INDEX){
    //     const uint32_t weiIndex = 0, shuIndex = 1;
    //     center[weiIndex].row = 1;
    //     center[weiIndex].column = 8;

    //     center[shuIndex].row = 15;
    //     center[shuIndex].column = 8;
    // }
}
Bing::~Bing(){

}
void Bing::Selected(uint32_t playerIndex, const Chess *pChess[4][COUNTRY_CHESS_COUNT], std::vector<ChessInfo>&canplays)const{
    //windows下兵不能吃，一吃就delete 崩溃
    //兵在自身境内不能后退
    const glm::vec2 back = GetCountryBack(mInfo.country);
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
    GetCenterPos(playerIndex, (ChessInfo *)mCenter);
#ifdef HAN_CAN_PLAY
    SelectChessInPalace(pChess, 3, mCenter, canplays);
#else
    SelectChessInPalace(pChess, 2, mCenter, canplays);
#endif
}

Pao::Pao(const ChessInfo&info):Chess(info){
    // center[WEI_COUNTRY_INDEX].row = 1;
    // center[WEI_COUNTRY_INDEX].column = 8;

    // center[SHU_COUNTRY_INDEX].row = 15;
    // center[SHU_COUNTRY_INDEX].column = 8;

    // center[WU_COUNTRY_INDEX].row = 8;
    // center[WU_COUNTRY_INDEX].column = 15;
}
Pao::~Pao(){
    
}
void Pao::Selected(uint32_t playerIndex, const Chess *pChess[4][COUNTRY_CHESS_COUNT], std::vector<ChessInfo>&canplays)const{
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
    GetCenterPos(playerIndex, (ChessInfo *)mCenter);
#ifdef HAN_CAN_PLAY
    SelectChessInPalace(pChess, 4, mCenter, canplays);
#else
    SelectChessInPalace(pChess, 3, mCenter, canplays);
#endif
}

Che::Che(const ChessInfo&info):Chess(info){
    // center[WEI_COUNTRY_INDEX].row = 1;
    // center[WEI_COUNTRY_INDEX].column = 8;

    // center[SHU_COUNTRY_INDEX].row = 15;
    // center[SHU_COUNTRY_INDEX].column = 8;

    // center[WU_COUNTRY_INDEX].row = 8;
    // center[WU_COUNTRY_INDEX].column = 15;
}
Che::~Che(){
    
}
void Che::Selected(uint32_t playerIndex, const Chess *pChess[4][COUNTRY_CHESS_COUNT], std::vector<ChessInfo>&canplays)const{
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
    GetCenterPos(playerIndex, (ChessInfo *)mCenter);
#ifdef HAN_CAN_PLAY
    SelectChessInPalace(pChess, 4, mCenter, canplays);
#else
    SelectChessInPalace(pChess, 3, mCenter, canplays);
#endif
}

Ma::Ma(const ChessInfo&info):Chess(info){
    // center[WEI_COUNTRY_INDEX].row = 1;
    // center[WEI_COUNTRY_INDEX].column = 8;

    // center[SHU_COUNTRY_INDEX].row = 15;
    // center[SHU_COUNTRY_INDEX].column = 8;

    // center[WU_COUNTRY_INDEX].row = 8;
    // center[WU_COUNTRY_INDEX].column = 15;
}
Ma::~Ma(){
    
}
void Ma::Selected(uint32_t playerIndex, const Chess *pChess[4][COUNTRY_CHESS_COUNT], std::vector<ChessInfo>&canplays)const{
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
    if(mInfo.country == WEI_COUNTRY_INDEX){
        if(row > 4 || column < 4 || column > 12)return true;
    }
    else if(mInfo.country == SHU_COUNTRY_INDEX){
        if(row < 12 || column < 4 || column > 12)return true;
    }
    else{
        if(column < 12 || row < 4 || row > 12)return true;
    }
    return false;
}
Xiang::Xiang(const ChessInfo&info):Chess(info){

}
Xiang::~Xiang(){
    
}
void Xiang::Selected(uint32_t playerIndex, const Chess *pChess[4][COUNTRY_CHESS_COUNT], std::vector<ChessInfo>&canplays)const{
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
    // if(info.country == WEI_COUNTRY_INDEX){
    //     mCenter.row = 1;
    //     mCenter.column = 8;
    // }
    // else if(info.country == SHU_COUNTRY_INDEX){
    //     mCenter.row = 15;
    //     mCenter.column = 8;
    // }
    // else if(info.country == WU_COUNTRY_INDEX){
    //     mCenter.row = 8;
    //     mCenter.column = 15;
    // }
    mCenter.row = info.row - 1;
    mCenter.column = info.column;
}
Shi::~Shi(){
    
}
void Shi::Selected(uint32_t playerIndex, const Chess *pChess[4][COUNTRY_CHESS_COUNT], std::vector<ChessInfo>&canplays)const{
    SelectChessInPalace(pChess, 1, &mCenter, canplays);
}