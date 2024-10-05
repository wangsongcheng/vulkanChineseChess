#ifndef LUA_FRAME_INCLUDE
#define LUA_FRAME_INCLUDE
#include <lua.hpp>
#include <cstdint>
#include <lualib.h>
#include <lauxlib.h>
#include "Game.h"
// #ifdef __linux
// #define LOW_WORD(N)(N & 0xff)
// #define HIGHT_WORD(N)(N >> 16)
// #endif
#define LUA_SUCCESS 0
#define LUA_CHECK_GET_TABLE_VAL(L, table, key, val, file, type)\
do{\
    int32_t err = get_table_val(L, table, key, val);\
    if(err != LUA_SUCCESS){\
        switch (err){\
        case -2:\
            std::cout << "error:" << table << " is not a table name" << "\n请确认是否在" << file << "文件中，文件中加入了该表名" << std::endl;\
            break;\
        case -3:\
            std::cout << "error:" << table << "[" << key << "] type is not a " << type << "\n请确定" << file << "文件中，" << table << "[" << key << "]是" << type << std::endl;\
            break;\
        default:\
            break;\
        }\
        lua_close(L);\
    }\
}while(0)

void GetSelectAndTarget(lua_State *L, Chess **pSelect, Chess **pTarget, uint32_t *row, uint32_t *column);

//给lua程序调用
int lua_IsChess(lua_State *L);

int lua_GetChess(lua_State *L);
int lua_GetCountry(lua_State *L);
int lua_GetCurrentCountry(lua_State *L);

int lua_GetCanPlayCount(lua_State *L);

int lua_GetRow(lua_State *L);
int lua_GetColumn(lua_State *L);
int lua_GetCanPlayRow(lua_State *L);
int lua_GetCanPlayColumn(lua_State *L);
// int l_draw_card(lua_State *L);
// int l_get_target(lua_State *L);
// int l_reduce_blood(lua_State *L);
// int l_get_target_count(lua_State *L);

// int l_use_card(lua_State *L);
// int l_use_equip(lua_State *L);
// int l_play_card(lua_State *L);
// int l_discard_card(lua_State *L);
// int l_get_knight_index(lua_State *L);
// int l_request_use_card(lua_State *L);
// int l_request_use_equip(lua_State *L);
// int l_request_play_card(lua_State *L);
// int l_request_discard_card(lua_State *L);

void luaError(lua_State *L, const char *fmt, ...);
void registration_function(lua_State *L);
// //如果返回结果是字符串，必须使用数组或类似的类型接收，不能用指针
// void callFun(lua_State *L, const char *func, const char *sig, ...);
// int32_t get_table_val(lua_State *L, const char *table_name, const char *key, bool&val);
// int32_t get_table_val(lua_State *L, const char *table_name, const char *key, uint32_t&val);
// int32_t get_table_val(lua_State *L, const char *table_name, const char *key, char *val);
#endif