#include <mutex>
#include <time.h>
#include <stdio.h>
#ifdef __linux
#include <unistd.h>
#endif
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "Chessboard.h"
#include "VulkanChessboard.h"
#define JOIN_AI
//HAN_CAN_PLAY在VulkanChess.h文件
// #define INTERNET_MODE
#ifdef INTERNET_MODE
#include <array>

#include "Server.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"
Client g_Client;
Server g_Server;
char g_ServerIp[MAX_BYTE];
VkCommandBuffer g_CommandBuffers;
bool g_ServerAppaction, g_GameStart;
std::array<PlayerInfo, 3>g_Players;//用来保存其他玩家的信息//尽管也包含自身信息
//本来只需要1个,但服务端能改ai的势力, 所以需要加上ai的
char g_CountryItems[2][5][MAX_BYTE];
glm::vec2 g_JiangPos[4];//目前只用于RotateChess
uint32_t g_AIClientIndex = INVALID_PLAYER_INDEX;
#else
bool g_UpdateScreen;
std::vector<VkCommandBuffer>g_CommandBuffers;
#endif
VulkanPool g_VulkanPool;
VulkanQueue g_VulkanQueue;
VulkanDevice g_VulkanDevice;
VulkanWindows g_VulkanWindows;
VulkanSynchronize g_VulkanSynchronize;
VkDebugUtilsMessengerEXT g_VulkanMessenger;
//                              163 * 2 + 400 + 11
uint32_t g_WindowWidth = CHESSBOARD_BIG_GRID_SIZE * 2 + 10 * CHESSBOARD_GRID_SIZE + 11 * CHESSBOARD_LINE_WIDTH, g_WindowHeight = g_WindowWidth;

