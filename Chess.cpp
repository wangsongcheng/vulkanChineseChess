#include "Chess.h"
const Chess g_Center[] = { Chess(CHESSBOARD_ROW - 1, 8), Chess(8, CHESSBOARD_COLUMN - 1), Chess(1, 8), Chess(8, 1) };
uint32_t GetTerritoryIndex(uint32_t row, uint32_t column){
    if(row < 13 && row > 3){
        if(column > 11){
            return WU_TERRITORY_INDEX;
        }
        else if(column < 5){
            return HAN_TERRITORY_INDEX;
        }
        else{
            return CENTER_TERRITORY_INDEX;
        }
    }
    else if(column > 3 && column < 13){
        //魏或蜀
        if(row > 11){
            return SHU_TERRITORY_INDEX;
        }
        else if(row < 5){
            return WEI_TERRITORY_INDEX;
        }
        else{
            return CENTER_TERRITORY_INDEX;
        }
    }
    return INVALID_TERRITORY_INDEX;
}
bool IsBoundary(uint32_t row, uint32_t column){
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
//返回所在九宫格的势力
uint32_t IsPalaceCenter(uint32_t row, uint32_t column){
    uint32_t country = MAX_COUNTRY_INDEX;
    for (size_t i = 0; i < MAX_COUNTRY_INDEX; ++i){
        if(row == g_Center[i].GetRow() && column == g_Center[i].GetColumn()){
            // printf("%d国,%d行%d列的棋子:%d, 在中心为%d行%d列的九宫格内\n", mInfo.country, row, column, mInfo.chess, g_Center[i].GetRow(), g_Center[i].GetColumn());
            country = i;
            break;
        }   
    }
    return country;
}
uint32_t IsInPalace(uint32_t row, uint32_t column){
    uint32_t country = IsPalaceCenter(row, column);
    if(country != MAX_COUNTRY_INDEX)return country;
    for (size_t i = 0; i < MAX_COUNTRY_INDEX; ++i){
        if(abs((float)row - g_Center[i].GetRow()) < 2 && abs((float)column - g_Center[i].GetColumn()) < 2){
            country = i;
            break;
        }
    }
    return country;
}
const Chess *GetChess(uint32_t country, uint32_t row, uint32_t column, Chess *pChess[MAX_COUNTRY_INDEX][DRAW_COUNTRY_CHESS_COUNT]){
    for (uint32_t uiChess = 0; uiChess < DRAW_COUNTRY_CHESS_COUNT; ++uiChess){
        const Chess *pc = pChess[country][uiChess];
        if(pc && pc->GetRow() == row && pc->GetColumn() == column){
            return pc;
        }
    }
    return nullptr;
}
const Chess *GetChess(uint32_t row, uint32_t column, Chess *pChess[MAX_COUNTRY_INDEX][DRAW_COUNTRY_CHESS_COUNT]){
    const Chess *pc = nullptr;
    for (uint32_t uiCountry = 0; uiCountry < MAX_COUNTRY_INDEX; ++uiCountry){
        pc = GetChess(uiCountry, row, column, pChess);
        if(pc)break;
    }
    return pc;
}
void SelectChessInPalace(uint32_t currentCountry, uint32_t row, uint32_t column, Chess *pChess[MAX_COUNTRY_INDEX][DRAW_COUNTRY_CHESS_COUNT], std::vector<Chess>&canplays){
    //只需要管斜线，其他位置归棋子子类管
    const Chess *pc = nullptr;
    uint32_t country = IsPalaceCenter(row, column);
    if(country != MAX_COUNTRY_INDEX){
        const Chess pos[] = {
            Chess(g_Center[country].GetRow() - 1, g_Center[country].GetColumn() - 1),
            Chess(g_Center[country].GetRow() - 1, g_Center[country].GetColumn() + 1),
            Chess(g_Center[country].GetRow() + 1, g_Center[country].GetColumn() - 1),
            Chess(g_Center[country].GetRow() + 1, g_Center[country].GetColumn() + 1)
        };
        //除馬、相外, 其他棋子在中心点时，能走的肯定就是斜线边的点
        for (size_t i = 0; i < sizeof(pos) / sizeof(Chess); ++i){
            pc = GetChess(pos[i].GetRow(), pos[i].GetColumn(), pChess);
            if(!pc || pc->GetCountry() != currentCountry){
                canplays.push_back(pos[i]);
            }
        }
    }
    else if((country = IsInPalace(row, column)) != MAX_COUNTRY_INDEX){
        //在斜线边的点上时, 能走的只有中心点
        pc = GetChess(g_Center[country].GetRow(), g_Center[country].GetColumn(), pChess);
        if(!pc || pc->GetCountry() != currentCountry)canplays.push_back(g_Center[country]);
    }
}

Chess::Chess(/* args */){
}

Chess::Chess(uint32_t row, uint32_t column){
    SetPos(row, column);
}

Chess::Chess(uint32_t chess, uint32_t country, uint32_t row, uint32_t column){
    SetChess(chess);
    SetPos(row, column);
    SetCountry(country);
}

Chess::Chess(uint32_t chess, uint32_t country, uint32_t fontIndex, uint32_t row, uint32_t column){
    SetChess(chess);
    SetPos(row, column);
    SetCountry(country);
    SetFontIndex(fontIndex);
}
Chess::~Chess(){
}

void Chess::SetPos(uint32_t row, uint32_t column){
    mRow = row;
    mColumn = column;
    mPos.y = ROW_TO_Y(mRow);
    mPos.x = COLUMN_TO_X(mColumn);
    if(INVALID_TERRITORY_INDEX == mTerriory)mTerriory = GetTerritoryIndex(row, column);
}

void Chess::Selected(Chess *pChess[MAX_COUNTRY_INDEX][DRAW_COUNTRY_CHESS_COUNT], std::vector<Chess> &canplays) const{
}

Wei::Wei(uint32_t row, uint32_t column):Chess(row, column){
}

Wei::Wei(){
}

Wei::Wei(uint32_t chess, uint32_t country, uint32_t row, uint32_t column):Chess(chess, country, row, column){
}

Wei::Wei(uint32_t chess, uint32_t country, uint32_t fontIndex, uint32_t row, uint32_t column):Chess(chess, country, fontIndex, row, column){
}

Wei::~Wei(){
}

void Wei::Selected(Chess *pChess[MAX_COUNTRY_INDEX][DRAW_COUNTRY_CHESS_COUNT], std::vector<Chess> &canplays) const{
    const Chess cInfo[] = { Chess(GetRow() + 1, GetColumn()), Chess(GetRow() - 1, GetColumn()), Chess(GetRow(), GetColumn() + 1), Chess(GetRow(), GetColumn() - 1) };
    for(uint32_t i = 0; i < sizeof(cInfo) / sizeof(Chess); ++i){
        if(!IsBoundary(cInfo[i].GetRow(), cInfo[i].GetColumn()) && MAX_COUNTRY_INDEX !=  IsInPalace(cInfo[i].GetRow(), cInfo[i].GetColumn())){//将不能出九宫格
            const Chess *pc = ::GetChess(cInfo[i].GetRow(), cInfo[i].GetColumn(), pChess);
            if(!pc || pc->GetCountry() != GetCountry())
                canplays.push_back(Chess(cInfo[i].GetRow(), cInfo[i].GetColumn()));
        }
    }
    SelectChessInPalace(GetCountry(), GetRow(), GetColumn(), pChess, canplays);
}

Shu::Shu(){
}

Shu::Shu(uint32_t row, uint32_t column):Chess(row, column){
}

Shu::Shu(uint32_t chess, uint32_t country, uint32_t row, uint32_t column):Chess(chess, country, row, column){
}

Shu::Shu(uint32_t chess, uint32_t country, uint32_t fontIndex, uint32_t row, uint32_t column):Chess(chess, country, fontIndex, row, column){
}

Shu::~Shu(){
}

void Shu::Selected(Chess *pChess[MAX_COUNTRY_INDEX][DRAW_COUNTRY_CHESS_COUNT], std::vector<Chess> &canplays) const{
    const Chess cInfo[] = { Chess(GetRow() + 1, GetColumn()), Chess(GetRow() - 1, GetColumn()), Chess(GetRow(), GetColumn() + 1), Chess(GetRow(), GetColumn() - 1) };
    for(uint32_t i = 0; i < sizeof(cInfo) / sizeof(Chess); ++i){
        if(!IsBoundary(cInfo[i].GetRow(), cInfo[i].GetColumn()) && MAX_COUNTRY_INDEX !=  IsInPalace(cInfo[i].GetRow(), cInfo[i].GetColumn())){//将不能出九宫格
            const Chess *pc = ::GetChess(cInfo[i].GetRow(), cInfo[i].GetColumn(), pChess);
            if(!pc || pc->GetCountry() != GetCountry())
                canplays.push_back(Chess(cInfo[i].GetRow(), cInfo[i].GetColumn()));
        }
    }
    SelectChessInPalace(GetCountry(), GetRow(), GetColumn(), pChess, canplays);
}

Wu::Wu(){
}

Wu::Wu(uint32_t row, uint32_t column):Chess(row, column){
}

Wu::Wu(uint32_t chess, uint32_t country, uint32_t row, uint32_t column):Chess(chess, country, row, column){
}

Wu::Wu(uint32_t chess, uint32_t country, uint32_t fontIndex, uint32_t row, uint32_t column):Chess(chess, country, fontIndex, row, column){
}

Wu::~Wu(){
}

void Wu::Selected(Chess *pChess[MAX_COUNTRY_INDEX][DRAW_COUNTRY_CHESS_COUNT], std::vector<Chess> &canplays) const{
    const Chess cInfo[] = { Chess(GetRow() + 1, GetColumn()), Chess(GetRow() - 1, GetColumn()), Chess(GetRow(), GetColumn() + 1), Chess(GetRow(), GetColumn() - 1) };
    for(uint32_t i = 0; i < sizeof(cInfo) / sizeof(Chess); ++i){
        if(!IsBoundary(cInfo[i].GetRow(), cInfo[i].GetColumn()) && MAX_COUNTRY_INDEX != IsInPalace(cInfo[i].GetRow(), cInfo[i].GetColumn())){//将不能出九宫格
            const Chess *pc = ::GetChess(cInfo[i].GetRow(), cInfo[i].GetColumn(), pChess);
            if(!pc || pc->GetCountry() != GetCountry())
                canplays.push_back(Chess(cInfo[i].GetRow(), cInfo[i].GetColumn()));
        }
    }
    SelectChessInPalace(GetCountry(), GetRow(), GetColumn(), pChess, canplays);
}

Han::Han(){
}

Han::Han(uint32_t row, uint32_t column):Chess(row, column){
}

Han::Han(uint32_t chess, uint32_t country, uint32_t row, uint32_t column):Chess(chess, country, row, column){
}

Han::Han(uint32_t chess, uint32_t country, uint32_t fontIndex, uint32_t row, uint32_t column):Chess(chess, country, fontIndex, row, column){
}

Han::~Han(){
}

void Han::Selected(Chess *pChess[MAX_COUNTRY_INDEX][DRAW_COUNTRY_CHESS_COUNT], std::vector<Chess> &canplays) const{
    const Chess cInfo[] = { Chess(GetRow() + 1, GetColumn()), Chess(GetRow() - 1, GetColumn()), Chess(GetRow(), GetColumn() + 1), Chess(GetRow(), GetColumn() - 1) };
    for(uint32_t i = 0; i < sizeof(cInfo) / sizeof(Chess); ++i){
        if(!IsBoundary(cInfo[i].GetRow(), cInfo[i].GetColumn()) && MAX_COUNTRY_INDEX !=  IsInPalace(cInfo[i].GetRow(), cInfo[i].GetColumn())){//将不能出九宫格
            const Chess *pc = ::GetChess(cInfo[i].GetRow(), cInfo[i].GetColumn(), pChess);
            if(!pc || pc->GetCountry() != GetCountry())
                canplays.push_back(Chess(cInfo[i].GetRow(), cInfo[i].GetColumn()));
        }
    }
    SelectChessInPalace(GetCountry(), GetRow(), GetColumn(), pChess, canplays);
}

glm::vec2 Bing::GetBingBack() const{
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

bool Bing::IsAbroad(uint32_t row, uint32_t column) const{
    return mTerriory != GetTerritoryIndex(row, column);
}

Bing::Bing(){
}

Bing::Bing(uint32_t row, uint32_t column):Chess(row, column){
    mTerriory = GetTerritoryIndex(row, column);
}

Bing::Bing(uint32_t chess, uint32_t country, uint32_t row, uint32_t column):Chess(chess, country, row, column){
    mTerriory = GetTerritoryIndex(row, column);
}

Bing::Bing(uint32_t chess, uint32_t country, uint32_t fontIndex, uint32_t row, uint32_t column):Chess(chess, country, fontIndex, row, column){
    mTerriory = GetTerritoryIndex(row, column);
}

Bing::~Bing(){
}

void Bing::Selected(Chess *pChess[MAX_COUNTRY_INDEX][DRAW_COUNTRY_CHESS_COUNT], std::vector<Chess> &canplays) const{
    //兵在自身境内不能后退
    const glm::vec2 back = GetBingBack();
    const bool bAbroad = IsAbroad(GetRow(), GetColumn());
    // const const ChessInfo *pChessInfo[] = { ChessInfo(mRow + 1, mColumn), ChessInfo(mRow - 1, mColumn), ChessInfo(mRow, mColumn + 1), ChessInfo(mRow, mColumn - 1) };
    const glm::vec2 cInfo[] = { glm::vec2(0, 1), glm::vec2(0, -1), glm::vec2(1, 0), glm::vec2(-1, 0) };
    for(uint32_t i = 0; i < sizeof(cInfo) / sizeof(glm::vec2); ++i){
        if(!bAbroad && cInfo[i].x == back.x && cInfo[i].y == back.y)continue;
        const glm::vec2 currentPos = glm::vec2(GetColumn(), GetRow()) + cInfo[i];
        if(!IsBoundary(currentPos.y, currentPos.x)){
            const Chess *pc = ::GetChess(currentPos.y, currentPos.x, pChess);
            if(!pc || pc->GetCountry() != GetCountry())
                canplays.push_back(Chess(currentPos.y, currentPos.x));
        }
    }
    SelectChessInPalace(GetCountry(), GetRow(), GetColumn(), pChess, canplays);
}

Pao::Pao(){
}

Pao::Pao(uint32_t row, uint32_t column):Chess(row, column){
}

Pao::Pao(uint32_t chess, uint32_t country, uint32_t row, uint32_t column):Chess(chess, country, row, column){
}

Pao::Pao(uint32_t chess, uint32_t country, uint32_t fontIndex, uint32_t row, uint32_t column):Chess(chess, country, fontIndex, row, column){
}

Pao::~Pao(){
}

void Pao::Selected(Chess *pChess[MAX_COUNTRY_INDEX][DRAW_COUNTRY_CHESS_COUNT], std::vector<Chess> &canplays) const{
    const glm::vec2 direction[] = { glm::vec2(1, 0), glm::vec2(-1, 0), glm::vec2(0, 1), glm::vec2(0, -1) };
    for(uint32_t i = 0; i < sizeof(direction) / sizeof(glm::vec2); ++i){
        const glm::vec2 currentPos = glm::vec2(GetColumn(), GetRow());
        glm::vec2 pos = currentPos + direction[i];
        while(!IsBoundary(pos.y, pos.x)){
            const Chess *pChessInfo = ::GetChess(pos.y, pos.x, pChess);
            if(pChessInfo){
                pos += direction[i];
                while(!IsBoundary(pos.y, pos.x)){
                    pChessInfo = ::GetChess(pos.y, pos.x, pChess);
                    if(pChessInfo && pChessInfo->GetCountry() != GetCountry()){
                        canplays.push_back(Chess(pos.y, pos.x));
                        break;
                    }
                    pos += direction[i];
                }
                break;
            }
            else{
                canplays.push_back(Chess(pos.y, pos.x));
            }
            pos += direction[i];
        }
    }
    SelectChessInPalace(GetCountry(), GetRow(), GetColumn(), pChess, canplays);
}

Che::Che(){
}

Che::Che(uint32_t row, uint32_t column):Chess(row, column){
}

Che::Che(uint32_t chess, uint32_t country, uint32_t row, uint32_t column):Chess(chess, country, row, column){
}

Che::Che(uint32_t chess, uint32_t country, uint32_t fontIndex, uint32_t row, uint32_t column):Chess(chess, country, fontIndex, row, column){
}

Che::~Che(){
}

void Che::Selected(Chess *pChess[MAX_COUNTRY_INDEX][DRAW_COUNTRY_CHESS_COUNT], std::vector<Chess> &canplays) const{
    const glm::vec2 direction[] = { glm::vec2(1, 0), glm::vec2(-1, 0), glm::vec2(0, 1), glm::vec2(0, -1) };
    for(uint32_t i = 0; i < sizeof(direction) / sizeof(glm::vec2); ++i){
        const glm::vec2 currentPos = glm::vec2(GetColumn(), GetRow());
        glm::vec2 pos = currentPos + direction[i];
        while(!IsBoundary(pos.y, pos.x)){
            const Chess *pc = ::GetChess(pos.y, pos.x, pChess);
            if(pc){
                if(pc->GetCountry() != GetCountry()){
                    canplays.push_back(Chess(pos.y, pos.x));
                }
                break;
            }
            else{
                canplays.push_back(Chess(pos.y, pos.x));
            }
            pos += direction[i];
        }
    }
    SelectChessInPalace(GetCountry(), GetRow(), GetColumn(), pChess, canplays);
}

Ma::Ma(){
}

Ma::Ma(uint32_t row, uint32_t column):Chess(row, column){
}

Ma::Ma(uint32_t chess, uint32_t country, uint32_t row, uint32_t column):Chess(chess, country, row, column){
}

Ma::Ma(uint32_t chess, uint32_t country, uint32_t fontIndex, uint32_t row, uint32_t column):Chess(chess, country, fontIndex, row, column){
}

Ma::~Ma(){
}

void Ma::Selected(Chess *pChess[MAX_COUNTRY_INDEX][DRAW_COUNTRY_CHESS_COUNT], std::vector<Chess> &canplays) const{
    const Chess cInfo[] = {
        Chess(GetRow() - 1, GetColumn() - 2), Chess(GetRow() - 2, GetColumn() - 1), Chess(GetRow() - 2, GetColumn() + 1), Chess(GetRow() - 1, GetColumn() + 2),
        Chess(GetRow() + 1, GetColumn() + 2), Chess(GetRow() + 2, GetColumn() + 1), Chess(GetRow() + 2, GetColumn() - 1), Chess(GetRow() + 1, GetColumn() - 2)
    };
    for(uint32_t i = 0; i < sizeof(cInfo) / sizeof(Chess); ++i){
        if(!IsBoundary(cInfo[i].GetRow(), cInfo[i].GetColumn())){//这里的马和相不会被"蹩马腿",所以不需要判断
            const Chess *pc = ::GetChess(cInfo[i].GetRow(), cInfo[i].GetColumn(), pChess);
            if(!pc || pc->GetCountry() != GetCountry())
                canplays.push_back(Chess(cInfo[i].GetRow(), cInfo[i].GetColumn()));
        }
    }
}

Xiang::Xiang(){
}

bool Xiang::IsAbroad(uint32_t row, uint32_t column) const{
    return mTerriory != GetTerritoryIndex(row, column);
}

Xiang::Xiang(uint32_t row, uint32_t column) : Chess(row, column){
    mTerriory = GetTerritoryIndex(row, column);
}

Xiang::Xiang(uint32_t chess, uint32_t country, uint32_t row, uint32_t column):Chess(chess, country, row, column){
    mTerriory = GetTerritoryIndex(row, column);
}

Xiang::Xiang(uint32_t chess, uint32_t country, uint32_t fontIndex, uint32_t row, uint32_t column):Chess(chess, country, fontIndex, row, column){
    mTerriory = GetTerritoryIndex(row, column);
}

Xiang::~Xiang(){
}

void Xiang::Selected(Chess *pChess[MAX_COUNTRY_INDEX][DRAW_COUNTRY_CHESS_COUNT], std::vector<Chess> &canplays) const{
    const Chess cInfo[] = { Chess(GetRow() + 2, GetColumn() + 2), Chess(GetRow() - 2, GetColumn() - 2), Chess(GetRow() - 2, GetColumn() + 2), Chess(GetRow() + 2, GetColumn() - 2) };
    for(uint32_t i = 0; i < sizeof(cInfo) / sizeof(Chess); ++i){
        if(!IsBoundary(cInfo[i].GetRow(), cInfo[i].GetColumn()) && !IsAbroad(cInfo[i].GetRow(), cInfo[i].GetColumn())){
            const Chess *pc = ::GetChess(cInfo[i].GetRow(), cInfo[i].GetColumn(), pChess);
            if(!pc || pc->GetColumn() != GetCountry())
                canplays.push_back(Chess(cInfo[i].GetRow(), cInfo[i].GetColumn()));
        }
    }
}

Shi::Shi(){
}

Shi::Shi(uint32_t row, uint32_t column):Chess(row, column){
}

Shi::Shi(uint32_t chess, uint32_t country, uint32_t row, uint32_t column):Chess(chess, country, row, column){
}

Shi::Shi(uint32_t chess, uint32_t country, uint32_t fontIndex, uint32_t row, uint32_t column):Chess(chess, country, fontIndex, row, column){
}

Shi::~Shi(){
}

void Shi::Selected(Chess *pChess[MAX_COUNTRY_INDEX][DRAW_COUNTRY_CHESS_COUNT], std::vector<Chess> &canplays) const{
    SelectChessInPalace(GetCountry(), GetRow(), GetColumn(), pChess, canplays);
}
