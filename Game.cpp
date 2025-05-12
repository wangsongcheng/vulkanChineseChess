#include "Game.h"
void Game::RemoveInvalidTarget(std::vector<glm::vec2>&canplays){
    if(!state.isControllable){
        const Country notAllianceCountry = mNotAlliance;
        if(notAllianceCountry != Invald_Country){
            const uint32_t allianceCountry = GET_ALLIANCE_COUNTRY(mCurrent, notAllianceCountry);
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
        auto pChess = mChessboard.GetChess((Country)uiCoutry);
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
ChessMove Game::GetSaveStep(Chess *pChess, uint32_t dstRow, uint32_t dstColumn){
    //注意:更新该函数后仍需要更新AI类的同名函数
    const Chess *pTarget = mChessboard.GetChess(dstRow, dstColumn);
    ChessMove move = {};
    move.chess = *pChess;
    move.is_capture = pTarget;
    char step[MAX_BYTE];
    if(dstRow > pChess->GetRow()){
        strcpy(step, u8"退");
    }
    else if(dstRow < pChess->GetRow()){
        strcpy(step, u8"进");
    }
    else{
        strcpy(step, u8"平");
    }
    uint32_t si = pChess->GetColumn(), di = strcmp(step, u8"平")?dstRow:dstColumn;
    sprintf(move.notation, "%s:%s%s%s%s", mCountry[pChess->GetCountry()].c_str(), mName[pChess->GetChess()].c_str(),  mNumber[si].c_str(), step, mNumber[di].c_str());
    if(move.is_capture){
        move.is_death = pTarget->GetChess() == Chess::Type::Jiang_Chess;
        if(move.is_death){
            move.death.country = pTarget->GetCountry();
            auto pCountryChess = mChessboard.GetChess(move.death.country);
            for (size_t uiChess = 0; uiChess < DRAW_CHESS_COUNT; uiChess++){
                if(pCountryChess[uiChess])move.death.chess.push_back(*pCountryChess[uiChess]);
            }
        }
        else{
            move.captured = *pTarget;
        }
    }
    else{
        move.captured.SetPos(dstRow, dstColumn);
    }
    move.is_check = Check(nullptr);
    //为测试是否见面，需要一定的修改
    const glm::vec2 pos = glm::vec2(pChess->GetColumn(), pChess->GetRow());
    pChess->SetPos(dstRow, dstColumn);
    for (uint32_t srcCountry = 0; srcCountry < mMaxCountryCount; srcCountry++){
        for (uint32_t dstountry = 0; dstountry < mMaxCountryCount; dstountry++){
            if(srcCountry != dstountry){
                if(mChessboard.areKingsFacing((Country)srcCountry, (Country)dstountry)){
                    move.is_facing = true;
                    move.facing[1].country = (Country)dstountry;
                    move.facing[0].country = (Country)srcCountry;
                    srcCountry = mMaxCountryCount;
                    break;
                }
            }
        }
    }
    pChess->SetPos(pos.y, pos.x);
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
    return move;
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
    mCountry[Wu_Country] = u8"吴";
    mCountry[Wei_Country] = u8"魏";
    mCountry[Shu_Country] = u8"蜀";
    mCountry[Han_Country] = u8"汉";
    mName[Chess::Type::Ma_Chess] = u8"馬";
    mName[Chess::Type::Shi_Chess] = u8"士";
    mName[Chess::Type::Pao_Chess] = u8"炮";
    mName[Chess::Type::Che_Chess] = u8"車";
    mName[Chess::Type::Bing_Chess] = u8"兵";
    mName[Chess::Type::Jiang_Chess] = u8"将";
    mName[Chess::Type::Xiang_Chess] = u8"相";

    mNumber[1] = u8"一";
    mNumber[2] = u8"二";
    mNumber[3] = u8"三";
    mNumber[4] = u8"四";
    mNumber[5] = u8"五";
    mNumber[6] = u8"六";
    mNumber[7] = u8"七";
    mNumber[8] = u8"八";
    mNumber[9] = u8"九";
    mNumber[10] = u8"十";
    mNumber[11] = u8"十一";
    mNumber[12] = u8"十二";
    mNumber[13] = u8"十三";
    mNumber[14] = u8"十四";
    mNumber[15] = u8"十五";
    mNumber[16] = u8"十六";
    mNumber[17] = u8"十七";
}

Game::~Game(){
}
bool Game::areKingsFacing(){
    bool facing = false;
    for (uint32_t srcCountry = 0; srcCountry < mMaxCountryCount; srcCountry++){
        for (uint32_t dstountry = 0; dstountry < mMaxCountryCount; dstountry++){
            if(srcCountry != dstountry && (Invald_Country == mNotAlliance || dstountry != GET_ALLIANCE_COUNTRY(srcCountry, mNotAlliance))){
                if(mChessboard.areKingsFacing((Country)srcCountry, (Country)dstountry)){
                    mChessboard.DestroyCountry((Country)srcCountry);
                    mChessboard.DestroyCountry((Country)dstountry);
                    srcCountry = mMaxCountryCount;
                    facing = true;
                    break;
                }
            }
        }
    }
    return facing;
}
const Chess *Game::Check(Country *sCountry) const{
    const Chess *pChess = nullptr;
    if(mChessboard.IsDeath(mCurrent))return nullptr;
    for (uint32_t srcCountry = 0; srcCountry < mMaxCountryCount; ++srcCountry){
        Country dstCountry = GetNextCountry(mCurrent);
        if(srcCountry != dstCountry){
            while (dstCountry != mCurrent){
                const Chess *pJiang = mChessboard.GetChess(dstCountry)[Chess::Type::Jiang_Chess];
                if(!pJiang)break;
                pChess = mChessboard.Check((Country)srcCountry, pJiang->GetRow(), pJiang->GetColumn());
                if(pChess){
                    if(sCountry)*sCountry = (Country)srcCountry;
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
        if(mChessboard.IsDeath((Country)i)){
            ++deathCount;
        }
    }
    bool hasEcxit = false;
    for (size_t i = 0; i < mMaxCountryCount; i++){
        if(mChessboard.IsHasExitPermission((Country)i)){
            hasEcxit = true;
            break;
        }       
    }
    return deathCount > mMaxCountryCount / 2 || !hasEcxit;
}
Country Game::GetNextCountry()const{
    return GetNextCountry(mCurrent);
}
Country Game::GetLastCountry() const{
    return GetLastCountry(mCurrent);
}
Country Game::GetLastCountry(Country currentCountry) const{
    Country country = currentCountry;
    do{
        country = (Country)((country - 1 + mMaxCountryCount) % mMaxCountryCount);
    } while (mChessboard.IsDeath(country));
    return country;
}
Country Game::GetNextCountry(Country country) const{
    do{
        country = (Country)((country + 1) % mMaxCountryCount);
    } while (mChessboard.IsDeath(country));
    return country;
}
void Game::InitinalizeGame(Country playerCountry, Country currentCountry){
    state.isGameStart = true;
    if(state.isControllable){
        mMaxCountryCount = MAX_COUNTRY_INDEX;
    }
    else{
        mMaxCountryCount = MAX_COUNTRY_INDEX - 1;
    }
    if(currentCountry != Invald_Country)
        mCurrent = currentCountry;
    else
        mCurrent = Wu_Country;
        // mCurrentCountry = rand()%mMaxCountryCount;
    if(playerCountry != Invald_Country)
        player.country = playerCountry;
    else
        player.country = (Country)(rand()%mMaxCountryCount);
    mNotAlliance = Invald_Country;
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
    mChessboard.SaveStep(GetSaveStep(pChess, dstRow, dstColumn));
    MoveChess(start, end, pChess->GetFontIndex(), pChess->GetCountry(), dynamicOffsets);
    if(pTarget){
        Country targetCountry = pTarget->GetCountry();
        mChessboard.CaptureChess(pChess, pTarget);
        if(mChessboard.IsDeath(targetCountry)){
            printf("%s国被%s国消灭\n", county[targetCountry], county[pChess->GetCountry()]);
            mChessboard.DestroyCountry(targetCountry);
            if(!state.isControllable){
                if(targetCountry == HAN_CHE_CHESS_COUNT){
                    //另外两个结盟
                    mNotAlliance = pChess->GetCountry();
                }
                else{
                    mNotAlliance = Invald_Country;
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
    // Country currerntCountry;//, lastCountry = GetLastCountry();
    //应该优先检查下一位势力是否被将
    const Chess *pCheck = Check(nullptr);
    // const Chess *pCheck = Check(&currerntCountry);
    if(pCheck){
        Country country = pCheck->GetCountry();
        //如果因为自己下的棋而被将，则不触发被将先走
        if(mCurrent != country){
            // if(currerntCountry == mCurrent && mCurrent != country){
            mCurrent = country;
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
    if(mNotAlliance != Invald_Country)return;
    glm::vec2 vPos[3];
    Territory territory = (Territory)GET_TERRITORY_INDEX(Han_Country, player.country);
    glm::vec2 center = mChessboard.GetPalacesCenter(territory);
    for (size_t i = 0; i < MAX_COUNTRY_INDEX; ++i){
        if((i != Han_Country) && mChessboard.IsDeath((Country)i)){
            //有势力阵亡，联盟就无效了
            return;
        }
    }
    //吴永远在中心点
    vPos[Wu_Country] = center;
    if(territory == Han_Territory){
        vPos[Wei_Country] = glm::vec2(center.x - 1, center.y + 1);
        vPos[Shu_Country] = glm::vec2(center.x - 1, center.y - 1);
    }
    else if(territory == Wei_Territory){
        vPos[Wei_Country] = glm::vec2(center.x - 1, center.y - 1);
        vPos[Shu_Country] = glm::vec2(center.x + 1, center.y - 1);
    }
    else if(territory == Shu_Territory){
        //理论上不可能，但算了，还是写上吧
        vPos[Wei_Country] = glm::vec2(center.x + 1, center.y - 1);
        vPos[Shu_Country] = glm::vec2(center.x - 1, center.y - 1);
    }
    else if(territory == Wu_Territory){
        vPos[Wei_Country] = glm::vec2(center.x + 1, center.y - 1);
        vPos[Shu_Country] = glm::vec2(center.x + 1, center.y + 1);
    }
    Country alliance = Invald_Country;
    for (size_t i = 0; i < MAX_COUNTRY_INDEX; ++i){
        if(vPos[i].x == column && vPos[i].y == row && country != i){
            alliance = (Country)i;
            break;
        }
    }
    if(alliance != Invald_Country && alliance != country){
        mNotAlliance = (Country)GET_NOT_ALLIANCE_COUNTRY(country, alliance);
        mChessboard.GetCountryChess(mNotAlliance, Han_Country);
        mChessboard.DestroyCountry(Han_Country);
    }
}
void Game::NextCountry(){
    mCurrent = GetNextCountry(mCurrent);
}
void Game::LastCountry(){
    mCurrent = GetLastCountry();
}
void Game::NewGame(Country playerCountry, Country currentCountry){
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
    player.country = Invald_Country;
    mCurrent = Invald_Country;
    mNotAlliance = Invald_Country;
    for (uint32_t uiCountry = 0; uiCountry < MAX_COUNTRY_INDEX; uiCountry++){
        mChessboard.DestroyCountry((Country)uiCountry);
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
