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
//目前的问题是:获取棋子的时候, 汉和其他势力棋子偏移出现冲突问题
#define JOIN_AI
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
const char* g_CountryItems[2][5];
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
uint32_t g_PlayerCountry = -1;
uint32_t g_CurrentCountry = -1;
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
void PlayChess(uint32_t country, uint32_t chess, const glm::vec2&mousePos);
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
    currentCountry = g_PlayerCountry;
    // do{
    //     currentCountry = (currentCountry + 1) % g_DefaultCountryCount;
    // } while (g_Chessboard.IsDeath(currentCountry));
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
    g_VulkanDevice.physicalDevice = vkf::GetPhysicalDevices(g_VulkanDevice.instance, VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU);
    vkf::CreateDebugUtilsMessenger(g_VulkanDevice.instance, g_VulkanMessenger, debugUtilsMessenger);
    glfwCreateWindowSurface(g_VulkanDevice.instance, window, nullptr, &g_VulkanWindows.surface);
    vkf::CreateDevice(g_VulkanDevice.physicalDevice, {}, g_VulkanWindows.surface, g_VulkanDevice.device);
    uint32_t queueFamilies[2];
    vkf::tool::GetGraphicAndPresentQueueFamiliesIndex(g_VulkanDevice.physicalDevice, g_VulkanWindows.surface, queueFamilies);
    vkGetDeviceQueue(g_VulkanDevice.device, queueFamilies[0], 0, &g_VulkanQueue.graphics);
    vkGetDeviceQueue(g_VulkanDevice.device, queueFamilies[1], 0, &g_VulkanQueue.present);
    vkf::CreateSwapchain(g_VulkanDevice.physicalDevice, g_VulkanDevice.device, g_VulkanWindows);
    vkf::CreateRenderPassForSwapchain(g_VulkanDevice.device, g_VulkanWindows.swapchainInfo.format, g_VulkanWindows.renderpass);
    vkf::CreateCommandPool(g_VulkanDevice.physicalDevice, g_VulkanDevice.device, g_VulkanPool.commandPool, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
    vkf::CreateFrameBufferForSwapchain(g_VulkanDevice.device, g_VulkanWindows);
    vkf::CreateSemaphoreAndFenceForSwapchain(g_VulkanDevice.device, 3, g_VulkanSynchronize);
    vkf::CreateDescriptorPool(g_VulkanDevice.device, 8, g_VulkanPool.descriptorPool);
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
    
    vkDestroyCommandPool(g_VulkanDevice.device, g_VulkanPool.commandPool, VK_NULL_HANDLE);
    vkDestroyDescriptorPool(g_VulkanDevice.device, g_VulkanPool.descriptorPool, VK_NULL_HANDLE);

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
void RandomCountry(){
    uint32_t countryIndex[3];
    uint32_t canUseIndex = 0, playerIndex = 0;
    const char *countryName[] = { "魏", "蜀", "吴", "汉" };
    RandomNumber(g_DefaultCountryCount, g_DefaultCountryCount, countryIndex);
    for (auto it = g_Players.begin(); it != g_Players.end(); ++it, ++playerIndex){
        if(it->random){
            do{
                if(canUseIndex >= g_DefaultCountryCount)break;
                strcpy(it->country, countryName[countryIndex[canUseIndex++]]);
                printf("尝试给%d个玩家赋值%s国\n", playerIndex, it->country);
            } while (!CanUseCountry(playerIndex, it->country));
            if(canUseIndex >= g_DefaultCountryCount){
                printf("error, no can use country, canUseIndex is %d\n", canUseIndex);
            }
        }
        canUseIndex = 0;
        it->index = countryToindex(it->country);
        printf("in function %s:country index:%d, country:%s\n", __FUNCTION__, it->index, it->country);
    }
}
void SendChangeCountryInfomation(uint32_t client, const char *country){
    GameMessage message;
    message.clientIndex = client;
    strcpy(message.player.country, country);
    message.event = CHANGE_PLAYER_COUNTRY_GAME_EVENT;
    g_Client.SendTo(&message, sizeof(message));
}
void SendPlayersInfoation(){
    GameMessage message;
    message.event = JOINS_GAME_GAME_EVENT;
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
    GameMessage message;
    message.event = CURRENT_COUNTRY_GAME_EVENT;
    // message.index = WEI_COUNTRY_INDEX;
    message.clientIndex = rand() % g_DefaultCountryCount;
    g_Server.SendToAllClient(&message, sizeof(message));
}
void SendPlayChessMessage(const PlayerInfo&game, const ChessInfo *pSelect, const ChessInfo *pTarget, const glm::vec2&mousePos){
    GameMessage message;
    message.event = PLAY_CHESS_GAME_EVENT;
    message.player = game;
    message.mousePos = mousePos;
    if(pSelect)message.select = *pSelect;
    if(pTarget)message.target = *pTarget;
    g_Client.SendTo(&message, sizeof(message));
}
//第一个参数是被修改的玩家
void InitCountryItem(uint32_t countryIndex, const char* countryItems[2][5]) {
    uint32_t index = 0;
    const char *country[] = { "?", "魏", "蜀", "吴", "汉" };
    for (auto it = g_Players.begin(); it != g_Players.end(); ++it, ++index) {
        if (it->index != INVALID_PLAYER_INDEX) {
            for (size_t i = 1; i < g_DefaultCountryCount + 1; ++i) {
                if(!strcmp(country[i], it->country) && it->index != countryIndex){
                    countryItems[countryIndex][i] = countryItems[countryIndex][0];
                    return;
                }
                else {
                    countryItems[countryIndex][i] = country[i];
                }
            }
        }
    }
}
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
std::mutex g_Mutex;
//HANDLE g_Mutex = CreateMutex(NULL, FALSE, NULL);
void *process_server(void *userData){
    GameMessage message;
    static uint32_t acceptCount;
    uint32_t socketindex = INVALID_SOCKET;
    //WaitForSingleObject(g_Mutex, INFINITE);
    g_Mutex.lock();
    if(INVALID_SOCKET != g_Server.AcceptClient(acceptCount, &message, sizeof(message))){
        socketindex = acceptCount;
        printf("in function %s:socket index %d, accept count:%d\n", __FUNCTION__, socketindex, acceptCount);
        ++acceptCount;
        if(message.event == AI_JOIN_GAME_GAME_EVENT){
            g_Players[socketindex].ai = true;
            strcpy(message.player.name, "ai");
            g_Server.ShutdownClient(socketindex);
            //因为只有服务端能加ai, 所以可以直接在这里初始化
            //而玩家不行
            InitCountryItem(socketindex, g_CountryItems);
        }
        else{
            sprintf(message.player.name, "player%d", socketindex + 1);
        }
        g_Players[socketindex].index = socketindex;
        strcpy(g_Players[socketindex].name, message.player.name);

        SendPlayersInfoation();
    }
    g_Mutex.unlock();
    //ReleaseMutex(g_Mutex);
    //虽然ai不需要收发消息, 但为了反正最小加ai, 所以即使该线程是ai也需要做下面的事
    if(acceptCount >= g_DefaultCountryCount){
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
                //证明有ai需要启动ai方面的线程
                g_AI.CreatePthread(AiPlayChess, NULL);
                break;
            }
        }
#endif  
    }
    if(message.event == AI_JOIN_GAME_GAME_EVENT){
        return nullptr;//ai不需要收发消息
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
// void *server_start(void *userData){
//     GameMessage message;
//     uint32_t aiIndex = -1;
//     g_Server.AcceptClient(g_DefaultCountryCount, g_Players);
//     RandomCountry();
//     SendCurrentCountry();
//     SendPlayersInfoation();
//     //必须用静态变量, 局部变量在函数结束后就销毁, 会导致传餐错误
//     static uint32_t socksetIndex[] = { 0, 1, 2 };
//     for (size_t i = 0; i < g_DefaultCountryCount; ++i){
//         CreateThread(&process_server, &socksetIndex[i]);
//     }
//     //游戏开始, 发消息告诉所有客户端游戏开始
//     message.event = GAME_START_GAME_EVENT;
//     g_Server.SendToAllClient(&message, sizeof(message));
// #ifdef JOIN_AI
//     //因为最多只有1个ai, 所以可以直接循环里面判断和创建线程
//     for (auto it = g_Players.begin(); it != g_Players.end(); ++it){
//         if(it->ai){
//             g_AI.CreatePthread(AiPlayChess, NULL);
//             break;
//         }
//     }
// #endif            
//     return nullptr;
// }
//但该程序为客户端时,一般不会用上面的函数而是用下面的函数创建创建线程
void *process_client(void *userData){
    //接收消息后做出动作即可
    GameMessage message;
    do{
        g_Client.RecvFrom(&message, sizeof(GameMessage));
        if(message.event == GAME_START_GAME_EVENT){
            g_GameStart = true;
            printf("game start\n");
            g_Chessboard.InitChess(g_VulkanDevice.device);
        }
        else if(message.event == JOINS_GAME_GAME_EVENT){
            printf("joins game, index:%d, name:%s\n", message.clientIndex, message.player.name);
            g_Players[message.clientIndex].ai = message.player.ai;
            g_Players[message.clientIndex].index = message.clientIndex;
            strcpy(g_Players[message.clientIndex].name, message.player.name);
            strcpy(g_Players[message.clientIndex].country, message.player.country);
            InitCountryItem(g_PlayerCountry, g_CountryItems);
        }
        else if(message.event == CURRENT_COUNTRY_GAME_EVENT){
            g_CurrentCountry = message.clientIndex;
        }
        else if(message.event == CHANGE_PLAYER_COUNTRY_GAME_EVENT){
            int32_t aiIndex = GetAiPlayerIndex();
            if (g_ServerAppaction) {
                if (message.clientIndex != aiIndex) {
                    if(aiIndex != INVALID_PLAYER_INDEX)
                        InitCountryItem(aiIndex, g_CountryItems);
                }
                else {
                    InitCountryItem(g_PlayerCountry, g_CountryItems);
                }
            }
            else if (message.clientIndex != g_PlayerCountry) {
                InitCountryItem(g_PlayerCountry, g_CountryItems);
            }
            strcpy(g_Players[message.clientIndex].country, message.player.country);
            g_Players[message.clientIndex].random = !strcmp(message.player.country, "?");
        }
        else if(message.event == SELF_PLAYER_INFORMATION_GAME_EVENT){
            g_PlayerCountry = message.clientIndex;
            g_Players[message.clientIndex].index = g_PlayerCountry;
        }
        // else if(message.event == CHANGE_PLAYER_INFORMATION_GAME_EVENT){
        //     const uint32_t index = message.clientIndex, playerIndex = message.player.index;
        //     // glm::vec3 countryColor[] = { WEI_CHESS_COUNTRY_COLOR, SHU_CHESS_COUNTRY_COLOR, WU_CHESS_COUNTRY_COLOR };
        //     // g_Players[index].color = countryColor[playerIndex];
        //     g_Players[index].index = playerIndex;
        //     strcpy(g_Players[index].country, message.player.country);
        // }
        else if(message.event == PLAY_CHESS_GAME_EVENT){
            //这里应该真正下棋而不是发消息
            const ChessInfo *pTarget  = g_Chessboard.GetChessInfos(message.target.country, message.target.row, message.target.column), *pSelect = g_Chessboard.GetChessInfos(g_CurrentCountry, message.select.row, message.select.column);
            if(pSelect){
                if(pTarget){
                    // printf("catpure chess:country:%d, chess:%d;target country:%d, chess:%d\n", pSelect->country, pSelect->chess, pTarget->country, pTarget->chess);
                    PlayChess(pSelect->country, pSelect->chess, pTarget->country, pTarget->chess);
                }
                else{
                    // printf("move chess:country:%d, chess:%d;target row:%.0f, column:%.0f\n", pSelect->country, pSelect->chess, message.player.row, message.player.column);
                    PlayChess(pSelect->country, pSelect->chess, message.mousePos);
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
    // memcpy(g_Server.name, name, MAX_BYTE);
    // g_Player.color = WEI_CHESS_COUNTRY_COLOR;
    // strcpy(g_Player.country, "魏");
    // g_Player.index = 0;
    // g_Player.name = ;
    g_Server.CreateServer(g_DefaultCountryCount);
    //必须用静态变量, 局部变量在函数结束后就销毁, 会导致传餐错误
    //static uint32_t socksetIndex[] = { 0, 1, 2 };
    for (size_t i = 0; i < g_DefaultCountryCount; ++i){
        CreateThread(process_server, nullptr);
    }
    // CreateThread(server_start, nullptr);
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
void ShowClientWidget(){
    /*
        需要选择国家, 就需要知道其他玩家选择的国家
        如果都没选择(即随机)，该玩家能选择所有国家
        否则只能选择其他国家
        //----
        所以, 需要在玩家加入的时候立即接受所有玩家信息
        接收后，还要注意。
        如果玩家在接收所有玩家信息前点开了选择国家消息。
        怎么做到在接收完所有玩家信息时，让该玩家只能选择某些国家
    */
    static std::array<int32_t, 2>countryItemIndex;
    //汉是为了在HAN_CAN_PLAY宏定义了的情况下设置的
    // static const char *currentCountry = countryItems[0];
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
        for (auto it = g_Players.begin(); it != g_Players.end(); ++it, ++comboIndex){
            if(it->index != INVALID_PLAYER_INDEX){
                //combo(或许其他也是)需要靠label查找。所以label不能重复
                char comboName[MAX_BYTE] = {};
                for (size_t i = 0; i < comboIndex + 1; ++i){
                    strcat(comboName, " ");
                }
                if(!g_GameStart && (it->index == g_PlayerCountry || (g_ServerAppaction && it->ai))){
                    if(ImGui::Combo(comboName, &countryItemIndex[it->ai], g_CountryItems[it->ai], g_DefaultCountryCount + 1)) {
                        it->random = !strcmp(it->country, "?");
                        strcpy(it->country, g_CountryItems[it->ai][countryItemIndex[it->ai]]);
                        printf("发送消息前:玩家更改势力, 新势力为%s\n", it->country);
                        SendChangeCountryInfomation(it->index, it->country);
                    }
                }
                else{
                    //这里只给看不给改
                    if(ImGui::BeginCombo(comboName, it->country)){
                        ImGui::EndCombo();
                    }
                }
            }
        }
        ImGui::EndTable();
    }
    // if(ImGui::BeginCombo(" ", it->country)){//加ai后，就会出现多余的项目
    //     for (size_t i = 0; i < g_DefaultCountryCount + 1; ++i){
    //         bool is_selected = (!strcmp(it->country, countryItems[i]));
    //         if (ImGui::Selectable(countryItems[i], is_selected)){
    //             strcpy(it->country, countryItems[i]);
    //             it->random = false;
    //             strcpy(it->country, it->country);
    //             SendChangeCountryInfomation(it->index, it->country);
    //         }
    //         if (is_selected)ImGui::SetItemDefaultFocus();   // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)
    //     }
    //     ImGui::EndCombo();
    // }
}
void ShowGameWidget(){
    const char *countryName[] = { "魏国", "蜀国", "吴国" };
    if(g_CurrentCountry != -1){
        ImGui::Text("你是%s, 该%s下棋\n", countryName[g_PlayerCountry], g_PlayerCountry == g_CurrentCountry?"你":countryName[g_CurrentCountry]);
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

    static bool bShowCreateServer = false, bShowConnectServer = false;
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
            //如果创建好房间或连接了客户端后, 不应该继续显示下面的按钮
            if(ImGui::BeginTable("客户端信息", 2)){
                ImGui::TableNextColumn();
                if(ImGui::Button("创建房间")){
                    bShowCreateServer = true;
                }
                ImGui::TableNextColumn();
                if(ImGui::Button("连接服务端")){
                    bShowConnectServer = true;
                }
                ImGui::EndTable();
            }
        }
        ImGui::End();
    }
    if(bShowCreateServer){
        if(ImGui::Begin("创建房间")){
            static char name[MAX_BYTE] = {0};
            ImGui::InputText("输入房间名", name, sizeof(name));
            //目前用不到房间名, 所以不需要判断
            if(ImGui::BeginTable("确定取消按钮", 2)){
                ImGui::TableNextColumn();
                if(ImGui::Button("确定")){
                    bShowCreateServer = false;
                    CreateServer(name);
                }
                ImGui::TableNextColumn();
                if(ImGui::Button("取消")){
                    bShowCreateServer = false;
                }
                ImGui::EndTable();
            }
            ImGui::End();
        }
    }
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
                    bShowConnectServer = false;
                    CreateClient(g_ServerIp);
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
int32_t GetCanPlay(uint32_t row, uint32_t column, const std::vector<ChessInfo>&canplays){
    int32_t index = -1;
    for (size_t i = 0; i < canplays.size(); ++i){
        if(row == canplays[i].row && column == canplays[i].column){
            index = i;
            break;
        }
    }
    return index;
}
//走子
void PlayChess(uint32_t country, uint32_t chess, const glm::vec2&mousePos){
    std::vector<ChessInfo>canplays;
    const Chess *pChess = g_Chessboard.GetChess(country, chess);
    pChess->Selected((const Chess* (*)[COUNTRY_CHESS_COUNT])g_Chessboard.GetChess(), canplays);
    const int32_t index = GetCanPlay(mousePos, canplays);
    if(index != -1){
        const ChessInfo *pChessInfo = pChess->GetInfo();
        MoveChess(pChessInfo->country, pChessInfo->chess, glm::vec2(COLUMN_TO_X(pChessInfo->column), ROW_TO_Y(pChessInfo->row)), glm::vec2(COLUMN_TO_X(canplays[index].column), ROW_TO_Y(canplays[index].row)));
        g_Chessboard.Move(g_VulkanDevice.device, country, chess, canplays[index].row, canplays[index].column);
        g_CurrentCountry = GetNextCountry(g_CurrentCountry);
#ifdef JOIN_AI
#ifdef INTERNET_MODE
        if(g_Players[g_CurrentCountry].ai){
            EnableAi();
        }
#else
        if(g_CurrentCountry != g_PlayerCountry){
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
    if(GetCanPlay(pDstChessInfo->row, pDstChessInfo->column, canplays) != -1){
        //不知道为什么，手动下棋看不到棋移动的效果//即使是重开一个线程也一样
        MoveChess(pSrcChessInfo->country, pSrcChessInfo->chess, glm::vec2(COLUMN_TO_X(pSrcChessInfo->column), ROW_TO_Y(pSrcChessInfo->row)), glm::vec2(COLUMN_TO_X(pDstChessInfo->column), ROW_TO_Y(pDstChessInfo->row)));
        g_Chessboard.Play(g_VulkanDevice.device, srcCountry, srcChess, dstCountry, dstChess);
        if(g_Chessboard.IsDeath(dstCountry)){
            printf("%s国被%s国消灭\n", county[dstCountry], county[g_CurrentCountry]);
            g_Chessboard.DestroyCountry(g_VulkanDevice.device, dstCountry);
        }
        g_CurrentCountry = GetNextCountry(g_CurrentCountry);
#ifdef JOIN_AI
#ifdef INTERNET_MODE
        if(g_Players[g_CurrentCountry].ai){
            EnableAi();
        }
#else
        if(g_CurrentCountry != g_PlayerCountry){
            EnableAi();
        }
#endif
#endif
    }
}
#ifdef JOIN_AI
#ifdef __linux
pthread_t g_AiPlayChess;
#endif
#ifdef WIN32
HANDLE g_AiPlayChess;
#endif
void aiPlay(){
    glm::vec2 mousePos;
    const ChessInfo *pSelect = nullptr, *pTarget = nullptr;
    g_AI.GetPlayChessInfo(g_CurrentCountry, &pSelect, &pTarget, &mousePos, &g_Chessboard);
    //想下的棋子、敌人上面设置
    //局域网模式下应该发送消息
#ifdef INTERNET_MODE
    if(pTarget){
        SendPlayChessMessage(g_Players[g_PlayerCountry], pSelect, pTarget, mousePos);
    }
    else{
        SendPlayChessMessage(g_Players[g_PlayerCountry], pSelect, nullptr, mousePos);
    }
#else
    g_Chessboard.Select(g_VulkanDevice.device, g_CurrentCountry, pSelect->chess);
    if(pTarget){
        PlayChess(g_CurrentCountry, pSelect->chess, pTarget->country, pTarget->chess);
    }
    else{
        PlayChess(g_CurrentCountry, pSelect->chess, mousePos);
    }
    g_UpdateScreen = true;
    g_Chessboard.UnSelect(g_VulkanDevice.device, g_CurrentCountry, pSelect->chess);
#endif
}
void *AiPlayChess(void *userData){
#ifdef __linux__
#ifdef INTERNET_MODE
    sem_init(&g_AiSemaphore, 0, g_Players[g_CurrentCountry].ai);
#else
    sem_init(&g_AiSemaphore, 0, g_PlayerCountry != g_CurrentCountry);
#endif
#endif
#ifdef WIN32
    g_AiSemaphore = CreateEvent(NULL, FALSE, FALSE, "AiSemaphore");
#endif
    while(!GameOver(g_DefaultCountryCount)){
        AiWait();
        // printf("ai下棋了\n");
        aiPlay();
        // printf("ai下完棋了\n");
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
    if(country == g_PlayerCountry)pSelected = g_Chessboard.GetChessInfos(country, mousePos);
    if(pSelected && pSelected->country == country){
        g_Chessboard.Select(g_VulkanDevice.device, country, pSelected->chess);
    }
    return pSelected;
}
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
        PlayChess(ppc.srcCountry, ppc.srcChess, ppc.mousePos);
    }
    return nullptr;
}
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
                    SendPlayChessMessage(g_Players[g_PlayerCountry], g_Selected, pTarget, mousePos);
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
                SendPlayChessMessage(g_Players[g_PlayerCountry], g_Selected, nullptr, mousePos);
                g_Selected = nullptr;
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
            g_Selected = SelectChess(g_CurrentCountry, mousePos);
        }
#ifndef INTERNET_MODE
        g_UpdateScreen = true;
#endif
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

void setup(GLFWwindow *windows){
#ifdef __linux__
    srandom(time(nullptr));
#endif
#ifdef WIN32
    srand(time(nullptr));
#endif // WIN32
    glfwSetMouseButtonCallback(windows, mousebutton);

    SetupDescriptorSetLayout(g_VulkanDevice.device);
    vkf::CreateTextureSampler(g_VulkanDevice.device, g_TextureSampler);
    vkf::CreatePipelineCache(g_VulkanDevice.device, "GraphicsPipelineCache", g_PipelineCache);
#ifndef INTERNET_MODE
    // g_CurrentCountry = WEI_COUNTRY_INDEX;
    // g_PlayerCountry = WEI_COUNTRY_INDEX;
    g_CurrentCountry = rand() % g_DefaultCountryCount;
    g_PlayerCountry = rand() % g_DefaultCountryCount;
#endif
    g_Chessboard.Setup(g_VulkanDevice.physicalDevice, g_VulkanDevice.device, g_SetLayout, g_WindowWidth, g_VulkanQueue.graphics, g_VulkanPool);
    g_Chessboard.CreatePipeline(g_VulkanDevice.device, g_VulkanWindows.renderpass, g_SetLayout, g_PipelineCache, g_WindowWidth);

    g_Chessboard.UpdateBackgroundUniform(g_VulkanDevice.device, g_WindowWidth);
    g_Chessboard.UpdateUniform(g_VulkanDevice.device, g_PlayerCountry);
#ifndef INTERNET_MODE
    g_Chessboard.InitChess(g_VulkanDevice.device, g_PlayerCountry);
#endif
    //局域网联机需要imgui
    //先通过ip连接, 后面知道怎么广播再说
    //或者说，先实现简单的网络联机，在解决广播问题
#ifdef INTERNET_MODE
    vkf::tool::AllocateCommandBuffers(g_VulkanDevice.device, g_VulkanPool.commandPool, 1, &g_CommandBuffers);
    //imgui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO&io = ImGui::GetIO();
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;//启用手柄
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForVulkan(windows, true);
    uint32_t queueFamily;
    vkf::tool::GetGraphicAndPresentQueueFamiliesIndex(g_VulkanDevice.physicalDevice, VK_NULL_HANDLE, &queueFamily);
    ImGui_ImplVulkan_InitInfo initInfo = {};
    initInfo.Instance = g_VulkanDevice.instance;
    initInfo.PhysicalDevice = g_VulkanDevice.physicalDevice;
    initInfo.Device = g_VulkanDevice.device;
    initInfo.QueueFamily = queueFamily;
    initInfo.Queue = g_VulkanQueue.graphics;
    initInfo.PipelineCache = VK_NULL_HANDLE;//g_GraphicsPipline.getCache();
    initInfo.DescriptorPool = g_VulkanPool.descriptorPool;
    initInfo.MinImageCount = 2;
    initInfo.ImageCount = g_VulkanWindows.framebuffers.size();
    initInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
    initInfo.Allocator = nullptr;
    initInfo.CheckVkResultFn = nullptr;
    ImGui_ImplVulkan_Init(&initInfo, g_VulkanWindows.renderpass);

    io.Fonts->AddFontFromFileTTF("fonts/SourceHanSerifCN-Bold.otf", 20, NULL, io.Fonts->GetGlyphRangesChineseFull());

    ImGui_ImplVulkan_CreateFontsTexture(g_VulkanPool.commandPool, g_VulkanQueue.graphics);
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
            g_CountryItems[i][j] = country[j];
        }
    }
    GetLocalIp(g_ServerIp);
#else
    g_CommandBuffers.resize(g_VulkanWindows.framebuffers.size());
    vkf::tool::AllocateCommandBuffers(g_VulkanDevice.device, g_VulkanPool.commandPool, g_CommandBuffers.size(), g_CommandBuffers.data());
    for (size_t i = 0; i < g_VulkanWindows.framebuffers.size(); ++i){
        RecordCommand(g_CommandBuffers[i], i);
    }
    g_AI.CreatePthread(AiPlayChess, nullptr);
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
    vkFreeCommandBuffers(g_VulkanDevice.device, g_VulkanPool.commandPool, g_CommandBuffers.size(), g_CommandBuffers.data());
    vkf::tool::AllocateCommandBuffers(g_VulkanDevice.device, g_VulkanPool.commandPool, g_CommandBuffers.size(), g_CommandBuffers.data());
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
