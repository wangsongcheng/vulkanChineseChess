#ifndef AI_INCLUDE_H
#define AI_INCLUDE_H
#ifdef WIN32
#include <Windows.h>
#else
#include <pthread.h>
#endif
#include "Chess.h"
#include "Chessboard.h"
class Ai{
#ifdef WIN32
    HANDLE mHandle;
#else
    pthread_t mPthread;
#endif
    int32_t CanPlay(uint32_t country, const std::vector<ChessInfo>&canplays, const Chessboard *pChessboard);
public:
    Ai(/* args */);
    ~Ai();
    int CreatePthread(void *(*__start_routine)(void *), void *__arg);
    void GetPlayChessInfo(uint32_t country, const ChessInfo **player, const ChessInfo **target, glm::vec2 *mousePos, const Chessboard *pChessboard);
};
#endif