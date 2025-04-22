#include "Chess.h"
#include <string.h>
#include <iostream>
#include "Chessboard.h"
uint32_t GetTerritoryIndex(uint32_t row, uint32_t column){
    //4是一个国家占的行数
    if(row <= CHESSBOARD_ROW - 4 && row > 3){
        if(column >= CHESSBOARD_COLUMN - 4){
            return WU_TERRITORY_INDEX;
        }
        else if(column < 5){
            return HAN_TERRITORY_INDEX;
        }
        else{
            return CENTER_TERRITORY_INDEX;
        }
    }
    else if(column > 3 && column <= CHESSBOARD_COLUMN - 4){
        //魏或蜀
        if(row > CHESSBOARD_COLUMN - 4){
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
// //返回所在九宫格的势力
// uint32_t IsPalaceCenter(uint32_t row, uint32_t column){
//     uint32_t country = MAX_COUNTRY_INDEX;
//     for (size_t i = 0; i < MAX_COUNTRY_INDEX; ++i){
//         if(row == g_Center[i].GetRow() && column == g_Center[i].GetColumn()){
//             // printf("%d国,%d行%d列的棋子:%d, 在中心为%d行%d列的九宫格内\n", mInfo.country, row, column, mInfo.chess, g_Center[i].GetRow(), g_Center[i].GetColumn());
//             country = i;
//             break;
//         }   
//     }
//     return country;
// }
// uint32_t IsInPalace(uint32_t row, uint32_t column){
//     uint32_t country = IsPalaceCenter(row, column);
//     if(country != MAX_COUNTRY_INDEX)return country;
//     for (size_t i = 0; i < MAX_COUNTRY_INDEX; ++i){
//         if(abs((float)row - g_Center[i].GetRow()) < 2 && abs((float)column - g_Center[i].GetColumn()) < 2){
//             country = i;
//             break;
//         }
//     }
//     return country;
// }
// void SelectChessInPalace(uint32_t currentCountry, uint32_t row, uint32_t column, const void *pBoard, std::vector<glm::vec2>&canplays){
//     //只需要管斜线，其他位置归棋子子类管
//     const Chess *pc = nullptr;
//     uint32_t country = IsPalaceCenter(row, column);
//     if(country != MAX_COUNTRY_INDEX){
//         const Chess pos[] = {
//             Chess(g_Center[country].GetRow() - 1, g_Center[country].GetColumn() - 1),
//             Chess(g_Center[country].GetRow() - 1, g_Center[country].GetColumn() + 1),
//             Chess(g_Center[country].GetRow() + 1, g_Center[country].GetColumn() - 1),
//             Chess(g_Center[country].GetRow() + 1, g_Center[country].GetColumn() + 1)
//         };
//         //除馬、相外, 其他棋子在中心点时，能走的肯定就是斜线边的点
//         for (size_t i = 0; i < sizeof(pos) / sizeof(Chess); ++i){
//             pc = GetChess(pos[i].GetRow(), pos[i].GetColumn(), pChess);
//             if(!pc || pc->GetCountry() != currentCountry){
//                 canplays.push_back(pos[i]);
//             }
//         }
//     }
//     else if((country = IsInPalace(row, column)) != MAX_COUNTRY_INDEX){
//         //在斜线边的点上时, 能走的只有中心点
//         pc = GetChess(g_Center[country].GetRow(), g_Center[country].GetColumn(), pChess);
//         if(!pc || pc->GetCountry() != currentCountry)canplays.push_back(g_Center[country]);
//     }
// }

void Chess::RemoveInvalidChess(const void *pBoard, std::vector<glm::vec2> &canplays) const{
    const Chessboard *board = (const Chessboard *)pBoard;
    for (auto it = canplays.begin(); it != canplays.end();){
        const Chess *pChess = board->GetChess(it->y, it->x);
        if(pChess && pChess->GetCountry() == mCountry){
            it = canplays.erase(it);
        }
        else
            ++it;
    }
}

void Chess::InPalaceMove(const glm::vec2 &palacesCenter, std::vector<glm::vec2> &canplays) const{
    if(mRow == palacesCenter.y && mColumn == palacesCenter.x){
        const glm::vec2 pos[] = {
            glm::vec2(palacesCenter.x - 1, palacesCenter.y - 1),
            glm::vec2(palacesCenter.x - 1, palacesCenter.y + 1),
            glm::vec2(palacesCenter.x + 1, palacesCenter.y - 1),
            glm::vec2(palacesCenter.x + 1, palacesCenter.y + 1)
        };
        //除馬、相外, 其他棋子在中心点时，能走的肯定就是斜线边的点
        for (size_t i = 0; i < sizeof(pos) / sizeof(Chess); ++i){
            canplays.push_back(pos[i]);
        }
    }
    else{
        canplays.push_back(palacesCenter);
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

void Chess::Select(const void *pBoard, std::vector<glm::vec2>&canplays) const{
    printf("in function %s\n", __FUNCTION__);
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

void Wei::Select(const void *pBoard, std::vector<glm::vec2>&canplays)const{
    const Chessboard *board = (const Chessboard *)pBoard;
    const glm::vec2 cInfo[] = { glm::vec2(mColumn + 1, mRow), glm::vec2(mColumn - 1, mRow), glm::vec2(mColumn, mRow + 1), glm::vec2(mColumn, mRow - 1) };
    for(uint32_t i = 0; i < sizeof(cInfo) / sizeof(glm::vec2); ++i){
        if(!IsBoundary(cInfo[i].y, cInfo[i].x) && 4 !=  board->IsInPalace(cInfo[i].y, cInfo[i].x)){//将不能出九宫格
            canplays.push_back(glm::vec2(cInfo[i].x, cInfo[i].y));
        }
    }
    const glm::vec2&center = board->GetPalacesCenter(mCountry);
    InPalaceMove(center, canplays);
    RemoveInvalidChess(pBoard, canplays);
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

void Shu::Select(const void *pBoard, std::vector<glm::vec2>&canplays) const{
    const Chessboard *board = (const Chessboard *)pBoard;
    const glm::vec2 cInfo[] = { glm::vec2(mColumn + 1, mRow), glm::vec2(mColumn - 1, mRow), glm::vec2(mColumn, mRow + 1), glm::vec2(mColumn, mRow - 1) };
    for(uint32_t i = 0; i < sizeof(cInfo) / sizeof(glm::vec2); ++i){
        if(!IsBoundary(cInfo[i].y, cInfo[i].x) && 4 !=  board->IsInPalace(cInfo[i].y, cInfo[i].x)){//将不能出九宫格
            canplays.push_back(glm::vec2(cInfo[i].x, cInfo[i].y));
        }
    }
    const glm::vec2&center = board->GetPalacesCenter(mCountry);
    InPalaceMove(center, canplays);
    RemoveInvalidChess(pBoard, canplays);
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

void Wu::Select(const void *pBoard, std::vector<glm::vec2>&canplays) const{
    const Chessboard *board = (const Chessboard *)pBoard;
    const glm::vec2 cInfo[] = { glm::vec2(mColumn + 1, mRow), glm::vec2(mColumn - 1, mRow), glm::vec2(mColumn, mRow + 1), glm::vec2(mColumn, mRow - 1) };
    for(uint32_t i = 0; i < sizeof(cInfo) / sizeof(glm::vec2); ++i){
        if(!IsBoundary(cInfo[i].y, cInfo[i].x) && 4 !=  board->IsInPalace(cInfo[i].y, cInfo[i].x)){//将不能出九宫格
            canplays.push_back(glm::vec2(cInfo[i].x, cInfo[i].y));
        }
    }
    const glm::vec2&center = board->GetPalacesCenter(mCountry);
    InPalaceMove(center, canplays);
    RemoveInvalidChess(pBoard, canplays);
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

void Han::Select(const void *pBoard, std::vector<glm::vec2>&canplays) const{
    const Chessboard *board = (const Chessboard *)pBoard;
    const glm::vec2 cInfo[] = { glm::vec2(mColumn + 1, mRow), glm::vec2(mColumn - 1, mRow), glm::vec2(mColumn, mRow + 1), glm::vec2(mColumn, mRow - 1) };
    for(uint32_t i = 0; i < sizeof(cInfo) / sizeof(glm::vec2); ++i){
        if(!IsBoundary(cInfo[i].y, cInfo[i].x) && 4 !=  board->IsInPalace(cInfo[i].y, cInfo[i].x)){//将不能出九宫格
            canplays.push_back(glm::vec2(cInfo[i].x, cInfo[i].y));
        }
    }
    const glm::vec2&center = board->GetPalacesCenter(mCountry);
    InPalaceMove(center, canplays);
    RemoveInvalidChess(pBoard, canplays);
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

void Bing::Select(const void *pBoard, std::vector<glm::vec2>&canplays) const{
    //兵在自身境内不能后退
    const glm::vec2 back = GetBingBack();
    const bool bAbroad = IsAbroad(mRow, mColumn);
    const glm::vec2 cInfo[] = { glm::vec2(0, 1), glm::vec2(0, -1), glm::vec2(1, 0), glm::vec2(-1, 0) };
    for(uint32_t i = 0; i < sizeof(cInfo) / sizeof(glm::vec2); ++i){
        if(!bAbroad && cInfo[i].x == back.x && cInfo[i].y == back.y)continue;
        const glm::vec2 currentPos = glm::vec2(mColumn, mRow) + cInfo[i];
        if(!IsBoundary(currentPos.y, currentPos.x)){
            canplays.push_back(currentPos);
        }
    }
    const Chessboard *board = (const Chessboard *)pBoard;
    const uint32_t inPalaceCountry = board->IsInPalace(mRow, mColumn);
    if(inPalaceCountry != -1)InPalaceMove(board->GetPalacesCenter(inPalaceCountry), canplays);
    RemoveInvalidChess(pBoard, canplays);
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

void Pao::Select(const void *pBoard, std::vector<glm::vec2>&canplays) const{
    const Chessboard *board = (const Chessboard *)pBoard;
    const glm::vec2 direction[] = { glm::vec2(1, 0), glm::vec2(-1, 0), glm::vec2(0, 1), glm::vec2(0, -1) };
    for(uint32_t i = 0; i < sizeof(direction) / sizeof(glm::vec2); ++i){
        const glm::vec2 currentPos = glm::vec2(mColumn, mRow);
        glm::vec2 pos = currentPos + direction[i];
        while(!IsBoundary(pos.y, pos.x)){
            const Chess *pChessInfo = board->GetChess(pos.y, pos.x);
            if(pChessInfo){
                pos += direction[i];
                while(!IsBoundary(pos.y, pos.x)){
                    pChessInfo = board->GetChess(pos.y, pos.x);
                    if(pChessInfo && pChessInfo->GetCountry() != mCountry){
                        canplays.push_back(pos);
                        break;
                    }
                    pos += direction[i];
                }
                break;
            }
            else{
                canplays.push_back(pos);
            }
            pos += direction[i];
        }
    }
    const uint32_t inPalaceCountry = board->IsInPalace(mRow, mColumn);
    if(inPalaceCountry != -1)InPalaceMove(board->GetPalacesCenter(inPalaceCountry), canplays);
    RemoveInvalidChess(pBoard, canplays);
}

// float Che::determinant(float *m, float column){
//     if(column > 4){
//         const uint32_t rColumn = column - 1;
//         float *r = new float[rColumn * rColumn];
//         float result = 0, rd = -1;
//         for (size_t i = 0; i < column; i++){
//             RemoveRowColumn(m, i + 1, column, r);
//             result +=  m[i] * determinant(m, rColumn) * rd;
//             rd *= -1;
//         }
// 		return result;
//     }
//     else{
//         glm::mat4 r;
//         floatToMatrix(m, r);
//         return glm::determinant(r);
//     }
// }
// void Che::floatToMatrix(const float *m, glm::mat4 &r){
//     for (uint32_t uiRow = 0; uiRow < 4; ++uiRow){
//         for (uint32_t uiColumn = 0; uiColumn < 4; ++uiColumn){
//             r[uiRow][uiColumn] = m[uiRow * 4 + uiColumn];
//         }
//     }
// }

// void Che::RemoveRowColumn(const float *m, uint32_t removeColumn, uint32_t column, float *r){
// 	const uint32_t rColumn = column - 1;
//     if(removeColumn == 1){
//         for (uint32_t uiRow = 0; uiRow < rColumn; ++uiRow){
//             memcpy(r + uiRow * rColumn, m + ((uiRow + 1) * column + removeColumn), sizeof(float) * (rColumn));
//         } 
//     }
//     else if (removeColumn == column){
//         for (uint32_t uiRow = 0; uiRow < rColumn; ++uiRow){
//             memcpy(r + uiRow * rColumn, m + (uiRow + 1) * column, sizeof(float) * (rColumn));
//         } 
//     }
//     else{
//         for (uint32_t uiRow = 0; uiRow < rColumn; ++uiRow){
//             memcpy(r + uiRow * rColumn, m + (uiRow + 1) * column, sizeof(float) * (removeColumn - 1));
//             memcpy(r + (uiRow * rColumn + removeColumn), m + ((uiRow + 1) * column + removeColumn + 1), sizeof(float) * (column - removeColumn));
//         }
//     }
// }

// void Che::SetMatrix(Chess **pChess, float m[CHESSBOARD_ROW * CHESSBOARD_COLUMN]){
//     //先设置为单位矩阵，再根据棋盘赋值
//     // auto r = glm::determinant(m[0][0]);
// }

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

void Che::Select(const void *pBoard, std::vector<glm::vec2>&canplays) const{
    const Chessboard *board = (const Chessboard *)pBoard;
    const glm::vec2 direction[] = { glm::vec2(1, 0), glm::vec2(-1, 0), glm::vec2(0, 1), glm::vec2(0, -1) };
    for(uint32_t i = 0; i < sizeof(direction) / sizeof(glm::vec2); ++i){
        const glm::vec2 currentPos = glm::vec2(mColumn, mRow);
        glm::vec2 pos = currentPos + direction[i];
        while(!IsBoundary(pos.y, pos.x)){
            const Chess *pc = board->GetChess(pos.y, pos.x);
            if(pc){
                if(pc->GetCountry() != GetCountry()){
                    canplays.push_back(pos);
                }
                break;
            }
            else{
                canplays.push_back(pos);
            }
            pos += direction[i];
        }
    }
    const uint32_t inPalaceCountry = board->IsInPalace(mRow, mColumn);
    if(inPalaceCountry != -1)InPalaceMove(board->GetPalacesCenter(inPalaceCountry), canplays);
    RemoveInvalidChess(pBoard, canplays);
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

void Ma::Select(const void *pBoard, std::vector<glm::vec2>&canplays) const{
    const glm::vec2 cInfo[] = {
        glm::vec2(mColumn - 1, mRow - 2), glm::vec2(mColumn - 2, mRow - 1), glm::vec2(mColumn - 2, mRow + 1), glm::vec2(mColumn - 1, mRow + 2),
        glm::vec2(mColumn + 1, mRow + 2), glm::vec2(mColumn + 2, mRow + 1), glm::vec2(mColumn + 2, mRow - 1), glm::vec2(mColumn + 1, mRow - 2)
    };
    const Chessboard *board = (const Chessboard *)pBoard;
    for(uint32_t i = 0; i < sizeof(cInfo) / sizeof(glm::vec2); ++i){
        if(!IsBoundary(cInfo[i].y, cInfo[i].x)){//这里的马和相不会被"蹩马腿",所以不需要判断
            const Chess *pc = board->GetChess(cInfo[i].y, cInfo[i].x);
            if(!pc || pc->GetCountry() != GetCountry())
                canplays.push_back(cInfo[i]);
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

void Xiang::Select(const void *pBoard, std::vector<glm::vec2>&canplays) const{
    const Chessboard *board = (const Chessboard *)pBoard;
    const glm::vec2 cInfo[] = { glm::vec2(mColumn + 2, mRow+ 2), glm::vec2(mColumn - 2, mRow - 2), glm::vec2(mColumn + 2, mRow - 2), glm::vec2(mColumn - 2,mRow + 2) };
    for(uint32_t i = 0; i < sizeof(cInfo) / sizeof(glm::vec2); ++i){
        if(!IsBoundary(cInfo[i].y, cInfo[i].x) && !IsAbroad(cInfo[i].y, cInfo[i].x)){
            const Chess *pc = board->GetChess(cInfo[i].y, cInfo[i].x);
            if(!pc || pc->GetColumn() != GetCountry())
                canplays.push_back(cInfo[i]);
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

void Shi::Select(const void *pBoard, std::vector<glm::vec2>&canplays) const{
    const Chessboard *board = (const Chessboard *)pBoard;
    InPalaceMove(board->GetPalacesCenter(mCountry), canplays);
    RemoveInvalidChess(pBoard, canplays);
}