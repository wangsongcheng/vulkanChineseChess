#ifndef AI_INCLUDE_H
#define AI_INCLUDE_H
#include <array>
#ifdef WIN32
#include <Windows.h>
#else
#include <pthread.h>
#include <semaphore.h>
#endif
#include "Chess.h"
#include "Game.h"
#include "VulkanChess.h"
class Ai{
#ifdef __linux
    sem_t mAiSemaphore;
    // pthread_t mPthreadId = 0;
#endif
#ifdef WIN32
    // HANDLE mHandle;
    HANDLE mAiSemaphore;
#endif
    Game *mGame;
    int32_t CanPlay(uint32_t country, const std::vector<Chess>&canplays)const;
public:
    Ai(/* args */);
    ~Ai();
    void Wait();
    void Enable();
    void CreatePthread(Game *pGame);
    void GetPlayChess(uint32_t country, Chess **pSelect, Chess **target, uint32_t *row, uint32_t *column)const;

    inline bool GameOver(){
        return mGame->GameOver();
    }
};
#endif