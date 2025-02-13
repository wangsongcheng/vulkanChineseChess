#include "LuaFrame.h"
#include <string>
#include <iostream>

//因为lua需要
extern Game g_Game;

// int SetLowAndHight(short hight, short low){
//     int32_t result;
//     result |= hight;
//     result <<= 16;

//     result |= low;
//     return result;
// }
void registration_function(lua_State *L){
    lua_register(L, "IsChess", lua_IsChess);
    lua_register(L, "GetChess", lua_GetChess);
    lua_register(L, "GetCountry", lua_GetCountry);
    lua_register(L, "GetCurrentCountry", lua_GetCurrentCountry);

    lua_register(L, "GetCanPlayCount", lua_GetCanPlayCount);

    lua_register(L, "GetRow", lua_GetRow);
    lua_register(L, "GetColumn", lua_GetColumn);
    lua_register(L, "GetCanPlayRow", lua_GetCanPlayRow);
    lua_register(L, "GetCanPlayColumn", lua_GetCanPlayColumn);

    // lua_pushcfunction(L, lua_GetChess);
    // lua_setglobal(L, "GetChess");
    // lua_pushcfunction(L, lua_GetCountry);
    // lua_setglobal(L, "GetCountry");
    // lua_pushcfunction(L, lua_GetCurrentCountry);
    // lua_setglobal(L, "GetCurrentCountry");
    // lua_pushcfunction(L, lua_GetCanPlayCount);
    // lua_setglobal(L, "GetCanPlayCount");
    // lua_pushcfunction(L, lua_GetRow);
    // lua_setglobal(L, "GetRow");
    // lua_pushcfunction(L, lua_GetColumn);
    // lua_setglobal(L, "GetColumn");
    // lua_pushcfunction(L, lua_GetCanPlayRow);
    // lua_setglobal(L, "GetCanPlayRow");
    // lua_pushcfunction(L, lua_GetCanPlayColumn);
    // lua_setglobal(L, "GetCanPlayColumn");
}

void GetSelectAndTarget(lua_State *L, Chess **pSelect, Chess **pTarget, uint32_t *row, uint32_t *column){
    lua_getglobal(L,"GetSelectAndTarget");
    lua_pushnumber(L, g_Game.GetCurrentCountry());
    lua_pushnumber(L, g_Game.GetChessCount(g_Game.GetCurrentCountry()));
    /*开始调用函数，有2个参数，4个返回值*/
    if(lua_pcall(L, 2, 4, 0) != LUA_OK){
        luaError(L, "call lua function GetSelectAndTarget error\n");
    }
    /*取出返回值*/
    uint32_t selectRow = (uint32_t)lua_tonumber(L, 1);
    uint32_t selectColumn = (uint32_t)lua_tonumber(L, 2);
    *row = (uint32_t)lua_tonumber(L, 3);
    *column = (uint32_t)lua_tonumber(L, 4);
    *pTarget = g_Game.GetChess(*row, *column);
    *pSelect = g_Game.GetChess(selectRow, selectColumn);
    /*清除返回值的栈*/
    lua_pop(L, 4);
}

int lua_GetCountry(lua_State *L){
    int32_t row = lua_tonumber(L, 1);
    int32_t column = lua_tonumber(L, 2);
    int32_t country = INVALID_COUNTRY_INDEX;
    Chess *pChess = g_Game.GetChess(row, column);
    if(pChess){
        country = pChess->GetCountry();
    }
    lua_pushnumber(L, country);
    // printf("function %s: row %d, column %d, country %d\n", __FUNCTION__, row, column, country);
    return 1;
}

int lua_IsChess(lua_State *L){
    int32_t chess = lua_tonumber(L, 2);
    int32_t country = lua_tonumber(L, 1);
    auto pChess = g_Game.GetChess();
    if(chess == INVALID_CHESS_INDEX)
        lua_pushboolean(L, false);
    else
        lua_pushboolean(L, pChess[ROW_COLUMN_CHESS_TO_INDEX(country, chess)] != nullptr);
    // printf("function %s:row %d, column %d, chess %d\n", __FUNCTION__, row, column, chess);
    return 1;
}

