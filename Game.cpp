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
    notAllianceCountry = INVALID_COUNTRY_INDEX;
    mChessboard.InitializeChess(mPlayer, mHanCanPlay);
}
void Game::NextCountry(){
    // if(mBackupPlayer != INVALID_COUNTRY_INDEX){
    //     mCurrentCountry = mBackupPlayer;
    //     mBackupPlayer = INVALID_COUNTRY_INDEX;
    // }
    // else{
    mCurrentCountry = GetNextCountry(mCurrentCountry);
    // }
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
    // mBackupPlayer = GetNextCountry(mCurrentCountry);
    mCurrentCountry = country;
}

void Game::RemoveInvalidTarget(const Chess *pChess, std::vector<glm::vec2>&canplays){
    if(!IsHanCanPslay()){
        for (auto it = canplays.begin(); it != canplays.end();){
            const Chess *pc = GetChess(it->y, it->x);
            if(pc && pc->GetChess() == JIANG_CHESS_INDEX && pc->GetCountry() == HAN_CHE_CHESS_COUNT && pChess->GetChess() != MA_CHESS_INDEX){
                it = canplays.erase(it);
            }
            else{
                ++it;
            }
        }
        const uint32_t notAllianceCountry = GetNotAllianceCountry();
        if(notAllianceCountry != INVALID_COUNTRY_INDEX){
            const uint32_t allianceCountry = GET_ALLIANCE_COUNTRY(mCurrentCountry, notAllianceCountry);
            for (auto it = canplays.begin(); it != canplays.end();){
                const Chess *pc = GetChess(it->y, it->x);
                if(pc &&  pc->GetCountry() == allianceCountry){
                    it = canplays.erase(it);
                }
                else{
                    ++it;
                }
            }    
        }
    }
}

void Game::CaptureChess(const Chess *play, const Chess *target){
    mChessboard.CaptureChess(play->GetCountry(), target->GetCountry(), target->GetChess());
    if(!mHanCanPlay && target->GetCountry() == HAN_COUNTRY_INDEX && target->GetChess() == JIANG_CHESS_INDEX){
        SetNotAllianceCountry(play->GetCountry(), play->GetRow(), play->GetColumn());
    }
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

void Game::SetNotAllianceCountry(uint32_t country, uint32_t row, uint32_t column){
    glm::vec2 vPos[3];
    glm::vec2 center = mChessboard.GetPalacesCenter(HAN_COUNTRY_INDEX);
    for (size_t i = 0; i < MAX_COUNTRY_INDEX; ++i){
        if((mHanCanPlay || i != HAN_COUNTRY_INDEX) && mChessboard.IsDeath(i)){
            //有势力阵亡，联盟就无效了
            return;
        }
    }
    //吴永远在中心点
    vPos[WU_COUNTRY_INDEX] = center;
    if(center.x == 1){
        //汉在汉的位置上
        vPos[WEI_COUNTRY_INDEX] = glm::vec2(COLUMN_TO_X(center.x - 1), ROW_TO_Y(center.y + 1));
        vPos[SHU_COUNTRY_INDEX] = glm::vec2(COLUMN_TO_X(center.x - 1), ROW_TO_Y(center.y - 1));
    }
    else if(center.y == 1){
        //汉在魏的位置上
        vPos[WEI_COUNTRY_INDEX] = glm::vec2(COLUMN_TO_X(center.x - 1), ROW_TO_Y(center.y - 1));
        vPos[SHU_COUNTRY_INDEX] = glm::vec2(COLUMN_TO_X(center.x + 1), ROW_TO_Y(center.y - 1));
    }
    else if(center.y == CHESSBOARD_ROW - 1){
        //汉在蜀的位置上....理论上不可能，但算了，还是写上吧
        vPos[WEI_COUNTRY_INDEX] = glm::vec2(COLUMN_TO_X(center.x + 1), ROW_TO_Y(center.y - 1));
        vPos[SHU_COUNTRY_INDEX] = glm::vec2(COLUMN_TO_X(center.x - 1), ROW_TO_Y(center.y - 1));
    }
    else if(center.x == CHESSBOARD_COLUMN - 1){
        //汉在吴的位置
        vPos[WEI_COUNTRY_INDEX] = glm::vec2(COLUMN_TO_X(center.x + 1), ROW_TO_Y(center.y - 1));
        vPos[SHU_COUNTRY_INDEX] = glm::vec2(COLUMN_TO_X(center.x + 1), ROW_TO_Y(center.y + 1));
    }
    int32_t allianceCountry = INVALID_COUNTRY_INDEX;
    for (size_t i = 0; i < MAX_COUNTRY_INDEX; ++i){
        if(vPos[i].x == column && vPos[i].y == row && country != i){
            allianceCountry = i;
            break;
        }
    }
    if(allianceCountry != INVALID_COUNTRY_INDEX){
        notAllianceCountry = GET_NOT_ALLIANCE_COUNTRY(country, allianceCountry);
        mChessboard.CaptureChess(notAllianceCountry, HAN_COUNTRY_INDEX, JIANG_CHESS_INDEX);    
    }
}
