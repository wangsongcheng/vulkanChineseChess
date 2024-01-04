#include <time.h>
#include <stdio.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "Chessboard.h"
#include "VulkanChessboard.h"
#ifdef __linux__
#include <unistd.h>
#define MAX_BYTE 0xff
#endif
//目前的问题是:获取棋子的时候, 汉和其他势力棋子偏移出现冲突问题
// #define HAN_CAN_PLAY
#define INTERNET_MODE
// #define AI_RANDOMLY_PLAY_CHESS

#ifdef AI_RANDOMLY_PLAY_CHESS
#ifdef WIN32
#include <Windows.h>
#else
#include <pthread.h>
#endif
#endif
#ifdef INTERNET_MODE
#include <array>
#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <WS2tcpip.h>
#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")
#define SHUT_RDWR 2
#else
#include <netdb.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/socket.h>
typedef unsigned int SOCKET;
#endif
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"
#define INTERNET_PORT 10086
#define MAX_NAME 0x255
#define INVALID_SOCKET 1000
enum GameEvent{
    GAME_OVER_GAMEEVENT = 0,
    GAME_START_GAMEEVENT,
    JOINS_GAME_GAMEEVENT,
    PLAY_CHESS_GAMEEVENT,
    CURRENT_COUNTRY_GAMEEVENT,
    SELF_PLAYER_INFORMATION_GAMEEVENT,
    CHANGE_PLAYER_INFORMATION_GAMEEVENT
};
struct GameInfo{
    uint32_t index;
    glm::vec3 color;
    char name[MAX_NAME];//角色名//如果有
    char country[MAX_NAME];
};
//记录服务端的房间名称密码(如果有)等。
struct ServerInfo{
    char name[MAX_NAME];//房间名
};
struct ClientInfo{
    char ip[MAX_NAME];
    char name[MAX_NAME];//用户名
    char serverIp[MAX_NAME];
};
struct GameMessage{
    // Ranks ranks;
    GameInfo game;
    uint32_t index;
    GameEvent event;
    ClientInfo client;
    glm::vec2 playChessPos, selectChessPos;
};
VkCommandBuffer g_CommandBuffers;

uint32_t g_PlayerIndex = -1;
std::array<GameInfo, 3>g_Players;//用来保存其他玩家的信息//尽管也包含自身信息
bool g_ServerAppaction;
char g_ServerIp[MAX_NAME];
ServerInfo g_Server;
//当该程序作为客户端时,应该使用该变量而不是std::vector<ClientInfo>g_Clients
ClientInfo g_Client;
//当该程序作为服务端和客户端时, 应该使用该变量而不是ClientInfo g_Client
std::array<ClientInfo, 3>g_Clients;
//如果该程序不包含服务端，忽略下面的值
SOCKET g_ServerSocket = -1;

SOCKET g_ClientSocket = -1;
std::array<SOCKET, 3>g_ClientSockets = { INVALID_SOCKET, INVALID_SOCKET, INVALID_SOCKET};
#ifdef __linux
pthread_t g_ServerPthreadId;
pthread_t g_ClientPthreadId;
#endif
#ifdef WIN32
HANDLE g_ServerPthreadId;
HANDLE g_ClientPthreadId;
#endif
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
uint32_t g_CurrentCountry = -1;
#ifdef HAN_CAN_PLAY
const uint32_t g_DefaultCountryCount = 4;
#else
const uint32_t g_DefaultCountryCount = 3;
#endif

