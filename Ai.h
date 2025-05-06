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
    //有些棋子因未知原因没正确同步，目前出问题的是魏的士
    // //用来推演的，这样就能判断吃子后是否被吃，并可以根据分数决定是否可以互换棋子，例如一开始就用炮吃马，可能是亏本行为
    // Chessboard mChessboard;
    
    const Chess *Check(uint32_t country, Chess::Type chess) const;
    const Chess *Check(uint32_t country, const glm::vec2&pos)const;
    int32_t CanPlay(uint32_t country, const std::vector<glm::vec2>&canplays)const;
    
    // Chess *GetCheck(uint32_t country, glm::vec2&pos)const;
    Chess *GetTarget(uint32_t country)const;
    //获得能吃到pTarget的棋子
    Chess *GetSelect(uint32_t country, const Chess *pTarget)const;
    Chess *GetCannonScreenPiece(const Chess *pao, const Chess *pTarget)const;
    Chess *GetTarget(const Chess *pSelect, const std::vector<glm::vec2>&canplays)const;

    bool IsBoundary(int32_t row, int32_t column)const;

    glm::vec2 GetPathBetween(const Chess *pChess, const Chess *pTarget)const;
    
    //返回的棋子肯定有位置可以下
    Chess *RandChess(uint32_t country)const;
    Chess *ResolveCheck(const Chess *pCheck, const Chess *pTarget, glm::vec2&pos);
    Chess *GetResolveCheck_Che(const Chess *pCheck, const Chess *pTarget, glm::vec2&pos);
    Chess *GetResolveCheck_Pao(const Chess *pCheck, const Chess *pTarget, glm::vec2&pos);

    void RemoveHanTarget(std::vector<glm::vec2>&canplays);
    Chess *RandTarget(uint32_t country, const std::vector<glm::vec2>&canplays, glm::vec2&pos)const;

    // void SetNotAllianceCountry(uint32_t country, uint32_t row, uint32_t column);
public:
    Ai(/* args */);
    ~Ai();
    void Wait();
    void Enable();
    void EnableNextCountry(bool autoPlay);
    void CreatePthread(Game *pGame, OnLine *pOnline);
    Chess *GetSelect(uint32_t country, glm::vec2&pos);
    // const Chess *GetTarget(const Chess *pSelect, uint32_t *row, uint32_t *column)const;
    
    // void SyncBoardCopy(Chess *pChess, uint32_t dstRow, uint32_t dstColumn);

    inline void End(){
        const bool end = mEnd;
        mEnd = true;
        if(!end){
            Enable();
        }
    }
    inline bool IsEnd()const{
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
    auto GetChess(uint32_t row, uint32_t column){
        return mGame->GetChessboard()->GetChess(row, column);
    }
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
        return mGame->GetPlayerCountry();
    }
    inline uint32_t GetCurrentCountry()const{
        return mGame->GetCurrentCountry();        
    }
    inline bool IsServer()const{
        return mOnline->IsServer();
    }
    inline void PlayChess(Chess *pChess, uint32_t dstRow, uint32_t dstColumn)const{
        mGame->PlayChess(pChess, dstRow, dstColumn);
    }
    inline void UnSelectChess()const{
        mGame->UnSelectChess();
    }
    inline void SelectChess(const Chess *pChess)const{
        mGame->SelectChess(pChess);
    }
    inline void SendPlayChessMessage(const Player&game, const Chess *pSelect, const Chess *pTarget)const{
        mOnline->SendPlayChessMessage(game, pSelect, pTarget);
    }
};
#endif