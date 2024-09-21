#ifndef AI_INCLUDE_H
#define AI_INCLUDE_H
#ifdef WIN32
#include <Windows.h>
#else
#include <pthread.h>
#endif
#include "Chess.h"
#include "Game.h"
#include "VulkanChess.h"
class Ai{
#ifdef WIN32
    HANDLE mHandle;
#else
    pthread_t mPthread;
#endif
    int32_t CanPlay(uint32_t country, const std::vector<Chess>&canplays, const Game *pChessboard);
public:
    Ai(/* args */);
    ~Ai();
    int CreatePthread(void *(*__start_routine)(void *), void *__arg);
    void GetPlayChess(uint32_t country, Chess **pSelect, Chess **target, uint32_t *row, uint32_t *column, Game *pChessboard);
};
#endif