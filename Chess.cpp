#include "Chess.h"
#include <string.h>
#include <iostream>
#include "Chessboard.h"
#include "VulkanChess.h"
uint32_t Chess::GetTerritoryIndex()const{
    return GetTerritoryIndex(mRow, mColumn);
}
Territory Chess::GetTerritoryIndex(uint32_t row, uint32_t column)const{
    //如果只是想优化if的判断条件, 那还是省省吧。之前的判断条件，在row = CHESSBOARD_BING_GRID_DENSITY的时候，会直接进入判断吴、汉地盘的条件
    if(row >= CHESSBOARD_BING_GRID_DENSITY && row <= CHESSBOARD_ROW - CHESSBOARD_BING_GRID_DENSITY && column <= CHESSBOARD_BING_GRID_DENSITY){
        return Han_Territory;
    }
    else if(row >= CHESSBOARD_BING_GRID_DENSITY && row <= CHESSBOARD_ROW - CHESSBOARD_BING_GRID_DENSITY && column >= CHESSBOARD_COLUMN - CHESSBOARD_BING_GRID_DENSITY){
        return Wu_Territory;
    }
    else if(row <= CHESSBOARD_BING_GRID_DENSITY && column >= CHESSBOARD_BING_GRID_DENSITY && column <= CHESSBOARD_ROW - CHESSBOARD_BING_GRID_DENSITY){
        return Wei_Territory;
    }
    else if(row >= CHESSBOARD_ROW - CHESSBOARD_BING_GRID_DENSITY){
        return Shu_Territory;
    }
    return Center_Territory;
}
void Chess::RemoveInvalidChess(const void *pBoard, std::vector<glm::vec2> &legal_moves) const{
    const Chessboard *board = (const Chessboard *)pBoard;
    for (auto it = legal_moves.begin(); it != legal_moves.end();){
        const Chess *pChess = board->GetChess(it->y, it->x);
        if(pChess && (pChess->GetCountry() == mCountry || pChess->GetCountry() == Han_Country)){
            it = legal_moves.erase(it);
        }
        else
            ++it;
    }
}

Chess::Chess(/* args */){
}

Chess::Chess(Country country, Territory territory, Type chess, uint32_t fontIndex, bool hasExitPermission){
    SetChess(chess);
    SetCountry(country);
    SetFontIndex(fontIndex);
    mTerritory = territory;
    mHasExitPermission = hasExitPermission;
}

Chess::Chess(Country country, Territory territory, Type chess, uint32_t fontIndex, uint32_t row, uint32_t column, bool hasExitPermission){
    SetChess(chess);
    SetPos(row, column);
    SetCountry(country);
    SetFontIndex(fontIndex);
    mTerritory = territory;
    mHasExitPermission = hasExitPermission;
}
Chess::~Chess(){
}

void Chess::SetPos(uint32_t row, uint32_t column){
    mRow = row;
    mColumn = column;
    mPos.y = CHESS_ROW_TO_Y(mRow);
    mPos.x = CHESS_COLUMN_TO_X(mColumn);
}

void Chess::Select(const void *pBoard, std::vector<glm::vec2>&legal_moves) const{
    printf("in function %s\n", __FUNCTION__);
}

Wei::Wei(){
}

Wei::Wei(Territory territory) : Chess(Wei_Country, territory, Jiang_Chess, FONT_INDEX_WEI, false){
}
Wei::Wei(Territory territory, uint32_t row, uint32_t column):Chess(Wei_Country, territory, Jiang_Chess, FONT_INDEX_WEI, row, column, false){
}


Wei::~Wei(){
}

void Wei::Select(const void *pBoard, std::vector<glm::vec2>&legal_moves)const{
    const Chessboard *board = (const Chessboard *)pBoard;
    const glm::vec2 cInfo[] = { glm::vec2(mColumn + 1, mRow), glm::vec2(mColumn - 1, mRow), glm::vec2(mColumn, mRow + 1), glm::vec2(mColumn, mRow - 1) };
    for(uint32_t i = 0; i < sizeof(cInfo) / sizeof(glm::vec2); ++i){
        if(!board->IsBoundary(cInfo[i].y, cInfo[i].x) && mTerritory ==  board->IsInPalace(cInfo[i].y, cInfo[i].x)){//将不能出九宫格
            legal_moves.push_back(glm::vec2(cInfo[i].x, cInfo[i].y));
        }
    }
    RemoveInvalidChess(pBoard, legal_moves);}