Chessboard g_Chessboard;
uint32_t g_ClientIndex = INVALID_COUNTRY_INDEX;
uint32_t g_CurrentCountry = INVALID_COUNTRY_INDEX;
#ifdef HAN_CAN_PLAY
const uint32_t g_DefaultCountryCount = 4;
#else
const uint32_t g_DefaultCountryCount = 3;
#endif
#ifdef JOIN_AI
#include "Ai.h"
#ifdef __linux
#include <semaphore.h>
sem_t g_AiSemaphore;
#endif
#ifdef WIN32
HANDLE g_AiSemaphore;
#endif
Ai g_AI;
#endif
VkSampler g_TextureSampler;
VkPipelineCache g_PipelineCache;
VkDescriptorSetLayout g_SetLayout;
void *AiPlayChess(void *userData);
// bool IsDangerous(uint32_t country, uint32_t rivalCountry, uint32_t chessIndex);
void MoveChess(uint32_t country, uint32_t chess, uint32_t row, uint32_t column);
void PlayChess(uint32_t srcCountry, uint32_t srcChess, uint32_t dstCountry, uint32_t dstChess);
// uint32_t jiang(uint32_t nextCounrty, const ChessInfo *pChess){
//     uint32_t countty = nextCounrty;
//     if(pChess){
//         g_Chessboard.SetSelected(pChess);
//         g_Chessboard.Selected(g_VulkanDevice.device);
//         auto canplay = g_Chessboard.GetCanPlay();
//         const char county[][MAX_BYTE] = { "魏", "蜀", "吴", "汉" };
//         for (size_t i = 0; i < canplay.size(); i++){
//             const ChessInfo *pInfo = g_Chessboard.GetChessInfo(canplay[i].row, canplay[i].column);
//             if(pInfo && pInfo->chess == JIANG_CHESS_INDEX){
//                 countty = pInfo->country;
//                 printf("%s国被\"将\"\n", pInfo->country);
//                 break;
//             }
//         }
//         g_Chessboard.ClearCanPlay(g_VulkanDevice.device);
//     }
//     return countty;
// }
bool GameOver(uint32_t countryCount){
    uint32_t deathCount = 0;
    for (size_t i = 0; i < countryCount; ++i){
        if(g_Chessboard.IsDeath(i)){
            ++deathCount;
        }
    }
    return deathCount > 1;
}
uint32_t GetNextCountry(uint32_t currentCountry){
    // currentCountry = g_ClientIndex;
    do{
        currentCountry = (currentCountry + 1) % g_DefaultCountryCount;
    } while (g_Chessboard.IsDeath(currentCountry));
    //"将"警告
    return currentCountry;
}
#ifdef JOIN_AI
void AiWait(){
#ifdef __linux__
    sem_wait(&g_AiSemaphore);
#endif
#ifdef WIN32
    WaitForSingleObject(g_AiSemaphore, INFINITE);
#endif // WIN32
}
void EnableAi(){
#ifdef __linux__
    sem_post(&g_AiSemaphore);
#endif
#ifdef WIN32
    SetEvent(g_AiSemaphore);
#endif // WIN32
}
#endif
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
void SetupVulkan(GLFWwindow *window){
    uint32_t count;
    const char** instanceExtension = glfwGetRequiredInstanceExtensions(&count);
    std::vector<const char*> extensions(instanceExtension, instanceExtension + count);
    vkf::CreateInstance(extensions, g_VulkanDevice.instance);
    g_VulkanDevice.physicalDevice = vkf::GetPhysicalDevices(g_VulkanDevice.instance, VK_NULL_HANDLE);
    vkf::CreateDebugUtilsMessenger(g_VulkanDevice.instance, g_VulkanMessenger, debugUtilsMessenger);
    glfwCreateWindowSurface(g_VulkanDevice.instance, window, nullptr, &g_VulkanWindows.surface);
    vkf::CreateDevice(g_VulkanDevice.physicalDevice, {}, g_VulkanWindows.surface, g_VulkanDevice.device);
    uint32_t queueFamilies[2];
    vkf::tool::GetGraphicAndPresentQueueFamiliesIndex(g_VulkanDevice.physicalDevice, g_VulkanWindows.surface, queueFamilies);
    vkGetDeviceQueue(g_VulkanDevice.device, queueFamilies[0], 0, &g_VulkanQueue.graphics);
    vkGetDeviceQueue(g_VulkanDevice.device, queueFamilies[1], 0, &g_VulkanQueue.present);
    vkf::CreateSwapchain(g_VulkanDevice.physicalDevice, g_VulkanDevice.device, g_VulkanWindows);
    vkf::CreateRenderPassForSwapchain(g_VulkanDevice.device, g_VulkanWindows.swapchainInfo.format, g_VulkanWindows.renderpass);
    vkf::CreateCommandPool(g_VulkanDevice.physicalDevice, g_VulkanDevice.device, g_VulkanPool.command, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
    vkf::CreateFrameBufferForSwapchain(g_VulkanDevice.device, g_VulkanWindows);
    vkf::CreateSemaphoreAndFenceForSwapchain(g_VulkanDevice.device, g_VulkanWindows.framebuffers.size(), g_VulkanSynchronize);
    vkf::CreateDescriptorPool(g_VulkanDevice.device, 8, g_VulkanPool.descriptor);
    //显示设备信息
    const char *deviceType;
    VkPhysicalDeviceProperties physicalDeviceProperties;
    vkGetPhysicalDeviceProperties(g_VulkanDevice.physicalDevice, &physicalDeviceProperties);
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
void CleanupSwapchain(VkDevice device, VulkanWindows&vulkanWindow){
    for (size_t i = 0; i < vulkanWindow.framebuffers.size(); ++i){
        vulkanWindow.swapchainImages[i].image = VK_NULL_HANDLE;
        vulkanWindow.swapchainImages[i].Destroy(device);
        vkDestroyFramebuffer(device, vulkanWindow.framebuffers[i], nullptr);
    }
    vulkanWindow.depthImage.Destroy(device);

    vkDestroyRenderPass(device, vulkanWindow.renderpass, nullptr);
    vkDestroySwapchainKHR(device, vulkanWindow.swapchain, nullptr);
}
void CleanupVulkan(){
    vkDeviceWaitIdle(g_VulkanDevice.device);
    for (size_t i = 0; i < g_VulkanSynchronize.fences.size(); ++i){
        vkDestroyFence(g_VulkanDevice.device, g_VulkanSynchronize.fences[i], VK_NULL_HANDLE);
        vkDestroySemaphore(g_VulkanDevice.device, g_VulkanSynchronize.imageAcquired[i], VK_NULL_HANDLE);
        vkDestroySemaphore(g_VulkanDevice.device, g_VulkanSynchronize.renderComplete[i], VK_NULL_HANDLE);
    }
    for (size_t i = 0; i < g_VulkanWindows.framebuffers.size(); ++i){
        g_VulkanWindows.swapchainImages[i].image = VK_NULL_HANDLE;
        g_VulkanWindows.swapchainImages[i].Destroy(g_VulkanDevice.device);
        vkDestroyFramebuffer(g_VulkanDevice.device, g_VulkanWindows.framebuffers[i], VK_NULL_HANDLE);
    }
    g_VulkanWindows.depthImage.Destroy(g_VulkanDevice.device);
    
    vkDestroyCommandPool(g_VulkanDevice.device, g_VulkanPool.command, VK_NULL_HANDLE);
    vkDestroyDescriptorPool(g_VulkanDevice.device, g_VulkanPool.descriptor, VK_NULL_HANDLE);

    vkDestroyRenderPass(g_VulkanDevice.device, g_VulkanWindows.renderpass, VK_NULL_HANDLE);
    vkDestroySwapchainKHR(g_VulkanDevice.device, g_VulkanWindows.swapchain, VK_NULL_HANDLE);

    vkDestroySurfaceKHR(g_VulkanDevice.instance, g_VulkanWindows.surface, VK_NULL_HANDLE);

    vkf::DestoryDebugUtilsMessenger(g_VulkanDevice.instance, g_VulkanMessenger);

    vkDestroyDevice(g_VulkanDevice.device, VK_NULL_HANDLE);
    vkDestroyInstance(g_VulkanDevice.instance, VK_NULL_HANDLE);
}
glm::vec2 lerp(const glm::vec2&p0, const glm::vec2&p1, float t){
    return (1 - t) * p0 + t * p1;
}
void MoveChess(uint32_t country, uint32_t chess, const glm::vec2&start, const glm::vec2&end){
    const ChessInfo *pInfo = g_Chessboard.GetChessInfo(country, chess);
    VkExtent2D size;
    size.width = CHESS_WIDTH;
    size.height = CHESS_HEIGHT;
    for (float t = 0; t < 1; t += .01){
    // for (float t = 0; t < 1; t += .015){
        const glm::vec2 pos = lerp(start, end, t);
        // printf("pos:%f, %f\n", pos.x, pos.y);
        g_Chessboard.UpdateChessUniform(g_VulkanDevice.device, country, chess, pos, pInfo->fontIndex, size);
#ifdef WIN32
        Sleep(1);
#else
        usleep(10000);
#endif
    }
}
void CreateThread(void *(*__start_routine)(void *), void *__arg){
#ifdef WIN32
    DWORD  threadId;
    HANDLE pthreadId;
    pthreadId = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)__start_routine, __arg, 0, &threadId);
#endif
#ifdef __linux
    pthread_t pthreadId;
    pthread_create(&pthreadId, nullptr, __start_routine, __arg);
#endif
}
#ifdef INTERNET_MODE
void InitJiangPos(){
    const Chess *pChess = g_Chessboard.GetChess(WEI_COUNTRY_INDEX, JIANG_CHESS_INDEX);
    g_JiangPos[WEI_COUNTRY_INDEX] = glm::vec2(pChess->GetInfo()->column, pChess->GetInfo()->row);
    pChess = g_Chessboard.GetChess(SHU_COUNTRY_INDEX, JIANG_CHESS_INDEX);
    g_JiangPos[SHU_COUNTRY_INDEX] = glm::vec2(pChess->GetInfo()->column, pChess->GetInfo()->row);
    pChess = g_Chessboard.GetChess(WU_COUNTRY_INDEX, JIANG_CHESS_INDEX);
    g_JiangPos[WU_COUNTRY_INDEX] = glm::vec2(pChess->GetInfo()->column, pChess->GetInfo()->row);
    pChess = g_Chessboard.GetChess(HAN_COUNTRY_INDEX, JIANG_CHESS_INDEX);
    g_JiangPos[HAN_COUNTRY_INDEX] = glm::vec2(pChess->GetInfo()->column, pChess->GetInfo()->row);
}
void JoinsGame(){
// void JoinsGame(const char *clientName){
    GameMessage message;
    // strcpy(message.name, clientName);
    message.event = JOINS_GAME_GAME_EVENT;
    g_Client.SendTo(&message, sizeof(message));
}
void GetLocalIp(char outIp[]){
    int status = -1;
    char buffer[MAX_BYTE] = {0};
    if(!gethostname(buffer, sizeof(buffer))){
#ifdef WIN32
        struct addrinfo* result = NULL;
        //struct addrinfo* ptr = NULL;
        struct addrinfo hints;
        ZeroMemory(&hints, sizeof(hints));
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_protocol = IPPROTO_TCP;
        getaddrinfo(buffer, nullptr, &hints, &result);
        struct addrinfo* ptr;
        for (ptr = result; ptr != NULL;) {
            char* local_ip = NULL;
            struct sockaddr_in* sockaddr = (struct sockaddr_in*)ptr->ai_addr;
            local_ip = inet_ntoa(sockaddr->sin_addr);
            if (local_ip) {
                strcpy(outIp, local_ip);
                if(strcmp("127.0.0.1", outIp) && strcmp("0.0.0.0", outIp)){
                    break;
                }
            }
            ptr = ptr->ai_next;
        }
#else
        hostent* temp = gethostbyname(buffer);
        if (temp) {
            for (size_t i = 0; temp->h_addr_list[i]; ++i) {
                char* local_ip = NULL;
                local_ip = inet_ntoa(*(struct in_addr*)temp->h_addr_list[i]);
                if (local_ip) {
                    strcpy(outIp, local_ip);
                    if (strcmp("127.0.0.1", outIp)) {
                        break;
                    }
                }
            }
        }
#endif
    }
}
void RandomNumber(uint32_t max, uint32_t count, uint32_t *result, uint32_t min = 0){
    for (size_t i = 0; i < count; ++i){
        result[i] = rand() % max + min;
        for (size_t j = 0; j < i; ++j){
            if(result[i] == result[j]){
                --i;
                continue;
            }
        }
    }
    
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
    else{
        printf("error:invalid country:%s\n", country);
    }
    return INVALID_COUNTRY_INDEX;
}
bool CanUseCountry(uint32_t countryIndex, const char *country){
    bool bCanUse = true;
    uint32_t playerIndex = 0;
    for (auto it = g_Players.begin(); it != g_Players.end(); ++it, ++playerIndex){
        if(countryIndex != playerIndex && !strcmp(it->country, country)){
            bCanUse = false;
            break;
        }
    }
    return bCanUse;
}
void Swap(uint32_t *src, uint32_t *dst){
    uint32_t temp = *src;
    *src = *dst;
    *dst = temp;
}
void RandomCountry(){
    uint32_t countryIndex[3];
    const char *countryName[4];
    countryName[WU_COUNTRY_INDEX] = "吴";
    countryName[WEI_COUNTRY_INDEX] = "魏";
    countryName[SHU_COUNTRY_INDEX] = "蜀";
#ifdef HAN_CAN_PLAY
    countryName[HAN_COUNTRY_INDEX] = "汉";
#endif
    RandomNumber(g_DefaultCountryCount, g_DefaultCountryCount, countryIndex);
    for (auto it = g_Players.begin(); it != g_Players.end(); ++it){
        uint32_t uiCountry = countryToindex(it->country);
        printf("in function %s:it->random %d country index:%d, country:%s\n", __FUNCTION__, it->random, it->index, it->country);
        if(!it->random && g_ClientIndex < g_DefaultCountryCount - 1){
            for (size_t i = g_ClientIndex + 1; i < g_DefaultCountryCount; ++i){
                if(uiCountry == countryIndex[i]){
                    Swap(&countryIndex[g_ClientIndex], &countryIndex[i]);
                    break;
                }
            }
        }
    }
    printf("---------------------------------\n");
    uint32_t playerIndex = 0;
    for (auto it = g_Players.begin(); it != g_Players.end(); ++it, ++playerIndex){
        strcpy(it->country, countryName[countryIndex[playerIndex]]);
        it->index = countryIndex[playerIndex];
        printf("in function %s:country index:%d, country:%s\n", __FUNCTION__, it->index, it->country);
    }
}
void SendChangeCountryInfomation(uint32_t client, const char *srcCountry){
    GameMessage message;
    message.clientIndex = client;
    strcpy(message.player.country, srcCountry);
    message.event = CHANGE_PLAYER_COUNTRY_GAME_EVENT;
    g_Client.SendTo(&message, sizeof(message));
}
void SendClientInfoation(){
    GameMessage message;
    message.event = JOINS_GAME_GAME_EVENT;
    for (uint32_t uiPlayer = 0; uiPlayer < g_Players.size(); ++uiPlayer){
        if(g_Players[uiPlayer].index != INVALID_PLAYER_INDEX){
            message.clientIndex = uiPlayer;
            strcpy(message.player.name, g_Players[uiPlayer].name);
            strcpy(message.player.country, g_Players[uiPlayer].country);
            g_Server.SendToAllClient(&message, sizeof(message));
        }
    }
}
void SendPlayersInfoation(){
    GameMessage message;
    message.event = CHANGE_PLAYER_INFORMATION_GAME_EVENT;
    for (uint32_t uiPlayer = 0; uiPlayer < g_Players.size(); ++uiPlayer){
        if(g_Players[uiPlayer].index != INVALID_PLAYER_INDEX){
            message.clientIndex = uiPlayer;
            message.player.ai = g_Players[uiPlayer].ai;
            message.player.index = g_Players[uiPlayer].index;
            strcpy(message.player.name, g_Players[uiPlayer].name);
            strcpy(message.player.country, g_Players[uiPlayer].country);
            g_Server.SendToAllClient(&message, sizeof(message));
        }
    }
}
void SendCurrentCountry(){
    // g_CurrentCountry = WEI_COUNTRY_INDEX;
    g_CurrentCountry = rand() % g_DefaultCountryCount;
    GameMessage message;
    message.event = CURRENT_COUNTRY_GAME_EVENT;
    message.clientIndex = g_CurrentCountry;
    g_Server.SendToAllClient(&message, sizeof(message));
}
void SendPlayChessMessage(const PlayerInfo&game, const ChessInfo *pSelect, const ChessInfo *pTarget){
    GameMessage message;
    message.event = PLAY_CHESS_GAME_EVENT;
    message.player = game;
    // message.mousePos = mousePos;
    if(pSelect)message.select = *pSelect;
    if(pTarget)message.target = *pTarget;
    g_Client.SendTo(&message, sizeof(message));
}
/*
    将char countryItems[5][MAX_BYTE]里面和玩家的势力相同的势力排除
    ->将char countryItems[5][MAX_BYTE]里面和xx相同势力排除
*/
//第二个参数:true:将items里面不和country重复的势力设置为原势力
void DeleteCountryItem(const char *country, char countryItems[5][MAX_BYTE], bool reset = true){
    const char *srcCountry[] = { "?", "魏", "蜀", "吴", "汉" };
    if(reset){
        for (size_t i = 0; i < sizeof(country) / sizeof(country[MAX_BYTE]); ++i){
            if(!strcmp(srcCountry[i], country)){
                strcpy(countryItems[i], countryItems[0]);
                break;
            }
            else {
                strcpy(countryItems[i], srcCountry[i]);
            }  
        }
    }
    else{
        for (size_t i = 0; i < sizeof(country) / sizeof(country[MAX_BYTE]); ++i){
            if(!strcmp(srcCountry[i], country)){
                strcpy(countryItems[i], countryItems[0]);
                break;
            }
        }
    }
}
// //第一个参数是被修改的玩家
// void DeleteCountryItem(uint32_t countryIndex, char countryItems[5][MAX_BYTE]) {
//     uint32_t index = 0;
//     for (auto it = g_Players.begin(); it != g_Players.end(); ++it, ++index) {
//         if (it->index != INVALID_PLAYER_INDEX && it->index != countryIndex) {
//             DeleteCountryItem(it->country, true, countryItems);
//         }
//     }
// }
int32_t GetAiPlayerIndex(){
    int32_t index = INVALID_PLAYER_INDEX;
    for (size_t i = 0; i < g_Players.size(); ++i){
        if(g_Players[i].ai){
            index = i;
            break;
        }
    }
    return index;
}
#define PROCESS_SERVER_RELAY
#ifdef PROCESS_SERVER_RELAY
void *process_server(void *userData){
    GameMessage message;
    uint32_t socketindex = *(SOCKET *)userData;
    do{
        g_Server.RecvFromClient(socketindex, &message, sizeof(message));
        g_Server.SendToAllClient(&message, sizeof(message));
    } while (message.event != GAME_OVER_GAME_EVENT);
    return nullptr;
}
void *server_start(void *userData){
    GameMessage message;
    static uint32_t socketIndex[] = { 0, 1, 2 };
    for (uint32_t i = 0; i < g_DefaultCountryCount; ++i){
        if(INVALID_SOCKET != g_Server.AcceptClient(i, &message, sizeof(message))){
            printf("in function %s:socket index %d\n", __FUNCTION__, i);
            if(message.event == AI_JOIN_GAME_GAME_EVENT){
                g_Players[i].ai = true;
                g_AIClientIndex = i;
                strcpy(message.player.name, "ai");
                g_Server.ShutdownClient(i);
                // if(i == 1)DeleteCountryItem(g_Players[0].country, g_CountryItems[1]);
            }
            else{
                sprintf(message.player.name, "player%d", i + 1);
            }
            g_Players[i].index = i;
            strcpy(g_Players[i].name, message.player.name);

            SendClientInfoation();
            if(message.event != AI_JOIN_GAME_GAME_EVENT)
                CreateThread(process_server, &socketIndex[i]);
        }        
    }
    RandomCountry();
    SendCurrentCountry();
    SendPlayersInfoation();
    //游戏开始, 发消息告诉所有客户端游戏开始
    message.event = GAME_START_GAME_EVENT;
    g_Server.SendToAllClient(&message, sizeof(message));
#ifdef JOIN_AI
    //因为最多只有1个ai, 所以可以直接循环里面判断和创建线程
    for (auto it = g_Players.begin(); it != g_Players.end(); ++it){
        if(it->ai){
            g_AI.CreatePthread(AiPlayChess, NULL);
            break;
        }
    }
#endif
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
#ifdef JOIN_AI
        //因为最多只有1个ai, 所以可以直接循环里面判断和创建线程
        for (auto it = g_Players.begin(); it != g_Players.end(); ++it){
            if(it->ai){
                g_AI.CreatePthread(AiPlayChess, NULL);
                break;
            }
        }
#endif  
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
// uint32_t GetTerritoryIndex(uint32_t row, uint32_t column){
//     if(row < 13 && row > 3){
//         if(column > 4){
//             return WU_TERRITORY_INDEX;
//         }
//         else{
//             return HAN_TERRITORY_INDEX;
//         }
//     }
//     else if(column > 3 && column < 13){
//         //魏或蜀
//         if(row > 4){
//             return SHU_TERRITORY_INDEX;
//         }
//         else{
//             return WEI_TERRITORY_INDEX;
//         }
//     }
//     return INVALID_TERRITORY_INDEX;
// }
void Invert(uint32_t row, uint32_t column, uint32_t&newRow, uint32_t&newColumn){
    newRow = abs((float)CHESSBOARD_ROW - row);
    newColumn = abs((float)CHESSBOARD_COLUMN - column);
}
void RotateChess(uint32_t row, uint32_t column, uint32_t country, uint32_t&newRow, uint32_t&newColumn){
    //如果将同列翻转，否则换行列
    //换行列:如果目标在汉位置, 则换完后列-16， 如果在魏那边行-16
    //select 14行11列  target 0行11列
    //14行11列->11行14列->11行2列
    //0行11列->11行0列->11行16列
    //14行11列->11行14列->5行2列
    if(g_ServerAppaction && country == g_Players[g_AIClientIndex].index)return;
    if(country != g_Players[g_ClientIndex].index){
        uint32_t aiIndex = GetAiPlayerIndex();
        //因为ai下棋的位置和服务端一样，所以必须用服务端的国家索引
        uint32_t uiCountry = country == aiIndex?g_Players[0].index:country;
        if(g_JiangPos[uiCountry].x == g_JiangPos[g_Players[g_ClientIndex].index].x){
            Invert(row, column, newRow, newColumn);
        }
        else{
            newRow = column;
            newColumn = row;
            //要获得country在哪个territory, 但不知道行列
            const uint32_t territory = GET_TERRITORY_INDEX(uiCountry, g_Players[g_ClientIndex].index);
            if(HAN_COUNTRY_INDEX == territory){
                newColumn = abs((float)newColumn - CHESSBOARD_COLUMN);
            }
            else{
                newRow = abs((float)newRow - CHESSBOARD_ROW);
            }
        }
    }
    // if(count){
    //     glm::vec3 point;
    //     if(row < 8 || column < 8){
    //         count += 2;
    //         point = glm::vec3(column + 8, row + 8, .0f);
    //     }
    //     else{
    //         point = glm::vec3(column - 8, row - 8, .0f);
    //     }
    //     const float angle = glm::radians(90.0f *  count) * -1;
    //     const glm::vec3 zAxis = glm::vec3(0, 0, 1);
    //     glm::mat4 rotate = glm::rotate(glm::mat4(1), angle, zAxis);
    //     point = rotate * glm::vec4(point, 1.0);
    //     point.x += 8;
    //     point.y += 8;
    //     //取小数部分
    //     float y = point.y - (uint32_t)point.y;
    //     float x = point.x - (uint32_t)point.x;
    //     newRow = point.y;
    //     newColumn = point.x;
    //     newRow = y > .5?newRow + 1:newRow;
    //     newColumn = x > .5?newColumn + 1:newColumn;
    // }
}
//但该程序为客户端时,一般不会用上面的函数而是用下面的函数创建创建线程
void *process_client(void *userData){
    //接收消息后做出动作即可
    GameMessage message;
    do{
        g_Client.RecvFrom(&message, sizeof(GameMessage));
        if(message.event == GAME_START_GAME_EVENT){
            g_GameStart = true;
            printf("game start, you index is %d, you country index is %d\n", g_ClientIndex, g_Players[g_ClientIndex].index);
            g_Chessboard.InitChess(g_VulkanDevice.device, g_Players[g_ClientIndex].index);
            
            InitJiangPos();
        }
        else if(message.event == JOINS_GAME_GAME_EVENT){
            printf("joins game, client index:%d\n", message.clientIndex);
            const uint32_t index = message.clientIndex, aiIndex = GetAiPlayerIndex();
            strcpy(g_Players[index].name, message.player.name);
            if(!g_ServerAppaction){
                g_Players[index].index = message.clientIndex;
                strcpy(g_Players[index].country, message.player.country);
            }
            if(message.clientIndex != g_ClientIndex){
                DeleteCountryItem(message.player.country, g_CountryItems[0]);
            }
            else if(aiIndex != INVALID_PLAYER_INDEX && aiIndex != message.clientIndex){
                //这里改的是ai的countryItems;
                DeleteCountryItem(message.player.country, g_CountryItems[1]);
            }
        }
        else if(message.event == CURRENT_COUNTRY_GAME_EVENT){
            g_CurrentCountry = message.clientIndex;
            printf("current country %d\n", g_CurrentCountry);
        }
        else if(message.event == CHANGE_PLAYER_COUNTRY_GAME_EVENT){
            const uint32_t aiIndex = GetAiPlayerIndex();
            if(message.clientIndex != g_ClientIndex){
                const int32_t aiIndex = GetAiPlayerIndex();
                if(aiIndex != INVALID_PLAYER_INDEX && aiIndex != message.clientIndex)
                    DeleteCountryItem(message.player.country, g_CountryItems[1]);
                else
                    DeleteCountryItem(message.player.country, g_CountryItems[0]);
            }
            strcpy(g_Players[message.clientIndex].country, message.player.country);
            g_Players[message.clientIndex].random = !strcmp(message.player.country, "?");
            g_Players[message.clientIndex].index = countryToindex(message.player.country);
        }
        else if(message.event == SELF_PLAYER_INFORMATION_GAME_EVENT){
            g_ClientIndex = message.clientIndex;
        }
        else if(message.event == CHANGE_PLAYER_INFORMATION_GAME_EVENT){
            const uint32_t index = message.clientIndex, playerIndex = message.player.index;
            g_Players[index].index = playerIndex;
            g_Players[index].ai = message.player.ai;
            strcpy(g_Players[index].name, message.player.name);
            strcpy(g_Players[index].country, message.player.country);
        }
        else if(message.event == PLAY_CHESS_GAME_EVENT){
            ChessInfo target = message.target, select = message.select;
            RotateChess(target.row, target.column, g_CurrentCountry, target.row, target.column);
            RotateChess(select.row, select.column, g_CurrentCountry, select.row, select.column);
            printf("client index:%d, message.select row %d column %d;select row %d, column %d\n", g_ClientIndex, message.select.row, message.select.column, select.row, select.column);
            // const ChessInfo *pTarget  = g_Chessboard.GetChessInfo(target.country, target.chess), *pSelect = g_Chessboard.GetChessInfo(g_CurrentCountry, select.chess);
            const ChessInfo *pTarget  = g_Chessboard.GetChessInfos(target.row, target.column), *pSelect = g_Chessboard.GetChessInfos(select.row, select.column);
            if(pSelect){
                if(pTarget){
                    printf("catpure chess:country:%d, chess:%d;target country:%d, chess:%d\n", pSelect->country, pSelect->chess, pTarget->country, pTarget->chess);
                    PlayChess(pSelect->country, pSelect->chess, pTarget->country, pTarget->chess);
                }
                else{
                    printf("move chess:country:%d, chess:%d\n", pSelect->country, pSelect->chess);
                    //不能直接给鼠标坐标, 直接给行列。即使直接给行列,  target也一定是null
                    MoveChess(pSelect->country, pSelect->chess, target.row, target.column);
                }
            }
        }
        else{
            printf("invalid event:%d\n", message.event);
        }
    }while(message.event != GAME_OVER_GAME_EVENT);
    return nullptr;
}
void CreateServer(const char *name){
    g_ServerAppaction = true;
    g_Server.CreateServer(g_DefaultCountryCount);
    //必须用静态变量, 局部变量在函数结束后就销毁, 会导致传餐错误
#ifdef PROCESS_SERVER_RELAY
    CreateThread(server_start, nullptr);
#else
    static uint32_t socketIndex[] = { 0, 1, 2 };
    for (size_t i = 0; i < g_DefaultCountryCount; ++i){
        CreateThread(process_server, &socketIndex[i]);
    }
#endif
    g_Client.CreateClient(g_ServerIp);
    //该程序也需要客户端方面的线程
    CreateThread(process_client, nullptr);
    JoinsGame();
    // JoinsGame(g_Client.GetName());
}
void CreateClient(const char *serverIp){
    g_Client.CreateClient(serverIp);
    CreateThread(process_client, nullptr);
    JoinsGame();
    // JoinsGame(g_Client.GetName());
}
void AddAi(const char *serverIp){
    Client client;
    client.CreateClient(serverIp);
    GameMessage message;
    message.event = AI_JOIN_GAME_GAME_EVENT;
    client.SendTo(&message, sizeof(message));
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
void ShowPlayerCountryCombo(uint32_t comboIndex, const auto it){
    char comboName[MAX_BYTE] = " ";
    catspace(comboName, comboIndex);
    uint32_t aiIndex = GetAiPlayerIndex();
    static const char *currentCountryItem[] = { g_CountryItems[0][0], g_CountryItems[1][0] };
    if(!g_GameStart && (comboIndex == g_ClientIndex || (g_ServerAppaction && comboIndex == aiIndex))){
        if(ImGui::BeginCombo(comboName, currentCountryItem[it->ai])){
            for (size_t i = 0; i < g_DefaultCountryCount + 1; ++i){
                bool is_selected = (!strcmp(currentCountryItem[it->ai], g_CountryItems[it->ai][i]));
                if (ImGui::Selectable(g_CountryItems[it->ai][i], is_selected)){
                    currentCountryItem[it->ai] = g_CountryItems[it->ai][i];
                    printf("玩家更改势力, 新势力为%s\n", currentCountryItem[it->ai]);
                    SendChangeCountryInfomation(comboIndex == aiIndex?aiIndex:g_ClientIndex, currentCountryItem[it->ai]);
                }
                if (is_selected)ImGui::SetItemDefaultFocus();   // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)
            }
            ImGui::EndCombo();
        }
    }
    else{
        //这里只给看不给改
        if(ImGui::BeginCombo(comboName, it->country)){
            ImGui::EndCombo();
        }
    }
}
void ShowClientWidget(){
    //汉是为了在HAN_CAN_PLAY宏定义了的情况下设置的
    if(ImGui::BeginTable("player information", 2)){
        ImGui::TableNextColumn();
        ImGui::Text("名字");
        for (auto it = g_Players.begin(); it != g_Players.end(); ++it){
            if (it->index != INVALID_PLAYER_INDEX)
                ImGui::Text(it->name);
        }
        ImGui::TableNextColumn();
        ImGui::Text("势力");
        uint32_t comboIndex = 0;
        //combo(或许其他也是)需要靠label查找。所以label不能重复
        for (auto it = g_Players.begin(); it != g_Players.end(); ++it, ++comboIndex){
            if(it->index != INVALID_PLAYER_INDEX){
                ShowPlayerCountryCombo(comboIndex, it);
            }
        }
        ImGui::EndTable();
    }
}
void ShowGameWidget(){
    const char *countryName[4];
    countryName[WU_COUNTRY_INDEX] = "吴";
    countryName[WEI_COUNTRY_INDEX] = "魏";
    countryName[SHU_COUNTRY_INDEX] = "蜀";
#ifdef HAN_CAN_PLAY
    countryName[HAN_COUNTRY_INDEX] = "汉";
#endif
    if(g_CurrentCountry != -1){
        ImGui::Text("你是%s, 该%s下棋\n", countryName[g_Players[g_ClientIndex].index], g_Players[g_ClientIndex].index == g_CurrentCountry?"你":countryName[g_CurrentCountry]);
    }
    ShowClientWidget();
}
void ShowServerWidget(bool gameStart){
    //在非广播模式下，直接显示本机ip。(或许广播模式下也可以显示)
    ImGui::Text("本机ip:%s", g_ServerIp);
    ShowClientWidget();
    if(!g_GameStart){
        if(!gameStart){
            if(ImGui::Button("添加电脑")){
                AddAi(g_ServerIp);
            }
        }
    }
}
void UpdateImguiWidget(){
    // static bool checkbuttonstatu;//检查框的状态。这个值传给imgui会影响到检查框
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    static bool bShowConnectServer = false;
    // static bool bShowCreateServer = false, bShowConnectServer = false;
    if(ImGui::Begin("局域网联机")){
        if(g_GameStart){
            ShowGameWidget();
        }
        else{
            if(g_ServerAppaction){
                ShowServerWidget(g_GameStart);
            }
            else{
                ShowClientWidget();
            }
            if(!g_ServerAppaction){
                //如果创建好房间或连接了客户端后, 不应该继续显示下面的按钮
                if(ImGui::BeginTable("客户端信息", 2)){
                    ImGui::TableNextColumn();
                    if(ImGui::Button("创建服务端")){
                        CreateServer("name");
                        // bShowCreateServer = true;
                    }
                    ImGui::TableNextColumn();
                    if(ImGui::Button("连接服务端")){
                        bShowConnectServer = true;
                    }
                    ImGui::EndTable();
                }
            }
        }
        ImGui::End();
    }
    // if(bShowCreateServer){
    //     if(ImGui::Begin("创建房间")){
    //         static char name[MAX_BYTE] = {0};
    //         ImGui::InputText("输入房间名", name, sizeof(name));
    //         //目前用不到房间名, 所以不需要判断
    //         if(ImGui::BeginTable("确定取消按钮", 2)){
    //             ImGui::TableNextColumn();
    //             if(ImGui::Button("确定")){
    //                 bShowCreateServer = false;
    //                 CreateServer(name);
    //             }
    //             ImGui::TableNextColumn();
    //             if(ImGui::Button("取消")){
    //                 bShowCreateServer = false;
    //             }
    //             ImGui::EndTable();
    //         }
    //         ImGui::End();
    //     }
    // }
    if(bShowConnectServer){
        if(ImGui::Begin("连接服务端")){
            static char serverIp[MAX_BYTE] = {0};
            ImGui::InputText("服务端ip", serverIp, sizeof(serverIp));
            if(ImGui::BeginTable("确定取消按钮", 3)){
                ImGui::TableNextColumn();
                if(ImGui::Button("确定")){
                    bShowConnectServer = false;
                    CreateClient(serverIp);
                }
                ImGui::TableNextColumn();
                if(ImGui::Button("使用本机ip连接")){//这个按钮只能测试用...
                    strcpy(serverIp, g_ServerIp);
                    // bShowConnectServer = false;
                    // CreateClient(g_ServerIp);
                }
                ImGui::TableNextColumn();
                if(ImGui::Button("取消")){
                    bShowConnectServer = false;
                }
                ImGui::EndTable();
            }
            ImGui::End();
        }
    }
    ImGui::Render();
    ImDrawData *draw_data = ImGui::GetDrawData();

    const bool isMinimized = (draw_data->DisplaySize.x <=.0f || draw_data->DisplaySize.y <= .0f);
    if(!isMinimized)ImGui_ImplVulkan_RenderDrawData(draw_data, g_CommandBuffers);
}
#endif
void RecordCommand(VkCommandBuffer command, uint32_t currentFrame){
    std::vector<VkClearValue> clearValues(1);
    clearValues[0].color = { 0.1f , 0.1f , 0.1f , 1.0f };
    VkRenderPassBeginInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.clearValueCount = clearValues.size();
    renderPassInfo.pClearValues = clearValues.data();
    renderPassInfo.renderArea = { 0, 0, g_WindowWidth, g_WindowHeight };
    renderPassInfo.renderPass = g_VulkanWindows.renderpass;
    renderPassInfo.framebuffer = g_VulkanWindows.framebuffers[currentFrame];
    VkCommandBufferBeginInfo beginInfo;
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.pNext = nullptr;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
    beginInfo.pInheritanceInfo = nullptr;
    vkBeginCommandBuffer(command, &beginInfo);
    vkCmdBeginRenderPass(command, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    g_Chessboard.RecordCommand(command, g_WindowWidth, g_CurrentCountry);
#ifdef INTERNET_MODE
    UpdateImguiWidget();
#endif
    vkCmdEndRenderPass(command);
    vkEndCommandBuffer(command);
}
int32_t GetCanPlay(uint32_t row, uint32_t column, const std::vector<ChessInfo>&canplays){
    int32_t index = -1, i = 0;
    for (auto it = canplays.begin(); it != canplays.end(); ++it, ++i){
        if(row == it->row && column == it->column){
            index = i;
            break;
        }
    }
    return index;
}
int32_t GetCanPlay(const glm::vec2&mousePos, const std::vector<ChessInfo>&canplays){
    int32_t index = -1;
    for (size_t i = 0; i < canplays.size(); ++i){
        const uint32_t chessY = ROW_TO_Y(canplays[i].row) - CHESSBOARD_GRID_SIZE * .5;
        const uint32_t chessX = COLUMN_TO_X(canplays[i].column) - CHESSBOARD_GRID_SIZE * .5;
        if(mousePos.x > chessX && mousePos.y > chessY && mousePos.x < chessX + CHESSBOARD_GRID_SIZE && mousePos.y < chessY + CHESSBOARD_GRID_SIZE){
            index = i;
            break;
        }
    }
    return index;    
}
//走子
void MoveChess(uint32_t country, uint32_t chess, const glm::vec2&mousePos){
    std::vector<ChessInfo>canplays;
    const Chess *pChess = g_Chessboard.GetChess(country, chess);
    pChess->Selected((const Chess* (*)[COUNTRY_CHESS_COUNT])g_Chessboard.GetChess(), canplays);
    const int32_t index = GetCanPlay(mousePos, canplays);
    if(index != -1){
        const ChessInfo *pChessInfo = pChess->GetInfo();
        MoveChess(pChessInfo->country, pChessInfo->chess, glm::vec2(COLUMN_TO_X(pChessInfo->column), ROW_TO_Y(pChessInfo->row)), mousePos);
        g_Chessboard.Move(g_VulkanDevice.device, country, chess, canplays[index].row, canplays[index].column);
        g_CurrentCountry = GetNextCountry(g_CurrentCountry);
#ifdef JOIN_AI
#ifdef INTERNET_MODE
        if(g_Players[g_CurrentCountry].ai){
            EnableAi();
        }
#else
        if(g_CurrentCountry != g_ClientIndex){
            EnableAi();
        }
#endif
#endif
    }
}
void MoveChess(uint32_t country, uint32_t chess, uint32_t row, uint32_t column){
    std::vector<ChessInfo>canplays;
    const Chess *pChess = g_Chessboard.GetChess(country, chess);
    pChess->Selected((const Chess* (*)[COUNTRY_CHESS_COUNT])g_Chessboard.GetChess(), canplays);
    const int32_t index = GetCanPlay(row, column, canplays);
    if(index != -1){
        const ChessInfo *pChessInfo = pChess->GetInfo();
        MoveChess(pChessInfo->country, pChessInfo->chess, glm::vec2(COLUMN_TO_X(pChessInfo->column), ROW_TO_Y(pChessInfo->row)), glm::vec2(COLUMN_TO_X(canplays[index].column), ROW_TO_Y(canplays[index].row)));
        g_Chessboard.Move(g_VulkanDevice.device, country, chess, canplays[index].row, canplays[index].column);
        g_CurrentCountry = GetNextCountry(g_CurrentCountry);
#ifdef JOIN_AI
#ifdef INTERNET_MODE
    if(g_ServerAppaction){
        for (auto it = g_Players.begin(); it != g_Players.end(); ++it){
            if(it->ai && it->index == g_CurrentCountry){
                EnableAi();
                break;
            }
        }
    }
#else
        if(g_CurrentCountry != g_ClientIndex){
            EnableAi();
        }
#endif
#endif
    }
}
//吃子
void PlayChess(uint32_t srcCountry, uint32_t srcChess, uint32_t dstCountry, uint32_t dstChess){
    std::vector<ChessInfo>canplays;
    const char county[][MAX_BYTE] = { "魏", "蜀", "吴", "汉" };
    const Chess *pSrcChess = g_Chessboard.GetChess(srcCountry, srcChess);
    const ChessInfo *pSrcChessInfo = pSrcChess->GetInfo(), *pDstChessInfo = g_Chessboard.GetChessInfo(dstCountry, dstChess);
    pSrcChess->Selected((const Chess* (*)[COUNTRY_CHESS_COUNT])g_Chessboard.GetChess(), canplays);
    MoveChess(pSrcChessInfo->country, pSrcChessInfo->chess, glm::vec2(COLUMN_TO_X(pSrcChessInfo->column), ROW_TO_Y(pSrcChessInfo->row)), glm::vec2(COLUMN_TO_X(pDstChessInfo->column), ROW_TO_Y(pDstChessInfo->row)));
    g_Chessboard.Play(g_VulkanDevice.device, srcCountry, srcChess, dstCountry, dstChess);
    if(g_Chessboard.IsDeath(dstCountry)){
        printf("%s国被%s国消灭\n", county[dstCountry], county[g_CurrentCountry]);
        g_Chessboard.DestroyCountry(g_VulkanDevice.device, dstCountry);
    }
    g_CurrentCountry = GetNextCountry(g_CurrentCountry);
#ifdef JOIN_AI
#ifdef INTERNET_MODE
    if(g_ServerAppaction){
        for (auto it = g_Players.begin(); it != g_Players.end(); ++it){
            if(it->ai && it->index == g_CurrentCountry){
                EnableAi();
                break;
            }
        }
    }
#else
    if(g_CurrentCountry != g_ClientIndex){
        EnableAi();
    }
#endif
#endif
}
#ifdef JOIN_AI
#ifdef __linux
pthread_t g_AiPlayChess;
#endif
#ifdef WIN32
HANDLE g_AiPlayChess;
#endif
void aiPlay(){
    ChessInfo target;
    const ChessInfo *pSelect = nullptr, *pTarget = nullptr;
    g_AI.GetPlayChessInfo(g_CurrentCountry, &pSelect, &pTarget, &target.row, &target.column, &g_Chessboard);
    //想下的棋子、敌人上面设置
    //局域网模式下应该发送消息
#ifdef INTERNET_MODE
    if(pTarget){
        SendPlayChessMessage(g_Players[g_AIClientIndex], pSelect, pTarget);
    }
    else{
        SendPlayChessMessage(g_Players[g_AIClientIndex], pSelect, &target);
    }
#else
    g_Chessboard.Select(g_VulkanDevice.device, g_CurrentCountry, pSelect->chess);
    if(pTarget){
        PlayChess(g_CurrentCountry, pSelect->chess, pTarget->country, pTarget->chess);
    }
    else{
        MoveChess(g_CurrentCountry, pSelect->chess, target.row, target.column);
    }
    g_UpdateScreen = true;
    g_Chessboard.UnSelect(g_VulkanDevice.device, g_CurrentCountry, pSelect->chess);
#endif
}
void *AiPlayChess(void *userData){
#ifdef __linux__
#ifdef INTERNET_MODE
    sem_init(&g_AiSemaphore, 0, g_Players[g_AIClientIndex].index == g_CurrentCountry);
#else
    sem_init(&g_AiSemaphore, 0, g_ClientIndex != g_CurrentCountry);
#endif
#endif
#ifdef WIN32
    g_AiSemaphore = CreateEvent(NULL, FALSE, FALSE, "AiSemaphore");
#endif
    while(!GameOver(g_DefaultCountryCount)){
        AiWait();
        aiPlay();
    }
#ifdef __linux__
    sem_destroy(&g_AiSemaphore);
#endif
#ifdef WIN32
    ResetEvent(g_AiSemaphore);
#endif
    return nullptr;
}
#endif
//一般情况下, ai不需要调用该函数
const ChessInfo *SelectChess(uint32_t country, const glm::vec2&mousePos){
    const ChessInfo *pSelected = nullptr;
    if(country == g_CurrentCountry)pSelected = g_Chessboard.GetChessInfos(country, mousePos);
    if(pSelected && pSelected->country == country){
        g_Chessboard.Select(g_VulkanDevice.device, country, pSelected->chess);
    }
    return pSelected;
}
#ifndef INTERNET_MODE
struct PPC{
    bool capture;
    uint32_t srcChess;
    uint32_t dstChess;
    glm::vec2 mousePos;
    uint32_t srcCountry;
    uint32_t dstCountry;
};
void *PlayerPlayChess(void *userData){
    PPC ppc = *(PPC *)userData;
    if(ppc.capture){
        PlayChess(ppc.srcCountry, ppc.srcChess, ppc.dstCountry, ppc.dstChess);
    }
    else{
        uint32_t row, column;
        MoveChess(ppc.srcCountry, ppc.srcChess, ppc.mousePos);
    }
#ifndef INTERNET_MODE
    g_UpdateScreen = true;
#endif
    return nullptr;
}
#endif
const ChessInfo *g_Selected = nullptr;
void mousebutton(GLFWwindow *windows, int button, int action, int mods){
    if(action == GLFW_RELEASE && button == GLFW_MOUSE_BUTTON_LEFT){
#ifdef INTERNET_MODE
        //这里判断了国家是否为-1
        if(g_CurrentCountry == -1)return;
#endif
        double xpos, ypos;
        glfwGetCursorPos(windows, &xpos, &ypos);
        const glm::vec2 mousePos = glm::vec2(xpos, ypos);
        if(g_Selected){
            g_Chessboard.UnSelect(g_VulkanDevice.device, g_Selected->country, g_Selected->chess);
            const ChessInfo *pTarget = g_Chessboard.GetChessInfos(mousePos);
#ifdef INTERNET_MODE
            if(pTarget){
                if(pTarget->country != g_CurrentCountry){
                    //单机和局域网联机差不多。无非就是要接收和发送消息
                    SendPlayChessMessage(g_Players[g_ClientIndex], g_Selected, pTarget);
                    g_Selected = nullptr;
                }
                else{
                    if(pTarget->row != g_Selected->row || pTarget->column != g_Selected->column)
                        g_Selected = SelectChess(g_CurrentCountry, mousePos);
                    else
                        g_Selected = nullptr;
                }
            }
            else{
                std::vector<ChessInfo>canplays;
                const Chess *pChess = g_Chessboard.GetChess(g_Selected->country, g_Selected->chess);
                if(!pChess){
                    printf("in function %s - error:pChess is nullptr, g_Selected->country:%d, g_Selected->chess:%d\n", g_Selected->country, g_Selected->chess);
                }
                pChess->Selected((const Chess *(*)[COUNTRY_CHESS_COUNT])g_Chessboard.GetChess(), canplays);
                int32_t canplayIndex = GetCanPlay(mousePos, canplays);
                if(canplayIndex != -1){
                    ChessInfo target;
                    target.row = canplays[canplayIndex].row;
                    target.column = canplays[canplayIndex].column;
                    SendPlayChessMessage(g_Players[g_ClientIndex], g_Selected, &target);
                    g_Selected = nullptr;
                }
            }
#else
            static PPC ppc;
            if(pTarget){
                if(pTarget->country != g_CurrentCountry){
                    ppc.capture = true;
                    ppc.dstChess = pTarget->chess;
                    ppc.srcChess = g_Selected->chess;
                    ppc.dstCountry = pTarget->country;
                    ppc.srcCountry = g_CurrentCountry;
                    CreateThread(PlayerPlayChess, &ppc);
                    g_Selected = nullptr;
                }
                else{
                    if(pTarget->row != g_Selected->row || pTarget->column != g_Selected->column)
                        g_Selected = SelectChess(g_CurrentCountry, mousePos);
                    else
                        g_Selected = nullptr;
                }
            }
            else{
                ppc.capture = false;
                ppc.mousePos = mousePos;
                ppc.srcChess = g_Selected->chess;
                ppc.srcCountry = g_CurrentCountry;
                CreateThread(PlayerPlayChess, &ppc);
                g_Selected = nullptr;
            }
#endif
        }
        else{
#ifdef INTERNET_MODE
            g_Selected = SelectChess(g_Players[g_ClientIndex].index, mousePos);
#else
            g_Selected = SelectChess(g_ClientIndex, mousePos);
#endif
        }
    }
}
void SetupDescriptorSetLayout(VkDevice device){
    VkDescriptorSetLayoutBinding descriptorSetLayoutBindings[2] = {};
    // descriptorSetLayoutBindings[0].binding = 0;
    descriptorSetLayoutBindings[0].descriptorCount = 1;
    descriptorSetLayoutBindings[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    descriptorSetLayoutBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
    descriptorSetLayoutBindings[1].binding = 1;
    descriptorSetLayoutBindings[1].descriptorCount = 1;
    descriptorSetLayoutBindings[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    descriptorSetLayoutBindings[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    vkf::CreateDescriptorSetLayout(device, sizeof(descriptorSetLayoutBindings) / sizeof(VkDescriptorSetLayoutBinding), descriptorSetLayoutBindings, &g_SetLayout);
}
#ifdef JOIN_AI
bool isEnableAi;
uint32_t g_BackupPlayer;
#endif
void keybutton(GLFWwindow *window,int key, int scancode, int action, int mods){
    if(action == GLFW_RELEASE){
#ifdef JOIN_AI
        if(key == GLFW_KEY_SPACE){
            g_BackupPlayer = g_ClientIndex;
            //玩家不参与游戏
            g_ClientIndex = INVALID_COUNTRY_INDEX;
            if(!isEnableAi){
                g_AI.CreatePthread(AiPlayChess, nullptr);
                isEnableAi = true;
                printf("启用ai并且玩家无法参与\n");
            }
        }
        else if(key == GLFW_KEY_ENTER){
            g_ClientIndex = g_BackupPlayer;
            if(!isEnableAi){
                g_AI.CreatePthread(AiPlayChess, nullptr);
                isEnableAi = true;
                printf("启用ai并且玩家可以参与\n");
            }
        }
#endif
    }
}
void setup(GLFWwindow *window){
#ifdef __linux__
    srandom(time(nullptr));
#endif
#ifdef WIN32
    srand(time(nullptr));
#endif // WIN32
    glfwSetKeyCallback(window, keybutton);
    glfwSetMouseButtonCallback(window, mousebutton);

    SetupDescriptorSetLayout(g_VulkanDevice.device);
    vkf::CreateTextureSampler(g_VulkanDevice.device, g_TextureSampler);
    vkf::CreatePipelineCache(g_VulkanDevice.device, "GraphicsPipelineCache", g_PipelineCache);
#ifndef INTERNET_MODE
    // g_CurrentCountry = rand() % g_DefaultCountryCount;
    // g_ClientIndex = rand() % g_DefaultCountryCount;
    g_CurrentCountry = WEI_COUNTRY_INDEX;
    g_ClientIndex = WEI_COUNTRY_INDEX;
#endif
    g_Chessboard.Setup(g_VulkanDevice.physicalDevice, g_VulkanDevice.device, g_SetLayout, g_WindowWidth, g_VulkanQueue.graphics, g_VulkanPool);
    g_Chessboard.CreatePipeline(g_VulkanDevice.device, g_VulkanWindows.renderpass, g_SetLayout, g_PipelineCache, g_WindowWidth);

    g_Chessboard.UpdateBackgroundUniform(g_VulkanDevice.device, g_WindowWidth);
    g_Chessboard.UpdateUniform(g_VulkanDevice.device, g_ClientIndex);
#ifndef INTERNET_MODE
    g_Chessboard.InitChess(g_VulkanDevice.device, g_ClientIndex);
#endif
    //局域网联机需要imgui
    //先通过ip连接, 后面知道怎么广播再说
    //或者说，先实现简单的网络联机，在解决广播问题
#ifdef INTERNET_MODE
    vkf::tool::AllocateCommandBuffers(g_VulkanDevice.device, g_VulkanPool.command, 1, &g_CommandBuffers);
    //imgui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO&io = ImGui::GetIO();
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;//启用手柄
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForVulkan(window, true);
    uint32_t queueFamily;
    vkf::tool::GetGraphicAndPresentQueueFamiliesIndex(g_VulkanDevice.physicalDevice, VK_NULL_HANDLE, &queueFamily);
    ImGui_ImplVulkan_InitInfo initInfo = {};
    initInfo.Instance = g_VulkanDevice.instance;
    initInfo.PhysicalDevice = g_VulkanDevice.physicalDevice;
    initInfo.Device = g_VulkanDevice.device;
    initInfo.QueueFamily = queueFamily;
    initInfo.Queue = g_VulkanQueue.graphics;
    initInfo.PipelineCache = VK_NULL_HANDLE;//g_GraphicsPipline.getCache();
    initInfo.DescriptorPool = g_VulkanPool.descriptor;
    initInfo.MinImageCount = 2;
    initInfo.ImageCount = g_VulkanWindows.framebuffers.size();
    initInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
    initInfo.Allocator = nullptr;
    initInfo.CheckVkResultFn = nullptr;
    ImGui_ImplVulkan_Init(&initInfo, g_VulkanWindows.renderpass);

    io.Fonts->AddFontFromFileTTF("fonts/SourceHanSerifCN-Bold.otf", 20, NULL, io.Fonts->GetGlyphRangesChineseFull());

    ImGui_ImplVulkan_CreateFontsTexture(g_VulkanPool.command, g_VulkanQueue.graphics);
#ifdef WIN32
    WSADATA wsaData;
    int wsaResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if(wsaResult != 0){
        printf("in function:%s:WSAStartup error, result = %d", __FUNCTION__, wsaResult);
    }
#endif
    const char *country[] = { "?", "魏", "蜀", "吴", "汉" };
    for (size_t i = 0; i < 2; i++){
        for (size_t j = 0; j < IM_ARRAYSIZE(country); ++j) {
            strcpy(g_CountryItems[i][j], country[j]);
        }
    }
    GetLocalIp(g_ServerIp);
#else
    g_CommandBuffers.resize(g_VulkanWindows.framebuffers.size());
    vkf::tool::AllocateCommandBuffers(g_VulkanDevice.device, g_VulkanPool.command, g_CommandBuffers.size(), g_CommandBuffers.data());
    for (size_t i = 0; i < g_VulkanWindows.framebuffers.size(); ++i){
        RecordCommand(g_CommandBuffers[i], i);
    }
#endif
} 
void cleanup(){
    vkDeviceWaitIdle(g_VulkanDevice.device);
#ifdef INTERNET_MODE
    ImGui_ImplGlfw_Shutdown();
    ImGui_ImplVulkan_Shutdown();

    GameMessage message;
    message.event = GAME_OVER_GAME_EVENT;
    g_Server.SendToAllClient(&message, sizeof(message));
    g_Server.ShutdownClient();
    g_Server.ShutdownServer();
#ifdef WIN32
    WSACleanup();
#endif
#endif
    vkDestroyDescriptorSetLayout(g_VulkanDevice.device, g_SetLayout, nullptr);
    vkf::DestroyPipelineCache(g_VulkanDevice.device, "GraphicsPipelineCache", g_PipelineCache);
    
    g_Chessboard.DestroyGraphicsPipeline(g_VulkanDevice.device);
    g_Chessboard.Cleanup(g_VulkanDevice.device);

    vkDestroySampler(g_VulkanDevice.device, g_TextureSampler, nullptr);
}
void RecreateSwapchain(void *userData){
    GLFWwindow* window = (GLFWwindow*)userData;
    int32_t width, height;
    glfwGetWindowSize(window, &width, &height);
    //windows下最小化后无法复原//调试显示, 在等待currentFrame为2的栏杆对象后得不到回复
    if (width < 1 || height < 1)return;
    //想不调用vkDeviceWaitIdle重建交换链也行, 在重新创建交换链时, VkSwapchainCreateInfoKHR的结构体中oldSwapChain设置为原来的交换链
    CleanupSwapchain(g_VulkanDevice.device, g_VulkanWindows);
   const VkExtent2D swapchainExtent = g_VulkanWindows.swapchainInfo.extent;
   vkf::CreateSwapchain(g_VulkanDevice.physicalDevice, g_VulkanDevice.device, g_VulkanWindows);
    g_WindowWidth = g_VulkanWindows.swapchainInfo.extent.width;
    g_WindowHeight = g_VulkanWindows.swapchainInfo.extent.height;
    vkf::CreateRenderPassForSwapchain(g_VulkanDevice.device, g_VulkanWindows.swapchainInfo.format, g_VulkanWindows.renderpass);
    vkf::CreateFrameBufferForSwapchain(g_VulkanDevice.device, g_VulkanWindows);
    if(swapchainExtent.width != g_WindowWidth || swapchainExtent.height != g_WindowHeight){
        g_Chessboard.DestroyGraphicsPipeline(g_VulkanDevice.device);
        g_Chessboard.CreatePipeline(g_VulkanDevice.device, g_VulkanWindows.renderpass, g_SetLayout, g_PipelineCache, g_WindowWidth);
    }
#ifndef INTERNET_MODE
    vkFreeCommandBuffers(g_VulkanDevice.device, g_VulkanPool.command, g_CommandBuffers.size(), g_CommandBuffers.data());
    vkf::tool::AllocateCommandBuffers(g_VulkanDevice.device, g_VulkanPool.command, g_CommandBuffers.size(), g_CommandBuffers.data());
    for (size_t i = 0; i < g_VulkanWindows.framebuffers.size(); ++i){
        RecordCommand(g_CommandBuffers[i], i);
    }
#endif
}
void display(GLFWwindow* window){
    static size_t currentFrame;
#ifdef INTERNET_MODE
    vkDeviceWaitIdle(g_VulkanDevice.device);
    RecordCommand(g_CommandBuffers, currentFrame);
    if(VK_ERROR_OUT_OF_DATE_KHR == vkf::DrawFrame(g_VulkanDevice.device, currentFrame, g_CommandBuffers, g_VulkanWindows.swapchain, g_VulkanQueue, g_VulkanSynchronize, RecreateSwapchain, window)){
        //重建交换链后, 即使currentFrame不为0, 获取的交换链图片索引不一定和currentFrame相同
        currentFrame = 0;
        //那重建交换链后, 那重建后有没有可能得到的索引不是0?
    }
    else{
        currentFrame = (currentFrame + 1) % g_VulkanWindows.framebuffers.size();
    }
#else
    if(g_UpdateScreen){
        g_UpdateScreen = false;
        vkDeviceWaitIdle(g_VulkanDevice.device);
        for (size_t i = 0; i < g_VulkanWindows.framebuffers.size(); ++i){
            RecordCommand(g_CommandBuffers[i], i);
        }
    }
    if(VK_ERROR_OUT_OF_DATE_KHR == vkf::DrawFrame(g_VulkanDevice.device, currentFrame, g_CommandBuffers[currentFrame], g_VulkanWindows.swapchain, g_VulkanQueue, g_VulkanSynchronize, RecreateSwapchain, window)){
        //重建交换链后, 即使currentFrame不为0, 获取的交换链图片索引不一定和currentFrame相同
        currentFrame = 0;
        //那重建交换链后, 那重建后有没有可能得到的索引不是0?
    }
    else{
        currentFrame = (currentFrame + 1) % g_VulkanWindows.framebuffers.size();
    }
#endif
}
// void createSurface(VkInstance instance, VkSurfaceKHR&surface, void* userData){
//     glfwCreateWindowSurface(instance, (GLFWwindow *)userData, nullptr, &surface);
// }
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
    setup(window);
    while (!glfwWindowShouldClose(window)) {
        display(window);

        glfwPollEvents();
    }
	glfwTerminate();

    vkDeviceWaitIdle(g_VulkanDevice.device);
    cleanup();
    CleanupVulkan();
    return 0;
}
