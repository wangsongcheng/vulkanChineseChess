#include "Game.h"
Game::Game(/* args */){
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
void Game::InitinalizeGame(int32_t player){
    mCurrentCountry = rand()%mCountryCount;
#ifndef INTERNET_MODE
    if(player != -1)
        mPlayer = player;
    else
        mPlayer = rand()%mCountryCount;
#endif
}
void Game::InitializeChess(){
    mChessboard.InitializeChess(mPlayer);
}
void Game::NextCountry(){
    mCurrentCountry = GetNextCountry(mCurrentCountry);
}
uint32_t Game::GetNextCountry(uint32_t country){
    do{
        country = (country + 1) % mCountryCount;
    } while (mChessboard.IsDeath(country));
    return country;
}


void Game::CaptureChess(const Chess *play, const Chess *target){
    mChessboard.CaptureChess(play, target);
}