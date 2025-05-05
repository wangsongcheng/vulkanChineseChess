#include "Chess.h"
#include <string.h>
#include <iostream>
#include "Chessboard.h"
#include "VulkanChess.h"
uint32_t Chess::GetTerritoryIndex()const{
    return GetTerritoryIndex(mRow, mColumn);
}
uint32_t Chess::GetTerritoryIndex(uint32_t row, uint32_t column)const{
    if(row > CHESSBOARD_BING_GRID_DENSITY && row < CHESSBOARD_ROW - CHESSBOARD_BING_GRID_DENSITY){
        //汉和吴的地盘
        if(column <= CHESSBOARD_BING_GRID_DENSITY){
            return HAN_TERRITORY_INDEX;
        }
        else if(column >= CHESSBOARD_COLUMN - CHESSBOARD_BING_GRID_DENSITY){
            return WU_TERRITORY_INDEX;
        }
        else{
            return CENTER_TERRITORY_INDEX;
        }
    }
    else if(column > CHESSBOARD_BING_GRID_DENSITY && column < CHESSBOARD_COLUMN - CHESSBOARD_BING_GRID_DENSITY){
        //魏和蜀的地盘
        if(row <= CHESSBOARD_BING_GRID_DENSITY){
            return WEI_TERRITORY_INDEX;
        }
        else if(row >= CHESSBOARD_ROW - CHESSBOARD_BING_GRID_DENSITY){
            return SHU_TERRITORY_INDEX;
        }
        else{
            return CENTER_TERRITORY_INDEX;
        }
    }
    return INVALID_TERRITORY_INDEX;
}
bool IsBoundary(int32_t row, int32_t column){
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

Chess::Chess(/* args */){
}

Chess::Chess(uint32_t country, Type chess, uint32_t fontIndex, bool hasExitPermission){
    SetChess(chess);
    SetCountry(country);
    SetFontIndex(fontIndex);
    mHasExitPermission = hasExitPermission;
}

Chess::Chess(uint32_t country, Type chess, uint32_t fontIndex, uint32_t row, uint32_t column, bool hasExitPermission){
    SetChess(chess);
    SetPos(row, column);
    SetCountry(country);
    SetFontIndex(fontIndex);
    mHasExitPermission = hasExitPermission;
}
Chess::~Chess(){
}

void Chess::SetPos(uint32_t row, uint32_t column){
    mRow = row;
    mColumn = column;
    mPos.y = CHESS_ROW_TO_Y(mRow);
    mPos.x = CHESS_COLUMN_TO_X(mColumn);
    if(INVALID_TERRITORY_INDEX == mTerritory)mTerritory = GetTerritoryIndex(row, column);
}

void Chess::Select(const void *pBoard, std::vector<glm::vec2>&canplays) const{
    printf("in function %s\n", __FUNCTION__);
}

Wei::Wei():Chess(WEI_COUNTRY_INDEX, Jiang_Chess, FONT_INDEX_WEI, false){
}
Wei::Wei(uint32_t row, uint32_t column):Chess(WEI_COUNTRY_INDEX, Jiang_Chess, FONT_INDEX_WEI, row, column, false){
}


Wei::~Wei(){
}

void Wei::Select(const void *pBoard, std::vector<glm::vec2>&canplays)const{
    const Chessboard *board = (const Chessboard *)pBoard;
    const glm::vec2 cInfo[] = { glm::vec2(mColumn + 1, mRow), glm::vec2(mColumn - 1, mRow), glm::vec2(mColumn, mRow + 1), glm::vec2(mColumn, mRow - 1) };
    for(uint32_t i = 0; i < sizeof(cInfo) / sizeof(glm::vec2); ++i){
        if(!IsBoundary(cInfo[i].y, cInfo[i].x) && mTerritory ==  board->IsInPalace(cInfo[i].y, cInfo[i].x)){//将不能出九宫格
            canplays.push_back(glm::vec2(cInfo[i].x, cInfo[i].y));
        }
    }
    RemoveInvalidChess(pBoard, canplays);}

Shu::Shu():Chess(SHU_COUNTRY_INDEX, Jiang_Chess, FONT_INDEX_SHU, false){
}

Shu::Shu(uint32_t row, uint32_t column):Chess(SHU_COUNTRY_INDEX, Jiang_Chess, FONT_INDEX_SHU, row, column, false){
}

Shu::~Shu(){
}

void Shu::Select(const void *pBoard, std::vector<glm::vec2>&canplays) const{
    const Chessboard *board = (const Chessboard *)pBoard;
    const glm::vec2 cInfo[] = { glm::vec2(mColumn + 1, mRow), glm::vec2(mColumn - 1, mRow), glm::vec2(mColumn, mRow + 1), glm::vec2(mColumn, mRow - 1) };
    for(uint32_t i = 0; i < sizeof(cInfo) / sizeof(glm::vec2); ++i){
        if(!IsBoundary(cInfo[i].y, cInfo[i].x) && mTerritory ==  board->IsInPalace(cInfo[i].y, cInfo[i].x)){//将不能出九宫格
            canplays.push_back(glm::vec2(cInfo[i].x, cInfo[i].y));
        }
    }
    RemoveInvalidChess(pBoard, canplays);
}

Wu::Wu():Chess(WU_COUNTRY_INDEX, Jiang_Chess, FONT_INDEX_WU, false){
}

Wu::Wu(uint32_t row, uint32_t column):Chess(WU_COUNTRY_INDEX, Jiang_Chess, FONT_INDEX_WU,row, column, false){
}

Wu::~Wu(){
}

void Wu::Select(const void *pBoard, std::vector<glm::vec2>&canplays) const{
    const Chessboard *board = (const Chessboard *)pBoard;
    const glm::vec2 cInfo[] = { glm::vec2(mColumn + 1, mRow), glm::vec2(mColumn - 1, mRow), glm::vec2(mColumn, mRow + 1), glm::vec2(mColumn, mRow - 1) };
    for(uint32_t i = 0; i < sizeof(cInfo) / sizeof(glm::vec2); ++i){
        if(!IsBoundary(cInfo[i].y, cInfo[i].x) && mTerritory ==  board->IsInPalace(cInfo[i].y, cInfo[i].x)){//将不能出九宫格
            canplays.push_back(glm::vec2(cInfo[i].x, cInfo[i].y));
        }
    }
    RemoveInvalidChess(pBoard, canplays);
}

Han::Han():Chess(HAN_COUNTRY_INDEX, Jiang_Chess, FONT_INDEX_HAN, false){
}

Han::Han(uint32_t row, uint32_t column):Chess(HAN_COUNTRY_INDEX, Jiang_Chess, FONT_INDEX_WU,row, column, false){
}

Han::~Han(){
}

void Han::Select(const void *pBoard, std::vector<glm::vec2>&canplays) const{
    const Chessboard *board = (const Chessboard *)pBoard;
    const glm::vec2 cInfo[] = { glm::vec2(mColumn + 1, mRow), glm::vec2(mColumn - 1, mRow), glm::vec2(mColumn, mRow + 1), glm::vec2(mColumn, mRow - 1) };
    for(uint32_t i = 0; i < sizeof(cInfo) / sizeof(glm::vec2); ++i){
        if(!IsBoundary(cInfo[i].y, cInfo[i].x) && mTerritory ==  board->IsInPalace(cInfo[i].y, cInfo[i].x)){//将不能出九宫格
            canplays.push_back(glm::vec2(cInfo[i].x, cInfo[i].y));
        }
    }
    RemoveInvalidChess(pBoard, canplays);
}

glm::vec2 Bing::GetBingBack() const{
    glm::vec2 back = glm::vec2(0);
    if(mTerritory == WU_TERRITORY_INDEX){
        back.x = 1;
    }
    else if(mTerritory == WEI_TERRITORY_INDEX){
        back.y = -1;
    }
    else if(mTerritory == SHU_TERRITORY_INDEX){
        back.y = 1;
    }
    else if(mTerritory == HAN_TERRITORY_INDEX){
        back.x = -1;
    }
    return back;
}

bool Bing::IsAbroad(uint32_t row, uint32_t column) const{
    return mTerritory != GetTerritoryIndex(row, column);
}

Bing::Bing(){
}

Bing::Bing(uint32_t country):Chess(country, Bing_Chess, FONT_INDEX_BING, true){
}

Bing::Bing(uint32_t country, uint32_t row, uint32_t column):Chess(country, Bing_Chess, FONT_INDEX_BING, row, column, true){
    mTerritory = GetTerritoryIndex(row, column);
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
    RemoveInvalidChess(pBoard, canplays);
}

// void Pao::InPalaceMove(const void *pBoard, std::vector<glm::vec2> &canplays) const{
//     const Chessboard *board = (const Chessboard *)pBoard;
//     const int32_t territory = board->IsInPalace(mRow, mColumn);
//     if(territory != INVALID_TERRITORY_INDEX && territory != CENTER_TERRITORY_INDEX){
//         const auto&center = board->GetPalacesCenter(territory);
//         if(mRow == center.y && mColumn == center.x){
//             const glm::vec2 pos[] = {
//                 glm::vec2(center.x - 1, center.y - 1),
//                 glm::vec2(center.x - 1, center.y + 1),
//                 glm::vec2(center.x + 1, center.y - 1),
//                 glm::vec2(center.x + 1, center.y + 1)
//             };
//             for (size_t i = 0; i < sizeof(pos) / sizeof(glm::vec2); ++i){
//                 //在中心的话,炮就丧失吃子能力//这里只处理斜边, 其他位置在函数外获得
//                 if(!board->GetChess(pos[i].y, pos[i].x))canplays.push_back(pos[i]);
//             }
//         }
//         else{
//             //炮不一样的地方在于:不在中心也有可能直接跳到斜对面
//             if(board->GetChess(center.y, center.x)){
//                 const glm::vec2 pao = glm::vec2(mColumn, mRow);
//                 const glm::vec2 dir = center - pao;
//                 const glm::vec2 pos = pao + dir * 2.0f;
//                 const Chess *pChess = board->GetChess(pos.y, pos.x);
//                 if(pChess && pChess->GetCountry() != mCountry)canplays.push_back(pos);
//             }
//             else{
//                 canplays.push_back(center);
//             }
//         }
//     }
// }

Pao::Pao(){
}

Pao::Pao(uint32_t country):Chess(country, Pao_Chess, FONT_INDEX_PAO, true){
}

Pao::Pao(uint32_t country, uint32_t row, uint32_t column):Chess(country, Pao_Chess, FONT_INDEX_PAO, row, column, true){
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
            const Chess *pChess = board->GetChess(pos.y, pos.x);
            if(pChess){
                pos += direction[i];
                while(!IsBoundary(pos.y, pos.x)){
                    const Chess *pTarget = board->GetChess(pos.y, pos.x);
                    if(pTarget){
                        if(pTarget->GetCountry() != mCountry)canplays.push_back(pos);
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
    // InPalaceMove(pBoard, canplays);
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

// void Che::SetMatrix(Chess **pChess, float m[CHESSBOARD_ROW * MAX_CHESSBOARD_COLUMN]){
//     //先设置为单位矩阵，再根据棋盘赋值
//     // auto r = glm::determinant(m[0][0]);
// }

Che::Che(){
}

Che::Che(uint32_t country):Chess(country, Che_Chess, FONT_INDEX_CHE, true){
}

Che::Che(uint32_t country, uint32_t row, uint32_t column):Chess(country, Che_Chess, FONT_INDEX_CHE, row, column, true){
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
    RemoveInvalidChess(pBoard, canplays);
}

Ma::Ma(){
}

Ma::Ma(uint32_t country):Chess(country, Ma_Chess, FONT_INDEX_MA, true){
}

Ma::Ma(uint32_t country, uint32_t row, uint32_t column):Chess(country, Ma_Chess, FONT_INDEX_MA, row, column, true){
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

Xiang::Xiang(uint32_t country):Chess(country, Xiang_Chess, FONT_INDEX_XIANG, false){
}

bool Xiang::IsAbroad(uint32_t row, uint32_t column) const{
    return mTerritory != GetTerritoryIndex(row, column);
}

Xiang::Xiang(uint32_t country, uint32_t row, uint32_t column):Chess(country, Xiang_Chess, FONT_INDEX_XIANG, row, column, false){
    mTerritory = GetTerritoryIndex(row, column);
}

Xiang::~Xiang(){
}

void Xiang::Select(const void *pBoard, std::vector<glm::vec2>&canplays) const{
    const Chessboard *board = (const Chessboard *)pBoard;
    const glm::vec2 cInfo[] = { glm::vec2(mColumn + 2, mRow + 2), glm::vec2(mColumn - 2, mRow - 2), glm::vec2(mColumn + 2, mRow - 2), glm::vec2(mColumn - 2,mRow + 2) };
    for(uint32_t i = 0; i < sizeof(cInfo) / sizeof(glm::vec2); ++i){
        if(!IsBoundary(cInfo[i].y, cInfo[i].x) && !IsAbroad(cInfo[i].y, cInfo[i].x)){
            const Chess *pc = board->GetChess(cInfo[i].y, cInfo[i].x);
            if(!pc || pc->GetCountry() != mCountry){
                canplays.push_back(cInfo[i]);
            }
        }
    }
}
void Shi::InPalaceMove(const void *pBoard, std::vector<glm::vec2> &canplays) const{
    const Chessboard *board = (const Chessboard *)pBoard;
    int32_t territory = board->IsInPalace(mRow, mColumn);
    if(territory != INVALID_TERRITORY_INDEX && territory != CENTER_TERRITORY_INDEX){
        const auto center = board->GetPalacesCenter(territory);
        if(mRow == center.y && mColumn == center.x){
            const glm::vec2 pos[] = {
                glm::vec2(center.x - 1, center.y - 1),
                glm::vec2(center.x - 1, center.y + 1),
                glm::vec2(center.x + 1, center.y - 1),
                glm::vec2(center.x + 1, center.y + 1)
            };
            //除馬、相外, 其他棋子在中心点时，能走的肯定就是斜线边的点
            for (size_t i = 0; i < sizeof(pos) / sizeof(glm::vec2); ++i){
                canplays.push_back(pos[i]);
            }
        }
        else{
            canplays.push_back(center);
        }    
    }
}
Shi::Shi(){
}

Shi::Shi(uint32_t country):Chess(country, Shi_Chess, FONT_INDEX_SHI, false){
}

Shi::Shi(uint32_t country, uint32_t row, uint32_t column):Chess(country, Shi_Chess, FONT_INDEX_SHI, row, column, false){
}

Shi::~Shi(){
}

void Shi::Select(const void *pBoard, std::vector<glm::vec2>&canplays) const{
    InPalaceMove(pBoard, canplays);
    RemoveInvalidChess(pBoard, canplays);
}