#include "Game.h"
Game::Game(/* args */){
    mCountryCount = MAX_COUNTRY_INDEX - 1;
}

Game::~Game(){
}
bool Game::GameOver(){
    uint32_t deathCount = 0;
    for (size_t i = 0; i < MAX_COUNTRY_INDEX; ++i){
        if((mHanCanPlay || i != HAN_COUNTRY_INDEX) && mChessboard.IsDeath(i)){
            ++deathCount;
        }
    }
    return deathCount > 1;
}
void Game::InitinalizeGame(int32_t player, int32_t currentCountry){
    StartGame();
    if(IsHanCanPslay()){
        mCountryCount = MAX_COUNTRY_INDEX;
    }
    else{
        mCountryCount = MAX_COUNTRY_INDEX - 1;
    }
    if(currentCountry != -1)
        mCurrentCountry = currentCountry;
    else
        mCurrentCountry = rand()%mCountryCount;
    if(player != -1)
        mPlayer = player;
    else
        mPlayer = rand()%mCountryCount;
    mChessboard.InitializeChess(mPlayer, mHanCanPlay);
}
void Game::NextCountry(){
    if(mBackupPlayer != INVALID_COUNTRY_INDEX){
        mCurrentCountry = mBackupPlayer;
        mBackupPlayer = INVALID_COUNTRY_INDEX;
    }
    else{
        mCurrentCountry = GetNextCountry(mCurrentCountry);
    }
}
uint32_t Game::GetNextCountry()const{
    return GetNextCountry(mCurrentCountry);
}
uint32_t Game::GetNextCountry(uint32_t country)const{
    do{
        country = (country + 1) % mCountryCount;
    } while (mChessboard.IsDeath(country));
    return country;
}

void Game::ExtraTurn(uint32_t country){
    mBackupPlayer = GetNextCountry(mCurrentCountry);
    mCurrentCountry = country;
}

void Game::CaptureChess(const Chess *play, const Chess *target){
    mChessboard.CaptureChess(play, target);
}

uint32_t Game::Check()const{
    uint32_t country = INVALID_COUNTRY_INDEX, currentCountry = mCurrentCountry;
    do{
        const Chess *pChess = Check(mCurrentCountry, currentCountry, JIANG_CHESS_INDEX);
        if(pChess){
            country = currentCountry;
            break;
        }
        currentCountry = GetNextCountry(currentCountry);
    } while (currentCountry != mCurrentCountry);
    return country;
}