VkSampler g_TextureSampler;
VkPipelineCache g_PipelineCache;
VkDescriptorSetLayout g_SetLayout;
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
uint32_t GetNextCountry(uint32_t currentCountry){
    do{
        currentCountry = (currentCountry + 1) % g_DefaultCountryCount;
    } while (g_Chessboard.IsDeath(currentCountry));
    // if(pChess){
    //     currentCountry = jiang(currentCountry, pChessInfo);
    // }
    // //被将的国家不论顺序, 下回合都轮到他
    // //应该检查除下棋的国家外所有国家是否有被将的
    // //另外, 如果同时将了2个国家, 那应该轮到那个国家?例如开局吴国, 出炮到列8的位置。即使禁了吴国开局，也有可能出现类似现象
    // bool bNext = true;
    // //如果两国约好, 其中一国将住另一国，而另一个国不解将则该国可以一直下
    // //能否只检查下棋的人对其他人有没有将。主要是这样检查只会提示一个国家被将，难保第二个国家不解。
    // // for (size_t i = 0; i < 3; ++i){
    // //     for (size_t j = 0; j < 3; ++j){
    // //         if(i != j){
    // //             if(IsDangerous(i, j, JIANG_CHESS_INDEX)){
    // //                 //吴开局, 这样做会直接轮到蜀。而实际上应该轮到魏
    // //                 bNext = false;
    // //                 printf("注意%s国的\"将\"\n", county[i]);
    // //                 g_CurrentCountry = i;
    // //                 break;
    // //             }
    // //         }
    // //     }                
    // // }
    // if(bNext){
    //     g_CurrentCountry = NextCountry(g_CurrentCountry);
    // }
    return currentCountry;
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
#ifdef INTERNET_MODE
void Send(int __fd, const GameMessage *__buf, size_t __n, int __flags){
    int sendSize, offset = 0;
    if(__fd != INVALID_SOCKET){
        do{
#ifdef WIN32
            sendSize = send(__fd, (const char *)__buf + offset, __n - offset, __flags);
#else
            sendSize = send(__fd, (const char *)__buf + offset, __n - offset, __flags);
#endif
            if(sendSize == -1){
                perror("send error");
                break;
            }
            else {
                printf("success send message %d byte, residue:%d, offset = %d, __n = %d\n", sendSize, __n - offset - sendSize, offset + sendSize, __n);
            }
            offset += sendSize;
        } while (offset < __n);
        printf("send end\n");
    }
}
void Recv(int __fd, GameMessage *__buf, size_t __n, int __flags){
    int recvSize, offset = 0;
    if(__fd != INVALID_SOCKET){
        do{
#ifdef WIN32
            recvSize = recv(__fd, (char *)__buf + offset, __n - offset, __flags);
#else
            recvSize = recv(__fd, (char *)__buf + offset, __n - offset, __flags);
#endif
            if(recvSize == -1){
                perror("recv error");
                break;
            }
            else {
                printf("success recv message %d byte, residue:%d, offset = %d, __n = %d\n", recvSize, __n - offset - recvSize, offset + recvSize, __n);
            }
            offset += recvSize;
        } while (offset < __n);
        printf("recv end\n");
    }
}
void SendToAllClient(const GameMessage *__buf, size_t __n){
    for (size_t i = 0; i < g_Clients.size(); ++i){
        if(g_ClientSockets[i] != INVALID_SOCKET){
            Send(g_ClientSockets[i], __buf, __n, 0);
        }
    }
}
void SendToServer(const GameMessage *__buf, size_t __n){
    Send(g_ClientSocket, __buf, __n, 0);
}
void CreateClient(const char *serverIp){
    struct in_addr address;
    struct sockaddr_in addr;
#ifdef WIN32
    if (1 != inet_pton(AF_INET, serverIp, &address)) {
        perror("inet_aton");
        return;
    }
#else
    if(!inet_aton(serverIp, &address)){
        perror("inet_aton");
        return;
    }
#endif
    addr.sin_addr = address;
    g_ClientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if(g_ClientSocket == -1){
        perror("create client:function:socket");
        return;
    }
    addr.sin_family = AF_INET;
    addr.sin_port = htons(INTERNET_PORT);
#ifdef WIN32
    ZeroMemory(&addr.sin_zero, sizeof(addr.sin_zero)); /* zero the rest of the struct */
#else
    bzero(&addr.sin_zero, sizeof(addr.sin_zero)); /* zero the rest of the struct */
#endif // WIN32
    if(connect(g_ClientSocket,(struct sockaddr *)&addr,sizeof(struct sockaddr)) == -1){
        perror("create client:function:connect");
        shutdown(g_ClientSocket, SHUT_RDWR);
        return;
    }
}
void CreateServer(int listenCount){
    g_ServerSocket = socket(AF_INET, SOCK_STREAM, 0);
    if(g_ServerSocket == -1){
        perror("create server:function:socket");
        // herror("create server:function:socket");
        return;
    }
    struct sockaddr_in my_addr;
    my_addr.sin_family = AF_INET; /* host byte order */
    my_addr.sin_addr.s_addr = INADDR_ANY; /* auto-fill with my IP */
    my_addr.sin_port = htons(INTERNET_PORT); /* short, network byte order */
#ifdef WIN32
    ZeroMemory(&my_addr.sin_zero, sizeof(my_addr.sin_zero)); /* zero the rest of the struct */
#else
    bzero(&my_addr.sin_zero, sizeof(my_addr.sin_zero)); /* zero the rest of the struct */
#endif
    if(bind(g_ServerSocket, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) == -1){
        perror("create server:function:bind");
        // herror("create server:function:bind");
        return;
    }
    if (listen(g_ServerSocket, listenCount) == -1){
        perror("create server:function:listen");
        // herror("create server:function:listen");
        return;
    }
    g_ServerAppaction = true;
}
void JoinsGame(const char *serverIp){
    GameMessage message;
    message.client = g_Client;
    message.event = JOINS_GAME_GAMEEVENT;
    SendToServer(&message, sizeof(message));
}
void GetLocalIp(char outIp[]){
    int status = -1;
    char buffer[MAX_NAME] = {0};
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
void RandomCountry(){
    uint32_t country[3];
    const char *countryName[] = { "魏", "蜀", "吴" };
    glm::vec3 countryColor[] = { WEI_CHESS_COUNTRY_COLOR, SHU_CHESS_COUNTRY_COLOR, WU_CHESS_COUNTRY_COLOR };
    const uint32_t count = sizeof(country) / sizeof(uint32_t);
    RandomNumber(count, count, country);
    for (size_t i = 0; i < count; ++i){
        g_Players[i].index = country[i];
        g_Players[i].color = countryColor[country[i]];
        strcpy(g_Players[i].country, countryName[country[i]]);
        printf("in function %s:country index:%d, country:%s\n", __FUNCTION__, country[i], g_Players[i].country);
    }
}
void *process_server(void *userData){
    GameMessage message;
    //该函数只负责向所有客户端转发消息
    SOCKET socket = *(SOCKET *)userData;
    do{ 
        Recv(socket, &message, sizeof(message), 0);
        SendToAllClient(&message, sizeof(message));
    } while (message.event != GAME_OVER_GAMEEVENT);
    return nullptr;
}
void SendSelfInfoation(SOCKET clientSocket, const GameInfo&player){
    GameMessage message;
    message.event = SELF_PLAYER_INFORMATION_GAMEEVENT;
    message.game = player;
    message.index = player.index;
    Send(clientSocket, &message, sizeof(message), 0);
}
void SendAllInfoation(uint32_t count){
    GameMessage message;
    message.event = JOINS_GAME_GAMEEVENT;
    for (uint32_t uiClient = 0; uiClient < count; ++uiClient){
        message.index = uiClient;
        strcpy(message.client.ip, g_Clients[uiClient].ip);
        strcpy(message.client.name, g_Clients[uiClient].name);
        strcpy(message.client.serverIp, g_Clients[uiClient].serverIp);
        SendToAllClient(&message, sizeof(message));
    }
}
void SendPlayersInfoation(){
    GameMessage message;
    message.event = CHANGE_PLAYER_INFORMATION_GAMEEVENT;
    for (uint32_t uiPlayer = 0; uiPlayer < g_Players.size(); ++uiPlayer){
        message.index = uiPlayer;
        message.game.index = g_Players[uiPlayer].index;
        strcpy(message.game.name, g_Players[uiPlayer].name);
        strcpy(message.game.country, g_Players[uiPlayer].country);
        SendToAllClient(&message, sizeof(message));
    }
}
void SendCurrentCountry(){
    GameMessage message;
    message.event = CURRENT_COUNTRY_GAMEEVENT;
    // message.index = WEI_COUNTRY_INDEX;
    message.index = rand() % g_DefaultCountryCount;
    SendToAllClient(&message, sizeof(message));
}
void *server_start(void *userData){
    GameMessage message;
    struct sockaddr_in client_addr;
    socklen_t size = sizeof(client_addr);
    for (size_t i = 0; i < 3; i++){
        g_ClientSockets[i] = accept(g_ServerSocket, (struct sockaddr *)&client_addr, &size);
        if(g_ClientSockets[i] != INVALID_SOCKET){
            //客户端连接后会发送自身信息
            Recv(g_ClientSockets[i], &message, sizeof(message), 0);
            strcpy(g_Clients[i].ip, message.client.ip);
            strcpy(g_Clients[i].name, message.client.name);
            strcpy(g_Clients[i].serverIp, message.client.serverIp);
            GameInfo player;
            player.index = i;
            SendSelfInfoation(g_ClientSockets[i], player);

            SendAllInfoation(i + 1);
        }
        else{
            perror("process_server:accept");
            // shutdown(g_ClientSockets[i], SHUT_RDWR);
        }
    }
    SendCurrentCountry();
    RandomCountry();
    SendPlayersInfoation();
#ifdef WIN32
    DWORD  threadId;
    for (size_t i = 0; i < 3; i++){
        g_ServerPthreadId = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)process_server, &g_ClientSockets[i], 0, &threadId);
    }
#endif
#ifdef __linux
    pthread_t pthreadId;
    for (size_t i = 0; i < 3; i++){
        pthread_create(&g_ServerPthreadId, nullptr, process_server, &g_ClientSockets[i]);
    }
#endif
    //游戏开始, 发消息告诉所有客户端游戏开始
    message.event = GAME_START_GAMEEVENT;
    SendToAllClient(&message, sizeof(message));
    return nullptr;
}
// void Play(uint32_t row, uint32_t column){
//     Play(glm::vec2(COLUMN_TO_X(column, CHESSBOARD_GRID_SIZE, CHESSBOARD_LINE_WIDTH), ROW_TO_Y(row, CHESSBOARD_GRID_SIZE, CHESSBOARD_LINE_WIDTH)));
// }
void SendPlayChessMessage(const GameInfo&game, const glm::vec2&playPos, const glm::vec2&selectPos){
    GameMessage message;
    message.event = PLAY_CHESS_GAMEEVENT;
    message.game = game;
    message.playChessPos = playPos;
    message.selectChessPos = selectPos;
    SendToServer(&message, sizeof(message));
}
//但该程序为客户端时,一般不会用上面的函数而是用下面的函数创建创建线程
void *process_client(void *userData){
    //接收消息后做出动作即可
    GameMessage message;
    SOCKET clientSocket = *(SOCKET *)userData;
    do{
        Recv(clientSocket, &message, sizeof(GameMessage), 0);
        if(message.event == GAME_START_GAMEEVENT){
            printf("game start\n");
            g_Chessboard.InitChess(g_VulkanDevice.device);
        }
        else if(message.event == SELF_PLAYER_INFORMATION_GAMEEVENT){
            g_PlayerIndex = message.index;
        }
        else if(message.event == CURRENT_COUNTRY_GAMEEVENT){
            g_CurrentCountry = message.index;
        }
        else if(message.event == JOINS_GAME_GAMEEVENT){
            printf("joins game, index:%d, name:%s, ip:%s\n", message.index, message.client.name, message.client.ip);
            g_Clients[message.index] = message.client;
        }
        else if(message.event == CHANGE_PLAYER_INFORMATION_GAMEEVENT){
            const uint32_t index = message.index, playerIndex = message.game.index;
            glm::vec3 countryColor[] = { WEI_CHESS_COUNTRY_COLOR, SHU_CHESS_COUNTRY_COLOR, WU_CHESS_COUNTRY_COLOR };
            strcpy(g_Players[index].name, message.game.name);
            g_Players[index].color = countryColor[playerIndex];
            strcpy(g_Players[index].country, message.game.country);
        }
        else if(message.event == PLAY_CHESS_GAMEEVENT){
            //这里应该真正下棋而不是发消息
            const ChessInfo *pRival  = g_Chessboard.GetChessInfos(message.playChessPos), *pSelect = g_Chessboard.GetChessInfos(message.selectChessPos);
            if(pSelect){
                if(pRival){
                    printf("catpure chess:country:%d, chess:%d;target country:%d, chess:%d\n", pSelect->country, pSelect->chess, pRival->country, pRival->chess);
                    PlayChess(pSelect->country, pSelect->chess, pRival->country, pRival->chess);
                }
                else{
                    printf("move chess:country:%d, chess:%d;target row:%.0f, column:%.0f\n", pSelect->country, pSelect->chess, message.playChessPos.y / CHESSBOARD_GRID_SIZE, message.playChessPos.x / CHESSBOARD_GRID_SIZE);
                    PlayChess(pSelect->country, pSelect->chess, message.playChessPos);
                }
            }
            else{
                printf("pSelect is nullptr;client index is %d;play chess pos:%.0f, %.0f;select chess pos:%.0f, %.0f\n", message.index, message.playChessPos.x, message.playChessPos.y, message.selectChessPos.x, message.selectChessPos.y);
            }
        }
    }while(message.event != GAME_OVER_GAMEEVENT);
    return nullptr;
}
void ClickCreateServer(const char *name){
    memcpy(g_Server.name, name, MAX_NAME);
    memcpy(g_Client.ip, g_ServerIp, MAX_NAME);
    // g_Player.color = WEI_CHESS_COUNTRY_COLOR;
    // strcpy(g_Player.country, "魏");
    // g_Player.index = 0;
    // g_Player.name = ;
    CreateServer(3);
#ifdef WIN32
    DWORD  threadId;
    g_ServerPthreadId = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)server_start, 0, 0, &threadId);
    CreateClient(g_ServerIp);
    g_ClientPthreadId = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)process_client, &g_ClientSocket, 0, &threadId);