Shu::Shu(){
}

Shu::Shu(Territory territory):Chess(Shu_Country, territory, Jiang_Chess, FONT_INDEX_SHU, false){
}

Shu::Shu(Territory territory, uint32_t row, uint32_t column):Chess(Shu_Country, territory, Jiang_Chess, FONT_INDEX_SHU, row, column, false){
}

Shu::~Shu(){
}

void Shu::Select(const void *pBoard, std::vector<glm::vec2>&legal_moves) const{
    const Chessboard *board = (const Chessboard *)pBoard;
    const glm::vec2 cInfo[] = { glm::vec2(mColumn + 1, mRow), glm::vec2(mColumn - 1, mRow), glm::vec2(mColumn, mRow + 1), glm::vec2(mColumn, mRow - 1) };
    for(uint32_t i = 0; i < sizeof(cInfo) / sizeof(glm::vec2); ++i){
        if(!board->IsBoundary(cInfo[i].y, cInfo[i].x) && mTerritory ==  board->IsInPalace(cInfo[i].y, cInfo[i].x)){//将不能出九宫格
            legal_moves.push_back(glm::vec2(cInfo[i].x, cInfo[i].y));
        }
    }
    RemoveInvalidChess(pBoard, legal_moves);
}

Wu::Wu(){
}

Wu::Wu(Territory territory):Chess(Wu_Country, territory, Jiang_Chess, FONT_INDEX_WU, false){
}

Wu::Wu(Territory territory, uint32_t row, uint32_t column):Chess(Wu_Country, territory, Jiang_Chess, FONT_INDEX_WU,row, column, false){
}

Wu::~Wu(){
}

void Wu::Select(const void *pBoard, std::vector<glm::vec2>&legal_moves) const{
    const Chessboard *board = (const Chessboard *)pBoard;
    const glm::vec2 cInfo[] = { glm::vec2(mColumn + 1, mRow), glm::vec2(mColumn - 1, mRow), glm::vec2(mColumn, mRow + 1), glm::vec2(mColumn, mRow - 1) };
    for(uint32_t i = 0; i < sizeof(cInfo) / sizeof(glm::vec2); ++i){
        if(!board->IsBoundary(cInfo[i].y, cInfo[i].x) && mTerritory ==  board->IsInPalace(cInfo[i].y, cInfo[i].x)){//将不能出九宫格
            legal_moves.push_back(glm::vec2(cInfo[i].x, cInfo[i].y));
        }
    }
    RemoveInvalidChess(pBoard, legal_moves);
}

Han::Han(){
}

Han::Han(Territory territory):Chess(Han_Country, territory, Jiang_Chess, FONT_INDEX_HAN, false){
}

Han::Han(Territory territory, uint32_t row, uint32_t column):Chess(Han_Country, territory, Jiang_Chess, FONT_INDEX_WU,row, column, false){
}

Han::~Han(){
}

void Han::Select(const void *pBoard, std::vector<glm::vec2>&legal_moves) const{
    const Chessboard *board = (const Chessboard *)pBoard;
    const glm::vec2 cInfo[] = { glm::vec2(mColumn + 1, mRow), glm::vec2(mColumn - 1, mRow), glm::vec2(mColumn, mRow + 1), glm::vec2(mColumn, mRow - 1) };
    for(uint32_t i = 0; i < sizeof(cInfo) / sizeof(glm::vec2); ++i){
        if(!board->IsBoundary(cInfo[i].y, cInfo[i].x) && mTerritory ==  board->IsInPalace(cInfo[i].y, cInfo[i].x)){//将不能出九宫格
            legal_moves.push_back(glm::vec2(cInfo[i].x, cInfo[i].y));
        }
    }
    RemoveInvalidChess(pBoard, legal_moves);
}

glm::vec2 Bing::GetBingBack() const{
    glm::vec2 back = glm::vec2(0);
    if(mTerritory == Wu_Territory){
        back.x = 1;
    }
    else if(mTerritory == Wei_Territory){
        back.y = -1;
    }
    else if(mTerritory == Shu_Territory){
        back.y = 1;
    }
    else if(mTerritory == Han_Territory){
        back.x = -1;
    }
    return back;
}

bool Bing::IsAbroad(uint32_t row, uint32_t column) const{
    return mTerritory != GetTerritoryIndex(row, column);
}

