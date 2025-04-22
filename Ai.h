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
#include "OnLine.h"
#include "Chessboard.h"
class Ai{
#ifdef __linux
    sem_t mAiSemaphore;
    // pthread_t mPthreadId = 0;
#endif
#ifdef WIN32
    // HANDLE mHandle;
    HANDLE mAiSemaphore;
#endif
    bool mEnd = true;
    // bool mPause;
    Game *mGame;
    OnLine *mOnline;
    int32_t CanPlay(uint32_t country, const std::vector<glm::vec2>&canplays)const;
public:
    Ai(/* args */);
    ~Ai();
    void Wait();
    void Enable();
    void EnableNextCountry();
    void CreatePthread(Game *pGame, OnLine *pOnline);
    void GetPlayChess(uint32_t country, Chess **pSelect, Chess **target, uint32_t *row, uint32_t *column)const;

    inline void End(){
        mEnd = true;
    }
    inline bool IsEnd(){
        return mEnd;
    }
    // inline bool IsPause(){
    //     return mPause;
    // }
    // inline void Pause(){
    //     mPause = true;
    // }
    // inline void Start(bool enable = false){
    //     mPause = false;
    //     if(enable)Enable();
    // }
    // inline const Chess *Check(uint32_t srcCountry, uint32_t dstCountry, uint32_t chess)const{
    //     mGame->Check(srcCountry, dstCountry, chess);
    // }
    inline bool IsOnline()const{
        return mGame->IsOnline();
    }
    inline bool GameOver()const{
        return mGame->GameOver();
    }
    inline auto GetAiClientIndex(){
        return mOnline->GetAiClientIndex();        
    }
    inline uint32_t GetPlayer()const{
        return mGame->GetPlayer();
    }
    inline void NextCountry(){
        mGame->NextCountry();
    }
    inline uint32_t GetCurrentCountry()const{
        return mGame->GetCurrentCountry();        
    }
    inline bool IsServer()const{
        return mOnline->IsServer();
    }
    inline void SendPlayChessMessage(const Player&game, const Chess *pSelect, const Chess *pTarget)const{
        mOnline->SendPlayChessMessage(game, pSelect, pTarget);
    }
};
#endif