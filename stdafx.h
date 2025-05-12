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
        bool bReplay;
        bool bOpenFile;
        bool bReplayInfo;
        bool bMain = true;
        bool bOpenFolder;
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
//最好不要修改
enum Country{
    Shu_Country = 0,
    Wu_Country,
    Wei_Country,
    Han_Country,
    Invald_Country
};
enum Territory{
    Wu_Territory = Wu_Country,
    Wei_Territory = Wei_Country,
    Shu_Territory = Shu_Country,
    Han_Territory = Han_Country,
    Center_Territory,
    Invald_Territory
};
#ifndef MAX_BYTE
#define MAX_BYTE 0xff
#endif
//定义该宏启用襄樊战役
// #define ENABLE_BATTLE_FAN_GUANYU

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

//该宏应该减少使用,尽量使用Game类中的国家总数
#define MAX_COUNTRY_INDEX 4

#define CHESS_WIDTH 20
#define CHESS_HEIGHT CHESS_WIDTH
/*
    增加、减少棋子步骤
    1.修改下边的宏
    2.在Chessboard.cpp文件内(关键词InitializeChess)修改棋子信息,具体看Chessboard.cpp文件说明
    添加新棋子
    1.在Chess.h实现派生类
    2.添加棋子数量宏
    3.添加棋子偏移宏
    4.goto"增加、减少棋子步骤"
*/
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

#define JIANG_CHESS_OFFSET 0
#define MA_CHESS_OFFSET (JIANG_CHESS_OFFSET + JIANG_CHESS_COUNT)
#define PAO_CHESS_OFFSET (MA_CHESS_OFFSET + MA_CHESS_COUNT)
#define CHE_CHESS_OFFSET (PAO_CHESS_OFFSET + PAO_CHESS_COUNT)
#define XIANG_CHESS_OFFSET (CHE_CHESS_OFFSET + CHE_CHESS_COUNT)
#define SHI_CHESS_OFFSET (XIANG_CHESS_OFFSET + XIANG_CHESS_COUNT)
#define BING_CHESS_OFFSET (SHI_CHESS_OFFSET + SHI_CHESS_COUNT)

#define HAN_CHE_CHESS_OFFSET (JIANG_CHESS_OFFSET + JIANG_CHESS_COUNT)
#define HAN_PAO_CHESS_OFFSET (HAN_CHE_CHESS_OFFSET + HAN_CHE_CHESS_COUNT)

#define COUNTRY_CHESS_COUNT (BING_CHESS_COUNT + PAO_CHESS_COUNT + CHE_CHESS_COUNT + MA_CHESS_COUNT + XIANG_CHESS_COUNT + SHI_CHESS_COUNT + JIANG_CHESS_COUNT)
//一般情况不需要修改
#define DRAW_CHESS_COUNT (COUNTRY_CHESS_COUNT * 3)
//每个国家需要绘制4个矩形
#define CHESSBOARD_PALACE_COUNT (MAX_COUNTRY_INDEX * 4)

#define REPLAY_FILE_TYP "*.cr"
//vulkan专用，无特殊情况不要修改
//如果val比alignment小，则返回alignment，否则如果val大于alignment但小于alignment*2则返回alignment*2以此类推
#define ALIGN(val, alignment)((val + alignment - 1) & ~(alignment - 1))

#define ROW_COLUMN_TO_INDEX(ROW_INDEX, COLUMN_INDEX, COLUMN)((ROW_INDEX) * (COLUMN) + (COLUMN_INDEX))

#define COLUMN_TO_X(COLUMN)((COLUMN) * (CHESSBOARD_RECT_SIZE) + (COLUMN) * CHESSBOARD_LINE_WIDTH)
#define ROW_TO_Y(ROW)COLUMN_TO_X(ROW)

#define CHESS_COLUMN_TO_X(COLUMN)COLUMN_TO_X(COLUMN + 1)
#define CHESS_ROW_TO_Y(ROW)(CHESS_COLUMN_TO_X(ROW))

// #define CHESS_COLUMN_TO_X(COLUMN)((COLUMN + 1) * (CHESSBOARD_RECT_SIZE) + (COLUMN + 1) * CHESSBOARD_LINE_WIDTH)
// #define CHESS_ROW_TO_Y(ROW)(CHESS_COLUMN_TO_X(ROW))
#endif