#include "Ai.h"
#include <array>
#ifdef __linux
#include <pthread.h>
#endif
#include <chrono>
#include <thread>
#include <mutex>
#include <ranges>
#include <random>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "Chess.h"
#include "Game.h"
#include "OnLine.h"
#include "Pipeline.hpp"
#include "VulkanWindow.h"

#include "VulkanImgui.h"
#include "imgui_impl_glfw.h"
VulkanPool g_VulkanPool;
VulkanQueue g_VulkanQueue;
VulkanDevice g_VulkanDevice;
VulkanWindow g_VulkanWindow;
VulkanSynchronize g_VulkanSynchronize;
VkDebugUtilsMessengerEXT g_VulkanMessenger;

uint32_t g_WindowWidth = CHESSBOARD_BIG_RECT_SIZE * 2 + 10 * CHESSBOARD_RECT_SIZE + 11 * CHESSBOARD_LINE_WIDTH, g_WindowHeight = g_WindowWidth;

VkCommandBuffer g_CommandBuffers;

VkPipelineCache g_pipelineCache;
VkPipelineLayout g_PipelineLayout;
VkDescriptorSetLayout g_SetLayout;
VkPipeline g_Fill, g_Wireframe, g_Font;

Ai g_Ai;
Game g_Game;

VulkanImgui g_VulkanImgui;

VulkanChess g_Chess;
VulkanWireframe g_SelectChess;
VulkanChessboard g_VulkanChessboard;

std::vector<glm::vec4>g_Record;

std::vector<std::string>g_CountryItems[MAX_COUNTRY_INDEX];

std::mutex g_PlayChessMutex;