int lua_GetChess(lua_State *L){
    int32_t row = lua_tonumber(L, 1);
    int32_t column = lua_tonumber(L, 2);
    int32_t chess = INVALID_CHESS_INDEX;
    Chess *pChess = g_Game.GetChess(row, column);
    if(pChess){
        chess = pChess->GetChess();
    }
    lua_pushnumber(L, chess);
    // printf("function %s:row %d, column %d, chess %d\n", __FUNCTION__, row, column, chess);
    return 1;
}

int lua_GetCurrentCountry(lua_State *L){
    // printf("function %s start\n", __FUNCTION__);
    lua_pushnumber(L, g_Game.GetCurrentCountry());
    return 1;
}

int lua_GetCanPlayCount(lua_State *L){
    int32_t count = 0;
    int32_t chess = lua_tonumber(L, 2);
    int32_t country = lua_tonumber(L, 1);
    auto pChess = g_Game.GetChess();
    if(pChess[ROW_COLUMN_CHESS_TO_INDEX(country, chess)]){
        std::vector<Chess>canplays;
        pChess[ROW_COLUMN_CHESS_TO_INDEX(country, chess)]->Selected(g_Game.GetChess(), canplays);
        count = canplays.size();
    }
    lua_pushnumber(L, count);
    // printf("in function %s:chess %d, country %d, count %d\n", __FUNCTION__, chess, country, count);
    return 1;
}

int lua_GetRow(lua_State *L){
    int32_t row = CHESSBOARD_ROW;
    int32_t chess = lua_tonumber(L, 2);
    int32_t country = lua_tonumber(L, 1);
    auto pChess = g_Game.GetChess();
    if(pChess[ROW_COLUMN_CHESS_TO_INDEX(country, chess)]){
        row = pChess[ROW_COLUMN_CHESS_TO_INDEX(country, chess)]->GetRow();
    }
    lua_pushnumber(L, row);
    // printf("function %s:country %d, chess %d, row %d\n", __FUNCTION__, country, chess, row);
    return 1;
}

int lua_GetColumn(lua_State *L){
    int32_t chess = lua_tonumber(L, 2);
    int32_t country = lua_tonumber(L, 1);
    int32_t column = CHESSBOARD_COLUMN;
    auto pChess = g_Game.GetChess();
    if(pChess[ROW_COLUMN_CHESS_TO_INDEX(country, chess)]){
        column = pChess[ROW_COLUMN_CHESS_TO_INDEX(country, chess)]->GetColumn();
    }
    lua_pushnumber(L, column);
    // printf("function %s:country %d, chess %d, column %d\n", __FUNCTION__, country, chess, column);
    return 1;
}

int lua_GetCanPlayRow(lua_State *L){
    int32_t chess = lua_tonumber(L, 2);
    int32_t index = lua_tonumber(L, 3);
    int32_t country = lua_tonumber(L, 1);

    int32_t row = CHESSBOARD_ROW;
    auto pChess = g_Game.GetChess();
    if(pChess[ROW_COLUMN_CHESS_TO_INDEX(country, chess)]){
        std::vector<Chess>canplays;
        pChess[ROW_COLUMN_CHESS_TO_INDEX(country, chess)]->Selected(g_Game.GetChess(), canplays);
        row = canplays[index].GetRow();
    }
    lua_pushnumber(L, row);
    // printf("in function %s:chess %d, country %d, index %d, row %d\n", __FUNCTION__, chess, country, index, row);
    return 1;
}

int lua_GetCanPlayColumn(lua_State *L){
    int32_t chess = lua_tonumber(L, 2);
    int32_t index = lua_tonumber(L, 3);
    int32_t country = lua_tonumber(L, 1);

    int32_t column = CHESSBOARD_COLUMN;
    auto pChess = g_Game.GetChess();
    if(pChess[ROW_COLUMN_CHESS_TO_INDEX(country, chess)]){
        std::vector<Chess>canplays;
        pChess[ROW_COLUMN_CHESS_TO_INDEX(country, chess)]->Selected(g_Game.GetChess(), canplays);
        column = canplays[index].GetColumn();
    }
    lua_pushnumber(L, column);
    // printf("in function %s:chess %d, country %d, index %d, column %d\n", __FUNCTION__, chess, country, index, column);
    return 1;
}

void luaError(lua_State *L, const char *fmt, ...){
    va_list argp;
    va_start(argp, fmt);
    vfprintf(stderr, fmt, argp);
    va_end(argp);
    lua_close(L);
}