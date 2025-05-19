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
ChessMove Game::GetSaveStep(const Chessboard *pBoard, Chess *pChess, uint32_t dstRow, uint32_t dstColumn){
    ChessMove move = {};
    Country srcCountry = pChess->GetCountry();
    move.number = mStepNumber;
    if(pBoard->IsBoundary(dstRow, dstColumn)){
        move.chess.SetCountry(srcCountry);
        move.chess.SetPos(0, 0);
        sprintf(move.step, "x");
        return move;
    }
    Chess::Type srcChess = pChess->GetChess();
    const Chess *pTarget = pBoard->GetChess(dstRow, dstColumn);
    move.chess = *pChess;
    move.is_capture = pTarget;
    if(move.is_capture){
        move.captured = *pTarget;
        move.is_death = pTarget->GetChess() == Chess::Type::Jiang_Chess;
        if(move.is_death){
            move.death.country = pTarget->GetCountry();
            auto pCountryChess = pBoard->GetChess(move.death.country);
            for (size_t uiChess = 0; uiChess < DRAW_CHESS_COUNT; uiChess++){
                if(pCountryChess[uiChess])move.death.chess.push_back(*pCountryChess[uiChess]);
            }
        }
    }
    else{
        move.captured.SetPos(dstRow, dstColumn);
    }
    Country check;
    //为测试是否见面，需要一定的修改
    const glm::vec2 pos = glm::vec2(pChess->GetColumn(), pChess->GetRow());
    pChess->SetPos(dstRow, dstColumn);
    const Chess *pc = Check(&check);
    move.is_check = pc;
    move.is_facing = areKingsFacing(pBoard, move.facing[0].country, move.facing[1].country);
    pChess->SetPos(pos.y, pos.x);
    if(move.is_facing){
        for (uint32_t i = 0; i < move.facing.size(); ++i){
            auto pCountryChess = pBoard->GetChess(move.facing[i].country);
            for (size_t uiChess = 0; uiChess < DRAW_CHESS_COUNT; uiChess++){
                if(pCountryChess[uiChess]){
                    move.facing[i].chess.push_back(*pCountryChess[uiChess]);
                }   
            }        
        }
    }
    char step[MAX_BYTE];
    uint32_t srcNumber, dstNumber;
    Territory srcTerritory = pChess->GetTerritory();
    if(srcTerritory == Han_Territory){
        srcNumber = MAX_CHESSBOARD_LINE - pChess->GetRow();
        if(dstColumn < pChess->GetColumn()){
            strcpy(step, "退");
        }
        else if(dstColumn > pChess->GetColumn()){
            strcpy(step, "进");
        }
        else{
            strcpy(step, "平");
        }
        if(srcChess == Chess::Type::Ma_Chess || srcChess == Chess::Type::Xiang_Chess || srcChess == Chess::Type::Shi_Chess){
            dstNumber = MAX_CHESSBOARD_LINE - dstRow;
        }
        else{
            dstNumber = strcmp(step, "平")?abs(pChess->GetColumn() - dstColumn):MAX_CHESSBOARD_LINE - dstRow;
        }
    }
    else if(srcTerritory == Wu_Territory){
        srcNumber = pChess->GetRow() + 1;
        if(dstColumn > pChess->GetColumn()){
            strcpy(step, "退");
        }
        else if(dstColumn < pChess->GetColumn()){
            strcpy(step, "进");
        }
        else{
            strcpy(step, "平");
        }
        if(srcChess == Chess::Type::Ma_Chess || srcChess == Chess::Type::Xiang_Chess || srcChess == Chess::Type::Shi_Chess){
            dstNumber =  dstRow + 1;
        }
        else{
            dstNumber = strcmp(step, "平")?abs(pChess->GetColumn() - dstColumn):dstRow + 1;
        }
    }
    else if(srcTerritory == Shu_Territory){
        srcNumber = MAX_CHESSBOARD_LINE - pChess->GetColumn();
        if(dstRow > pChess->GetRow()){
            strcpy(step, "退");
        }
        else if(dstRow < pChess->GetRow()){
            strcpy(step, "进");
        }
        else{
            strcpy(step, "平");
        }
        if(srcChess == Chess::Type::Ma_Chess || srcChess == Chess::Type::Xiang_Chess || srcChess == Chess::Type::Shi_Chess){
            dstNumber = MAX_CHESSBOARD_LINE - dstColumn;
        }
        else{
            dstNumber = strcmp(step, "平")?abs(pChess->GetRow() - dstRow):MAX_CHESSBOARD_LINE - dstColumn;
        }
    }
    else{
        srcNumber = pChess->GetColumn() + 1;
        if(dstRow < pChess->GetRow()){
            strcpy(step, "退");
        }
        else if(dstRow > pChess->GetRow()){
            strcpy(step, "进");
        }
        else{
            strcpy(step, "平");
        }
        if(srcChess == Chess::Type::Ma_Chess || srcChess == Chess::Type::Xiang_Chess || srcChess == Chess::Type::Shi_Chess){
            dstNumber =  dstColumn;
        }
        else{
            dstNumber = strcmp(step, "平")?abs(pChess->GetRow() - dstRow):dstColumn + 1;
        }
    }
    //修改或取消number的显示，要注意imgui那边已经用了strstr
    sprintf(move.step, "%s%d%s%d_%d", mChessName[srcChess].c_str(),  srcNumber, step, dstNumber, move.number);

    if(move.is_check){
        sprintf(move.notation, "%s将%s", mCountryName[check].c_str(), mCountryName[pc->GetCountry()].c_str());
    }
    if(move.is_facing){
        sprintf(move.notation, "%s%s见面", mCountryName[move.facing[0].country].c_str(), mCountryName[move.facing[1].country].c_str());
    }
    if(move.is_capture){
        sprintf(move.notation, "%s%s%s%s", mCountryName[move.chess.GetCountry()].c_str(), mChessName[move.chess.GetChess()].c_str(), mCountryName[move.captured.GetCountry()].c_str(), mChessName[move.captured.GetChess()].c_str());
    }
    if(move.is_death){
        sprintf(move.notation, "%s灭%s", mCountryName[srcCountry].c_str(), mCountryName[move.death.country].c_str());
    }
    if(move.notAlliance != Invald_Country){
        Country aliance = Invald_Country;
        if(pChess->GetCountry() != move.notAlliance){
            aliance = (Country)GET_NOT_ALLIANCE_COUNTRY(pChess->GetCountry(), move.notAlliance);
        }
        sprintf(move.notation, "%s%s结盟", mCountryName[aliance].c_str(), mCountryName[move.death.country].c_str());
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
    mCountryName[Wu_Country] = "吴";
    mCountryName[Wei_Country] = "魏";
    mCountryName[Shu_Country] = "蜀";
    mCountryName[Han_Country] = "汉";
    mChessName[Chess::Type::Ma_Chess] = "馬";
    mChessName[Chess::Type::Shi_Chess] = "士";
    mChessName[Chess::Type::Pao_Chess] = "炮";
    mChessName[Chess::Type::Che_Chess] = "車";
    mChessName[Chess::Type::Bing_Chess] = "兵";
    mChessName[Chess::Type::Jiang_Chess] = "将";
    mChessName[Chess::Type::Xiang_Chess] = "相";

    // mNumberName[0] = "零";
    // mNumberName[1] = "一";
    // mNumberName[2] = "二";
    // mNumberName[3] = "三";
    // mNumberName[4] = "四";
    // mNumberName[5] = "五";
    // mNumberName[6] = "六";
    // mNumberName[7] = "七";
    // mNumberName[8] = "八";
    // mNumberName[9] = "九";
    // mNumberName[10] = "十";
    // mNumberName[11] = "十一";
    // mNumberName[12] = "十二";
    // mNumberName[13] = "十三";
    // mNumberName[14] = "十四";
    // mNumberName[15] = "十五";
    // mNumberName[16] = "十六";
    // mNumberName[17] = "十七";
}

Game::~Game(){
}
bool Game::areKingsFacing(Country&srcCountry, Country&dstCountry){
    return areKingsFacing(&mChessboard, srcCountry, dstCountry);
}
bool Game::areKingsFacing(const Chessboard *pBoard, Country &sCountry, Country &dCountry){
    bool facing = false;
    for (uint32_t srcCountry = 0; srcCountry < mMaxCountryCount; srcCountry++){
        for (uint32_t dstCountry = 0; dstCountry < mMaxCountryCount; dstCountry++){
            if(srcCountry != dstCountry && (Invald_Country == mNotAlliance || dstCountry != GET_ALLIANCE_COUNTRY(srcCountry, mNotAlliance))){
                if(pBoard->areKingsFacing((Country)srcCountry, (Country)dstCountry)){
                    facing = true;
                    dCountry = (Country)dstCountry;
                    sCountry = (Country)srcCountry;
                    srcCountry = mMaxCountryCount;
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
    mStepNumber = 0;
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
void Game::PlayChess(Chess *pChess, uint32_t dstRow, uint32_t dstColumn, bool skipAnim){
    // const char county[][MAX_BYTE] = { "蜀", "吴", "魏", "汉" };
    const uint32_t dynamicOffsets = ROW_COLUMN_TO_INDEX(pChess->GetCountry(), pChess->GetChessOffset(), DRAW_CHESS_COUNT);
    const glm::vec2 start = glm::vec2(CHESS_COLUMN_TO_X(pChess->GetColumn()), CHESS_ROW_TO_Y(pChess->GetRow())), end = glm::vec2(CHESS_COLUMN_TO_X(dstColumn), CHESS_ROW_TO_Y(dstRow));
    // Chess chess = *pChess;
    ChessMove move = GetSaveStep(&mChessboard, pChess, dstRow, dstColumn);
    if(!skipAnim){
        mMutex.lock();
        MoveChess(start, end, pChess->GetFontIndex(), pChess->GetCountry(), dynamicOffsets);
    }
    if(!state.isReplay){
        mChessboard.DeleteFollowingStep(mStepNumber);
    }
    Country srcCountry = mChessboard.PlayChess(pChess, dstRow, dstColumn), dstCountry;
    if(!state.isControllable && srcCountry != Invald_Country){
        if(srcCountry == Han_Country){
            move.notAlliance = pChess->GetCountry();
            //另外两个结盟
            mNotAlliance = move.notAlliance;
        }
        else{
            mNotAlliance = Invald_Country;
        }
    }
    if(areKingsFacing(srcCountry, dstCountry)){
        printf("两将见面了\n");
        mChessboard.DestroyCountry(srcCountry);
        mChessboard.DestroyCountry(dstCountry);
    }
    if(!skipAnim){
        UpdateChessUniform(vulkan.device.device);
        mMutex.unlock();    
    }
    if(!state.isControllable && pChess->GetChess() == Chess::Type::Ma_Chess){
        move.notAlliance = mNotAlliance;
        //不用提示了。因为结盟后，汉的棋子会归另一方
        SetNotAllianceCountry(pChess->GetCountry(), pChess->GetRow(), pChess->GetColumn());
    }
    //应该优先检查下一位势力是否被将
    const Chess *pCheck = Check(&srcCountry);
    if(pCheck){
        Country country = pCheck->GetCountry();
        //如果因为自己下的棋而被将，则不触发被将先走//调用GetNextCountry主要是为了不误判论空
        if(mCurrent != country && country != GetNextCountry(pChess->GetCountry())){
            //吴将魏蜀
            const uint32_t skip_count = GET_SKIP_COUNTRY_COUNT(srcCountry, pCheck->GetCountry(), mMaxCountryCount);
            for (size_t i = 0; i < skip_count; i++){
                Chess c;
                srcCountry = GetNextCountry(srcCountry);
                c.SetCountry(srcCountry);
                mChessboard.SaveStep(GetSaveStep(&mChessboard, &c, 0, 0));
            }
            mCurrent = country;
        }
        else{
            NextCountry();
        }
    }
    else{
        NextCountry();
    }
    mChessboard.SaveStep(move);
    ++mStepNumber;
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
// void Game::SetStepNumber(uint32_t number){
//     mStepNumber = 0;
//     auto record = mChessboard.GetRecord();
//     InitinalizeGame(player.country);
//     uint32_t n = 0;
//     for (auto it = record.begin(); it != record.end() && n <= number; ++it, ++n){
//         Chess *pStart = mChessboard.GetChess(it->chess.GetRow(), it->chess.GetColumn());
//         if(!mChessboard.IsBoundary(it->chess.GetRow(), it->chess.GetColumn())){
//             PlayChess(pStart, it->captured.GetRow(), it->captured.GetColumn(), true);
//         }
//     }
// }
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
