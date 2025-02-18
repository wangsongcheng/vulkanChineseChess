#include "Game.h"
Game::Game(/* args */){
    mCountryCount = MAX_COUNTRY_INDEX - 1;
}

Game::~Game(){
}
bool Game::GameOver(){
    uint32_t deathCount = 0;
    for (size_t i = 0; i < MAX_COUNTRY_INDEX; ++i){
        if(mChessboard.IsDeath(i)){
            ++deathCount;
        }
    }
    return deathCount > 1;
}
void Game::InitinalizeGame(int32_t player, int32_t currentCountry){
    StartGame();
    if(HanCanPslay()){
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
}
void Game::InitializeChess(){
    mChessboard.InitializeChess(mPlayer);
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
uint32_t Game::GetNextCountry(uint32_t country){
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