Bing::Bing(){
}

Bing::Bing(Country country, Territory territory):Chess(country, territory, Bing_Chess, FONT_INDEX_BING, true){
}

Bing::Bing(Country country, Territory territory, uint32_t row, uint32_t column):Chess(country, territory, Bing_Chess, FONT_INDEX_BING, row, column, true){
}

Bing::~Bing(){
}

void Bing::Select(const void *pBoard, std::vector<glm::vec2>&legal_moves) const{
    //兵在自身境内不能后退
    const glm::vec2 back = GetBingBack();
    const bool bAbroad = IsAbroad(mRow, mColumn);
    const glm::vec2 cInfo[] = { glm::vec2(0, 1), glm::vec2(0, -1), glm::vec2(1, 0), glm::vec2(-1, 0) };
    for(uint32_t i = 0; i < sizeof(cInfo) / sizeof(glm::vec2); ++i){
        if(!bAbroad && cInfo[i].x == back.x && cInfo[i].y == back.y)continue;
        const glm::vec2 currentPos = glm::vec2(mColumn, mRow) + cInfo[i];
        if(!((Chessboard *)pBoard)->IsBoundary(currentPos.y, currentPos.x)){
            legal_moves.push_back(currentPos);
        }
    }
    RemoveInvalidChess(pBoard, legal_moves);
}

// void Pao::InPalaceMove(const void *pBoard, std::vector<glm::vec2> &legal_moves) const{
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
//                 if(!board->GetChess(pos[i].y, pos[i].x))legal_moves.push_back(pos[i]);
//             }
//         }
//         else{
//             //炮不一样的地方在于:不在中心也有可能直接跳到斜对面
//             if(board->GetChess(center.y, center.x)){
//                 const glm::vec2 pao = glm::vec2(mColumn, mRow);
//                 const glm::vec2 dir = center - pao;
//                 const glm::vec2 pos = pao + dir * 2.0f;
//                 const Chess *pChess = board->GetChess(pos.y, pos.x);
//                 if(pChess && pChess->GetCountry() != mCountry)legal_moves.push_back(pos);
//             }
//             else{
//                 legal_moves.push_back(center);
//             }
//         }
//     }
// }

Pao::Pao(){
}

Pao::Pao(Country country, Territory territory):Chess(country, territory, Pao_Chess, FONT_INDEX_PAO, true){
}

Pao::Pao(Country country, Territory territory, uint32_t row, uint32_t column):Chess(country, territory, Pao_Chess, FONT_INDEX_PAO, row, column, true){
}

Pao::~Pao(){
}

