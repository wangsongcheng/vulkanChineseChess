#ifndef SOCKSET_FRAME_INCLUDE_H
#define SOCKSET_FRAME_INCLUDE_H
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
#define MAX_BYTE 0xff
typedef unsigned long long SOCKET;
#endif
#include "Chess.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#define INTERNET_PORT 10086
#define INVALID_SOCKET 100000
enum GameEvent{
    GAME_OVER_GAMEEVENT = 0,
    GAME_START_GAMEEVENT,
    JOINS_GAME_GAMEEVENT,
    PLAY_CHESS_GAMEEVENT,
    AI_JOIN_GAME_GAMEEVENT,
    CURRENT_COUNTRY_GAMEEVENT,
    SELF_PLAYER_INFORMATION_GAMEEVENT,
    CHANGE_PLAYER_INFORMATION_GAMEEVENT
};
//这个结构体不应该放这里
struct PlayerInfo{
    uint32_t index;
    glm::vec3 color;
    char country[MAX_BYTE];
};
struct GameMessage{
    uint32_t index;
    PlayerInfo game;
    GameEvent event;
    ChessInfo target;
    ChessInfo player;
    glm::vec2 mousePos;
    char name[MAX_BYTE];
};
void Recv(int __fd, void *__buf, size_t __n, int __flags);
void Send(int __fd, const void *__buf, size_t __n, int __flags);
// void Send(int __fd, const GameMessage *__buf, size_t __n, int __flags);
#endif