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
    pthread_t pthreadId;
    sem_t mAiSemaphore;
    // pthread_t mPthreadId = 0;
#endif
#ifdef WIN32
    DWORD  pthreadId;
    // HANDLE mHandle;
    HANDLE mAiSemaphore;
#endif
    bool mEnd = true;
    // bool mPause;
    Game *mGame;
    OnLine *mOnline;
    //有些棋子因未知原因没正确同步，目前出问题的是魏的士
    // //用来推演的，这样就能判断吃子后是否被吃，并可以根据分数决定是否可以互换棋子，例如一开始就用炮吃马，可能是亏本行为
    Chessboard mChessboard;
    //返回见面的势力;country为当前势力
    Country areKingsFacing(Country country);
    
    const Chess *Check(Country country) const;
    const Chess *Check(Country country, const glm::vec2&pos)const;
    
    // Chess *GetCheck(CountryType country, glm::vec2&pos)const;
    Chess *GetSelect(Country country)const;
    //获得能吃到pTarget的棋子
    Chess *GetSelect(Country country, const Chess *pTarget)const;
    Chess *GetCannonScreenPiece(const Chess *pao, const Chess *pTarget)const;
    Chess *GetTarget(const Chess *pSelect, const std::vector<glm::vec2>&canplays)const;

    Chess *GetResolveCheck(const Chess *pCheck, const Chess *pTarget, glm::vec2&pos);
    Chess *GetResolveCheck_Che(const Chess *pCheck, const Chess *pTarget, glm::vec2&pos);
    Chess *GetResolveCheck_Pao(const Chess *pCheck, const Chess *pTarget, glm::vec2&pos);
    //获得现在就能保护到pTarget的棋子
    Chess *GetNowDefender(const Chess *pCheck, const Chess **pTarget);
    //获得下一步能保护到pTarget的棋子的棋子
    Chess *GetNextDefender(const Chess *pCheck, const Chess **pTarget, glm::vec2&pos);

    //获得车到目标那个方向所有能下的位置
    std::vector<glm::vec2>GetPathBetween(const Chess *pChess, const Chess *pTarget)const;
    
    //返回的棋子肯定有位置可以下
    Chess *RandChess(Country country)const;

    glm::vec2 RandTarget(const Chess *pSelect, const std::vector<glm::vec2>&canplays);
public:
    Ai(/* args */);
    ~Ai();
    void CreatePthread();

    void Enable();
    void EnableNextCountry(bool autoPlay);
    
    Chess *GetSelect(Country country, glm::vec2&pos);

    void Wait();

    void WaitThread();
    // const Chess *GetTarget(const Chess *pSelect, uint32_t *row, uint32_t *column)const;
    
    void SyncBoard(const Chess *pChess, uint32_t dstRow, uint32_t dstColumn);

    inline void End(){
        const bool end = mEnd;
        mEnd = true;
        mChessboard.DestroyCountry();
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
    inline void InitializeAi(Game *pGame, OnLine *pOnline){
        mGame = pGame;
        mOnline = pOnline;
    }
    inline void InitializeChessboard(){
        mChessboard.InitializeChess(mGame->GetPlayer(), mGame->IsControllable());
    }
    inline void UndoStep(uint32_t step = 1){
        mChessboard.UndoStep(step);
    }
    inline auto GetChess(uint32_t row, uint32_t column){
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
    inline auto GetPlayer()const{
        return mGame->GetPlayer();
    }
    inline auto GetCurrentCountry()const{
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