#endif
#ifdef __linux
    pthread_create(&g_ServerPthreadId, nullptr, server_start, nullptr);
    CreateClient(g_ServerIp);
    //该程序也需要客户端方面的线程
    pthread_create(&g_ClientPthreadId, nullptr, process_client, &g_ClientSocket);
#endif
    JoinsGame(g_ServerIp);
}
void ClickCreateClient(const char *serverIp){
    memcpy(g_Client.serverIp, serverIp, MAX_NAME);
    CreateClient(serverIp);
    char ip[MAX_NAME] = { 0 };
    GetLocalIp(ip);//用来存自己的ip
    memcpy(g_Client.ip, ip, MAX_NAME);
#ifdef WIN32
    DWORD  threadId;
    g_ClientPthreadId = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)process_client, &g_ClientSocket, 0, &threadId);
#endif
#ifdef __linux
    pthread_create(&g_ClientPthreadId, nullptr, process_client, &g_ClientSocket);
#endif
    JoinsGame(g_Client.serverIp);
}
void updateImguiWidget(){
    // static bool checkbuttonstatu;//检查框的状态。这个值传给imgui会影响到检查框
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    static bool bShowCreateServer = false, bShowConnectServer = false;
    if(ImGui::Begin("局域网联机")){
        static char name[MAX_NAME];
        //在非广播模式下，直接显示本机ip。(或许广播模式下也可以显示)
        ImGui::Text("本机ip:%s", g_ServerIp);
        const char *countryName[] = { "魏", "蜀", "吴" };
        if(g_CurrentCountry != -1){
            ImGui::Text("该%s国下棋\n", countryName[g_CurrentCountry]);
        }
        ImGui::InputText("用户名", name, sizeof(name));
        if(strcmp(name, g_Client.name)){
            memcpy(g_Client.name, name, MAX_NAME);
        }
        //当有客户端程序接入后，显示名称和ip，然后初始化棋盘并开始游戏
        if(ImGui::BeginTable("client information", 3)){
            ImGui::TableNextColumn();
            ImGui::Text("name");
            for (size_t i = 0; i < g_Clients.size(); ++i){
                ImGui::Text(g_Clients[i].name);
            }
            ImGui::TableNextColumn();
            ImGui::Text("ip");
            for (size_t i = 0; i < g_Clients.size(); ++i){
                ImGui::Text(g_Clients[i].ip);
            }
            ImGui::TableNextColumn();
            ImGui::Text("country");
            for (size_t i = 0; i < g_Players.size(); ++i){
                ImGui::Text(g_Players[i].country);
            }
            // const char *countryItem[] = {
            //     "魏",
            //     "蜀",
            //     "吴",
            // };
            // for (size_t i = 0; i < g_Clients.size(); ++i){
            //     if(ImGui::Combo("国", &g_Clients[i], countryItem, sizeof(countryItem) / sizeof(char *))){
                    
            //         ImGui::TreePop();
            //     }
            // }
            ImGui::EndTable();
        }
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
        ImGui::End();
    }
    if(bShowCreateServer){
        if(ImGui::Begin("创建房间")){
            static char name[MAX_NAME] = {0};
            ImGui::InputText("输入房间名", name, sizeof(name));
            //目前用不到房间名, 所以不需要判断
            if(ImGui::BeginTable("确定取消按钮", 2)){
                ImGui::TableNextColumn();
                if(ImGui::Button("确定")){
                    bShowCreateServer = false;
                    ClickCreateServer(name);
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
            static char serverIp[MAX_NAME] = {0};
            ImGui::InputText("服务端ip", serverIp, sizeof(serverIp));
            if(ImGui::BeginTable("确定取消按钮", 2)){
                ImGui::TableNextColumn();
                if(ImGui::Button("确定")){
                    bShowConnectServer = false;
                    ClickCreateClient(serverIp);
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
    updateImguiWidget();
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
    pChess->Selected((const Chess* (*)[21])g_Chessboard.GetChess(), canplays);
    const int32_t index = GetCanPlay(mousePos, canplays);
    if(index != -1){
        const ChessInfo *pChessInfo = pChess->GetInfo();
        //不知道为什么，手动下棋看不到棋移动的效果
#if (defined AI_RANDOMLY_PLAY_CHESS) || (defined INTERNET_MODE)
        MoveChess(pChessInfo->country, pChessInfo->chess, glm::vec2(COLUMN_TO_X(pChessInfo->column), ROW_TO_Y(pChessInfo->row)), mousePos);
#endif
        g_Chessboard.Move(g_VulkanDevice.device, country, chess, canplays[index].row, canplays[index].column);
        g_CurrentCountry = GetNextCountry(g_CurrentCountry);
    }
}
//吃子
void PlayChess(uint32_t srcCountry, uint32_t srcChess, uint32_t dstCountry, uint32_t dstChess){
    std::vector<ChessInfo>canplays;
    const char county[][MAX_BYTE] = { "魏", "蜀", "吴", "汉" };
    const Chess *pSrcChess = g_Chessboard.GetChess(srcCountry, srcChess);
    const ChessInfo *pSrcChessInfo = pSrcChess->GetInfo(), *pDstChessInfo = g_Chessboard.GetChessInfo(dstCountry, dstChess);
    pSrcChess->Selected((const Chess* (*)[21])g_Chessboard.GetChess(), canplays);
    if(GetCanPlay(pDstChessInfo->row, pDstChessInfo->column, canplays) != -1){
        //不知道为什么，手动下棋看不到棋移动的效果
#if (defined AI_RANDOMLY_PLAY_CHESS) || (defined INTERNET_MODE)
        MoveChess(pSrcChessInfo->country, pSrcChessInfo->chess, glm::vec2(COLUMN_TO_X(pSrcChessInfo->column), ROW_TO_Y(pSrcChessInfo->row)), glm::vec2(COLUMN_TO_X(pDstChessInfo->column), ROW_TO_Y(pDstChessInfo->row)));
#endif
        g_Chessboard.Play(g_VulkanDevice.device, srcCountry, srcChess, dstCountry, dstChess);
        if(g_Chessboard.IsDeath(dstCountry)){
            printf("%s国被%s国消灭\n", county[dstCountry], county[g_CurrentCountry]);
            g_Chessboard.DestroyCountry(g_VulkanDevice.device, dstCountry);
        }
        g_CurrentCountry = GetNextCountry(g_CurrentCountry);
    }
}
#ifdef AI_RANDOMLY_PLAY_CHESS
#ifdef __linux
pthread_t g_AiPlayChess;
#endif
#ifdef WIN32
HANDLE g_AiPlayChess;
#endif
bool GameOver(){
    uint32_t deathCount = 0;
    for (size_t i = 0; i < g_DefaultCountryCount; ++i){
        if(g_Chessboard.IsDeath(i)){
            ++deathCount;
        }
    }
    return deathCount > 1;
}
// //返回第一个满足条件的敌人
// const ChessInfo *GetRival(const std::vector<ChessInfo>&canplays, auto condition){
//     const ChessInfo *pRival = nullptr;
//     for (size_t i = 0; i < canplays.size(); ++i){
//         pRival = g_Chessboard.GetChessInfos(canplays[i].row, canplays[i].column);
//         if(pRival && condition(pRival)){
//             break;
//         }
//         else{
//             pRival = nullptr;
//         }
//     }
//     return pRival;
// }
int32_t CanPlay(const std::vector<ChessInfo>&canplays){
    int32_t index = -1;
    for (size_t i = 0; i < canplays.size(); ++i){
        const ChessInfo *pInfo = g_Chessboard.GetChessInfos(canplays[i].row, canplays[i].column);
        if(!pInfo || pInfo->country != g_CurrentCountry){
            index = i;
            break;
        }
    }
    return index;
}
// //返回rivalCountry中能吃到country的chessIndex棋子
// const ChessInfo *GetRival(uint32_t country, uint32_t rivalCountry, uint32_t chessIndex){
//     const ChessInfo *pChessInfo = nullptr;
//     for (size_t uiChess = 0; uiChess < COUNTRY_CHESS_COUNT; ++uiChess){
//         const ChessInfo *pRival = nullptr;
//         pChessInfo = g_Chessboard.GetChessInfo(rivalCountry, uiChess);
//         if(pChessInfo->row <= CHESSBOARD_ROW && pChessInfo->column <= CHESSBOARD_COLUMN){
//             g_Chessboard.SetSelected(pChessInfo);
//             g_Chessboard.Select(VK_NULL_HANDLE);
//             auto canPlayInfo = g_Chessboard.GetCanPlay();
//             pRival = GetRival(canPlayInfo, [country, chessIndex](const ChessInfo *r){return r->country == country && r->chess == chessIndex;});
//             g_Chessboard.ClearCanPlay(VK_NULL_HANDLE);
//         }
//         if(pRival){
//             break;
//         }
//         else{
//             pChessInfo = nullptr;
//         }
//     }
//     return pChessInfo;
// }
// //返回所有国家中, 能吃到country的chessIndex的棋子
// const ChessInfo *GetRival(uint32_t country, uint32_t chessIndex){
//     const ChessInfo *pChessInfo = nullptr;
//     for (size_t i = 0; i < g_DefaultCountryCount; i++){
//         pChessInfo = GetRival(country, i, chessIndex);
//         if(pChessInfo)break;
//     }
//     return pChessInfo;
// }
// //返回country的第一个敌人
// const ChessInfo *GetRival(uint32_t country, const ChessInfo **pChessInfo){
//     const Chess *pRival = nullptr;
//     for (size_t uiChess = 0; uiChess < COUNTRY_CHESS_COUNT; ++uiChess){
//         *pChessInfo = g_Chessboard.GetChess(country, uiChess);
//         if(*pChessInfo && (*pChessInfo)->row <= CHESSBOARD_ROW && (*pChessInfo)->column <= CHESSBOARD_COLUMN){
//             g_Chessboard.SetSelected(*pChessInfo);
//             g_Chessboard.Select(VK_NULL_HANDLE);
//             auto canPlayInfo = g_Chessboard.GetCanPlay();
//             pRival = GetRival(canPlayInfo, [](const ChessInfo *){return true;});
//             g_Chessboard.ClearCanPlay(VK_NULL_HANDLE);
//             if(pRival)break;
//         }
//     }
//     return pRival;
// }
// //返回cCountry的chessIndex是否能被nextCountry棋子吃;
// bool IsDangerous(uint32_t cCountry, uint32_t nextCountry, uint32_t chessIndex){
//     return GetRival(cCountry, nextCountry, chessIndex);
// }
// // //返回所有国家中, 是否有棋子能吃到country的chessIndex
// // bool IsDangerous(uint32_t country, uint32_t chessIndex){
// //     bool bDangerous = false;
// //     for (size_t i = 0; i < g_DefaultCountryCount; ++i){
// //         if(country != i){
// //             bDangerous = IsDangerous(country, i, chessIndex);
// //             if(bDangerous)break;
// //         }
// //     }
// //     return bDangerous;
// // }
// void aiPlayChess(const ChessInfo*pInfo, const ChessInfo *pRival){
//     if(!g_Chessboard.IsBoundary(pInfo->row, pInfo->column)){
//         glm::vec2 pos = glm::vec2(COLUMN_TO_X(pInfo->column), ROW_TO_Y(pInfo->row));
//         // glm::vec2 pos = glm::vec2(COLUMN_TO_X(pInfo->column) - CHESSBOARD_GRID_SIZE * .5, ROW_TO_Y(pInfo->row) - CHESSBOARD_GRID_SIZE * .5);
//         // const ChessInfo *pSelected = g_Chessboard.GetSelected();
//         const ChessInfo *pChessInfo = g_Chessboard.GetSelectInfo(pos);
//         if(pChessInfo){
//             g_Chessboard.UnSelect(g_VulkanDevice.device);
//             if(pChessInfo->country == g_CurrentCountry){
//                 g_Chessboard.SetSelected(pChessInfo);
//             }
//             const ChessInfo *pSelected = nullptr;
//             auto canPlayInfo = g_Chessboard.GetCanPlay();
//             // if(canPlayInfo.empty()){
//             //     printf("canplay is empty; select row:%d, select column:%d\n", pChessInfo->row, pChessInfo->column);
//             // }
//             if(CanPlay(canPlayInfo)){
//                 if(pRival){
//                     pos = glm::vec2(COLUMN_TO_X(pRival->column), ROW_TO_Y(pRival->row));
//                     Play(pos, pRival);
//                 }
//                 else{
//                     ChessInfo canplay = canPlayInfo[rand() % canPlayInfo.size()];
//                     pos = glm::vec2(COLUMN_TO_X(canplay.column), ROW_TO_Y(canplay.row));
//                     pSelected = g_Chessboard.GetSelectInfo(pos);
//                     Play(pos, pSelected);
//                 }
//                 g_UpdateScreen = true;
//             }
//             g_Chessboard.ClearCanPlay(g_VulkanDevice.device);
//         }
//     }
// }
// /*
//     将
//         面临被吃的危险
//             1,有其他棋子(包括自身)可以解则解
//             2.没路走就走其他棋子,不然要死循环了
//             3.没路、没其他棋走则直接跳过。反正这种情况少见
//     其他棋子:
//         可以先找敌人
//         找不到的话则说明没有保护的棋子
//         面临被吃的危险
//             1.有其他棋子(包括自身)可以解则解
//             2.没有的话如果有保护的棋子，也可以不走
//             3.都没有就而且没路的话就参考上面
//     //除将外，其他棋子均可以在有保护的情况下不走
// */
// void Dangerous(const ChessInfo **pDangerous, const ChessInfo **pRival){
//     if((*pDangerous)->chess == SHI_CHESS_INDEX_1 || (*pDangerous)->chess == SHI_CHESS_INDEX_2){
//         *pDangerous = nullptr;
//         *pRival = nullptr;
//         return;
//     }
//     // g_Chessboard.SetSelected(*pDangerous);
//     // g_Chessboard.Selected(VK_NULL_HANDLE);
//     //找一个解围的棋子
//     const ChessInfo *pChess = GetRival((*pRival)->country, (*pDangerous)->country, (*pRival)->chess);
//     if(pChess){
//         *pDangerous = pChess;
//     }
//     else{
//         if((*pDangerous)->chess != JIANG_CHESS_INDEX){
//             //找一个可以保护pDangerous的棋子
//             pChess = GetRival((*pDangerous)->country, (*pDangerous)->country, (*pDangerous)->chess);
//             if(pChess){
//                 //如果找到就别管了
//                 *pDangerous = nullptr;
//                 *pRival = nullptr;
//                 return;
//             }
//         }
//         if(!CanPlay(g_Chessboard.GetCanPlay())){
//             *pDangerous = nullptr;//其实应该重新设置
//             *pRival = nullptr;
//         }
//     }   
//     // g_Chessboard.ClearCanPlay(VK_NULL_HANDLE);
// }
void aiPlay(){
    const Chess *pChess = nullptr;
    std::vector<ChessInfo>canplays;
    const ChessInfo *pSelect = nullptr, *pRival = nullptr;
    // const uint32_t nextCountry = GetNextCountry(g_CurrentCountry);
    // const ChessInfo * = GetRival(g_CurrentCountry, (uint32_t)JIANG_CHESS_INDEX);
    // if(pRival){
    //     pSelect = g_Chessboard.GetChessInfo(g_CurrentCountry, JIANG_CHESS_INDEX);
    //     Dangerous(&pSelect, &pRival);
    // }
    // else{
    //     pRival = GetRival(g_CurrentCountry, &pSelect);
    //     if(!pRival){
    //         pSelect = nullptr;
    //         //如果某个棋子面临被吃问题, 如果有棋子可以解则解，无再逃走
    //         for (uint32_t uiChess = 1; uiChess < COUNTRY_CHESS_COUNT; ++uiChess){
    //             pRival = GetRival(g_CurrentCountry, nextCountry, uiChess);
    //             if(pRival){
    //                 pSelect = g_Chessboard.GetChessInfo(g_CurrentCountry, uiChess);
    //                 Dangerous(&pSelect, &pRival);
    //                 break;
    //             }
    //         }
    //     }
    // }
    if(!pSelect){
        do{
            pSelect = g_Chessboard.GetChessInfo(g_CurrentCountry, rand() % COUNTRY_CHESS_COUNT);
            if(pSelect){
                canplays.clear();
                pChess = g_Chessboard.GetChess(g_CurrentCountry, pSelect->chess);
                pChess->Selected((const Chess* (*)[21])g_Chessboard.GetChess(), canplays);
            }
        }while(!pSelect || -1 == CanPlay(canplays));//不用担心判断pSelect后死循环,因为"将"肯定会存在,否则就输了
    }
    //想下的棋子、敌人上面设置
    if(pRival){
        PlayChess(g_CurrentCountry, pSelect->chess, pRival->country, pRival->chess);
    }
    else{
        //这个分支也有吃子的可能, 只不过是随机的
        canplays.clear();
        pChess = g_Chessboard.GetChess(g_CurrentCountry, pSelect->chess);
        pChess->Selected((const Chess* (*)[21])g_Chessboard.GetChess(), canplays);
        uint32_t index = 0;
        const ChessInfo *pInfo = nullptr;
        do{
            index = rand() % canplays.size();
            pInfo = g_Chessboard.GetChessInfos(canplays[index].row, canplays[index].column);
        } while (pInfo && pInfo->country == g_CurrentCountry);
        g_Chessboard.Select(g_VulkanDevice.device, g_CurrentCountry, pSelect->chess);
        if(pInfo){
            PlayChess(g_CurrentCountry, pSelect->chess, pInfo->country, pInfo->chess);
        }
        else{
            PlayChess(g_CurrentCountry, pSelect->chess, glm::vec2(COLUMN_TO_X(canplays[index].column), ROW_TO_Y(canplays[index].row)));
        }
        g_Chessboard.UnSelect(g_VulkanDevice.device, g_CurrentCountry, pSelect->chess);
    }
}
void *AiPlayChess(void *userData){
    while(!GameOver()){
        aiPlay();
        // sleep(1);//因为下棋的时候有sleep所以这里不需要
    }
    return nullptr;
}
#endif
const ChessInfo *g_Selected = nullptr;
void mousebutton(GLFWwindow *windows, int button, int action, int mods){
    if(action == GLFW_RELEASE && button == GLFW_MOUSE_BUTTON_LEFT){
        //这里判断了国家是否为-1
        if(g_CurrentCountry == -1)return;
        //单机和局域网联机差不多。无非就是要接收和发送消息
        double xpos, ypos;
        glfwGetCursorPos(windows, &xpos, &ypos);
        const glm::vec2 mousePos = glm::vec2(xpos, ypos);
        if(g_Selected){
            g_Chessboard.UnSelect(g_VulkanDevice.device, g_Selected->country, g_Selected->chess);
            const ChessInfo *pSelected = g_Chessboard.GetChessInfos(mousePos);
#ifdef INTERNET_MODE
            if(pSelected){
                if(pSelected->country != g_CurrentCountry){
                    SendPlayChessMessage(g_Players[g_PlayerIndex], glm::vec2(COLUMN_TO_X(pSelected->column), ROW_TO_Y(pSelected->row)), glm::vec2(COLUMN_TO_X(g_Selected->column), ROW_TO_Y(g_Selected->row)));
                }
            }
            else{
                SendPlayChessMessage(g_Players[g_PlayerIndex], mousePos, glm::vec2(COLUMN_TO_X(g_Selected->column), ROW_TO_Y(g_Selected->row)));
            }
#else
            if(pSelected){
                if(pSelected->country != g_CurrentCountry){
                    PlayChess(g_CurrentCountry, g_Selected->chess, pSelected->country, pSelected->chess);
                }
            }
            else{
                PlayChess(g_CurrentCountry, g_Selected->chess, mousePos);
            }
#endif
            g_Selected = nullptr;
        }
        else{
#ifdef INTERNET_MODE
            if(g_CurrentCountry == g_PlayerIndex)g_Selected = g_Chessboard.GetChessInfos(g_CurrentCountry, mousePos);
#else
            g_Selected = g_Chessboard.GetChessInfos(g_CurrentCountry, mousePos);
#endif
            if(g_Selected && g_Selected->country == g_CurrentCountry){
                g_Chessboard.Select(g_VulkanDevice.device, g_CurrentCountry, g_Selected->chess);
            }
        }
        // aiPlay();//从这边调用就看不到移动效果
#ifndef INTERNET_MODE
        g_UpdateScreen = true;
#endif
    }
// #ifdef AI_RANDOMLY_PLAY_CHESS
//     if(action == GLFW_RELEASE){
// #ifdef WIN32
//         DWORD  threadId;
//         g_AiPlayChess = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)AiPlayChess, nullptr, 0, &threadId);
// #endif
// #ifdef __linux
//         pthread_create(&g_AiPlayChess, nullptr, AiPlayChess, nullptr);
// #endif
//     }
// #endif
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

    g_Chessboard.Setup(g_VulkanDevice.physicalDevice, g_VulkanDevice.device, g_SetLayout, g_WindowWidth, g_VulkanQueue.graphics, g_VulkanPool);
    g_Chessboard.CreatePipeline(g_VulkanDevice.device, g_VulkanWindows.renderpass, g_SetLayout, g_PipelineCache, g_WindowWidth);

    g_Chessboard.UpdateBackgroundUniform(g_VulkanDevice.device, g_WindowWidth);
    g_Chessboard.UpdateUniform(g_VulkanDevice.device);
#ifndef INTERNET_MODE
    g_Chessboard.InitChess(g_VulkanDevice.device);
    // g_CurrentCountry = WEI_COUNTRY_INDEX;
    g_CurrentCountry = rand() % g_DefaultCountryCount;
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
    GetLocalIp(g_ServerIp);
#else
    g_CommandBuffers.resize(g_VulkanWindows.framebuffers.size());
    vkf::tool::AllocateCommandBuffers(g_VulkanDevice.device, g_VulkanPool.commandPool, g_CommandBuffers.size(), g_CommandBuffers.data());
    for (size_t i = 0; i < g_VulkanWindows.framebuffers.size(); ++i){
        RecordCommand(g_CommandBuffers[i], i);
    }
#ifdef AI_RANDOMLY_PLAY_CHESS
#ifdef WIN32
    DWORD  threadId;
    g_AiPlayChess = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)AiPlayChess, nullptr, 0, &threadId);
#endif
#ifdef __linux
    pthread_create(&g_AiPlayChess, nullptr, AiPlayChess, nullptr);
#endif
#endif
#endif
} 
void cleanup(){
    vkDeviceWaitIdle(g_VulkanDevice.device);
#ifdef INTERNET_MODE
    ImGui_ImplGlfw_Shutdown();
    ImGui_ImplVulkan_Shutdown();

    GameMessage message;
    message.event = GAME_OVER_GAMEEVENT;
    if(g_ServerAppaction)SendToAllClient(&message, sizeof(message));
    for (size_t i = 0; i < g_Clients.size(); ++i){
        if(g_ClientSockets[i] != INVALID_SOCKET)shutdown(g_ClientSockets[i], SHUT_RDWR);
    }
    if(g_ServerSocket != INVALID_SOCKET)shutdown(g_ServerSocket, SHUT_RDWR);
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
#ifdef DRAW_CUBE
    vkf::CreateRenderPassForSwapchain(g_VulkanDevice.device, g_VulkanWindows.swapchainInfo.format, g_VulkanWindows.renderpass, true);
    vkf::CreateFrameBufferForSwapchain(g_VulkanDevice.device, g_VulkanWindows, true);
#else
    vkf::CreateRenderPassForSwapchain(g_VulkanDevice.device, g_VulkanWindows.swapchainInfo.format, g_VulkanWindows.renderpass);
    vkf::CreateFrameBufferForSwapchain(g_VulkanDevice.device, g_VulkanWindows);
#endif
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
