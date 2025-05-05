#include <array>
#include <thread>
#include <chrono>
#include <iostream>

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

#include "Ai.h"
#include "Game.h"
#include "OnLine.h"
#include "Pipeline.hpp"
#include "VulkanImgui.h"
#include "VulkanWindow.h"
#include "imgui_impl_glfw.h"
VulkanPool g_VulkanPool;
VulkanQueue g_VulkanQueue;
VulkanDevice g_VulkanDevice;
VulkanWindow g_VulkanWindow;
VulkanSynchronize g_VulkanSynchronize;

uint32_t g_WindowWidth = (CHESSBOARD_RECT_COUNT + 2) * CHESSBOARD_RECT_SIZE + MAX_CHESSBOARD_LINE * CHESSBOARD_LINE_WIDTH, g_WindowHeight = g_WindowWidth;

VulkanImGui g_VulkanImGui;
VkCommandBuffer g_CommandBuffers;

VkPipelineCache g_PipelineCache;
VkPipelineLayout g_PipelineLayout;
VkDescriptorSetLayout g_SetLayout;
VkPipeline g_Fill, g_Wireframe, g_Font;

Ai g_Ai;
Game g_Game;
OnLine g_OnLine;
ImGuiInput g_ImGuiInput;
glm::vec2 g_JiangPos[MAX_COUNTRY_INDEX];//目前只用于RotateChess
std::array<Player, MAX_COUNTRY_INDEX>g_Players;
//用数组是为了在联机模式下显示和控制其他玩家的国籍;目前只能选择ai的国籍
std::array<std::vector<std::string>, MAX_COUNTRY_INDEX>g_CountryItems;
void ResetCountryItem(std::vector<std::string>&countryItems){
    std::vector<std::string>country = { "?", "魏", "蜀", "吴" };
    if(g_Game.IsControllable()){
        country.push_back("汉");
    }
    if(countryItems.size() != country.size())countryItems.resize(country.size());
    for (size_t i = 0; i < countryItems.size(); ++i){
        countryItems[i] = country[i];
    }
}
void ShowPlayerCountryCombo(){
    static std::string currentCountryItem = g_CountryItems[0][0];
    if(ImGui::BeginCombo("自选势力", currentCountryItem.c_str())){
        for (auto countryIt = g_CountryItems[0].begin(); countryIt != g_CountryItems[0].end(); ++countryIt){
            bool is_selected = currentCountryItem == *countryIt;
            if (ImGui::Selectable(countryIt->c_str(), is_selected)){
                currentCountryItem = *countryIt;
                if(currentCountryItem == "?"){
                    g_Game.NewGame();
                }
                else if(currentCountryItem == "魏"){
                    g_Game.NewGame(WEI_COUNTRY_INDEX);
                }
                else if(currentCountryItem == "蜀"){
                    g_Game.NewGame(SHU_COUNTRY_INDEX);
                }
                else if(currentCountryItem == "吴"){
                    g_Game.NewGame(WU_COUNTRY_INDEX);
                }
                else if(currentCountryItem == "汉"){
                    g_Game.NewGame(HAN_COUNTRY_INDEX);
                }
                break;
            }
        }
        ImGui::EndCombo();
    }
}
void RandomNumber(uint32_t max, uint32_t count, Player *result, uint32_t min = 0){
#if _MSVC_LANG >= 202002L || __cplusplus >= 202002L
    std::vector<Player> v(count);
    std::random_device rd;
    std::iota(v.begin(), v.end(), 0);
    std::ranges::shuffle(v, rd);
    memcpy(result, v.data(), sizeof(Player) * count);
#else
    for (size_t i = 0; i < count; ++i){
        if(result[i].random)result[i].uCountry = rand() % max + min;
        for (size_t j = 0; j < i; ++j){
            if(result[i].uCountry == result[j].uCountry){
                --i;
                continue;
            }
        }
    }
#endif
}
uint32_t countryToindex(const char *country){
    if(!strcmp(country, "魏")){
        return WEI_COUNTRY_INDEX;
    }
    else if(!strcmp(country, "蜀")){
        return SHU_COUNTRY_INDEX;
    }
    else if(!strcmp(country, "吴")){
        return WU_COUNTRY_INDEX;
    }
    else if(!strcmp(country, "汉")){
        return HAN_COUNTRY_INDEX;
    }
    return INVALID_COUNTRY_INDEX;
}
void RandomCountry(){
    Player countryIndex[MAX_COUNTRY_INDEX];
    const char *countryName[MAX_COUNTRY_INDEX];
    countryName[WU_COUNTRY_INDEX] = "吴";
    countryName[WEI_COUNTRY_INDEX] = "魏";
    countryName[SHU_COUNTRY_INDEX] = "蜀";
    countryName[HAN_COUNTRY_INDEX] = "汉";
    uint32_t playerIndex = 0;
    for (auto it = g_Players.begin(); it != g_Players.end(); ++it, ++playerIndex){
        if(strcmp(it->country, "?")){
            countryIndex[playerIndex].random = false;
            countryIndex[playerIndex].uCountry = countryToindex(it->country);
        }
        else{
            countryIndex[playerIndex].random = true;
        }
    }
    const uint32_t countryCount = g_Game.GetCountryCount();
    RandomNumber(countryCount, countryCount, countryIndex);
    playerIndex = 0;
    for (auto it = g_Players.begin(); it != g_Players.end() && playerIndex < g_Game.GetCountryCount(); ++playerIndex, ++it){
        strcpy(it->country, countryName[countryIndex[playerIndex].uCountry]);
        it->uCountry = countryIndex[playerIndex].uCountry;    
    }
}
void SendPlayersInfoation(){
    for (uint32_t uiPlayer = 0; uiPlayer < g_Players.size(); ++uiPlayer){
        if(g_Players[uiPlayer].uCountry != INVALID_PLAYER_INDEX){
            g_OnLine.SendPlayersInfoation(uiPlayer, &g_Players[uiPlayer]);
        }
    }
}
auto CreateThread(void *(*__start_routine)(void *), void *__arg){
    #ifdef WIN32
    DWORD  threadId;
    HANDLE pthreadId;
    pthreadId = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)__start_routine, __arg, 0, &threadId);
#endif
#ifdef __linux
    pthread_t pthreadId;
    pthread_create(&pthreadId, nullptr, __start_routine, __arg);