void Pao::Select(const void *pBoard, std::vector<glm::vec2>&legal_moves) const{
    const Chessboard *board = (const Chessboard *)pBoard;
    const glm::vec2 direction[] = { glm::vec2(1, 0), glm::vec2(-1, 0), glm::vec2(0, 1), glm::vec2(0, -1) };
    for(uint32_t i = 0; i < sizeof(direction) / sizeof(glm::vec2); ++i){
        const glm::vec2 currentPos = glm::vec2(mColumn, mRow);
        glm::vec2 pos = currentPos + direction[i];
        while(!board->IsBoundary(pos.y, pos.x)){
            const Chess *pChess = board->GetChess(pos.y, pos.x);
            if(pChess){
                pos += direction[i];
                while(!board->IsBoundary(pos.y, pos.x)){
                    const Chess *pTarget = board->GetChess(pos.y, pos.x);
                    if(pTarget){
                        if(pTarget->GetCountry() != mCountry)legal_moves.push_back(pos);
                        break;
                    }
                    pos += direction[i];
                }
                break;
            }
            else{
                legal_moves.push_back(pos);
            }
            pos += direction[i];
        }
    }
    // InPalaceMove(pBoard, legal_moves);
    RemoveInvalidChess(pBoard, legal_moves);
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

Che::Che(Country country, Territory territory):Chess(country, territory, Che_Chess, FONT_INDEX_CHE, true){
}

Che::Che(Country country, Territory territory, uint32_t row, uint32_t column):Chess(country, territory, Che_Chess, FONT_INDEX_CHE, row, column, true){
}

Che::~Che(){
}

void Che::Select(const void *pBoard, std::vector<glm::vec2>&legal_moves) const{
    const Chessboard *board = (const Chessboard *)pBoard;
    const glm::vec2 direction[] = { glm::vec2(1, 0), glm::vec2(-1, 0), glm::vec2(0, 1), glm::vec2(0, -1) };
    for(uint32_t i = 0; i < sizeof(direction) / sizeof(glm::vec2); ++i){
        const glm::vec2 currentPos = glm::vec2(mColumn, mRow);
        glm::vec2 pos = currentPos + direction[i];
        while(!board->IsBoundary(pos.y, pos.x)){
            const Chess *pc = board->GetChess(pos.y, pos.x);
            if(pc){
                if(pc->GetCountry() != GetCountry()){
                    legal_moves.push_back(pos);
                }
                break;
            }
            else{
                legal_moves.push_back(pos);
            }
            pos += direction[i];
        }
    }
    RemoveInvalidChess(pBoard, legal_moves);
}
void Ma::RemoveInvalidChess(const void *pBoard, std::vector<glm::vec2>&legal_moves)const{
    const Chessboard *board = (const Chessboard *)pBoard;
    for (auto it = legal_moves.begin(); it != legal_moves.end();){
        const Chess *pChess = board->GetChess(it->y, it->x);
        if(pChess && (pChess->GetCountry() == mCountry || (pChess->GetCountry() == Han_Country && pChess->GetChess() != Chess::Type::Jiang_Chess))){
            it = legal_moves.erase(it);
        }
        else
            ++it;
    }
}
Ma::Ma(){
}

Ma::Ma(Country country, Territory territory):Chess(country, territory, Ma_Chess, FONT_INDEX_MA, true){
}

Ma::Ma(Country country, Territory territory, uint32_t row, uint32_t column):Chess(country, territory, Ma_Chess, FONT_INDEX_MA, row, column, true){
}

Ma::~Ma(){
}

void Ma::Select(const void *pBoard, std::vector<glm::vec2>&legal_moves) const{
    const Chessboard *board = (const Chessboard *)pBoard;
    const glm::vec2 cInfo[] = {
        glm::vec2(mColumn - 1, mRow - 2), glm::vec2(mColumn - 2, mRow - 1), glm::vec2(mColumn - 2, mRow + 1), glm::vec2(mColumn - 1, mRow + 2),
        glm::vec2(mColumn + 1, mRow + 2), glm::vec2(mColumn + 2, mRow + 1), glm::vec2(mColumn + 2, mRow - 1), glm::vec2(mColumn + 1, mRow - 2)
    };
    //用来判断某个点有没有在边界上//要判断是否"蹩马腿"也可以用类似的办法，只需要换个方向
    const glm::vec2 bound[] = {
        glm::vec2(mColumn, mRow - 2), glm::vec2(mColumn - 2 , mRow), glm::vec2(mColumn - 2, mRow), glm::vec2(mColumn, mRow + 2),
        glm::vec2(mColumn, mRow + 2), glm::vec2(mColumn + 2, mRow), glm::vec2(mColumn + 2, mRow), glm::vec2(mColumn, mRow - 2)
    };
    const glm::vec2 boundpoint[] = {
        glm::vec2(mColumn - 1, mRow - 1), glm::vec2(mColumn - 1, mRow - 1), glm::vec2(mColumn - 1, mRow + 1), glm::vec2(mColumn - 1, mRow + 1),
        glm::vec2(mColumn + 1, mRow + 1), glm::vec2(mColumn + 1, mRow + 1), glm::vec2(mColumn + 1, mRow - 1), glm::vec2(mColumn + 1, mRow - 1)
    };
    for(uint32_t i = 0; i < sizeof(cInfo) / sizeof(glm::vec2); ++i){
        if(!board->IsBoundary(cInfo[i].y, cInfo[i].x) && !board->IsBoundary(boundpoint[i].y, boundpoint[i].x) && !board->IsBoundary(bound[i].y, bound[i].x)){//这里的马和相不会被"蹩马腿",所以不需要判断
            legal_moves.push_back(cInfo[i]);
        }
    }
    RemoveInvalidChess(pBoard, legal_moves);
}

Xiang::Xiang(){
}

Xiang::Xiang(Country country, Territory territory):Chess(country, territory, Xiang_Chess, FONT_INDEX_XIANG, false){
}

bool Xiang::IsAbroad(uint32_t row, uint32_t column) const{
    return mTerritory != GetTerritoryIndex(row, column);
}

Xiang::Xiang(Country country, Territory territory, uint32_t row, uint32_t column):Chess(country, territory, Xiang_Chess, FONT_INDEX_XIANG, row, column, false){
}

Xiang::~Xiang(){
}

void Xiang::Select(const void *pBoard, std::vector<glm::vec2>&legal_moves) const{
    const Chessboard *board = (const Chessboard *)pBoard;
    const glm::vec2 cInfo[] = { glm::vec2(mColumn + 2, mRow + 2), glm::vec2(mColumn - 2, mRow - 2), glm::vec2(mColumn + 2, mRow - 2), glm::vec2(mColumn - 2,mRow + 2) };
    for(uint32_t i = 0; i < sizeof(cInfo) / sizeof(glm::vec2); ++i){
        if(!board->IsBoundary(cInfo[i].y, cInfo[i].x) && !IsAbroad(cInfo[i].y, cInfo[i].x)){
            legal_moves.push_back(cInfo[i]);
        }
    }
    RemoveInvalidChess(pBoard, legal_moves);
}
void Shi::InPalaceMove(const void *pBoard, std::vector<glm::vec2> &legal_moves) const{
    const Chessboard *board = (const Chessboard *)pBoard;
    Territory territory = board->IsInPalace(mRow, mColumn);
    if(territory != Invald_Territory && territory != Center_Territory){
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
                legal_moves.push_back(pos[i]);
            }
        }
        else{
            legal_moves.push_back(center);
        }    
    }
}
Shi::Shi(){
}

