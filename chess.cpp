#include "chess.h"
#include "VulkanChess.h"
#include "VulkanChessboard.h"
// #define STB_IMAGE_IMPLEMENTATION
// #define STB_TRUETYPE_IMPLEMENTATION
// #define STB_IMAGE_WRITE_IMPLEMENTATION
// #include "stb_image.h"
// #include "stb_truetype.h"
// #include "stb_image_write.h"

const ChessInfo *Chess::GetChessInfo(const ChessInfo *pInfo, uint32_t count, uint32_t row, uint32_t column)const{
    const ChessInfo *pChessInfo = nullptr;
    for (size_t i = 0; i < count; ++i){
        // printf("i = %d, country = %d, row = %d, column = %d\n", i, pInfo[i].country, pInfo[i].row, pInfo[i].column);
        if(pInfo[i].row == row && pInfo[i].column == column){
            pChessInfo = &pInfo[i];
            break;
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
bool Chess::IsInPalace(uint32_t row, uint32_t column, uint32_t centerCount, const ChessInfo *center) const{
    bool bInIsPalace = false;
    if(IsPalaceCenter(row, column, centerCount, center))return true;
    for (size_t i = 0; i < centerCount; ++i){
        if(abs((float)row - center[i].row) < 2 && abs((float)column - center[i].column) < 2){
            bInIsPalace = true;
            break;
        }
    }
    return bInIsPalace;
}
bool Chess::IsPalaceCenter(uint32_t row, uint32_t column, uint32_t centerCount, const ChessInfo *center)const{
    bool bIsPalaceCenter = false;
    for (size_t i = 0; i < centerCount; ++i){
        if(row == center[i].row && column == center[i].column){
            bIsPalaceCenter = true;
            break;
        }   
    }
    return bIsPalaceCenter;
}
void Chess::SelectChessInPalace(uint32_t centerCount, const ChessInfo *center, std::vector<ChessInfo>&canplays)const{
    //只需要管斜线，其他位置归棋子子类管
    for (size_t i = 0; i < centerCount; ++i){
        if(!IsInPalace(mInfo.row, mInfo.column, centerCount, center))continue;
        //上面的if已经排除了不在九宫格内的情况
        if(center[i].row == mInfo.row && center[i].column == mInfo.column){
            //除馬、相外, 其他棋子在中心点时，能走的肯定就是斜线边的点
            canplays.push_back(ChessInfo(center[i].row - 1, center[i].column - 1));
            canplays.push_back(ChessInfo(center[i].row + 1, center[i].column - 1));
            canplays.push_back(ChessInfo(center[i].row + 1, center[i].column + 1));
            canplays.push_back(ChessInfo(center[i].row - 1, center[i].column + 1));
            break;
        }
        else if(mInfo.row != center[i].row && mInfo.column != center[i].column){
            //在斜线边的点上时, 能走的只有中心点
            canplays.push_back(ChessInfo(center[i].row, center[i].column));
        }
    }
}
Wei::Wei(const ChessInfo &info) : Chess(info)
{
    mCenter.row = 1;
    mCenter.column = 8;
}
Wei::~Wei(){

}
void Wei::Selected(const ChessInfo *pInfo, uint32_t count, std::vector<ChessInfo>&canplays){
    //需要知道某个地方有没有棋子，该棋子的信息
    const ChessInfo cInfo[] = { ChessInfo(mInfo.row + 1, mInfo.column), ChessInfo(mInfo.row - 1, mInfo.column), ChessInfo(mInfo.row, mInfo.column + 1), ChessInfo(mInfo.row, mInfo.column - 1) };
    for(uint32_t i = 0; i < sizeof(cInfo) / sizeof(ChessInfo); ++i){
        if(!IsBoundary(cInfo[i].row, cInfo[i].column)){
            if(IsInPalace(cInfo[i].row, cInfo[i].column, 1, &mCenter)){//将不能出九宫格
                const ChessInfo *pChessInfo = GetChessInfo(pInfo, count, cInfo[i].row, cInfo[i].column);
                if(!pChessInfo || pChessInfo->country != mInfo.country){
                    canplays.push_back(ChessInfo(cInfo[i].row, cInfo[i].column));
                }
            }
        }
    }
    SelectChessInPalace(1, &mCenter, canplays);
}

Shu::Shu(const ChessInfo&info):Chess(info){
    mCenter.row = 15;
    mCenter.column = 8;
}
Shu::~Shu(){

}
void Shu::Selected(const ChessInfo *pInfo, uint32_t count, std::vector<ChessInfo>&canplays){
    const ChessInfo cInfo[] = { ChessInfo(mInfo.row + 1, mInfo.column), ChessInfo(mInfo.row - 1, mInfo.column), ChessInfo(mInfo.row, mInfo.column + 1), ChessInfo(mInfo.row, mInfo.column - 1) };
    for(uint32_t i = 0; i < sizeof(cInfo) / sizeof(ChessInfo); ++i){
        if(!IsBoundary(cInfo[i].row, cInfo[i].column)){
            if(IsInPalace(cInfo[i].row, cInfo[i].column, 1, &mCenter)){
                const ChessInfo *pChessInfo = GetChessInfo(pInfo, count, cInfo[i].row, cInfo[i].column);
                if(!pChessInfo || pChessInfo->country != mInfo.country)
                    canplays.push_back(ChessInfo(cInfo[i].row, cInfo[i].column));
            }
        }
    }
    SelectChessInPalace(1, &mCenter, canplays);
}
Wu::Wu(const ChessInfo&info):Chess(info){
    mCenter.row = 15;
    mCenter.column = 8;
}
Wu::~Wu(){

}
void Wu::Selected(const ChessInfo *pInfo, uint32_t count, std::vector<ChessInfo>&canplays){
    const ChessInfo cInfo[] = { ChessInfo(mInfo.row, mInfo.column - 1), ChessInfo(mInfo.row + 1, mInfo.column), ChessInfo(mInfo.row - 1, mInfo.column), ChessInfo(mInfo.row, mInfo.column + 1) };
    for(uint32_t i = 0; i < sizeof(cInfo) / sizeof(ChessInfo); ++i){
        if(!IsBoundary(cInfo[i].row, cInfo[i].column)){
            if(IsInPalace(cInfo[i].row, cInfo[i].column, 1, &mCenter)){
                const ChessInfo *pChessInfo = GetChessInfo(pInfo, count, cInfo[i].row, cInfo[i].column);
                if(!pChessInfo || pChessInfo->country != mInfo.country)
                    canplays.push_back(ChessInfo(cInfo[i].row, cInfo[i].column));
            }
        }
    }
    SelectChessInPalace(1, &mCenter, canplays);
}
// void Wu::CreateFontTexture(VkDevice device, const unsigned char *fontfiledata, VkCommandPool pool, VkQueue graphics){
//     unsigned char *bitmap = (unsigned char *)malloc(CHESS_FONT_IMAGE_WIDTH * CHESS_FONT_IMAGE_HEIGHT);
//     memset(bitmap, 0, CHESS_FONT_IMAGE_WIDTH * CHESS_FONT_IMAGE_HEIGHT);
//     GetFontImageData(fontfiledata, CHESS_FONT_IMAGE_WIDTH, CHESS_FONT_IMAGE_HEIGHT, L'吴', bitmap);
//     vkf::CreateFontImage(device, bitmap, CHESS_FONT_IMAGE_WIDTH, CHESS_FONT_IMAGE_HEIGHT, mFont, pool, graphics);
//     free(bitmap);

//     // if(!CreateTexture(device, CHESS_FONT_ROOT_PATH"吴.png", pool, graphics)){
//     //     // CreateFontImage("fonts/SourceHanSerifCN-Bold.otf");
//     //     // CreateTexture(device, CHESS_FONT_ROOT_PATH"吴.png", pool, graphics);
//     // }
// }

Han::Han(const ChessInfo&info):Chess(info){

}
Han::~Han(){

}
void Han::Selected(const ChessInfo *pInfo, uint32_t count, std::vector<ChessInfo>&canplays){
    //漢不能被选择
}
// void Han::CreateFontTexture(VkDevice device, const unsigned char *fontfiledata, VkCommandPool pool, VkQueue graphics){
//     unsigned char *bitmap = (unsigned char *)malloc(CHESS_FONT_IMAGE_WIDTH * CHESS_FONT_IMAGE_HEIGHT);
//     memset(bitmap, 0, CHESS_FONT_IMAGE_WIDTH * CHESS_FONT_IMAGE_HEIGHT);
//     GetFontImageData(fontfiledata, CHESS_FONT_IMAGE_WIDTH, CHESS_FONT_IMAGE_HEIGHT, L'漢', bitmap);
//     vkf::CreateFontImage(device, bitmap, CHESS_FONT_IMAGE_WIDTH, CHESS_FONT_IMAGE_HEIGHT, mFont, pool, graphics);
//     free(bitmap);

//     // if(!CreateTexture(device, CHESS_FONT_ROOT_PATH"漢.png", pool, graphics)){
//     //     // CreateFontImage("fonts/SourceHanSerifCN-Bold.otf");
//     //     // CreateTexture(device, CHESS_FONT_ROOT_PATH"漢.png", pool, graphics);
//     // }
// }
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
    else{
        printf("无效的国家:%d\n", country);
    }
    return back;
}
Bing::Bing(const ChessInfo&info):Chess(info){
    if(mInfo.country == WEI_COUNTRY_INDEX){
        mCenter[0].row = 15;
        mCenter[0].column = 8;

        mCenter[1].row = 8;
        mCenter[1].column = 15;
    }
    else if(mInfo.country == SHU_COUNTRY_INDEX){
        mCenter[0].row = 1;
        mCenter[0].column = 8;

        mCenter[1].row = 8;
        mCenter[1].column = 15;
    }
    else{
        mCenter[0].row = 1;
        mCenter[0].column = 8;

        mCenter[1].row = 15;
        mCenter[1].column = 8;
    }
}
Bing::~Bing(){

}
void Bing::Selected(const ChessInfo *pInfo, uint32_t count, std::vector<ChessInfo>&canplays){
    //兵不能后退
    const glm::vec2 back = GetCountryBack(mInfo.country);
    // const const ChessInfo *pChessInfo[] = { ChessInfo(mRow + 1, mColumn), ChessInfo(mRow - 1, mColumn), ChessInfo(mRow, mColumn + 1), ChessInfo(mRow, mColumn - 1) };
    const glm::vec2 cInfo[] = { glm::vec2(0, 1), glm::vec2(0, -1), glm::vec2(1, 0), glm::vec2(-1, 0) };
    for(uint32_t i = 0; i < sizeof(cInfo) / sizeof(glm::vec2); ++i){
        if(cInfo[i].x == back.x && cInfo[i].y == back.y)continue;
        const glm::vec2 currentPos = glm::vec2(mInfo.column, mInfo.row) + cInfo[i];
        if(!IsBoundary(currentPos.y, currentPos.x)){
            const ChessInfo *pChessInfo = GetChessInfo(pInfo, count, currentPos.y, currentPos.x);
            if(!pChessInfo || pChessInfo->country != mInfo.country)
                canplays.push_back(ChessInfo(currentPos.y, currentPos.x));
        }
    }
    SelectChessInPalace(2, mCenter, canplays);
}
// void Bing::CreateFontTexture(VkDevice device, const unsigned char *fontfiledata, VkCommandPool pool, VkQueue graphics){
//     unsigned char *bitmap = (unsigned char *)malloc(CHESS_FONT_IMAGE_WIDTH * CHESS_FONT_IMAGE_HEIGHT);
//     memset(bitmap, 0, CHESS_FONT_IMAGE_WIDTH * CHESS_FONT_IMAGE_HEIGHT);
//     GetFontImageData(fontfiledata, CHESS_FONT_IMAGE_WIDTH, CHESS_FONT_IMAGE_HEIGHT, L'兵', bitmap);
//     vkf::CreateFontImage(device, bitmap, CHESS_FONT_IMAGE_WIDTH, CHESS_FONT_IMAGE_HEIGHT, mFont, pool, graphics);
//     free(bitmap);

//     // if(!CreateTexture(device, CHESS_FONT_ROOT_PATH"兵.png", pool, graphics)){
//     //     // CreateFontImage("fonts/SourceHanSerifCN-Bold.otf");
//     //     // CreateTexture(device, CHESS_FONT_ROOT_PATH"兵.png", pool, graphics);
//     // }
// }

Pao::Pao(const ChessInfo&info):Chess(info){
    mCenter[WEI_COUNTRY_INDEX].row = 1;
    mCenter[WEI_COUNTRY_INDEX].column = 8;

    mCenter[SHU_COUNTRY_INDEX].row = 15;
    mCenter[SHU_COUNTRY_INDEX].column = 8;

    mCenter[WU_COUNTRY_INDEX].row = 8;
    mCenter[WU_COUNTRY_INDEX].column = 15;
}
Pao::~Pao(){
    
}
void Pao::Selected(const ChessInfo *pInfo, uint32_t count, std::vector<ChessInfo>&canplays){
    // bool bCapture = false;
    const glm::vec2 direction[] = { glm::vec2(1, 0), glm::vec2(-1, 0), glm::vec2(0, 1), glm::vec2(0, -1) };
    for(uint32_t i = 0; i < sizeof(direction) / sizeof(glm::vec2); ++i){
        const glm::vec2 currentPos = glm::vec2(mInfo.column, mInfo.row);
        glm::vec2 pos = currentPos + direction[i];
        while(!IsBoundary(pos.y, pos.x)){
            const ChessInfo *pChessInfo = GetChessInfo(pInfo, count, pos.y, pos.x);
            if(pChessInfo){
                pos += direction[i];
                while(!IsBoundary(pos.y, pos.x)){
                    pChessInfo = GetChessInfo(pInfo, count, pos.y, pos.x);
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
    SelectChessInPalace(3, mCenter, canplays);
}
// void Pao::CreateFontTexture(VkDevice device, const unsigned char *fontfiledata, VkCommandPool pool, VkQueue graphics){
//     unsigned char *bitmap = (unsigned char *)malloc(CHESS_FONT_IMAGE_WIDTH * CHESS_FONT_IMAGE_HEIGHT);
//     memset(bitmap, 0, CHESS_FONT_IMAGE_WIDTH * CHESS_FONT_IMAGE_HEIGHT);
//     GetFontImageData(fontfiledata, CHESS_FONT_IMAGE_WIDTH, CHESS_FONT_IMAGE_HEIGHT, L'炮', bitmap);
//     vkf::CreateFontImage(device, bitmap, CHESS_FONT_IMAGE_WIDTH, CHESS_FONT_IMAGE_HEIGHT, mFont, pool, graphics);
//     free(bitmap);

//     // if(!CreateTexture(device, CHESS_FONT_ROOT_PATH"炮.png", pool, graphics)){
//     //     // CreateFontImage("fonts/SourceHanSerifCN-Bold.otf");
//     //     // CreateTexture(device, CHESS_FONT_ROOT_PATH"炮.png", pool, graphics);
//     // }
// }

Che::Che(const ChessInfo&info):Chess(info){
    mCenter[WEI_COUNTRY_INDEX].row = 1;
    mCenter[WEI_COUNTRY_INDEX].column = 8;

    mCenter[SHU_COUNTRY_INDEX].row = 15;
    mCenter[SHU_COUNTRY_INDEX].column = 8;

    mCenter[WU_COUNTRY_INDEX].row = 15;
    mCenter[WU_COUNTRY_INDEX].column = 8;
}
Che::~Che(){
    
}
void Che::Selected(const ChessInfo *pInfo, uint32_t count, std::vector<ChessInfo>&canplays){
    const glm::vec2 direction[] = { glm::vec2(1, 0), glm::vec2(-1, 0), glm::vec2(0, 1), glm::vec2(0, -1) };
    for(uint32_t i = 0; i < sizeof(direction) / sizeof(glm::vec2); ++i){
        const glm::vec2 currentPos = glm::vec2(mInfo.column, mInfo.row);
        glm::vec2 pos = currentPos + direction[i];
        while(!IsBoundary(pos.y, pos.x)){
            const ChessInfo *pChessInfo = GetChessInfo(pInfo, count, pos.y, pos.x);
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
    SelectChessInPalace(3, mCenter, canplays);
}
// void Che::CreateFontTexture(VkDevice device, const unsigned char *fontfiledata, VkCommandPool pool, VkQueue graphics){
//     unsigned char *bitmap = (unsigned char *)malloc(CHESS_FONT_IMAGE_WIDTH * CHESS_FONT_IMAGE_HEIGHT);
//     memset(bitmap, 0, CHESS_FONT_IMAGE_WIDTH * CHESS_FONT_IMAGE_HEIGHT);
//     GetFontImageData(fontfiledata, CHESS_FONT_IMAGE_WIDTH, CHESS_FONT_IMAGE_HEIGHT, L'車', bitmap);
//     vkf::CreateFontImage(device, bitmap, CHESS_FONT_IMAGE_WIDTH, CHESS_FONT_IMAGE_HEIGHT, mFont, pool, graphics);
//     free(bitmap);

//     // if(!CreateTexture(device, CHESS_FONT_ROOT_PATH"車.png", pool, graphics)){
//     //     // CreateFontImage("fonts/SourceHanSerifCN-Bold.otf");
//     //     // CreateTexture(device, CHESS_FONT_ROOT_PATH"車.png", pool, graphics);
//     // }
// }

Ma::Ma(const ChessInfo&info):Chess(info){
    mCenter[WEI_COUNTRY_INDEX].row = 1;
    mCenter[WEI_COUNTRY_INDEX].column = 8;

    mCenter[SHU_COUNTRY_INDEX].row = 15;
    mCenter[SHU_COUNTRY_INDEX].column = 8;

    mCenter[WU_COUNTRY_INDEX].row = 15;
    mCenter[WU_COUNTRY_INDEX].column = 8;
}
Ma::~Ma(){
    
}
void Ma::Selected(const ChessInfo *pInfo, uint32_t count, std::vector<ChessInfo>&canplays){
    const ChessInfo cInfo[] = {
        ChessInfo(mInfo.row - 1, mInfo.column - 2), ChessInfo(mInfo.row - 2, mInfo.column - 1), ChessInfo(mInfo.row - 2, mInfo.column + 1), ChessInfo(mInfo.row - 1, mInfo.column + 2),
        ChessInfo(mInfo.row + 1, mInfo.column + 2), ChessInfo(mInfo.row + 2, mInfo.column + 1), ChessInfo(mInfo.row + 2, mInfo.column - 1), ChessInfo(mInfo.row + 1, mInfo.column - 2)
    };
    for(uint32_t i = 0; i < sizeof(cInfo) / sizeof(ChessInfo); ++i){
        if(!IsBoundary(cInfo[i].row, cInfo[i].column)){//这里的马和相不会被"蹩马腿",所以不需要判断
            const ChessInfo *pChessInfo = GetChessInfo(pInfo, count, cInfo[i].row, cInfo[i].column);
            if(!pChessInfo || pChessInfo->country != mInfo.country)
                canplays.push_back(ChessInfo(cInfo[i].row, cInfo[i].column));
        }
    }
}
// void Ma::CreateFontTexture(VkDevice device, const unsigned char *fontfiledata, VkCommandPool pool, VkQueue graphics){
//     unsigned char *bitmap = (unsigned char *)malloc(CHESS_FONT_IMAGE_WIDTH * CHESS_FONT_IMAGE_HEIGHT);
//     memset(bitmap, 0, CHESS_FONT_IMAGE_WIDTH * CHESS_FONT_IMAGE_HEIGHT);
//     GetFontImageData(fontfiledata, CHESS_FONT_IMAGE_WIDTH, CHESS_FONT_IMAGE_HEIGHT, L'馬', bitmap);
//     vkf::CreateFontImage(device, bitmap, CHESS_FONT_IMAGE_WIDTH, CHESS_FONT_IMAGE_HEIGHT, mFont, pool, graphics);
//     free(bitmap);

//     // if(!CreateTexture(device, CHESS_FONT_ROOT_PATH"馬.png", pool, graphics)){
//     //     // CreateFontImage("fonts/SourceHanSerifCN-Bold.otf");
//     //     // CreateTexture(device, CHESS_FONT_ROOT_PATH"馬.png", pool, graphics);
//     // }
// }
Xiang::Xiang(const ChessInfo&info):Chess(info){

}
Xiang::~Xiang(){
    
}
void Xiang::Selected(const ChessInfo *pInfo, uint32_t count, std::vector<ChessInfo>&canplays){
    const ChessInfo cInfo[] = { ChessInfo(mInfo.row + 2, mInfo.column + 2), ChessInfo(mInfo.row - 2, mInfo.column - 2), ChessInfo(mInfo.row - 2, mInfo.column + 2), ChessInfo(mInfo.row + 2, mInfo.column - 2) };
    for(uint32_t i = 0; i < sizeof(cInfo) / sizeof(ChessInfo); ++i){
        if(!IsBoundary(cInfo[i].row, cInfo[i].column)){
            const ChessInfo *pChessInfo = GetChessInfo(pInfo, count, cInfo[i].row, cInfo[i].column);
            if(!pChessInfo || pChessInfo->country != mInfo.country)
                canplays.push_back(ChessInfo(cInfo[i].row, cInfo[i].column));
        }
    }
}
// void Xiang::CreateFontTexture(VkDevice device, const unsigned char *fontfiledata, VkCommandPool pool, VkQueue graphics){
//     unsigned char *bitmap = (unsigned char *)malloc(CHESS_FONT_IMAGE_WIDTH * CHESS_FONT_IMAGE_HEIGHT);
//     memset(bitmap, 0, CHESS_FONT_IMAGE_WIDTH * CHESS_FONT_IMAGE_HEIGHT);
//     GetFontImageData(fontfiledata, CHESS_FONT_IMAGE_WIDTH, CHESS_FONT_IMAGE_HEIGHT, L'相', bitmap);
//     vkf::CreateFontImage(device, bitmap, CHESS_FONT_IMAGE_WIDTH, CHESS_FONT_IMAGE_HEIGHT, mFont, pool, graphics);
//     free(bitmap);

//     // if(!CreateTexture(device, CHESS_FONT_ROOT_PATH"相.png", pool, graphics)){
//     //     // CreateFontImage("fonts/SourceHanSerifCN-Bold.otf");
//     //     // CreateTexture(device, CHESS_FONT_ROOT_PATH"相.png", pool, graphics);
//     // }
// }
Shi::Shi(const ChessInfo&info):Chess(info){
    if(info.country == WEI_COUNTRY_INDEX){
        mCenter.row = 1;
        mCenter.column = 8;
    }
    else if(info.country == SHU_COUNTRY_INDEX){
        mCenter.row = 15;
        mCenter.column = 8;
    }
    else{
        mCenter.row = 15;
        mCenter.column = 8;
    }
}
Shi::~Shi(){
    
}
void Shi::Selected(const ChessInfo *pInfo, uint32_t count, std::vector<ChessInfo>&canplays){
    SelectChessInPalace(1, &mCenter, canplays);
}
// void Shi::CreateFontTexture(VkDevice device, const unsigned char *fontfiledata, VkCommandPool pool, VkQueue graphics){
//     unsigned char *bitmap = (unsigned char *)malloc(CHESS_FONT_IMAGE_WIDTH * CHESS_FONT_IMAGE_HEIGHT);
//     memset(bitmap, 0, CHESS_FONT_IMAGE_WIDTH * CHESS_FONT_IMAGE_HEIGHT);
//     GetFontImageData(fontfiledata, CHESS_FONT_IMAGE_WIDTH, CHESS_FONT_IMAGE_HEIGHT, L'士', bitmap);
//     vkf::CreateFontImage(device, bitmap, CHESS_FONT_IMAGE_WIDTH, CHESS_FONT_IMAGE_HEIGHT, mFont, pool, graphics);
//     free(bitmap);

//     // if(!CreateTexture(device, CHESS_FONT_ROOT_PATH"士.png", pool, graphics)){
//     //     // CreateFontImage("fonts/SourceHanSerifCN-Bold.otf");
//     //     // CreateTexture(device, CHESS_FONT_ROOT_PATH"士.png", pool, graphics);
//     // }
// }