OnLine g_OnLine;
glm::vec2 g_JiangPos[MAX_COUNTRY_INDEX];//目前只用于RotateChess
std::vector<Player>g_Players(MAX_COUNTRY_INDEX);
// void createSurface(VkInstance instance, VkSurfaceKHR&surface, void* userData){
//     glfwCreateWindowSurface(instance, (GLFWwindow *)userData, nullptr, &surface);
// }
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
    uint32_t queueFamilies[2];
    queueFamilies[GRAPHICS_QUEUE_INDEX] = g_VulkanDevice.GetQueueFamiliesIndex(VK_QUEUE_GRAPHICS_BIT, g_VulkanWindow.surface);
    if(queueFamilies[GRAPHICS_QUEUE_INDEX] != -1){
        queueFamilies[PRESENT_QUEUE_INDEX] = queueFamilies[GRAPHICS_QUEUE_INDEX];
        vkGetDeviceQueue(g_VulkanDevice.device, queueFamilies[PRESENT_QUEUE_INDEX], queueFamilies[GRAPHICS_QUEUE_INDEX], &g_VulkanQueue.present);
        vkGetDeviceQueue(g_VulkanDevice.device, queueFamilies[GRAPHICS_QUEUE_INDEX], queueFamilies[GRAPHICS_QUEUE_INDEX], &g_VulkanQueue.graphics);
    }
    g_VulkanWindow.swapchain.CreateSwapchain(g_VulkanDevice, g_VulkanWindow.surface);

    g_VulkanWindow.CreateRenderPass(g_VulkanDevice.device);
    g_VulkanWindow.CreateFrameBuffer(g_VulkanDevice, true);

    g_VulkanPool.CreatePool(g_VulkanDevice, 8);

    g_VulkanSynchronize.CreateSynchronize(g_VulkanDevice.device, g_VulkanWindow.swapchain.images.size());
    //显示设备信息
    const char *deviceType;
    VkPhysicalDeviceProperties physicalDeviceProperties;
    g_VulkanDevice.GetPhysicalDeviceProperties(physicalDeviceProperties);
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
}
void CleanupVulkan(){
    g_VulkanSynchronize.Cleanup(g_VulkanDevice.device);

    g_VulkanPool.Cleanup(g_VulkanDevice.device);

    g_VulkanWindow.Cleanup(g_VulkanDevice);

    g_VulkanDevice.Cleanup();
}
void UpdateChessUniform(VkDevice device){
    auto pChess = g_Game.GetChess();
    for (uint32_t uiCoutry = 0; uiCoutry < MAX_COUNTRY_INDEX; ++uiCoutry){
        for(uint32_t uiChess = 0; uiChess < DRAW_COUNTRY_CHESS_COUNT; ++uiChess){
            const Chess *pc = pChess[ROW_COLUMN_CHESS_TO_INDEX(uiCoutry, uiChess)];
            const uint32_t dynamicOffsets = uiCoutry * DRAW_COUNTRY_CHESS_COUNT + uiChess;
            if(pc){
                g_Chess.UpdateUniform(device, pc->GetFontIndex(), glm::vec3(pc->GetPos(), 0), dynamicOffsets);
            }
            else{
                const glm::vec3 pos = glm::vec3(COLUMN_TO_X(CHESSBOARD_ROW + 10), ROW_TO_Y(CHESSBOARD_COLUMN + 10), 0);
                g_Chess.UpdateUniform(device, FONT_INDEX_HAN, pos, dynamicOffsets);
            }
        }
    }
}
void ClearSelectWireframeUnfirom(std::vector<glm::mat4>&model){
    for (size_t i = 0; i < CHESSBOARD_ROW * 2; ++i){
        model[i] = glm::translate(glm::mat4(1), glm::vec3(COLUMN_TO_X(100), ROW_TO_Y(100), 0));
    }
}
glm::vec2 lerp(const glm::vec2&p0, const glm::vec2&p1, float t){
    return (1 - t) * p0 + t * p1;
}
void MoveChess(const glm::vec2&start, const glm::vec2&end, uint32_t fontIndex, uint32_t dynamicOffsets){
    for (float t = 0; t < 1; t += .01){
        const glm::vec2 pos = lerp(start, end, t);
        g_Chess.UpdateUniform(g_VulkanDevice.device, fontIndex, glm::vec3(pos, 0), dynamicOffsets);
#ifdef WIN32
        Sleep(1);
#else
        usleep(10000);
#endif
    }
}
void MoveChess(Chess *pStart, const Chess *pEnd){
    const uint32_t dynamicOffsets = pStart->GetCountry() * DRAW_COUNTRY_CHESS_COUNT + pStart->GetChess();
    const glm::vec2 start = glm::vec2(COLUMN_TO_X(pStart->GetColumn()), ROW_TO_Y(pStart->GetRow())), end = glm::vec2(COLUMN_TO_X(pEnd->GetColumn()), ROW_TO_Y(pEnd->GetRow()));
    MoveChess(start, end, pStart->GetFontIndex(), dynamicOffsets);
}
void UnSelectChess(){
    std::vector<glm::mat4>model(CHESSBOARD_ROW * 2);
    ClearSelectWireframeUnfirom(model);
    g_SelectChess.UpdateUniform(g_VulkanDevice.device, model);
}
void PlayChess(Chess *pChess, uint32_t dstRow, uint32_t dstColumn){
    g_PlayChessMutex.lock();
    UnSelectChess();
    //想写撤回功能的话, 还需要记录是不是吃子。另外，棋子被吃后也别直接销毁棋子，而是加一个标记然后不显示
    g_Record.push_back(glm::vec4(pChess->GetRow(), pChess->GetColumn(), dstRow, dstColumn));
    const Chess *pTarget = g_Game.GetChess(dstRow, dstColumn);
    if(pTarget){
        MoveChess(pChess, pTarget);
        uint32_t targetCountry = pTarget->GetCountry();
        g_Game.CaptureChess(pChess, pTarget);
        const char county[][MAX_BYTE] = { "蜀", "吴", "魏", "汉" };
        if(g_Game.IsDeath(targetCountry)){
            printf("%s国被%s国消灭\n", county[targetCountry], county[pChess->GetCountry()]);
            g_Game.DestroyCountry(targetCountry);
        }
    }
    else{
        const uint32_t dynamicOffsets = pChess->GetCountry() * DRAW_COUNTRY_CHESS_COUNT + pChess->GetChess();
        const glm::vec2 start = glm::vec2(COLUMN_TO_X(pChess->GetColumn()), ROW_TO_Y(pChess->GetRow())), end = glm::vec2(COLUMN_TO_X(dstColumn), ROW_TO_Y(dstRow));
        MoveChess(start, end, pChess->GetFontIndex(), dynamicOffsets);
    }
    pChess->SetPos(dstRow, dstColumn);
    UpdateChessUniform(g_VulkanDevice.device);
    g_PlayChessMutex.unlock();
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
void NewGame(int32_t player = -1, int32_t currentCountry = -1){
    g_Game.StartGame();
    g_Ai.End();
    // if(!g_Game.IsOnline())g_Ai.Pause();
    g_PlayChessMutex.lock();
    g_Game.InitinalizeGame(player, currentCountry);
    UpdateChessUniform(g_VulkanDevice.device);
    // if(!g_Game.IsOnline())g_Ai.Start();
    g_PlayChessMutex.unlock();
    if(g_Ai.IsEnd()){
        if(g_Game.IsOnline()){
            if(g_OnLine.IsServer()){
                for (auto it = g_Players.begin(); it != g_Players.end(); ++it){
                    if(it->ai){
                        g_Ai.CreatePthread(&g_Game, &g_OnLine);
                        break;
                    }
                }
            }
        }
        else{
            g_Ai.CreatePthread(&g_Game, &g_OnLine);
        }
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
                    NewGame();
                }
                else if(currentCountryItem == "魏"){
                    NewGame(WEI_COUNTRY_INDEX);
                }
                else if(currentCountryItem == "蜀"){
                    NewGame(SHU_COUNTRY_INDEX);
                }
                else if(currentCountryItem == "吴"){
                    NewGame(WU_COUNTRY_INDEX);
                }
                else if(currentCountryItem == "汉"){
                    NewGame(HAN_COUNTRY_INDEX);
                }
                break;
            }
        }
        ImGui::EndCombo();
    }
}
void ResetCountryItem(std::vector<std::string>&countryItems){
    std::vector<std::string>country = { "?", "魏", "蜀", "吴" };
    if(g_Game.HanCanPslay()){
        country.push_back("汉");
    }
    if(countryItems.empty())countryItems.resize(country.size());
    for (size_t i = 0; i < countryItems.size(); ++i){
        countryItems[i] = country[i];
    }
}
void RandomNumber(uint32_t max, uint32_t count, Player *result, uint32_t min = 0){
    long cpp_std =
#ifdef _MSVC_LANG
        _MSVC_LANG;
#else
        __cplusplus;
#endif
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
        if(!strcmp(it->country, "?")){
            countryIndex[playerIndex].uCountry = countryToindex(it->country);
        }
    }
    static const uint32_t countryCount = g_Game.GetCountryCount();
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
#define PROCESS_SERVER_RELAY
#ifdef PROCESS_SERVER_RELAY
void *process_server(void *userData){
    GameMessage message;
    uint32_t socketindex = *(SOCKET *)userData;
    do{
        g_OnLine.RecvFromClient(socketindex, &message, sizeof(message));
        // printf("in function %s:recv event %d\n", __FUNCTION__, message.event);
        g_OnLine.SendToAllClient(&message, sizeof(message));
    } while (message.event != GAME_OVER_GAME_EVENT);
    printf("function %s end\n", __FUNCTION__);
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
    if(g_Game.HanCanPslay()){
        message.event = ENABLE_HANG_GAME_EVENT;
        g_OnLine.SendToAllClient(&message, sizeof(GameMessage));
    }
    RandomCountry();
    SendPlayersInfoation();
    g_OnLine.SendCurrentCountry(rand()%g_Game.GetCountryCount());
    //游戏开始, 发消息告诉所有客户端游戏开始
    message.event = GAME_START_GAME_EVENT;
    g_OnLine.SendToAllClient(&message, sizeof(message));
    return nullptr;
}
#else
std::mutex g_Mutex;
void *process_server(void *userData){
    GameMessage message;
    uint32_t socketindex = *(SOCKET *)userData;
    g_Mutex.lock();
    if(INVALID_SOCKET != g_Server.AcceptClient(socketindex, &message, sizeof(message))){
        printf("in function %s:socket index %d, accept count:%d\n", __FUNCTION__, socketindex, socketindex);
        if(message.event == AI_JOIN_GAME_GAME_EVENT){
            g_Players[socketindex].ai = true;
            strcpy(message.player.name, "ai");
            g_Server.ShutdownClient(socketindex);
            // if(socketindex == 1)DeleteCountryItem(g_Players[0].country, g_CountryItems[1]);
        }
        else{
            sprintf(message.player.name, "player%d", socketindex + 1);
        }
        g_Players[socketindex].index = socketindex;
        strcpy(g_Players[socketindex].name, message.player.name);

        SendClientInfoation();
    }
    g_Mutex.unlock();
    //为了防止最后加ai, 所以即使该线程是ai也需要做下面的事
    if(socketindex == g_DefaultCountryCount){
        RandomCountry();
        SendCurrentCountry();
        SendPlayersInfoation();
        //游戏开始, 发消息告诉所有客户端游戏开始
        message.event = GAME_START_GAME_EVENT;
        g_Server.SendToAllClient(&message, sizeof(message));
        //因为最多只有1个ai, 所以可以直接循环里面判断和创建线程
        for (auto it = g_Players.begin(); it != g_Players.end(); ++it){
            if(it->ai){
                g_Ai.CreatePthread(&g_Game);
                break;
            }
        }
    }
    if(message.event == AI_JOIN_GAME_GAME_EVENT){
        return nullptr;//ai和服务端用同一个客户端
    }
    // int32_t aiIndex = GetAiPlayerIndex();
    // if(aiIndex != INVALID_PLAYER_INDEX && aiIndex == socketindex){
    //     printf("客户端索引%d为ai, 但前函数socket index %d。所以退出函数%s\n", aiIndex, socketindex, __FUNCTION__);
    //     return nullptr;//ai不需要收发消息
    // }
    do{
        g_Server.RecvFromClient(socketindex, &message, sizeof(message));
        g_Server.SendToAllClient(&message, sizeof(message));
    } while (message.event != GAME_OVER_GAME_EVENT);
    return nullptr;
}
#endif
void InitJiangPos(){
    const Chess *pChess = g_Game.GetChess()[ROW_COLUMN_CHESS_TO_INDEX(WEI_COUNTRY_INDEX, JIANG_CHESS_INDEX)];
    g_JiangPos[WEI_COUNTRY_INDEX] = glm::vec2(pChess->GetColumn(), pChess->GetRow());
    pChess = g_Game.GetChess()[ROW_COLUMN_CHESS_TO_INDEX(SHU_COUNTRY_INDEX, JIANG_CHESS_INDEX)];
    g_JiangPos[SHU_COUNTRY_INDEX] = glm::vec2(pChess->GetColumn(), pChess->GetRow());
    pChess = g_Game.GetChess()[ROW_COLUMN_CHESS_TO_INDEX(WU_COUNTRY_INDEX, JIANG_CHESS_INDEX)];
    g_JiangPos[WU_COUNTRY_INDEX] = glm::vec2(pChess->GetColumn(), pChess->GetRow());
    pChess = g_Game.GetChess()[ROW_COLUMN_CHESS_TO_INDEX(HAN_COUNTRY_INDEX, JIANG_CHESS_INDEX)];
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
    if(g_Game.HanCanPslay()){
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
            g_Game.SetOnline(true);
            uint32_t aiIndex = 1, index = 0;
            for (auto it = g_Players.begin() + 1; it != g_Players.end(); ++it, ++aiIndex){
                if(it->ai){
                    g_OnLine.SetAiClientIndex(index, aiIndex);
                    ++index;
                }
            }
            g_Game.StartGame();
            const uint32_t clientIndex = g_OnLine.GetClientIndex();
            printf("game start, you country index is %d\n", g_Players[clientIndex].uCountry);
            NewGame(g_Players[clientIndex].uCountry, g_Game.GetCurrentCountry());
            InitJiangPos();
        }
        else if(message.event == ENABLE_HANG_GAME_EVENT){
            g_Game.EnableHan();
        }
        else if(message.event == JOINS_GAME_GAME_EVENT){
            const uint32_t clientIndex = message.clientIndex;
            memcpy(&g_Players[clientIndex], &message.player, sizeof(Player));
            if(clientIndex != clientIndex){
                PlayerChangeCountry(clientIndex, message.player.country);
            }
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
            auto pChess = g_Game.GetChess();
            uint32_t dstRow = message.target.GetRow(), dstColumn = message.target.GetColumn();
            Chess *pStart = pChess[ROW_COLUMN_CHESS_TO_INDEX(message.select.GetCountry(), message.select.GetChess())], *pTarget = nullptr;
            if(message.target.GetCountry() >= MAX_COUNTRY_INDEX){
                RotateChess(g_Game.GetCurrentCountry(), dstRow, dstColumn,dstRow, dstColumn);
            }
            else{
                pTarget = pChess[ROW_COLUMN_CHESS_TO_INDEX(message.target.GetCountry(), message.target.GetChess())];
                dstRow = pTarget->GetRow();
                dstColumn = pTarget->GetColumn();
            }
            PlayChess(pStart, dstRow, dstColumn);
            const uint32_t country = g_Game.Check();
            uint32_t lastCountry = g_Game.GetCurrentCountry();
            if(country != INVALID_COUNTRY_INDEX && country != lastCountry && g_Game.GetNextCountry() != country){
                g_Game.ExtraTurn(country);
            }
            else{
                g_Game.NextCountry();
            }
            if(g_OnLine.IsServer())g_Ai.EnableNextCountry();
        }
        else if(message.event == GAME_OVER_GAME_EVENT){
            printf("in function %s:game over\n", __FUNCTION__);
            break;
        }
        else{
            printf("invalid event:%d\n", message.event);
        }
    }while(message.event != GAME_OVER_GAME_EVENT);
    return nullptr;
}
void CreateClient(const char *serverIp){
    if(g_OnLine.CreateClient(serverIp)){
        CreateThread(process_client, nullptr);
        g_OnLine.JoinsGame();
    }
}
void CreateServer(const char *name){
    static const uint32_t countryCount = g_Game.GetCountryCount();
    if(g_OnLine.CeateServer(countryCount)){
        //必须用静态变量, 局部变量在函数结束后就销毁, 会导致传餐错误
#ifdef PROCESS_SERVER_RELAY
        CreateThread(server_start, nullptr);
#else
        static uint32_t socketIndex[] = { 0, 1, 2 };
        for (size_t i = 0; i < countryCount; ++i){
            CreateThread(process_server, &socketIndex[i]);
        }
#endif
        CreateClient(g_OnLine.GetLocalIp().c_str());
    }
}
/*
    需求:
        输入用户名
        一开始必须有创建房间和连接服务端的按钮;这时候还不能确定该程序是客户端还是服务端
*/
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
void ShowClientWidget(){
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
void ShowGameWidget(){
    if(ImGui::Begin("三国象棋")){
        const char *countryName[4];
        countryName[WU_COUNTRY_INDEX] = "吴";
        countryName[WEI_COUNTRY_INDEX] = "魏";
        countryName[SHU_COUNTRY_INDEX] = "蜀";
        countryName[HAN_COUNTRY_INDEX] = "汉";
        const uint32_t currentCountry = g_Game.GetCurrentCountry(), player = g_Game.GetPlayer(), clientIndex = g_OnLine.GetClientIndex();
        if(currentCountry != INVALID_COUNTRY_INDEX){
            ImGui::Text("你是%s, 该%s下棋\n", countryName[g_Players[clientIndex].uCountry], g_Players[clientIndex].uCountry == currentCountry?"你":countryName[currentCountry]);
        }
    }
    ImGui::End();
}
void ShowServerWidget(){
    //在非广播模式下，直接显示本机ip。(或许广播模式下也可以显示)
    static std::string localIp = g_OnLine.GetLocalIp();
    ImGui::Text("本机ip:%s", localIp.c_str());
    ShowClientWidget();
    if(!g_Game.IsGameStart()){
        if(ImGui::Button("添加电脑")){
            uint32_t aiCount = 0;
            for (auto it = g_Players.begin(); it != g_Players.end(); it++){
                if(!strcmp(it->name, "ai")){
                    ++aiCount;
                    break;
                }
            }
            //减掉一个服务端玩家和一个客户端玩家
            if(aiCount < g_Game.GetCountryCount() - 2)g_OnLine.AddAi(localIp.c_str());
        }
    }
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
                    CreateServer("name");
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
    const char *countryName[4];
    countryName[WU_COUNTRY_INDEX] = "吴";
    countryName[WEI_COUNTRY_INDEX] = "魏";
    countryName[SHU_COUNTRY_INDEX] = "蜀";
    countryName[HAN_COUNTRY_INDEX] = "汉";
    const uint32_t currentCountry = g_Game.GetCurrentCountry(), player = g_Game.GetPlayer();
    if(ImGui::Begin("单人模式")){
        if(currentCountry != INVALID_COUNTRY_INDEX && player != INVALID_COUNTRY_INDEX){
            ImGui::Text("你是%s, 该%s下棋\n", countryName[player], player == currentCountry?"你":countryName[currentCountry]);
        }
        static bool enableHan;
        if(ImGui::Checkbox("启用汉势力", &enableHan)){
            if(enableHan){
                g_Game.EnableHan();
            }
            else{
                g_Game.DiscardHan();
            }
            NewGame();
        }
        if(ImGui::BeginTable("单人模式-按钮表格", 2)){
            ImGui::TableNextColumn();
            if(ImGui::Button("新游戏")){
                NewGame();
            }
            ImGui::TableNextColumn();
            if(ImGui::Button(g_Ai.IsEnd()?"开始":"暂停")){
                if(g_Ai.IsEnd()){
                    g_Ai.CreatePthread(&g_Game, &g_OnLine);
                }
                else{
                    g_Ai.End();
                }
            }
            ImGui::EndTable();
        }
        ShowPlayerCountryCombo();
        if(ImGui::Button("返回")){
            *mainInterface = true;
            ret = true;
            g_Game.StartGame(false);
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
    // static bool checkbuttonstatu;//检查框的状态。这个值传给imgui会影响到检查框
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    
    static bool bMainInterface = true, bSinglePlayerInterface, bOnlineInterface;
    if(bMainInterface){
        bMainInterface = MainInterface(&bSinglePlayerInterface, &bOnlineInterface);
    }
    else if(bSinglePlayerInterface){
        bSinglePlayerInterface = ShowSinglePlayerModeMainInterface(&bMainInterface);
    }
    else if(bOnlineInterface){
        bOnlineInterface = ShowOnlineModeMainInterface(&bMainInterface);
    }

    ImGui::Render();
    ImDrawData *draw_data = ImGui::GetDrawData();
    const bool isMinimized = (draw_data->DisplaySize.x <=.0f || draw_data->DisplaySize.y <= .0f);
    if(!isMinimized)g_VulkanImgui.RenderDrawData(command, draw_data);
}
void RecordCommand(VkCommandBuffer command, VkFramebuffer frame){
    vulkanFrame::BeginCommands(command, VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT);
    vulkanFrame::BeginRenderPassGeneral(command, frame, g_VulkanWindow.renderPass, g_WindowWidth, g_WindowHeight);

    const glm::mat4 projection = glm::ortho(0.0f, (float)g_WindowWidth, 0.0f, (float)g_WindowHeight, -1.0f, 1.0f);
    vkCmdBindPipeline(command, VK_PIPELINE_BIND_POINT_GRAPHICS, g_Fill);
    vkCmdPushConstants(command, g_PipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(glm::mat4), &projection);

    g_VulkanChessboard.Draw(command,g_PipelineLayout);

    vkCmdBindPipeline(command, VK_PIPELINE_BIND_POINT_GRAPHICS, g_Wireframe);
    g_VulkanChessboard.DrawWireframe(command, g_PipelineLayout);
    g_SelectChess.Draw(command, g_PipelineLayout);

    vkCmdBindPipeline(command, VK_PIPELINE_BIND_POINT_GRAPHICS, g_Fill);
    g_Chess.DrawChess(command, g_PipelineLayout, g_Game.GetCurrentCountry());
    vkCmdBindPipeline(command, VK_PIPELINE_BIND_POINT_GRAPHICS, g_Font);
    g_Chess.DrawFont(command, g_PipelineLayout);

    UpdateImgui(command);

    vkCmdEndRenderPass(command);
    vkEndCommandBuffer(command);
}
void UpdateSelectChessUniform(VkDevice device, std::vector<Chess>&canplays){
    //本来, 为了点击棋子之后有效果...是会改变棋子大小的..但如今发现..或许不需要了
    std::vector<glm::mat4>model(CHESSBOARD_ROW * 2);
    const glm::mat4 scale = glm::scale(glm::mat4(1), glm::vec3(CHESSBOARD_RECT_SIZE, CHESSBOARD_RECT_SIZE, 1));
    for (size_t i = 0; i < canplays.size(); ++i){
        model[i] = glm::translate(glm::mat4(1), glm::vec3(COLUMN_TO_X(canplays[i].GetColumn()) - CHESSBOARD_RECT_SIZE * .5, ROW_TO_Y(canplays[i].GetRow()) - CHESSBOARD_RECT_SIZE * .5, 0)) * scale;
    }
    g_SelectChess.UpdateUniform(device, model);
}
//以下2个函数只处理显示的线框
void SelectChess(VkDevice device, const Chess *pChess){
    std::vector<Chess>canplays;
    pChess->Selected(g_Game.GetChess(), canplays);
    UpdateSelectChessUniform(device, canplays);
}
struct PPC{
    uint32_t srcRow;
    uint32_t dstRow;
    uint32_t srcColumn;
    uint32_t dstColumn;
    uint32_t srcCountry;
    uint32_t dstCountry;
};
void *PlayChessFun(void *userData){
    PPC ppc = *(PPC *)userData;
    Chess *pStart = g_Game.GetChess(ppc.srcCountry, ppc.srcRow, ppc.srcColumn);
    PlayChess(pStart, ppc.dstRow, ppc.dstColumn);
    uint32_t country = g_Game.Check();
    uint32_t lastCountry = g_Game.GetCurrentCountry();
    if(country != INVALID_COUNTRY_INDEX && country != lastCountry && g_Game.GetNextCountry() != country){
        g_Game.ExtraTurn(country);
    }
    else{
        g_Game.NextCountry();
    }
    //下完棋要调用下面的函数
    g_Ai.EnableNextCountry();
    return nullptr;
}
int32_t GetCanPlay(const glm::vec2&mousePos, const std::vector<Chess>&canplays){
    int32_t index = -1;
    for (size_t i = 0; i < canplays.size(); ++i){
        const uint32_t chessY = ROW_TO_Y(canplays[i].GetRow()) - CHESSBOARD_RECT_SIZE * .5;
        const uint32_t chessX = COLUMN_TO_X(canplays[i].GetColumn()) - CHESSBOARD_RECT_SIZE * .5;
        if(mousePos.x > chessX && mousePos.y > chessY && mousePos.x < chessX + CHESSBOARD_RECT_SIZE && mousePos.y < chessY + CHESSBOARD_RECT_SIZE){
            index = i;
            break;
        }
    }
    return index;    
}
Chess *GetChess(const glm::vec2&mousePos){
    Chess *pChess = nullptr;
    for (size_t uiCountry = 0; uiCountry < MAX_COUNTRY_INDEX; ++uiCountry){
        pChess = g_Game.GetChess(uiCountry, mousePos);
        if(pChess){
            break;
        }
    }
    return pChess;
}

const Chess *SelectChess(uint32_t country, const glm::vec2&mousePos){
    //该函数只使用于单人模式
#ifdef DEBUG
    const Chess *pSelected = GetChess(mousePos);
    //想让玩家不能操作其他势力棋子, 可以用下面的函数
#else
    const Chess *pSelected = g_Game.GetChess(country, mousePos);
#endif
    if(g_Game.IsOnline()){
        if(country != g_Players[g_OnLine.GetClientIndex()].uCountry){
            pSelected = nullptr;
        }
    }
    // else{
    //     if(country != g_Game.GetPlayer()){
    //         pSelected = nullptr;
    //     }
    // }
    if(pSelected){
        SelectChess(g_VulkanDevice.device, pSelected);
    }
    return pSelected;
}
void PreparePlayChess(const Chess *pSelect, const glm::vec2&mousePos){
    static PPC ppc;
    const uint32_t clientIndex = g_OnLine.GetClientIndex();
    std::vector<Chess>canplays;
    pSelect->Selected(g_Game.GetChess(), canplays);
    int32_t index = GetCanPlay(mousePos, canplays);

    UnSelectChess();
    Chess *pTarget = GetChess(mousePos);
    if(pTarget){
        //选到的是棋子，不论是否自己的
        if(pTarget->GetCountry() != pSelect->GetCountry() && -1 != index){
            if(g_Game.IsOnline()){
                //单机和局域网联机差不多。无非就是要接收和发送消息
                g_OnLine.SendPlayChessMessage(g_Players[clientIndex], pSelect, pTarget);
            }
            else{
                ppc.dstRow = pTarget->GetRow();
                ppc.srcRow = pSelect->GetRow();
                ppc.dstColumn = pTarget->GetColumn();
                ppc.dstCountry = pTarget->GetCountry();
                ppc.srcColumn = pSelect->GetColumn();
                ppc.srcCountry = pSelect->GetCountry();
                CreateThread(PlayChessFun, &ppc);
            }
        }
    }
    else{
        if(index != -1){
            if(g_Game.IsOnline()){
                //单机和局域网联机差不多。无非就是要接收和发送消息
                Chess target;
                target.SetCountry(MAX_COUNTRY_INDEX);
                target.SetPos(canplays[index].GetRow(), canplays[index].GetColumn());
                g_OnLine.SendPlayChessMessage(g_Players[clientIndex], pSelect, &target);
            }
            else{
                ppc.srcRow = pSelect->GetRow();
                ppc.srcColumn = pSelect->GetColumn();
                ppc.dstRow = canplays[index].GetRow();
                ppc.dstColumn = canplays[index].GetColumn();
                ppc.srcCountry = pSelect->GetCountry();
                CreateThread(PlayChessFun, &ppc);
            }
        }
    }
}
const Chess *g_Selected = nullptr;
void mousebutton(GLFWwindow *window,int button,int action,int mods){
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    const glm::vec2 mousePos = glm::vec2(xpos, ypos);
    if (g_Game.IsGameStart() && action == GLFW_RELEASE && button == GLFW_MOUSE_BUTTON_LEFT){
        if(g_Selected){
            PreparePlayChess(g_Selected, mousePos);
            g_Selected = nullptr;
        }
        else{
            g_Selected = SelectChess(g_Game.GetCurrentCountry(), mousePos);
        }
    }
}
// #ifdef JOIN_AI
// uint32_t g_BackupPlayer;
// #endif
void keybutton(GLFWwindow *window,int key, int scancode, int action, int mods){
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
    vkDestroyPipeline(g_VulkanDevice.device, g_Fill, VK_NULL_HANDLE);
    vkDestroyPipeline(g_VulkanDevice.device, g_Font, VK_NULL_HANDLE);
    vkDestroyPipeline(g_VulkanDevice.device, g_Wireframe, VK_NULL_HANDLE);
}
void CreateGraphicsPipeline(VkDevice device, VkPipelineLayout layout){
    // state.mColorBlend.blendEnable = VK_TRUE;
    // state.mColorBlend.alphaBlendOp = VK_BLEND_OP_ADD;
    // state.mColorBlend.colorBlendOp = VK_BLEND_OP_ADD;
    // //该混合方式, 不依赖着色器输出的颜色决定透明度, 但必须需要2个片段以上才看得出来
    // state.mColorBlend.srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_COLOR;
    // state.mColorBlend.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_COLOR;
    // state.mColorBlend.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
    // state.mColorBlend.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
    // //该混合方式, 依赖着色器输出颜色中的a(w)值决定透明度, 1个片段也能看出来
    // // state.mColorBlend.srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    // // state.mColorBlend.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    // // state.mColorBlend.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    // // state.mColorBlend.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    //imgui绘制字体时的混合方式
    // state.mColorBlend.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    // state.mColorBlend.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    // state.mColorBlend.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    // state.mColorBlend.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;//VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
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

    shaderStages[0] = Pipeline::LoadShader(device, "shaders/baseVert.spv", VK_SHADER_STAGE_VERTEX_BIT);
    shaderStages[1] = Pipeline::LoadShader(device, "shaders/baseFrag.spv", VK_SHADER_STAGE_FRAGMENT_BIT);

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
    VK_CHECK(vkCreateGraphicsPipelines(device, g_pipelineCache, 1, &pipelineCreateInfo, nullptr, &g_Fill));
    rasterizationState = Pipeline::initializers::pipelineRasterizationStateCreateInfo(VK_POLYGON_MODE_LINE, VK_CULL_MODE_NONE, VK_FRONT_FACE_CLOCKWISE, 0);
    VK_CHECK(vkCreateGraphicsPipelines(device, g_pipelineCache, 1, &pipelineCreateInfo, nullptr, &g_Wireframe));
    vkDestroyShaderModule(device, shaderStages[0].module, VK_NULL_HANDLE);
    vkDestroyShaderModule(device, shaderStages[1].module, VK_NULL_HANDLE);
    rasterizationState = Pipeline::initializers::pipelineRasterizationStateCreateInfo(VK_POLYGON_MODE_FILL, VK_CULL_MODE_NONE, VK_FRONT_FACE_CLOCKWISE, 0);
    shaderStages[0] = Pipeline::LoadShader(device, "shaders/baseFontVert.spv", VK_SHADER_STAGE_VERTEX_BIT);
    shaderStages[1] = Pipeline::LoadShader(device, "shaders/baseFontFrag.spv", VK_SHADER_STAGE_FRAGMENT_BIT);
    VK_CHECK(vkCreateGraphicsPipelines(device, g_pipelineCache, 1, &pipelineCreateInfo, nullptr, &g_Font));
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
    
    Pipeline::CreatePipelineCache(g_VulkanDevice.device, "GraphicsPipelineCache", g_pipelineCache);

    CreatePipelineLayout(g_VulkanDevice.device, g_SetLayout);
    CreateGraphicsPipeline(g_VulkanDevice.device, g_PipelineLayout);

    g_Chess.Setup(g_VulkanDevice, g_SetLayout, g_VulkanQueue.graphics, g_VulkanPool);
    g_VulkanChessboard.Setup(g_VulkanDevice, g_SetLayout, g_VulkanQueue.graphics, g_VulkanPool);
    g_VulkanChessboard.UpdateUniform(g_VulkanDevice.device, g_WindowWidth);
    g_SelectChess.Setup(g_VulkanDevice, CHESSBOARD_ROW + CHESSBOARD_COLUMN, g_SetLayout, g_VulkanQueue.graphics, g_VulkanPool);

    // NewGame();

    g_VulkanPool.AllocateCommandBuffers(g_VulkanDevice.device, 1, &g_CommandBuffers);

    //imgui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO&io = ImGui::GetIO();
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;//启用手柄
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForVulkan(window, true);
    g_VulkanImgui.Setup(g_VulkanDevice, g_VulkanQueue.graphics, g_VulkanPool);
    g_VulkanImgui.CreatePipeline(g_VulkanDevice.device, g_VulkanWindow.renderPass, g_pipelineCache);
    g_OnLine.Init();
    for (size_t i = 0; i < MAX_COUNTRY_INDEX - 1; i++){
        ResetCountryItem(g_CountryItems[i]);
    }
}

void Cleanup(){
    vkDeviceWaitIdle(g_VulkanDevice.device);
    ImGui_ImplGlfw_Shutdown();

    g_OnLine.Cleanup();
    g_VulkanImgui.Cleanup(g_VulkanDevice.device);
    Pipeline::DestroyPipelineCache(g_VulkanDevice.device, "GraphicsPipelineCache", g_pipelineCache);

    vkDestroyDescriptorSetLayout(g_VulkanDevice.device, g_SetLayout, VK_NULL_HANDLE);
    vkDestroyPipelineLayout(g_VulkanDevice.device, g_PipelineLayout, VK_NULL_HANDLE);

    DestroyGraphicsPipeline(g_VulkanDevice.device);

    g_Chess.Cleanup(g_VulkanDevice.device);
    g_SelectChess.Cleanup(g_VulkanDevice.device);
    g_VulkanChessboard.Cleanup(g_VulkanDevice.device);

}
void RecreateSwapchain(void *userData){
    vkDeviceWaitIdle(g_VulkanDevice.device);
    GLFWwindow* window = (GLFWwindow*)userData;
    int32_t width, height;
    glfwGetWindowSize(window, &width, &height);
    //windows下最小化后无法复原//调试显示, 在等待currentFrame为2的栏杆对象后得不到回复
    if (width < 1 || height < 1)return;
    //想不调用vkDeviceWaitIdle重建交换链也行, 在重新创建交换链时, VkSwapchainCreateInfoKHR的结构体中oldSwapChain设置为原来的交换链
    g_VulkanWindow.CleanupSwapchain(g_VulkanDevice.device);
   const VkExtent2D swapchainExtent = g_VulkanWindow.swapchain.extent;
   g_VulkanWindow.swapchain.CreateSwapchain(g_VulkanDevice, g_VulkanWindow.surface);
    g_WindowWidth = g_VulkanWindow.swapchain.extent.width;
    g_WindowHeight = g_VulkanWindow.swapchain.extent.height;
    g_VulkanWindow.CreateRenderPass(g_VulkanDevice.device, g_VulkanWindow.swapchain.format);
    g_VulkanWindow.CreateFrameBuffer(g_VulkanDevice, true);
    
    if(swapchainExtent.width != g_WindowWidth || swapchainExtent.height != g_WindowHeight){
        DestroyGraphicsPipeline(g_VulkanDevice.device);
        CreatePipelineLayout(g_VulkanDevice.device, g_SetLayout);
        CreateGraphicsPipeline(g_VulkanDevice.device, g_PipelineLayout);
    }
}
void display(GLFWwindow* window){
    static size_t currentFrame;
    vkDeviceWaitIdle(g_VulkanDevice.device);
    RecordCommand(g_CommandBuffers, g_VulkanWindow.framebuffers[currentFrame]);
    if(VK_ERROR_OUT_OF_DATE_KHR == vulkanFrame::Render(g_VulkanDevice.device, currentFrame, g_CommandBuffers, g_VulkanWindow.swapchain.swapchain, g_VulkanQueue, g_VulkanSynchronize, RecreateSwapchain, window)){
        //重建交换链后, 即使currentFrame不为0, 获取的交换链图片索引不一定和currentFrame相同
        currentFrame = 0;
        //那重建交换链后, 那重建后有没有可能得到的索引不是0?
    }
    else{
        currentFrame = (currentFrame + 1) % g_VulkanWindow.framebuffers.size();
    }
}
int main(){
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

    Cleanup();
    CleanupVulkan();
    return 0;
}