#endif
    return pthreadId;
}
//不知道为什么，玩家离开事件中，发送游戏结束事件，该函数一直收不到
//但process_client函数能收到, 我怀疑是直接绕过该函数发到process_client函数, 但为什么呢?
void *process_server(void *userData){
    GameMessage message;
    uint32_t socketindex = *(SOCKET *)userData;
    printf("function %s start, socketindex is %d\n", __FUNCTION__, socketindex);
    do{
        g_OnLine.RecvFromClient(socketindex, &message, sizeof(message));
        g_OnLine.SendToAllClient(&message, sizeof(message));
        if(message.event == PLAYER_EXIT_GAME_EVENT && socketindex == message.clientIndex)break;
    } while (message.event != GAME_OVER_GAME_EVENT);
    printf("function %s end, socketindex is %d\n", __FUNCTION__, socketindex);
    return nullptr;
}
void *server_start(void *userData){
    GameMessage message;
    static uint32_t socketIndex[] = { 0, 1, 2, 3 };
    for (uint32_t i = 0; i < g_Game.GetCountryCount(); ++i){
        if(INVALID_SOCKET != g_OnLine.AcceptClient(i, &message, sizeof(message))){
            if(message.event == AI_JOIN_GAME_GAME_EVENT){
                g_Players[i].ai = true;
                g_OnLine.ShutdownClient(i);
                strcpy(g_Players[i].name, "ai");
            }
            else{
                sprintf(g_Players[i].name, "player%d", i + 1);
            }
            //调用这个函数，期望的是能告诉所有客户端，有新客户端加入，并且会把加入的客户端信息发送给所有客户端
            //然后单独给刚加入的客户端发送所有玩家信息
            g_OnLine.SendClientInfoation(i, &g_Players[i]);
            for (uint32_t uiClientIndex = 0; uiClientIndex < i; ++uiClientIndex){
                g_OnLine.SendClientInfoation(uiClientIndex, &g_Players[uiClientIndex]);
            }
            if(message.event != AI_JOIN_GAME_GAME_EVENT)CreateThread(process_server, &socketIndex[i]);
        }
    }
    if(g_Game.IsControllable()){
        message.event = ENABLE_HANG_GAME_EVENT;
        g_OnLine.SendToAllClient(&message, sizeof(GameMessage));
    }
    RandomCountry();
    SendPlayersInfoation();
    g_OnLine.SendCurrentCountry(WU_COUNTRY_INDEX);
    //游戏开始, 发消息告诉所有客户端游戏开始
    message.event = GAME_START_GAME_EVENT;
    g_OnLine.SendToAllClient(&message, sizeof(message));
    return nullptr;
}
void InitJiangPos(){
    const Chessboard *pBoard = g_Game.GetChessboard();
    const Chess *pChess = pBoard->GetChess(WEI_COUNTRY_INDEX)[Chess::Type::Jiang_Chess];
    g_JiangPos[WEI_COUNTRY_INDEX] = glm::vec2(pChess->GetColumn(), pChess->GetRow());
    pChess = pBoard->GetChess(SHU_COUNTRY_INDEX)[Chess::Type::Jiang_Chess];
    g_JiangPos[SHU_COUNTRY_INDEX] = glm::vec2(pChess->GetColumn(), pChess->GetRow());
    pChess = pBoard->GetChess(WU_COUNTRY_INDEX)[Chess::Type::Jiang_Chess];
    g_JiangPos[WU_COUNTRY_INDEX] = glm::vec2(pChess->GetColumn(), pChess->GetRow());
    pChess = pBoard->GetChess(HAN_COUNTRY_INDEX)[Chess::Type::Jiang_Chess];
    g_JiangPos[HAN_COUNTRY_INDEX] = glm::vec2(pChess->GetColumn(), pChess->GetRow());
}
void Invert(uint32_t row, uint32_t column, uint32_t&newRow, uint32_t&newColumn){
    newRow = abs((float)CHESSBOARD_ROW - row);
    newColumn = abs((float)CHESSBOARD_COLUMN - column);
}
//country为下棋的国家
void RotateChess(uint32_t country, uint32_t row, uint32_t column, uint32_t&newRow, uint32_t&newColumn){
    uint32_t uiCountry = country;
    const uint32_t clientIndex = g_OnLine.GetClientIndex();
    for (size_t i = 0; i < g_Game.GetCountryCount(); ++i){
        if(g_Players[i].ai && g_Players[i].uCountry == country){
            uiCountry = g_Players[0].uCountry;
            break;
        }
    }
    if(uiCountry != g_Players[clientIndex].uCountry){
        //因为ai下棋的位置和服务端一样，所以必须用服务端的国家索引
        if(g_JiangPos[uiCountry].x == g_JiangPos[g_Players[clientIndex].uCountry].x){
            Invert(row, column, newRow, newColumn);
        }
        else{
            newRow = column;
            newColumn = row;
            //要获得country在哪个territory, 但不知道行列
            const uint32_t territory = GET_TERRITORY_INDEX(uiCountry, g_Players[clientIndex].uCountry);
            if(HAN_COUNTRY_INDEX == territory){
                newColumn = abs((float)newColumn - CHESSBOARD_COLUMN);
            }
            else{
                newRow = abs((float)newRow - CHESSBOARD_ROW);
            }
        }
    }
}
void DeleteCountryItem(const char *country, std::vector<std::string>&countryItems){
    const std::string newCountry[] = { "?", "魏", "蜀", "吴", "汉" };
    uint32_t count = IM_ARRAYSIZE(newCountry) - 1;
    if(g_Game.IsControllable()){
        ++count;
    }
    for (size_t i = 0; i < count; ++i){
        if(newCountry[i] == country){
            countryItems[i] =countryItems[0];
            break;
        }
    }
}
void PlayerChangeCountry(uint32_t clientIndex, const char *country){
    strcpy(g_Players[clientIndex].country, country);
    // g_Players[clientIndex].random = !strcmp(country, "?");
    const uint32_t client = g_OnLine.GetClientIndex();
    ResetCountryItem(g_CountryItems[client]);
    for (auto it = g_Players.begin(); it != g_Players.end(); ++it){
        if(strcmp(it->country, "?")){
            DeleteCountryItem(it->country, g_CountryItems[client]);
        }
    }
    if(g_OnLine.IsServer()){
        auto aiClientIndex = g_OnLine.GetAiClientIndex();
        for (size_t i = 0; i < 2; ++i){
            if(aiClientIndex[i] != INVALID_PLAYER_INDEX)
                DeleteCountryItem(country, g_CountryItems[aiClientIndex[i]]);
        }
    }
}
void *process_client(void *userData){
    GameMessage message;
    do{
        g_OnLine.ClientRecvFrom(&message, sizeof(GameMessage));
        if(message.event == GAME_START_GAME_EVENT){
            g_Game.EnableOnline();
            uint32_t aiIndex = 1, index = 0;
            for (auto it = g_Players.begin() + 1; it != g_Players.end(); ++it, ++aiIndex){
                if(it->ai){
                    g_OnLine.SetAiClientIndex(index, aiIndex);
                    ++index;
                }
            }
            g_Game.StartGame();
            const uint32_t clientIndex = g_OnLine.GetClientIndex();
            printf("game start, current country is %d, you country index is %d\n", g_Game.GetCurrentCountry(), g_Players[clientIndex].uCountry);
            g_Game.NewGame(g_Players[clientIndex].uCountry, g_Game.GetCurrentCountry());
            InitJiangPos();
        }
        else if(message.event == ENABLE_HANG_GAME_EVENT){
            g_Game.EnableHan();
        }
        else if(message.event == JOINS_GAME_GAME_EVENT){
            const uint32_t clientIndex = message.clientIndex;
            memcpy(&g_Players[clientIndex], &message.player, sizeof(Player));
            PlayerChangeCountry(clientIndex, message.player.country);
            printf("player joins game:name %s, country %s\n", g_Players[clientIndex].name, g_Players[clientIndex].country);
        }
        else if(message.event == CURRENT_COUNTRY_GAME_EVENT){
            g_Game.SetCurrentCountry(message.player.uCountry);
            printf("current country %d\n", message.player.uCountry);
        }
        else if(message.event == CHANGE_PLAYER_COUNTRY_GAME_EVENT){
            PlayerChangeCountry(message.clientIndex, message.player.country);
        }
        else if(message.event == SELF_CLIENT_INFORMATION_GAME_EVENT){
            g_OnLine.SetClientIndex(message.clientIndex);
        }
        else if(message.event == CHANGE_PLAYER_INFORMATION_GAME_EVENT){
            g_Players[message.clientIndex].ai = message.player.ai;
            strcpy(g_Players[message.clientIndex].name, message.player.name);
            g_Players[message.clientIndex].uCountry = message.player.uCountry;
            strcpy(g_Players[message.clientIndex].country, message.player.country);
        }
        else if(message.event == PLAY_CHESS_GAME_EVENT){
            auto pBoard = g_Game.GetChessboard();
            auto pChess = pBoard->GetChess(message.select.GetCountry());
            uint32_t dstRow = message.target.GetRow(), dstColumn = message.target.GetColumn();
            Chess *pStart = pChess[message.select.GetChessOffset()], *pTarget = nullptr;
            if(message.target.GetCountry() >= MAX_COUNTRY_INDEX){
                RotateChess(g_Game.GetCurrentCountry(), dstRow, dstColumn,dstRow, dstColumn);
            }
            else{
                pChess = pBoard->GetChess(message.target.GetCountry());
                pTarget = pChess[message.target.GetChessOffset()];
                dstRow = pTarget->GetRow();
                dstColumn = pTarget->GetColumn();
            }
            g_Game.PlayChess(pStart, dstRow, dstColumn);
            if(!g_Ai.IsEnd()){
                if(g_Game.GameOver()){
                    g_Ai.End();
                }
                g_Ai.EnableNextCountry(g_ImGuiInput.enableAutoPlay);
                g_Ai.SyncBoardCopy(pStart, dstRow, dstColumn);    
            }
        }
        else if(message.event == PLAYER_EXIT_GAME_EVENT){
            printf("player exit, client index %d\n", message.clientIndex);
            if(g_OnLine.IsServer()){
                g_OnLine.ShutdownClient(message.clientIndex);
                const uint32_t playerCount = g_OnLine.GetPlayerCount();
                if(playerCount > 1){//因为服务端本身还创建了一个客户端
                    message.event = CHANGE_PLAYER_INFORMATION_GAME_EVENT;
                    message.player.ai = true;
                    strcpy(message.player.name, "ai");
                    message.player.uCountry = g_Players[message.clientIndex].uCountry;
                    strcpy(message.player.country, g_Players[message.clientIndex].country);
                }
                else{
                    message.event = GAME_OVER_GAME_EVENT;
                }
                g_OnLine.SendToAllClient(&message, sizeof(message));
                message.event = INVALID_GAMER_EVENT;
            }
            else if(message.clientIndex == g_OnLine.GetClientIndex()){
                g_Ai.End();
                g_OnLine.Cleanup();
                g_ImGuiInput.Init();
                g_Game.EndGame();
                for (uint32_t i = 0; i < g_Players.size(); ++i){
                    g_Players[i].ai = false;
                    memset(g_Players[i].name, 0, strlen(g_Players[i].name));
                    memset(g_Players[i].country, 0, strlen(g_Players[i].country));
                }        
                break;
            }
        }
        else if(message.event == GAME_OVER_GAME_EVENT){
            printf("game over, client indec is %d\n", message.clientIndex);
            g_Ai.End();
            g_OnLine.Cleanup();
            g_ImGuiInput.Init();
            g_Game.EndGame();
            for (uint32_t i = 0; i < g_Players.size(); ++i){
                g_Players[i].ai = false;
                memset(g_Players[i].name, 0, strlen(g_Players[i].name));
                memset(g_Players[i].country, 0, strlen(g_Players[i].country));
            }
        }
        else{
            printf("invalid event:%d\n", message.event);
        }
    }while(message.event != GAME_OVER_GAME_EVENT);
    printf("function %s end\n", __FUNCTION__);
    return nullptr;
}
void CreateClient(const char *serverIp){
    if(g_OnLine.CreateClient(serverIp)){
        CreateThread(process_client, nullptr);
        g_OnLine.JoinsGame();
    }
}
void CreateServer(){
    static const uint32_t countryCount = g_Game.GetCountryCount();
    if(g_OnLine.CeateServer(countryCount)){
        //必须用静态变量, 局部变量在函数结束后就销毁, 会导致传餐错误
        CreateThread(server_start, nullptr);
        CreateClient(g_OnLine.GetLocalIp().c_str());
    }
}
void catspace(char *dst, uint32_t count){
    for (size_t i = 0; i < count; ++i){
        strcat(dst, " ");
    }
}
void ShowPlayerCountryCombo(uint32_t comboIndex, const char *country){
    char comboName[MAX_BYTE] = " ";
    const uint32_t clientIndex = g_OnLine.GetClientIndex();
    static const auto aiClientIndex = g_OnLine.GetAiClientIndex();
    catspace(comboName, comboIndex);
    if(!g_Game.IsGameStart() && (comboIndex == clientIndex || (g_OnLine.IsServer() && (comboIndex == aiClientIndex[0] || comboIndex == aiClientIndex[1])))){
        static std::string currentCountryItem = g_CountryItems[comboIndex][0];
        if(ImGui::BeginCombo(comboName, currentCountryItem.c_str())){
            for (auto countryIt = g_CountryItems[comboIndex].begin(); countryIt != g_CountryItems[comboIndex].end(); ++countryIt){
                bool is_selected = currentCountryItem == *countryIt;
                if (ImGui::Selectable(countryIt->c_str(), is_selected)){
                    currentCountryItem = *countryIt;
                    printf("玩家更改势力, 新势力为%s\n", currentCountryItem.c_str());
                    g_OnLine.SendCountryInfomation(comboIndex, currentCountryItem.c_str());
                    break;
                }
            }
            ImGui::EndCombo();
        }
    }
    else{
        //这里只给看不给改
        if(ImGui::BeginCombo(comboName, country)){
            ImGui::EndCombo();
        }
    }
}
void ShowGameWidget(){
    const char *countryName[MAX_COUNTRY_INDEX];
    countryName[WU_COUNTRY_INDEX] = "吴";
    countryName[WEI_COUNTRY_INDEX] = "魏";
    countryName[SHU_COUNTRY_INDEX] = "蜀";
    countryName[HAN_COUNTRY_INDEX] = "汉";
    if(g_OnLine.IsServer()){
        if(ImGui::Checkbox("启用AI", &g_ImGuiInput.enableAi)){
            if(g_Game.IsGameStart()){
                if(g_ImGuiInput.enableAi){
                    if(!g_Ai.IsEnd()){
                        g_Ai.End();
                    }
                    g_Ai.CreatePthread(&g_Game, &g_OnLine);
                }
                else{
                    g_Ai.End();
                }
            }
        }
    }
    const uint32_t currentCountry = g_Game.GetCurrentCountry(), clientIndex = g_OnLine.GetClientIndex();
    if(currentCountry != INVALID_COUNTRY_INDEX && clientIndex < g_Game.GetCountryCount()){
        ImGui::Text("你是%s, 该%s下棋\n", countryName[g_Players[clientIndex].uCountry], g_Players[clientIndex].uCountry == currentCountry?"你":countryName[currentCountry]);
    }
    if(ImGui::Button("返回主菜单")){
        g_OnLine.ExitGame();
    }
}
void ShowClientWidget(){
    if(g_Game.IsGameStart()){
        ShowGameWidget();
    }
    if(ImGui::BeginTable("player information", 2)){
        ImGui::TableNextColumn();
        ImGui::Text("名字");
        for (auto it = g_Players.begin(); it != g_Players.end(); ++it){
            if (strcmp(it->name, ""))
                ImGui::Text(it->name);
        }
        ImGui::TableNextColumn();
        ImGui::Text("势力");
        uint32_t comboIndex = 0;
        //combo(或许其他也是)需要靠label查找。所以label不能重复
        for (auto it = g_Players.begin(); it != g_Players.end(); ++it, ++comboIndex){
            if (strcmp(it->name, "")){
                ShowPlayerCountryCombo(comboIndex, it->country);
            }
        }
        ImGui::EndTable();
    }    
}
uint32_t GetAiCount(){
    uint32_t aiCount = 0;
    for (auto it = g_Players.begin(); it != g_Players.end(); ++it){
        if(it->ai){
            ++aiCount;
            break;
        }
    }
    return aiCount;
}
void ShowServerWidget(){
    //在非广播模式下，直接显示本机ip。(或许广播模式下也可以显示)
    if(!g_Game.IsGameStart()){
        static std::string localIp = g_OnLine.GetLocalIp();
        ImGui::Text("本机ip:%s", localIp.c_str());    
            if(ImGui::Button("添加电脑")){
            const uint32_t aiCount = GetAiCount();
            //减掉一个服务端玩家和一个客户端玩家
            if(aiCount < g_Game.GetCountryCount() - 2)g_OnLine.AddAi(localIp.c_str());
        }
    }
    ShowClientWidget();
}
bool ShowConnectServerInterface(){
    bool show = true;
    if(ImGui::Begin("连接服务端")){
        static char serverIp[MAX_BYTE] = {0};
        ImGui::InputText("服务端ip", serverIp, sizeof(serverIp));
        if(ImGui::BeginTable("确定取消按钮", 3)){
            ImGui::TableNextColumn();
            if(ImGui::Button("确定")){
                show = false;
                CreateClient(serverIp);
            }
            ImGui::TableNextColumn();
            if(ImGui::Button("使用本机ip连接")){
                strcpy(serverIp, g_OnLine.GetLocalIp().c_str());
                // show = false;
                // CreateClient(g_ServerIp);
            }
            ImGui::TableNextColumn();
            if(ImGui::Button("取消")){
                show = false;
            }
            ImGui::EndTable();
        }
        ImGui::End();
    }
    return show;
}
bool ShowOnlineModeMainInterface(bool *mainInterface){
    bool ret = false;
    static bool bShowConnectServer;
    if(ImGui::Begin("局域网联机")){
        if(g_OnLine.IsServer()){
            ShowServerWidget();
        }
        else{
            ShowClientWidget();
        }
        if(!g_OnLine.IsServer() && !g_Game.IsGameStart()){
            static bool enableHan;
            if(ImGui::Checkbox("启用汉势力", &enableHan)){
                if(enableHan){
                    g_Game.EnableHan();
                }
                else{
                    g_Game.DiscardHan();
                }
            }
            //如果创建好房间或连接了客户端后, 不应该继续显示下面的按钮
            if(ImGui::BeginTable("客户端信息", 2)){
                ImGui::TableNextColumn();
                if(ImGui::Button("创建服务端")){
                    CreateServer();
                }
                ImGui::TableNextColumn();
                if(ImGui::Button("连接服务端")){
                    bShowConnectServer = true;
                }
                ImGui::EndTable();
            }
        }
        if(ImGui::Button("返回")){
            ret = true;
            *mainInterface = true;
            if(g_Game.IsGameStart())g_Game.EndGame();
        }
    }
    ImGui::End();
    if(bShowConnectServer){
        bShowConnectServer = ShowConnectServerInterface();
    }
    return !ret;
}
bool ShowSinglePlayerModeMainInterface(bool *mainInterface){
    bool ret = false;
    const char *countryName[MAX_COUNTRY_INDEX];
    countryName[WU_COUNTRY_INDEX] = "吴";
    countryName[WEI_COUNTRY_INDEX] = "魏";
    countryName[SHU_COUNTRY_INDEX] = "蜀";
    countryName[HAN_COUNTRY_INDEX] = "汉";
    const uint32_t currentCountry = g_Game.GetCurrentCountry(), player = g_Game.GetPlayerCountry();
    if(ImGui::Begin("单人模式")){
        if(currentCountry != INVALID_COUNTRY_INDEX && player != INVALID_COUNTRY_INDEX){
            ImGui::Text("你是%s, 该%s下棋\n", countryName[player], player == currentCountry?"你":countryName[currentCountry]);
        }
        if(ImGui::Button("新游戏")){
            g_Game.NewGame();
        }
        if(g_Game.IsGameStart()){
            if(ImGui::Checkbox("启用AI", &g_ImGuiInput.enableAi)){
                if(g_ImGuiInput.enableAi){
                    if(!g_Ai.IsEnd()){
                        g_Ai.End();
                    }
                    g_Ai.CreatePthread(&g_Game, &g_OnLine);
                }
                else{
                    g_Ai.End();
                }
            }
        }
        if(ImGui::BeginTable("检查框列表", 2)){
            ImGui::TableNextColumn();
            if(ImGui::Checkbox("托管", &g_ImGuiInput.enableAutoPlay)){
                if(g_ImGuiInput.enableAutoPlay && currentCountry == player){
                    g_Ai.Enable();
                }
            }
            ImGui::TableNextColumn();
            if(ImGui::Checkbox("启用汉势力", &g_ImGuiInput.enableHan)){
                if(g_ImGuiInput.enableHan){
                    g_Game.EnableHan();
                }
                else{
                    g_Game.DiscardHan();
                }
                ResetCountryItem(g_CountryItems[0]);
            }
            ImGui::EndTable();
        }
        ShowPlayerCountryCombo();
        if(ImGui::Button("返回")){
            *mainInterface = true;
            ret = true;
            g_Ai.End();
            g_ImGuiInput.Init();
            if(g_Game.IsGameStart())g_Game.EndGame();
        }
    }
    ImGui::End();
    return !ret;
}
bool MainInterface(bool *bSinglePlayer, bool *bOnline){
    if(ImGui::Begin("主界面")){
        if(ImGui::BeginTable("主界面-按钮表格", 2)){
            ImGui::TableNextColumn();
            if(ImGui::Button("单人")){
                *bSinglePlayer = true;
            }
            ImGui::TableNextColumn();
            if(ImGui::Button("局域网联机")){
                *bOnline = true;
            }
            ImGui::EndTable();
        }
    }
    ImGui::End();
    return !*bSinglePlayer && !*bOnline;
}
void UpdateImgui(VkCommandBuffer command){
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    
    if(g_ImGuiInput.interface.bMain){
        g_ImGuiInput.interface.bMain = MainInterface(&g_ImGuiInput.interface.bSinglePlayer, &g_ImGuiInput.interface.bOnline);
    }
    else if(g_ImGuiInput.interface.bSinglePlayer){
        g_ImGuiInput.interface.bSinglePlayer = ShowSinglePlayerModeMainInterface(&g_ImGuiInput.interface.bMain);
    }
    else if(g_ImGuiInput.interface.bOnline){
        g_ImGuiInput.interface.bOnline = ShowOnlineModeMainInterface(&g_ImGuiInput.interface.bMain);
    }
    // if(g_Game.IsGameStart() && g_Game.IsOnline()){
    //     ShowGameWidget();
    // }

    ImGui::Render();
    ImDrawData *draw_data = ImGui::GetDrawData();
    const bool isMinimized = (draw_data->DisplaySize.x <=.0f || draw_data->DisplaySize.y <= .0f);
    if(!isMinimized)g_VulkanImGui.RenderDrawData(command, draw_data);
}
void RecordCommand(VkCommandBuffer command, VkFramebuffer frame){
    vulkanFrame::BeginCommands(command, VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT);
    vulkanFrame::BeginRenderPass(command, frame, g_VulkanWindow.renderPass, g_WindowWidth, g_WindowHeight);

    const glm::mat4 projection = glm::ortho(0.0f, (float)g_WindowWidth, 0.0f, (float)g_WindowHeight, -1.0f, 1.0f);
    vkCmdBindPipeline(command, VK_PIPELINE_BIND_POINT_GRAPHICS, g_Fill);
    vkCmdPushConstants(command, g_PipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(glm::mat4), &projection);

    vkCmdBindPipeline(command, VK_PIPELINE_BIND_POINT_GRAPHICS, g_Fill);
    g_Game.Draw(command, g_PipelineLayout);
    vkCmdBindPipeline(command, VK_PIPELINE_BIND_POINT_GRAPHICS, g_Wireframe);
    g_Game.DrawWireframe(command, g_PipelineLayout);
    vkCmdBindPipeline(command, VK_PIPELINE_BIND_POINT_GRAPHICS, g_Font);
    g_Game.DrawFont(command, g_PipelineLayout);

    UpdateImgui(command);

    vkCmdEndRenderPass(command);
    vkEndCommandBuffer(command);
}
void keybutton(GLFWwindow *window, int key, int scancode, int action, int mods){
    //没解决这个问题前，不给撤回
    //单步下没问题，但如果是调用destroycountry销毁的呢
    //必须是玩家回合才能撤销。不然就要处理很多同步问题//撤销功能使用后，需要注意当前下棋的玩家,也就是说，撤回后，需要正确修改当前下棋的国籍
    // if(action == GLFW_RELEASE){
    //     if(key == GLFW_KEY_Z){
    //         if(GLFW_PRESS == glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) || GLFW_PRESS == glfwGetKey(window, GLFW_KEY_LEFT_CONTROL)){
    //             auto pBoard = g_Game.GetChessboard();
    //             pBoard->UndoStep();
    //             g_Game.UpdateUniform(g_VulkanDevice.device, g_WindowWidth);
    //         }
    //     }
    // }
}
void *PlayChessFun(void *userData){
    glm::vec4 info = *(glm::vec4 *)userData;
    auto pBoard = g_Game.GetChessboard();
    Chess *pStart = pBoard->GetChess(info.y, info.x);
    g_Game.PlayChess(pStart, info.w, info.z);
    if(g_ImGuiInput.enableAi){
        g_Ai.SyncBoardCopy(pStart, info.w, info.z);
        if(g_Game.GameOver()){
            g_Ai.End();
        }
        g_Ai.EnableNextCountry(g_ImGuiInput.enableAutoPlay);
    }
    return nullptr;
}
const Chess *g_Select;
bool SelectChess(const glm::vec2 &mousePos){
    auto pBoard = g_Game.GetChessboard();
    const uint32_t currentCountry = g_Game.GetCurrentCountry(), playerCountry = g_Game.GetPlayerCountry();
    if((!g_Ai.IsEnd() || g_Game.IsOnline()) && currentCountry != playerCountry)return false;
    if(g_Select){
        g_Game.UnSelectChess();
        static glm::vec4 info;
        info = g_Game.PrepareChess(g_Select, mousePos);
        if(info.x == 0 && info.y == 0){
            g_Select = nullptr;
            return false;
        }
        if(g_Game.IsOnline()){
            //单机和局域网联机差不多。无非就是要接收和发送消息
            Chess target;
            const Chess *pTarget = pBoard->GetChess(info.w, info.z);
            if(pTarget){
                target = *pTarget;
            }
            else{
                target.SetPos(info.w, info.z);
                target.SetCountry(MAX_COUNTRY_INDEX);
            }
            const uint32_t clientIndex = g_OnLine.GetClientIndex();
            g_OnLine.SendPlayChessMessage(g_Players[clientIndex], g_Select, &target);
        }
        else{
            g_Game.UnSelectChess();
            //得另开线程才看到棋子移动效果
            CreateThread(PlayChessFun, &info);
        }
        g_Select = nullptr;
    }
    else{
        g_Select = pBoard->GetChess(currentCountry, mousePos);
    }
    return true;
}
void mousebutton(GLFWwindow *window,int button,int action,int mods){
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    const glm::vec2 mousePos = glm::vec2(xpos, ypos);
    if (g_Game.IsGameStart() && action == GLFW_RELEASE && button == GLFW_MOUSE_BUTTON_LEFT){
        SelectChess(mousePos);
        g_Game.SelectChess(g_Select);
    }
}
VkVertexInputBindingDescription inputBindingDescription(uint32_t binding) {
    return VkVertexInputBindingDescription({ binding, sizeof(glm::vec3) * 2, VK_VERTEX_INPUT_RATE_VERTEX });
}
VkVertexInputAttributeDescription inputAttributeDescription(uint32_t location, VkFormat format, uint32_t offset, uint32_t binding = 0){
    VkVertexInputAttributeDescription info;
    info.offset = offset;
    info.format = format;
    info.binding = binding;
    info.location = location;
    return info;
}
static std::array<VkVertexInputAttributeDescription, 2> inputAttributeDescriptions(uint32_t binding) {
    return { inputAttributeDescription(0, VK_FORMAT_R32G32B32_SFLOAT, 0, binding), inputAttributeDescription(1, VK_FORMAT_R32G32B32_SFLOAT, sizeof(glm::vec3), binding) };
}
void CreatePipelineLayout(VkDevice device, VkDescriptorSetLayout layout){
    VkPipelineLayoutCreateInfo pipelineLayoutInfo = Pipeline::initializers::pipelineLayoutCreateInfo(1, 1);
    VkPushConstantRange pushConstantRange = Pipeline::initializers::pushConstantRange(VK_SHADER_STAGE_VERTEX_BIT, sizeof(glm::mat4), 0);
    pipelineLayoutInfo.pSetLayouts = &layout;
    pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
    VK_CHECK(vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &g_PipelineLayout));
}
void DestroyGraphicsPipeline(VkDevice device){
    vkDestroyPipeline(device, g_Fill, VK_NULL_HANDLE);
    vkDestroyPipeline(device, g_Font, VK_NULL_HANDLE);
    vkDestroyPipeline(device, g_Wireframe, VK_NULL_HANDLE);
    vkDestroyPipelineLayout(device, g_PipelineLayout, VK_NULL_HANDLE);
}
void CreateGraphicsPipeline(VkDevice device, VkPipelineLayout layout){
    VkPipelineVertexInputStateCreateInfo inputState{};
    std::array<VkPipelineShaderStageCreateInfo,2> shaderStages;
    VkVertexInputBindingDescription bindingDescriptions = inputBindingDescription(0);
    std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions = inputAttributeDescriptions(0);

    VkPipelineViewportStateCreateInfo viewportState = Pipeline::initializers::pipelineViewportStateCreateInfo(1, 1, 0);
    VkGraphicsPipelineCreateInfo pipelineCreateInfo = Pipeline::initializers::pipelineCreateInfo(layout, g_VulkanWindow.renderPass, 0);

    VkPipelineColorBlendAttachmentState blendAttachmentState = Pipeline::initializers::pipelineColorBlendAttachmentState(0xf, VK_FALSE);
    VkPipelineColorBlendStateCreateInfo colorBlendState = Pipeline::initializers::pipelineColorBlendStateCreateInfo(1, &blendAttachmentState);
    VkPipelineMultisampleStateCreateInfo multisampleState = Pipeline::initializers::pipelineMultisampleStateCreateInfo(VK_SAMPLE_COUNT_1_BIT, 0);
    VkPipelineDepthStencilStateCreateInfo depthStencilState = Pipeline::initializers::pipelineDepthStencilStateCreateInfo(VK_FALSE, VK_FALSE, VK_COMPARE_OP_LESS_OR_EQUAL);
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyState = Pipeline::initializers::pipelineInputAssemblyStateCreateInfo(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, 0, VK_FALSE);
    VkPipelineRasterizationStateCreateInfo rasterizationState = Pipeline::initializers::pipelineRasterizationStateCreateInfo(VK_POLYGON_MODE_FILL, VK_CULL_MODE_NONE, VK_FRONT_FACE_CLOCKWISE, 0);
    // std::array<VkDynamicState> dynamicStateEnables = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
    // VkPipelineDynamicStateCreateInfo dynamicState = Pipeline::initializers::pipelineDynamicStateCreateInfo(dynamicStateEnables.data(), static_cast<uint32_t>(dynamicStateEnables.size()), 0);
    VkRect2D scissors = Pipeline::Rect2D(g_WindowWidth, g_WindowHeight);
    VkViewport viewport = Pipeline::Viewport(g_WindowWidth, g_WindowHeight);
    viewportState.pScissors = &scissors;
    viewportState.pViewports = &viewport;

    inputState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    inputState.vertexBindingDescriptionCount = 1;
    inputState.pVertexBindingDescriptions = &bindingDescriptions;
    inputState.pVertexAttributeDescriptions = attributeDescriptions.data();
    inputState.vertexAttributeDescriptionCount = attributeDescriptions.size();

    shaderStages[0] = Pipeline::LoadShader(device, "shaders/color.vert.spv", VK_SHADER_STAGE_VERTEX_BIT);
    shaderStages[1] = Pipeline::LoadShader(device, "shaders/color.frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT);

    // pipelineCreateInfo.pTessellationState = ;
    pipelineCreateInfo.layout = layout;
    pipelineCreateInfo.pStages = shaderStages.data();
    pipelineCreateInfo.stageCount = shaderStages.size();

    pipelineCreateInfo.pVertexInputState = &inputState;
    pipelineCreateInfo.pViewportState = &viewportState;
    // pipelineCreateInfo.pDynamicState = &dynamicState;
    pipelineCreateInfo.pColorBlendState = &colorBlendState;
    pipelineCreateInfo.pMultisampleState = &multisampleState;
    pipelineCreateInfo.pDepthStencilState = &depthStencilState;
    pipelineCreateInfo.pInputAssemblyState = &inputAssemblyState;
    pipelineCreateInfo.pRasterizationState = &rasterizationState;
    VK_CHECK(vkCreateGraphicsPipelines(device, g_PipelineCache, 1, &pipelineCreateInfo, nullptr, &g_Fill));
    rasterizationState = Pipeline::initializers::pipelineRasterizationStateCreateInfo(VK_POLYGON_MODE_LINE, VK_CULL_MODE_NONE, VK_FRONT_FACE_CLOCKWISE, 0);
    VK_CHECK(vkCreateGraphicsPipelines(device, g_PipelineCache, 1, &pipelineCreateInfo, nullptr, &g_Wireframe));
    vkDestroyShaderModule(device, shaderStages[0].module, VK_NULL_HANDLE);
    vkDestroyShaderModule(device, shaderStages[1].module, VK_NULL_HANDLE);
    rasterizationState = Pipeline::initializers::pipelineRasterizationStateCreateInfo(VK_POLYGON_MODE_FILL, VK_CULL_MODE_NONE, VK_FRONT_FACE_CLOCKWISE, 0);
    shaderStages[0] = Pipeline::LoadShader(device, "shaders/font.vert.spv", VK_SHADER_STAGE_VERTEX_BIT);
    shaderStages[1] = Pipeline::LoadShader(device, "shaders/font.frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT);
    VK_CHECK(vkCreateGraphicsPipelines(device, g_PipelineCache, 1, &pipelineCreateInfo, nullptr, &g_Font));
    vkDestroyShaderModule(device, shaderStages[0].module, VK_NULL_HANDLE);
    vkDestroyShaderModule(device, shaderStages[1].module, VK_NULL_HANDLE);
}
void SetupDescriptorSetLayout(VkDevice device){
    VkDescriptorSetLayoutBinding setlayoutBindings[2] = {};
    // setlayoutBinding.binding = 0;
    setlayoutBindings[0].descriptorCount = 1;
    setlayoutBindings[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    setlayoutBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
    setlayoutBindings[1].binding = 1;
    setlayoutBindings[1].descriptorCount = 1;
    setlayoutBindings[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    setlayoutBindings[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    vulkanFrame::CreateDescriptorSetLayout(device, setlayoutBindings, 2, &g_SetLayout);
}
void Setup(GLFWwindow *window){
    glfwSetKeyCallback(window, keybutton);
    glfwSetMouseButtonCallback(window, mousebutton);

    SetupDescriptorSetLayout(g_VulkanDevice.device);
    
    Pipeline::CreatePipelineCache(g_VulkanDevice.device, "GraphicsPipelineCache", g_PipelineCache);
    CreatePipelineLayout(g_VulkanDevice.device, g_SetLayout);
    CreateGraphicsPipeline(g_VulkanDevice.device, g_PipelineLayout);

    g_Game.Setup(g_VulkanDevice, g_SetLayout, g_VulkanQueue.graphics, g_VulkanPool);
    g_Game.UpdateUniform(g_VulkanDevice.device, g_WindowWidth);

    g_VulkanPool.AllocateCommandBuffers(g_VulkanDevice.device, 1, &g_CommandBuffers);

    //imgui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;//启用手柄
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForVulkan(window, true);
    g_VulkanImGui.Setup(g_VulkanDevice, g_VulkanPool);
    g_VulkanImGui.CreatePipeline(g_VulkanDevice.device, g_VulkanWindow.renderPass, g_PipelineCache);
    g_VulkanImGui.CreateFont(g_VulkanDevice, "fonts/ukai.ttc", g_VulkanQueue.graphics, g_VulkanPool);
    g_OnLine.Init();
    for (size_t i = 0; i < MAX_COUNTRY_INDEX - 1; i++){
        ResetCountryItem(g_CountryItems[i]);
    }
    // if(g_Ai.IsEnd()){
    //     if(g_Game.IsOnline()){
    //         if(g_OnLine.IsServer()){
    //             for (auto it = g_Players.begin(); it != g_Players.end(); ++it){
    //                 if(it->ai){
    //                     g_Ai.CreatePthread(&g_Game, &g_OnLine);
    //                     break;
    //                 }
    //             }
    //         }
    //     }
    //     else{
    //         g_Ai.CreatePthread(&g_Game, &g_OnLine);
    //     }
    // }
}

void Cleanup(VkDevice device){
    ImGui_ImplGlfw_Shutdown();
    vkDestroyDescriptorSetLayout(device, g_SetLayout, VK_NULL_HANDLE);
    DestroyGraphicsPipeline(device);
    Pipeline::DestroyPipelineCache(device, "GraphicsPipelineCache", g_PipelineCache);
    g_VulkanImGui.Cleanup(device);
    g_Game.Cleanup(device);
    g_OnLine.ExitGame();
    g_OnLine.Cleanup();
}
void RecreateSwapchain(void *userData){
    GLFWwindow* window = (GLFWwindow*)userData;
    int32_t width, height;
    while(width == 0 || height == 0){
        glfwGetFramebufferSize(window, &width, &height);
        glfwWaitEvents();
    }
    vkDeviceWaitIdle(g_VulkanDevice.device);
    //想不调用vkDeviceWaitIdle重建交换链也行, 在重新创建交换链时, VkSwapchainCreateInfoKHR的结构体中oldSwapChain设置为原来的交换链
    g_VulkanWindow.CleanupSwapchain(g_VulkanDevice.device);
   const VkExtent2D swapchainExtent = g_VulkanWindow.swapchain.extent;
   g_VulkanWindow.swapchain.CreateSwapchain(g_VulkanDevice, g_VulkanWindow.surface);
    g_WindowWidth = g_VulkanWindow.swapchain.extent.width;
    g_WindowHeight = g_VulkanWindow.swapchain.extent.height;
    g_VulkanWindow.CreateRenderPass(g_VulkanDevice.device);
    g_VulkanWindow.CreateFrameBuffer(g_VulkanDevice);    
    if(swapchainExtent.width != g_WindowWidth || swapchainExtent.height != g_WindowHeight){
        DestroyGraphicsPipeline(g_VulkanDevice.device);
        CreatePipelineLayout(g_VulkanDevice.device, g_SetLayout);
        CreateGraphicsPipeline(g_VulkanDevice.device, g_PipelineLayout);

        //等到能正确创建交换链后, 再重建帧缓冲
        // g_PickingTexture.CreateOffscreen(g_VulkanDevice, g_WindowWidth, g_WindowHeight);
        // g_PickingTexture.DestroyGraphicsPipeline(g_VulkanDevice.device);
        // auto attributeDescriptions = Vertex::inputAttributeDescriptions(0);
        // g_PickingTexture.CreateGraphicsPipeline(g_VulkanDevice.device, g_Pipeline.layout, g_Pipeline.cache, Vertex::inputBindingDescription(0), attributeDescriptions.data(), attributeDescriptions.size());
    }
}
void display(GLFWwindow* window){
    static size_t currentFrame;
    vkDeviceWaitIdle(g_VulkanDevice.device);
    RecordCommand(g_CommandBuffers, g_VulkanWindow.framebuffers[currentFrame]);
    vulkanFrame::Render(g_VulkanDevice.device, currentFrame, g_CommandBuffers, g_VulkanWindow.swapchain.swapchain, g_VulkanQueue, g_VulkanSynchronize, RecreateSwapchain, window);

    currentFrame = (currentFrame + 1) % g_VulkanWindow.framebuffers.size();
}
VkBool32 VKAPI_PTR debugUtilsMessenger(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageTypes, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData){
    const char* strMessageSeverity = nullptr;//, *strMessageTypes = nullptr;
    if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT) {
        strMessageSeverity = "VERBOSE";
    }
    else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT) {
        strMessageSeverity = "INFO";
    }
    else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
        strMessageSeverity = "WARNING";
    }
    else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
        strMessageSeverity = "ERROR";
    }
    else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_FLAG_BITS_MAX_ENUM_EXT) {
        strMessageSeverity = "FLAG";
    }
    printf("[VULKAN VALIDATION LAYER]\nSEVERITY:%s\nMESSAGE:%s\n", strMessageSeverity, pCallbackData->pMessage);
    return VK_FALSE;
}
bool GetPhysicalDevices(VkPhysicalDevice physicalDevice){
    VkPhysicalDeviceProperties physicalDeviceProperties;
    vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);
    return physicalDeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU;
}
void SetupVulkan(GLFWwindow *window){
    uint32_t count;
    const char** instanceExtension = glfwGetRequiredInstanceExtensions(&count);
    std::vector<const char*> extensions(instanceExtension, instanceExtension + count);
    VK_CHECK(g_VulkanDevice.CreateInstance(extensions));
    g_VulkanDevice.GetPhysicalDevices(GetPhysicalDevices);
    VK_CHECK(g_VulkanDevice.CreateDebugUtilsMessenger(debugUtilsMessenger));
    glfwCreateWindowSurface(g_VulkanDevice.instance, window, nullptr, &g_VulkanWindow.surface);
    VK_CHECK(g_VulkanDevice.CreateDevice(g_VulkanWindow.surface));
    g_VulkanQueue.CreateQueue(g_VulkanDevice, g_VulkanWindow.surface);
    g_VulkanWindow.swapchain.CreateSwapchain(g_VulkanDevice, g_VulkanWindow.surface);
    g_VulkanWindow.CreateRenderPass(g_VulkanDevice.device);
    g_VulkanWindow.CreateFrameBuffer(g_VulkanDevice);
    g_VulkanPool.CreatePool(g_VulkanDevice, 20);
    g_VulkanSynchronize.CreateSynchronize(g_VulkanDevice.device, g_VulkanWindow.swapchain.images.size());
    //显示设备信息
    const char *deviceType;
    VkPhysicalDeviceProperties physicalDeviceProperties = g_VulkanDevice.GetPhysicalDeviceProperties();
    switch (physicalDeviceProperties.deviceType){
    case VK_PHYSICAL_DEVICE_TYPE_CPU:
        deviceType = "CPU";
        break;
    case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
        deviceType = "DISCRETE GPU";
        break;
    case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
        deviceType = "INTEGRATED GPU";
        break;
    case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
        deviceType = "VIRTUAL GPU";
        break;
    default:
        deviceType = "OTHER";
        break;
    }
	printf("gpu name:%s, gpu type:%s\n", physicalDeviceProperties.deviceName, deviceType);
    // PrintVulkanInfo(&physicalDeviceProperties);
}
void CleanupVulkan(){
    g_VulkanSynchronize.Cleanup(g_VulkanDevice.device);

    g_VulkanPool.Cleanup(g_VulkanDevice.device);

    g_VulkanWindow.Cleanup(g_VulkanDevice);

    g_VulkanDevice.Cleanup();
}
int main(){
    srandom(time(nullptr));
    if (GLFW_FALSE == glfwInit()) {
        printf("initialize glfw error");
        return 1;
    }
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow* window = glfwCreateWindow(g_WindowWidth, g_WindowHeight, "demo", NULL, NULL);
    // Setup Vulkan
    if (!glfwVulkanSupported()){
        printf("GLFW: Vulkan Not Supported\n");
        return 1;
    }
    SetupVulkan(window);
    Setup(window);

    const double targetFrameTime = 1.0 / 60.0; // 目标帧时间（例如60 FPS）
    auto previousTime = std::chrono::high_resolution_clock::now();
    while (!glfwWindowShouldClose(window)) {
        auto currentTime = std::chrono::high_resolution_clock::now();
        double elapsedTime = std::chrono::duration<double>(currentTime - previousTime).count();

        if (elapsedTime < targetFrameTime) {
            // 计算剩余需要等待的时间
            double sleepTime = (targetFrameTime - elapsedTime) * 1000; // 转换为毫秒
            std::this_thread::sleep_for(std::chrono::milliseconds((int)sleepTime));
        }

        // 更新前一帧时间
        previousTime = std::chrono::high_resolution_clock::now();

        display(window);

        glfwPollEvents();
    }
	glfwTerminate();

    vkDeviceWaitIdle(g_VulkanDevice.device);
    Cleanup(g_VulkanDevice.device);
    CleanupVulkan();
    return 0;
}