Shi::Shi(Country country, Territory territory):Chess(country, territory, Shi_Chess, FONT_INDEX_SHI, false){
}

Shi::Shi(Country country, Territory territory, uint32_t row, uint32_t column):Chess(country, territory, Shi_Chess, FONT_INDEX_SHI, row, column, false){
}

Shi::~Shi(){
}

void Shi::Select(const void *pBoard, std::vector<glm::vec2>&legal_moves) const{
    InPalaceMove(pBoard, legal_moves);
    RemoveInvalidChess(pBoard, legal_moves);
}

glm::vec2 ChaoJiBing::GetBingBack() const{
    glm::vec2 back = glm::vec2(0);
    if(mTerritory == Wu_Territory){
        back.x = 1;
    }
    else if(mTerritory == Wei_Territory){
        back.y = -1;
    }
    else if(mTerritory == Shu_Territory){
        back.y = 1;
    }
    else if(mTerritory == Han_Territory){
        back.x = -1;
    }
    return back;
}

bool ChaoJiBing::IsAbroad(uint32_t row, uint32_t column) const{
    return mTerritory != GetTerritoryIndex(row, column);
}

ChaoJiBing::ChaoJiBing(/* args */){
}

ChaoJiBing::ChaoJiBing(Country country, Territory territory):Chess(country, territory, Bing_Chess, FONT_INDEX_BING, true){
}

ChaoJiBing::ChaoJiBing(Country country, Territory territory, uint32_t row, uint32_t column):Chess(country, territory, Bing_Chess, FONT_INDEX_BING, row, column, true){
}

ChaoJiBing::~ChaoJiBing(){
}
void ChaoJiBing::Select(const void *pBoard, std::vector<glm::vec2>&legal_moves)const{
    const Chessboard *board = (const Chessboard *)pBoard;
    const glm::vec2 direction[] = { glm::vec2(1, 0), glm::vec2(1, 1), glm::vec2(-1, 0), glm::vec2(-1, 1), glm::vec2(0, 1), glm::vec2(-1, -1), glm::vec2(0, -1), glm::vec2(1, -1) };
    const uint32_t stepCount = 1;
    for(uint32_t i = 0; i < sizeof(direction) / sizeof(glm::vec2); ++i){
        uint32_t stepIndex = 0;
        const glm::vec2 currentPos = glm::vec2(mColumn, mRow);
        glm::vec2 pos = currentPos + direction[i];
        while(!board->IsBoundary(pos.y, pos.x) && stepIndex++ < stepCount){
            const Chess *pc = board->GetChess(pos.y, pos.x);
            if(pc){
                if(pc->GetCountry() != GetCountry()){
                    legal_moves.push_back(pos);
                }
                break;
            }
            else{
                legal_moves.push_back(pos);
            }
            pos += direction[i];
        }
    }
    RemoveInvalidChess(pBoard, legal_moves);
}