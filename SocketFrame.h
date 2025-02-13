#ifndef SOCKSET_FRAME_INCLUDE_H
#define SOCKSET_FRAME_INCLUDE_H
#include <string>
#include <iostream>
#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <WS2tcpip.h>
#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")
#define SHUT_RDWR 2
#else
#include <netdb.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/socket.h>
typedef unsigned long long SOCKET;
#endif
#include "Chess.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#define MAX_BYTE 0xff
#define INVALID_SOCKET -1
#define INTERNET_PORT 10086
#define INVALID_PLAYER_INDEX -1
enum GameEvent{
    GAME_OVER_GAME_EVENT = 0,
    GAME_START_GAME_EVENT,
    JOINS_GAME_GAME_EVENT,
    PLAY_CHESS_GAME_EVENT,
    AI_JOIN_GAME_GAME_EVENT,
    CURRENT_COUNTRY_GAME_EVENT,
    SELECTED_COUNTRY_GAME_EVENT,
    CHANGE_PLAYER_NAME_GAME_EVENT,
    CHANGE_PLAYER_COUNTRY_GAME_EVENT,
    SELF_CLIENT_INFORMATION_GAME_EVENT,
    CHANGE_PLAYER_INFORMATION_GAME_EVENT,
};
//这个结构体不应该放这里
struct Player{
    bool ai;
    bool random;
    uint32_t index;//国家索引
    // glm::vec3 color;
    char name[MAX_BYTE];
    char country[MAX_BYTE];
    Player(){
        ai = false;
        random = true;
        strcpy(country, "?");
        index = INVALID_PLAYER_INDEX;
    }
};
struct GameMessage{
    Chess target;
    Chess select;
    Player player;
    GameEvent event;
    // glm::vec2 mousePos;
    uint32_t clientIndex;
};
void Recv(int __fd, void *__buf, size_t __n, int __flags);
void Send(int __fd, const void *__buf, size_t __n, int __flags);
// void Send(int __fd, const GameMessage *__buf, size_t __n, int __flags);
#endif