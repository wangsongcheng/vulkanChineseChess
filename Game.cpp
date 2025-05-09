#include "Game.h"
void Game::RemoveInvalidTarget(std::vector<glm::vec2>&canplays){
    if(!state.isControllable){
        const int32_t notAllianceCountry = mNotAllianceCountry;
        if(notAllianceCountry != INVALID_COUNTRY_INDEX){
            const uint32_t allianceCountry = GET_ALLIANCE_COUNTRY(mCurrentCountry, notAllianceCountry);
            for (auto it = canplays.begin(); it != canplays.end();){
                const Chess *pc = mChessboard.GetChess(it->y, it->x);
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
uint32_t Game::GetCanPlay(const glm::vec2&mousePos, const std::vector<glm::vec2>&canplays){
    uint32_t index = canplays.size();
    for (size_t i = 0; i < canplays.size(); ++i){
        const uint32_t chessY = CHESS_ROW_TO_Y(canplays[i].y) - CHESSBOARD_RECT_SIZE * .5;
        const uint32_t chessX = CHESS_COLUMN_TO_X(canplays[i].x) - CHESSBOARD_RECT_SIZE * .5;
        if(mousePos.x > chessX && mousePos.y > chessY && mousePos.x < chessX + CHESSBOARD_RECT_SIZE && mousePos.y < chessY + CHESSBOARD_RECT_SIZE){
            index = i;
            break;
        }
    }
    return index;
}
void Game::UpdateSelectChessUniform(VkDevice device, std::vector<glm::vec2>&canplays){
    //本来, 为了点击棋子之后有效果...是会改变棋子大小的..但如今发现..或许不需要了
    std::vector<glm::mat4>model(CHESSBOARD_RECT_COUNT * 2);
    const glm::mat4 scale = glm::scale(glm::mat4(1), glm::vec3(CHESSBOARD_RECT_SIZE, CHESSBOARD_RECT_SIZE, 1));
    for (size_t i = 0; i < canplays.size(); ++i){
        //原是以圆心为原点，矩形不是,所以需要-自身的一半
        model[i] = glm::translate(glm::mat4(1), glm::vec3(CHESS_COLUMN_TO_X(canplays[i].x) - CHESSBOARD_RECT_SIZE / 2, CHESS_ROW_TO_Y(canplays[i].y) - CHESSBOARD_RECT_SIZE / 2, 0)) * scale;
    }
    vulkan.select.UpdateUniform(device, model);
}
void Game::UnSelectChess(){
    std::vector<glm::mat4>model(CHESSBOARD_RECT_COUNT * 2);
    for (size_t i = 0; i < CHESSBOARD_RECT_COUNT * 2; ++i){
        model[i] = glm::translate(glm::mat4(1), glm::vec3(CHESS_COLUMN_TO_X(100), CHESS_ROW_TO_Y(100), 0));
    }
    vulkan.select.UpdateUniform(vulkan.device.device, model);
    UpdateChessUniform(vulkan.device.device);
}
void Game::UpdateChessUniform(VkDevice device){
    for (uint32_t uiCoutry = 0; uiCoutry < MAX_COUNTRY_INDEX; ++uiCoutry){
        auto pChess = mChessboard.GetChess(uiCoutry);
        for(uint32_t uiChess = 0; uiChess < DRAW_CHESS_COUNT; ++uiChess){
            const Chess *pc = pChess[uiChess];
            const uint32_t dynamicOffsets = uiCoutry * DRAW_CHESS_COUNT + uiChess;
            if(pc){
                vulkan.chess.UpdateUniform(device, pc->GetFontIndex(), uiCoutry, pc->GetPos(),dynamicOffsets);
            }
            else{
                const glm::vec3 pos = glm::vec3(CHESS_COLUMN_TO_X(CHESSBOARD_ROW + 10), CHESS_ROW_TO_Y(CHESSBOARD_COLUMN + 10), 0);
                vulkan.chess.UpdateUniform(device, FONT_INDEX_HAN, uiCoutry, pos, dynamicOffsets);
            }
        }
    }
}
void Game::SelectChess(const Chess *pChess){
    if(pChess){
        std::vector<glm::vec2>canplays;
        pChess->Select(&mChessboard, canplays);
        RemoveInvalidTarget(canplays);
        UpdateSelectChessUniform(vulkan.device.device, canplays);
        vulkan.chess.UpdateUniform(vulkan.device.device, pChess->GetFontIndex(), pChess->GetCountry(), pChess->GetPos(), CHESS_WIDTH * 1.2, CHESS_HEIGHT * 1.2, ROW_COLUMN_TO_INDEX(pChess->GetCountry(), pChess->GetChessOffset(), DRAW_CHESS_COUNT));
    }
}
void Game::SaveStep(Chess *pChess, uint32_t dstRow, uint32_t dstColumn){
    /*
        int move_number;
        bool is_facing, is_death;
        Facing facing;//如果因为见面被销毁或被灭亡后，棋子可以记这里
    */
    const Chess *pTarget = mChessboard.GetChess(dstRow, dstColumn);
    ChessMove move;
    move.chess = *pChess;
    move.is_capture = pTarget;
    if(move.is_capture){
        move.captured = *pTarget;
    }
    move.is_check = Check(nullptr);
    for (uint32_t srcCountry = 0; srcCountry < mMaxCountryCount; srcCountry++){
        for (uint32_t dstountry = 0; dstountry < mMaxCountryCount; dstountry++){
            if(srcCountry != dstountry){
                if(mChessboard.areKingsFacing(srcCountry, dstountry)){
                    move.is_facing = true;
                    srcCountry = mMaxCountryCount;
                    move.facing[1].country = dstountry;
                    move.facing[0].country = srcCountry;
                    break;
                }
            }
        }
    }
    if(move.is_facing){
        for (uint32_t i = 0; i < move.facing.size(); ++i){
            auto pCountryChess = mChessboard.GetChess(move.facing[i].country);
            for (size_t uiChess = 0; uiChess < DRAW_CHESS_COUNT; uiChess++){
                if(pCountryChess[uiChess]){
                    move.facing[i].chess.push_back(*pCountryChess[uiChess]);
                }   
            }        
        }
    }
    mChessboard.SaveStep(move);
}
glm::vec4 Game::PrepareChess(const Chess *pSelect, const glm::vec2 &mousePos){
    // const uint32_t clientIndex = g_OnLine.GetClientIndex();
    std::vector<glm::vec2>canplays;
    pSelect->Select(&mChessboard, canplays);
    uint32_t index = GetCanPlay(mousePos, canplays);
    glm::vec4 info = glm::vec4(0);
    if(index != canplays.size()){
        info.z = canplays[index].x;
        info.w = canplays[index].y;
        info.y = pSelect->GetRow();
        info.x = pSelect->GetColumn();
    }
    return info;
}
Game::Game(/* args */){
}

Game::~Game(){
}
bool Game::areKingsFacing(){
    bool facing = false;
    for (uint32_t srcCountry = 0; srcCountry < mMaxCountryCount; srcCountry++){
        for (uint32_t dstountry = 0; dstountry < mMaxCountryCount; dstountry++){
            if(srcCountry != dstountry && (INVALID_COUNTRY_INDEX == mNotAllianceCountry || dstountry != GET_ALLIANCE_COUNTRY(srcCountry, mNotAllianceCountry))){
                if(mChessboard.areKingsFacing(srcCountry, dstountry)){
                    mChessboard.DestroyCountry(srcCountry);
                    mChessboard.DestroyCountry(dstountry);
                    srcCountry = mMaxCountryCount;
                    facing = true;
                    break;
                }
            }
        }
    }
    return facing;
}
const Chess *Game::Check(uint32_t *sCountry) const{
    const Chess *pChess = nullptr;
    if(mChessboard.IsDeath(mCurrentCountry))return nullptr;
    for (size_t srcCountry = 0; srcCountry < mMaxCountryCount; ++srcCountry){
        uint32_t dstCountry = GetNextCountry(mCurrentCountry);
        if(srcCountry != dstCountry){
            while (dstCountry != mCurrentCountry){
                const Chess *pJiang = mChessboard.GetChess(dstCountry)[Chess::Type::Jiang_Chess];
                if(!pJiang)break;
                pChess = mChessboard.Check(srcCountry, pJiang->GetRow(), pJiang->GetColumn());
                if(pChess){
                    if(sCountry)*sCountry = srcCountry;
                    return pJiang;
                }
                dstCountry = GetNextCountry(dstCountry);
            }
        }
    }
    return pChess;
}

bool Game::GameOver(){
    uint32_t deathCount = 0;
    for (size_t i = 0; i < mMaxCountryCount; ++i){
        if(mChessboard.IsDeath(i)){
            ++deathCount;
        }
    }
    bool hasEcxit = false;
    for (size_t i = 0; i < mMaxCountryCount; i++){
        if(mChessboard.IsHasExitPermission(i)){
            hasEcxit = true;
            break;
        }       
    }
    return deathCount > mMaxCountryCount / 2 || !hasEcxit;
}
uint32_t Game::GetNextCountry()const{
    return GetNextCountry(mCurrentCountry);
}
uint32_t Game::GetLastCountry() const{
    uint32_t country = mCurrentCountry;
    do{
        country = (country - 1 + mMaxCountryCount) % mMaxCountryCount;
    } while (mChessboard.IsDeath(country));
    return country;
}
uint32_t Game::GetNextCountry(uint32_t country) const{
    do{
        country = (country + 1) % mMaxCountryCount;
    } while (mChessboard.IsDeath(country));
    return country;
}
void Game::InitinalizeGame(int32_t playerCountry, int32_t currentCountry){
    state.isGameStart = true;
    if(state.isControllable){
        mMaxCountryCount = MAX_COUNTRY_INDEX;
    }
    else{
        mMaxCountryCount = MAX_COUNTRY_INDEX - 1;
    }
    if(currentCountry != INVALID_COUNTRY_INDEX)
        mCurrentCountry = currentCountry;
    else
        mCurrentCountry = WU_COUNTRY_INDEX;
        // mCurrentCountry = rand()%mMaxCountryCount;
    if(playerCountry != INVALID_COUNTRY_INDEX)
        player.country = playerCountry;
    else
        player.country = rand()%mMaxCountryCount;
    mNotAllianceCountry = INVALID_COUNTRY_INDEX;
    mChessboard.InitializeChess(player.country, state.isControllable);
}
glm::vec2 lerp(const glm::vec2&p0, const glm::vec2&p1, float t){
    return (1 - t) * p0 + t * p1;
}
void Game::MoveChess(const glm::vec2&start, const glm::vec2&end, uint32_t fontIndex, uint32_t country, uint32_t dynamicOffsets){
    for (float t = 0; t < 1; t += .01){
        const glm::vec2 pos = lerp(start, end, t);
        // vulkan.chess.UpdateUniform(vulkan.device.device, fontIndex, pos, dynamicOffsets);
        vulkan.chess.UpdateUniform(vulkan.device.device, fontIndex, country, pos, CHESS_WIDTH * 1.2, CHESS_HEIGHT * 1.2, dynamicOffsets);
#ifdef WIN32
        Sleep(1);
#else
        usleep(10000);
#endif
    }
}
void Game::PlayChess(Chess *pChess, uint32_t dstRow, uint32_t dstColumn){
    const char county[][MAX_BYTE] = { "蜀", "吴", "魏", "汉" };
    const Chess *pTarget = mChessboard.GetChess(dstRow, dstColumn);
    const uint32_t dynamicOffsets = ROW_COLUMN_TO_INDEX(pChess->GetCountry(), pChess->GetChessOffset(), DRAW_CHESS_COUNT);
    const glm::vec2 start = glm::vec2(CHESS_COLUMN_TO_X(pChess->GetColumn()), CHESS_ROW_TO_Y(pChess->GetRow())), end = glm::vec2(CHESS_COLUMN_TO_X(dstColumn), CHESS_ROW_TO_Y(dstRow));
    //为什么不能正确移动原偏移的棋子，导致移动的时候，原位的棋子位置不变
    mMutex.lock();
    SaveStep(pChess, dstRow, dstColumn);
    MoveChess(start, end, pChess->GetFontIndex(), pChess->GetCountry(), dynamicOffsets);
    if(pTarget){
        uint32_t targetCountry = pTarget->GetCountry();
        mChessboard.CaptureChess(pChess, pTarget);
        if(mChessboard.IsDeath(targetCountry)){
            printf("%s国被%s国消灭\n", county[targetCountry], county[pChess->GetCountry()]);
            mChessboard.DestroyCountry(targetCountry);
            if(!state.isControllable){
                if(targetCountry == HAN_CHE_CHESS_COUNT){
                    //另外两个结盟
                    mNotAllianceCountry = pChess->GetCountry();
                }
                else{
                    mNotAllianceCountry = INVALID_COUNTRY_INDEX;
                }    
            }
        }
    }
    pChess->SetPos(dstRow, dstColumn);
    if(areKingsFacing()){
        printf("两将见面了\n");
    }
    UpdateChessUniform(vulkan.device.device);
    mMutex.unlock();
    if(!state.isControllable && pChess->GetChess() == Chess::Type::Ma_Chess){
        //不用提示了。因为结盟后，汉的棋子会归另一方
        SetNotAllianceCountry(pChess->GetCountry(), pChess->GetRow(), pChess->GetColumn());
    }
    uint32_t currerntCountry;
    //应该优先检查下一位势力是否被将
    const Chess *pCheck = Check(&currerntCountry);
    if(pCheck){
        uint32_t country = pCheck->GetCountry();
        if(currerntCountry == mCurrentCountry){
            mCurrentCountry = country;
        }
        else{
            NextCountry();
        }
    }
    else{
        NextCountry();
    }
}

void Game::SetNotAllianceCountry(uint32_t country, uint32_t row, uint32_t column){
    //如果已经结过盟了，则无法继续结盟
    if(mNotAllianceCountry != INVALID_COUNTRY_INDEX)return;
    glm::vec2 vPos[3];
    const uint32_t territory = GET_TERRITORY_INDEX(HAN_COUNTRY_INDEX, player.country);
    glm::vec2 center = mChessboard.GetPalacesCenter(territory);
    for (size_t i = 0; i < MAX_COUNTRY_INDEX; ++i){
        if((i != HAN_COUNTRY_INDEX) && mChessboard.IsDeath(i)){
            //有势力阵亡，联盟就无效了
            return;
        }
    }
    //吴永远在中心点
    vPos[WU_COUNTRY_INDEX] = center;
    if(territory == HAN_TERRITORY_INDEX){
        vPos[WEI_COUNTRY_INDEX] = glm::vec2(center.x - 1, center.y + 1);
        vPos[SHU_COUNTRY_INDEX] = glm::vec2(center.x - 1, center.y - 1);
    }
    else if(territory == WEI_TERRITORY_INDEX){
        vPos[WEI_COUNTRY_INDEX] = glm::vec2(center.x - 1, center.y - 1);
        vPos[SHU_COUNTRY_INDEX] = glm::vec2(center.x + 1, center.y - 1);
    }
    else if(territory == SHU_TERRITORY_INDEX){
        //理论上不可能，但算了，还是写上吧
        vPos[WEI_COUNTRY_INDEX] = glm::vec2(center.x + 1, center.y - 1);
        vPos[SHU_COUNTRY_INDEX] = glm::vec2(center.x - 1, center.y - 1);
    }
    else if(territory == WU_TERRITORY_INDEX){
        vPos[WEI_COUNTRY_INDEX] = glm::vec2(center.x + 1, center.y - 1);
        vPos[SHU_COUNTRY_INDEX] = glm::vec2(center.x + 1, center.y + 1);
    }
    uint32_t allianceCountry = INVALID_COUNTRY_INDEX;
    for (size_t i = 0; i < MAX_COUNTRY_INDEX; ++i){
        if(vPos[i].x == column && vPos[i].y == row && country != i){
            allianceCountry = i;
            break;
        }
    }
    if(allianceCountry != INVALID_COUNTRY_INDEX && allianceCountry != country){
        mNotAllianceCountry = GET_NOT_ALLIANCE_COUNTRY(country, allianceCountry);
        mChessboard.GetCountryChess(mNotAllianceCountry, HAN_COUNTRY_INDEX);
        mChessboard.DestroyCountry(HAN_COUNTRY_INDEX);
    }
}
void Game::NextCountry(){
    mCurrentCountry = GetNextCountry(mCurrentCountry);
}
void Game::LastCountry(){
    mCurrentCountry = GetLastCountry();
}
void Game::NewGame(int32_t playerCountry, int32_t currentCountry){
    mMutex.lock();
    InitinalizeGame(playerCountry, currentCountry);
    UpdateChessUniform(vulkan.device.device);
    mMutex.unlock();
    vulkan.chessboard.UpdateFontUniform(vulkan.device.device, player.country);
}

void Game::Cleanup(VkDevice device){
    vulkan.chess.Cleanup(device);
    vulkan.select.Cleanup(device);
    vulkan.chessboard.Cleanup(device);
}

void Game::Draw(VkCommandBuffer command, VkPipelineLayout layout){
    vulkan.chessboard.Draw(command,layout);
    vulkan.chess.DrawChess(command, layout);
}
void Game::DrawFont(VkCommandBuffer command, VkPipelineLayout layout){
    vulkan.chessboard.DrawAllianceFont(command, layout);
    vulkan.chess.DrawFont(command, layout);
}
void Game::DrawWireframe(VkCommandBuffer command, VkPipelineLayout layout){
    vulkan.chessboard.DrawWireframe(command, layout);
    vulkan.select.Draw(command, layout);
}

void Game::EndGame(){
    mMaxCountryCount = 3;
    state.isOnline = false;
    state.isGameStart = false;
    state.isControllable = false;
    player.country = INVALID_COUNTRY_INDEX;
    mCurrentCountry = INVALID_COUNTRY_INDEX;
    mNotAllianceCountry = INVALID_COUNTRY_INDEX;
    for (uint32_t uiCountry = 0; uiCountry < MAX_COUNTRY_INDEX; uiCountry++){
        mChessboard.DestroyCountry(uiCountry);
    }
    UpdateChessUniform(vulkan.device.device);
}

void Game::Setup(VulkanDevice device, VkDescriptorSetLayout layout, VkQueue graphics, VulkanPool pool){
    vulkan.device = device;
    vulkan.chess.Setup(device, layout, graphics, pool);
    vulkan.chessboard.Setup(device, layout, graphics, pool);
    vulkan.select.Setup(device, CHESSBOARD_ROW + CHESSBOARD_COLUMN, layout, graphics, pool);
}

void Game::UpdateUniform(VkDevice device, uint32_t windowWidth){
    UpdateChessUniform(device);
    vulkan.chessboard.UpdateUniform(device, windowWidth, player.country);
}
