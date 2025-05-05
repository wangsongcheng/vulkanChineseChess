#ifndef STDAFX_H
#define STDAFX_H
#include <array>
#include <unistd.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "vulkanFrame.h"
struct ImGuiInput{
    bool enableAi;
    bool enableHan;
    bool enableAutoPlay;
    struct{
        bool bOnline;
        bool bMain = true;
        bool bSinglePlayer;
        void RetMain(){
            bMain = true;
            bOnline = false;
            bSinglePlayer = false;    
        }
    }interface;
    void Init(){
        enableAi = false;
        enableHan = false;
        enableAutoPlay = false;
        interface.RetMain();
    }
};
#ifndef MAX_BYTE
#define MAX_BYTE 0xff
#endif
//定义该宏启用襄樊战役
#define ENABLE_BATTLE_FAN_GUANYU

#define CHESSBOARD_LINE_WIDTH 1

#define CHESSBOARD_RECT_SIZE 50

//棋盘内一行或一列的小矩形数量, 如果要算窗口大小，应该加2
#ifdef ENABLE_BATTLE_FAN_GUANYU
#define CHESSBOARD_RECT_COUNT 12
#else
#define CHESSBOARD_RECT_COUNT 16
#endif
//四个大矩形中，每个矩形所占的行数
#ifdef ENABLE_BATTLE_FAN_GUANYU
#define CHESSBOARD_BING_GRID_DENSITY 3
#else
#define CHESSBOARD_BING_GRID_DENSITY 4
#endif

#define MAX_CHESSBOARD_LINE (CHESSBOARD_RECT_COUNT + 1)
//通常用于数组遍历
#define CHESSBOARD_ROW CHESSBOARD_RECT_COUNT
#define CHESSBOARD_COLUMN CHESSBOARD_ROW

//黑色背景...一般情况下无需修改
#define CHESSBOARD_BACKGROUND_SIZE(WINDOW_WIDTH)(WINDOW_WIDTH - 2 * CHESSBOARD_RECT_SIZE)

#define CHESSBOARD_BOUNDARY_CENTER_RECT_COUNT (CHESSBOARD_RECT_COUNT - CHESSBOARD_BING_GRID_DENSITY * 2)
//该宏作用非常小，修改价值低
#define CHESSBOARD_BIG_GRID_COUNT 4
#define CHESSBOARD_BIG_RECT_SIZE (CHESSBOARD_RECT_SIZE * CHESSBOARD_BING_GRID_DENSITY + CHESSBOARD_LINE_WIDTH * CHESSBOARD_BING_GRID_DENSITY - 1)

#define ROW_COLUMN_TO_INDEX(ROW_INDEX, COLUMN_INDEX, COLUMN)((ROW_INDEX) * (COLUMN) + (COLUMN_INDEX))

#define MAX_COUNTRY_INDEX 4
//最好不要修改
#define WU_COUNTRY_INDEX 1
#define WEI_COUNTRY_INDEX 2
#define SHU_COUNTRY_INDEX 0
#define HAN_COUNTRY_INDEX 3
#define INVALID_COUNTRY_INDEX MAX_COUNTRY_INDEX
//该宏应该减少使用,尽量使用Game类中的国家总数

//联盟相关宏, 一般情况下需要修改
#define ALLIANCE_POINT_FONT_WIDTH 25
#define ALLIANCE_POINT_FONT_HEIGHT ALLIANCE_POINT_FONT_WIDTH
#define ALLIANCE_POINT_FONT_INDEX_WU WU_COUNTRY_INDEX
#define ALLIANCE_POINT_FONT_INDEX_WEI WEI_COUNTRY_INDEX
#define ALLIANCE_POINT_FONT_INDEX_SHU SHU_COUNTRY_INDEX
#define ALLIANCE_POINT_WIDTH 15
#define ALLIANCE_POINT_HEIGHT ALLIANCE_POINT_WIDTH

#define CHESS_WIDTH 20
#define CHESS_HEIGHT CHESS_WIDTH

#define FONT_INDEX_MA 7
#define FONT_INDEX_PAO 5
#define FONT_INDEX_CHE 6
#define FONT_INDEX_SHI 9
#define FONT_INDEX_BING 4
#define FONT_INDEX_XIANG 8
//
#define FONT_INDEX_WU WU_COUNTRY_INDEX
#define FONT_INDEX_SHU SHU_COUNTRY_INDEX
#define FONT_INDEX_WEI WEI_COUNTRY_INDEX
#define FONT_INDEX_HAN HAN_COUNTRY_INDEX

//仅修改数量不够, 还需设置它们的位置
#ifdef ENABLE_BATTLE_FAN_GUANYU
#define SHI_CHESS_COUNT 2
#define MA_CHESS_COUNT 3
#define PAO_CHESS_COUNT 2
#define CHE_CHESS_COUNT 0
#define BING_CHESS_COUNT 3
#define JIANG_CHESS_COUNT 1
#define XIANG_CHESS_COUNT 0
#define HAN_PAO_CHESS_COUNT 0
#define HAN_CHE_CHESS_COUNT 3
#else
#define SHI_CHESS_COUNT 2
#define MA_CHESS_COUNT 2
#define PAO_CHESS_COUNT 2
#define CHE_CHESS_COUNT 2
#define BING_CHESS_COUNT 5
#define JIANG_CHESS_COUNT 1
#define XIANG_CHESS_COUNT 2
#define HAN_PAO_CHESS_COUNT 1
#define HAN_CHE_CHESS_COUNT 3
#endif

#define COUNTRY_CHESS_COUNT (BING_CHESS_COUNT + PAO_CHESS_COUNT + CHE_CHESS_COUNT + MA_CHESS_COUNT + XIANG_CHESS_COUNT + SHI_CHESS_COUNT + JIANG_CHESS_COUNT)
//一般情况不需要修改
#define DRAW_CHESS_COUNT (COUNTRY_CHESS_COUNT * 3)
//每个国家需要绘制4个矩形
#define CHESSBOARD_PALACE_COUNT (MAX_COUNTRY_INDEX * 4)

#define WU_TERRITORY_INDEX WU_COUNTRY_INDEX
#define WEI_TERRITORY_INDEX WEI_COUNTRY_INDEX
#define SHU_TERRITORY_INDEX SHU_COUNTRY_INDEX
#define HAN_TERRITORY_INDEX HAN_COUNTRY_INDEX
#define CENTER_TERRITORY_INDEX (HAN_TERRITORY_INDEX + 1)
#define INVALID_TERRITORY_INDEX (CENTER_TERRITORY_INDEX + 1)

//vulkan专用，无特殊情况不要修改
//如果val比alignment小，则返回alignment，否则如果val大于alignment但小于alignment*2则返回alignment*2以此类推
#define ALIGN(val, alignment)((val + alignment - 1) & ~(alignment - 1))

#define ROW_COLUMN_TO_INDEX(ROW_INDEX, COLUMN_INDEX, COLUMN)((ROW_INDEX) * (COLUMN) + (COLUMN_INDEX))

#define CHESS_COLUMN_TO_X(COLUMN)((COLUMN + 1) * (CHESSBOARD_RECT_SIZE) + (COLUMN + 1) * CHESSBOARD_LINE_WIDTH)
#define CHESS_ROW_TO_Y(ROW)(CHESS_COLUMN_TO_X(ROW))
#endif