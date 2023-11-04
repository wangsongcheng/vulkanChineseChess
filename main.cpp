#include <stdio.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "Chessboard.h"
#include "vulkanFrame.h"

#define INTERNET_MODE

#ifdef INTERNET_MODE
#include <array>
#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <WS2tcpip.h>
#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")
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
enum GameEvent{
    GAME_OVER_GAMEEVENT = 0,
    GAME_START_GAMEEVENT,
    JOINS_GAME_GAMEEVENT,
    PLAY_CHESS_GAMEEVENT,
    CHANGE_PLAY_INFORMATION_GAMEEVENT
};
//考虑一下怎么记录该程序是那国
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
    glm::vec2 playPos, selectPos;
};
VkCommandBuffer g_CommandBuffers;

GameInfo g_Player;//用来存自己角色的信息
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
std::array<SOCKET, 3>g_ClientSockets;
#ifdef __linux
pthread_t g_ServerPthreadId;
pthread_t g_ClientPthreadId;
#endif
#ifdef WIN32
HANDLE g_ServerPthreadId;
HANDLE g_ClientPthreadId;
#endif
#else
std::vector<VkCommandBuffer>g_CommandBuffers;
#endif
VulkanPool g_VulkanPool;
VulkanQueue g_VulkanQueue;
VulkanDevice g_VulkanDevice;
VulkanWindows g_VulkanWindows;
VulkanSynchronize g_VulkanSynchronize;
VkDebugUtilsMessengerEXT g_VulkanMessenger;
//                              163 * 2 + 400 + 11
const uint32_t g_WindowWidth = CHESSBOARD_GRID_SIZE_BIG * 2 + 10 * CHESSBOARD_GRID_SIZE + 11 * CHESSBOARD_LINE_WIDTH, g_WindowHeight = g_WindowWidth;

Chessboard g_Chessboard;
glm::vec3 g_CurrentCountry;

VkSampler g_TextureSampler;
glm::vec3 NextCountry(){
    std::vector<glm::vec3>color;
    static uint32_t index = 0;
    if(!g_Chessboard.IsPerish(WEI_CHESS_INDEX))color.push_back(WEI_CHESS_COUNTRY_COLOR);
    if(!g_Chessboard.IsPerish(SHU_CHESS_INDEX))color.push_back(SHU_CHESS_COUNTRY_COLOR);
    if(!g_Chessboard.IsPerish(WU_CHESS_INDEX))color.push_back(WU_CHESS_COUNTRY_COLOR);
    glm::vec3 next = color[index];
    index = (index + 1) % color.size();
    return next;
}
void InitChessboard(){
    g_Chessboard.InitChessboard(g_VulkanDevice.device);

    g_CurrentCountry = NextCountry();
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
void setupVulkan(GLFWwindow *window){
    uint32_t count;
    const char** instanceExtension = glfwGetRequiredInstanceExtensions(&count);
    std::vector<const char*> extensions(instanceExtension, instanceExtension + count);
    vkf::CreateInstance(extensions, g_VulkanDevice.instance);
    g_VulkanDevice.physicalDevice = vkf::GetPhysicalDevices(g_VulkanDevice.instance);
    vkf::CreateDebugUtilsMessenger(g_VulkanDevice.instance, g_VulkanMessenger, debugUtilsMessenger);
    glfwCreateWindowSurface(g_VulkanDevice.instance, window, nullptr, &g_VulkanWindows.surface);
    vkf::CreateDevice(g_VulkanDevice.physicalDevice, {}, g_VulkanWindows.surface, g_VulkanDevice.device);
    int queueFamilies[2];
    vkf::tool::GetGraphicAndPresentQueueFamiliesIndex(g_VulkanDevice.physicalDevice, g_VulkanWindows.surface, queueFamilies);
    vkGetDeviceQueue(g_VulkanDevice.device, queueFamilies[0], 0, &g_VulkanQueue.graphics);
    vkGetDeviceQueue(g_VulkanDevice.device, queueFamilies[1], 0, &g_VulkanQueue.present);
    vkf::CreateSwapchain(g_VulkanDevice.physicalDevice, g_VulkanDevice.device, g_VulkanWindows.surface, g_VulkanWindows.swapchain);
    vkf::CreateRenderPassForSwapchain(g_VulkanDevice.device, g_VulkanWindows.renderpass);
    vkf::CreateCommandPool(g_VulkanDevice.physicalDevice, g_VulkanDevice.device, g_VulkanPool.commandPool, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
    vkf::CreateFrameBufferForSwapchain(g_VulkanDevice.device, { g_WindowWidth, g_WindowHeight }, g_VulkanWindows, g_VulkanPool.commandPool, g_VulkanQueue.graphics);
    vkf::CreateSemaphoreAndFenceForSwapchain(g_VulkanDevice.device, 3, g_VulkanSynchronize);
#ifdef INTERNET_MODE
    vkf::CreateDescriptorPool(g_VulkanDevice.device, 148, g_VulkanPool.descriptorPool);
#else
    vkf::CreateDescriptorPool(g_VulkanDevice.device, 147, g_VulkanPool.descriptorPool);
#endif
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
void cleanupVulkan(){
    for (size_t i = 0; i < g_VulkanSynchronize.fences.size(); ++i){
        vkDestroyFence(g_VulkanDevice.device, g_VulkanSynchronize.fences[i], nullptr);
        vkDestroySemaphore(g_VulkanDevice.device, g_VulkanSynchronize.imageAcquired[i], nullptr);
        vkDestroySemaphore(g_VulkanDevice.device, g_VulkanSynchronize.renderComplete[i], nullptr);
    }
    for (size_t i = 0; i < g_VulkanWindows.framebuffers.size(); ++i){
        vkDestroyImageView(g_VulkanDevice.device, g_VulkanWindows.swapchainImageViews[i], nullptr);
        vkDestroyFramebuffer(g_VulkanDevice.device, g_VulkanWindows.framebuffers[i], nullptr);
    }
    g_VulkanWindows.depthImage.Destroy(g_VulkanDevice.device);
    
    vkDestroyCommandPool(g_VulkanDevice.device, g_VulkanPool.commandPool, nullptr);
    vkDestroyDescriptorPool(g_VulkanDevice.device, g_VulkanPool.descriptorPool, nullptr);

    vkDestroyRenderPass(g_VulkanDevice.device, g_VulkanWindows.renderpass, nullptr);
    vkDestroySwapchainKHR(g_VulkanDevice.device, g_VulkanWindows.swapchain, nullptr);

    vkDestroySurfaceKHR(g_VulkanDevice.instance, g_VulkanWindows.surface, nullptr);

    vkf::DestoryDebugUtilsMessenger(g_VulkanDevice.instance, g_VulkanMessenger);

    vkDestroyDevice(g_VulkanDevice.device, nullptr);
    vkDestroyInstance(g_VulkanDevice.instance, nullptr);
}
void Play(const glm::vec2&mousePos){
    bool bNext;
    uint32_t country;
    if(g_Chessboard.Play(g_VulkanDevice.device, mousePos, country, bNext)){
        // vkDeviceWaitIdle(g_VulkanDevice.device);
        g_Chessboard.DestroyChess(g_VulkanDevice.device, country);
    }
    if(bNext){
        g_CurrentCountry = NextCountry();
    }
}
#ifdef INTERNET_MODE
void Send(int __fd, const GameMessage *__buf, size_t __n, int __flags){
    int sendSize, offset;
    if(__fd != -1){
        do{
            offset = 0;
            sendSize = 0;
#ifdef WIN32
            sendSize = send(__fd, (const char *)__buf + offset, __n - offset, __flags);
#else
            sendSize = send(__fd, __buf + offset, __n - offset, __flags);
#endif
            if(sendSize == -1){
                perror("send error");
                printf("send error:send size:%d\n", sendSize);
                break;
            }
            else {
                printf("success send message %d byte, address:%p, %d byte, sizeof(GameMessage) = %d\n", sendSize, __buf + offset, __n - offset, sizeof(GameMessage));
            }
            offset += sendSize;
        } while (offset < __n);
    }
}
void Recv(int __fd, GameMessage *__buf, size_t __n, int __flags){
    int recvSize, offset;
    if(__fd != -1){
        do{
            offset = 0;
            recvSize = 0;
#ifdef WIN32
            recvSize = recv(__fd, (char *)__buf + offset, __n - offset, __flags);
#else
            recvSize = recv(__fd, __buf + offset, __n - offset, __flags);
#endif
            if(recvSize == -1){
                perror("recv error");
                printf("recv error:recv size:%d\n", recvSize);
                break;
            }
            else {
                printf("success recv message %d byte, address:%p, %d byte, sizeof(GameMessage) = %d\n", recvSize, __buf + offset, __n - offset, sizeof(GameMessage));
            }
            offset += recvSize;
        } while (offset < __n);
    }
}
void SendToAllClient(const GameMessage *__buf, size_t __n){
    for (size_t i = 0; i < g_Clients.size(); ++i){
        if(g_ClientSockets[i] != -1){
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
    message.event = JOINS_GAME_GAMEEVENT;
    // message.clientIndex = ;
    message.client = g_Client;
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
void AssignmentCountry(uint32_t uiCountry){
    GameMessage message;
    message.event = CHANGE_PLAY_INFORMATION_GAMEEVENT;
    message.game.index = -1;
    strcpy(message.game.country, "魏");
    message.game.color = WEI_CHESS_COUNTRY_COLOR;
    if(uiCountry == 1){
        message.game.color = SHU_CHESS_COUNTRY_COLOR;
        strcpy(message.game.country, "蜀");
    }
    else if(uiCountry == 2){
        message.game.color = WU_CHESS_COUNTRY_COLOR;
        strcpy(message.game.country, "吴");
    }
    g_Players[uiCountry] = message.game;
    Send(g_ClientSockets[uiCountry], &message, sizeof(message), 0);
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
void *server_start(void *userData){
    GameMessage message;
    struct sockaddr_in client_addr;
    socklen_t size = sizeof(client_addr);
    //直接在这里等会导致imgui控件无法更新
    for (size_t i = 0; i < 3; i++){
        g_ClientSockets[i] = accept(g_ServerSocket, (struct sockaddr *)&client_addr, &size);
        if(g_ClientSockets[i] == -1){
            perror("process_server:accept");
        }
        else{
            //客户端连接后会发送自身信息
            Recv(g_ClientSockets[i], &message, sizeof(message), 0);
            printf("玩家加入, 名字:%s, ip:%s\n", message.client.name, message.client.ip);
            // memcpy(g_Clients[i].name, message.client.name, MAX_NAME);
            // memcpy(g_Clients[i].ip, message.client.ip, MAX_NAME);
            g_Clients[i] = message.client;
            AssignmentCountry(i);
            for (uint32_t uiClient = 0; uiClient < g_Clients.size(); ++uiClient){
                message.index = uiClient;
                message.game = g_Players[uiClient];
                message.client = g_Clients[uiClient];
                SendToAllClient(&message, sizeof(message));
            }
        }
    }
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
void Play(const GameInfo&game, const glm::vec2&playPos, const glm::vec2&selectPos){
    GameMessage message;
    message.event = PLAY_CHESS_GAMEEVENT;
    message.game = game;
    message.playPos = playPos;
    message.selectPos = selectPos;
    SendToServer(&message, sizeof(message));
}
//但该程序为客户端时,一般不会用上面的函数而是用下面的函数创建创建线程
void *process_client(void *userData){
    //接收消息后做出动作即可
    GameMessage message;
    do{
        Recv(g_ClientSocket, &message, sizeof(GameMessage), 0);
        if(message.event == GAME_START_GAMEEVENT){
            printf("game start\n");
            InitChessboard();
        }
        else if(message.event == JOINS_GAME_GAMEEVENT){
            g_Clients[message.index] = message.client;
        }
        else if(message.event == PLAY_CHESS_GAMEEVENT){
            //下棋和选棋的坐标不一样
            g_Chessboard.Select(g_VulkanDevice.device, message.selectPos);
            // printf("%s国下棋:位置:%f, %f\n", message.game.country, message.mousePos.x, message.mousePos.y);
            Play(message.playPos);
            // Play(message.ranks.row, message.ranks.column);
        }
        else if(message.event == CHANGE_PLAY_INFORMATION_GAMEEVENT){
            if(message.game.index == -1)
                g_Player = message.game;
            else
                g_Players[message.game.index] = message.game;
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
    g_ClientPthreadId = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)process_client, 0, 0, &threadId);
#endif
#ifdef __linux
    pthread_create(&g_ServerPthreadId, nullptr, server_start, nullptr);
    CreateClient(g_ServerIp);
    //该程序也需要客户端方面的线程
    pthread_create(&g_ClientPthreadId, nullptr, process_client, nullptr);
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
    g_ClientPthreadId = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)process_client, 0, 0, &threadId);
#endif
#ifdef __linux
    pthread_create(&g_ClientPthreadId, nullptr, process_client, nullptr);
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
void RecordCommand(uint32_t currentFrame){
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
    vkBeginCommandBuffer(g_CommandBuffers, &beginInfo);
    vkCmdBeginRenderPass(g_CommandBuffers, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    g_Chessboard.RecordCommand(g_CommandBuffers, g_CurrentCountry);

    updateImguiWidget();

    vkCmdEndRenderPass(g_CommandBuffers);
    vkEndCommandBuffer(g_CommandBuffers);
}
#else
void RecordCommand(uint32_t currentFrame){
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
    vkBeginCommandBuffer(g_CommandBuffers[currentFrame], &beginInfo);
    vkCmdBeginRenderPass(g_CommandBuffers[currentFrame], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    g_Chessboard.RecordCommand(g_CommandBuffers[currentFrame], g_CurrentCountry);

    vkCmdEndRenderPass(g_CommandBuffers[currentFrame]);
    vkEndCommandBuffer(g_CommandBuffers[currentFrame]);
}
#endif
glm::vec2 g_SelectPos;
void mousebutton(GLFWwindow *windows, int button, int action, int mods){
    double xpos, ypos;
    glfwGetCursorPos(windows, &xpos, &ypos);
    if(action == GLFW_RELEASE){
        if(button == GLFW_MOUSE_BUTTON_LEFT){
            //单机和局域网联机差不多。无非就是要接收和发送消息
            const Chess *chess = g_Chessboard.GetChess(glm::vec2(xpos, ypos));
            if(!g_Chessboard.IsSelected() || (chess && g_Chessboard.IsSelfChess(chess->GetCountryColor()))){
                if(chess){
#ifdef INTERNET_MODE
                    if(g_CurrentCountry == g_Player.color){
                        g_SelectPos = glm::vec2(xpos, ypos);
                        g_Chessboard.Select(g_VulkanDevice.device, g_SelectPos);
                    }
#else
                    if(g_CurrentCountry == chess->GetCountryColor()){
                        g_Chessboard.Select(g_VulkanDevice.device, glm::vec2(xpos, ypos));
                    }
#endif
                }
            }
            else{
#ifdef INTERNET_MODE
                if(!chess || chess->GetCountryColor() != g_Player.color)Play(g_Player, glm::vec2(xpos, ypos), g_SelectPos);
#else
                Play(glm::vec2(xpos, ypos));
                vkDeviceWaitIdle(g_VulkanDevice.device);
                for (size_t i = 0; i < g_VulkanWindows.framebuffers.size(); ++i){
                    RecordCommand(i);
                }
#endif
            }
        }
    }
}
void setup(GLFWwindow *windows){
    glfwSetMouseButtonCallback(windows, mousebutton);
    vkf::CreateTextureSampler(g_VulkanDevice.device, g_TextureSampler);

    g_Chessboard.CreatePipeline(g_VulkanDevice.device, g_VulkanWindows.renderpass, g_WindowWidth);
    g_Chessboard.CreateVulkanResource(g_VulkanDevice.physicalDevice, g_VulkanDevice.device, g_WindowWidth, g_VulkanQueue.graphics, g_VulkanPool, g_VulkanWindows.renderpass, g_TextureSampler);

    g_Chessboard.UpdateSet(g_VulkanDevice.device);
#ifndef INTERNET_MODE
    InitChessboard();
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
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;//启用手柄    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForVulkan(windows, true);
    int queueFamily;
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

    VkCommandBuffer cmd;
    vkf::tool::BeginSingleTimeCommands(g_VulkanDevice.device, g_VulkanPool.commandPool, cmd);
    ImGui_ImplVulkan_CreateFontsTexture(cmd);
    vkf::tool::EndSingleTimeCommands(g_VulkanDevice.device, g_VulkanPool.commandPool, g_VulkanQueue.graphics, cmd);
    ImGui_ImplVulkan_DestroyFontUploadObjects();
#ifdef WIN32
    WSADATA wsaData;
    int wsaResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if(wsaResult != 0){
        printf("in function:%s:WSAStartup error, result = %d", __FUNCTION__, wsaResult);
    }
#endif
    GetLocalIp(g_ServerIp);
    g_ClientSockets[0] = -1;
    g_ClientSockets[1] = -1;
    g_ClientSockets[2] = -1;
#else
    g_CommandBuffers.resize(g_VulkanWindows.framebuffers.size());
    vkf::tool::AllocateCommandBuffers(g_VulkanDevice.device, g_VulkanPool.commandPool, g_CommandBuffers.size(), g_CommandBuffers.data());
    for (size_t i = 0; i < g_VulkanWindows.framebuffers.size(); ++i){
        RecordCommand(i);
    }
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
        if(g_ClientSockets[i] != -1)shutdown(g_ClientSockets[i], 2);
    }
    if(g_ServerSocket != -1)shutdown(g_ServerSocket, 2);
#ifdef WIN32
    WSACleanup();
#endif
#endif
    g_Chessboard.Cleanup(g_VulkanDevice.device);
    vkDestroySampler(g_VulkanDevice.device, g_TextureSampler, nullptr);
}
void display(GLFWwindow* window){
    static size_t currentFrame;
    vkDeviceWaitIdle(g_VulkanDevice.device);
#ifdef INTERNET_MODE
    RecordCommand(currentFrame);
    vkf::DrawFrame(g_VulkanDevice.device, currentFrame, g_CommandBuffers, g_VulkanWindows.swapchain, g_VulkanQueue, g_VulkanSynchronize);
#else
    vkf::DrawFrame(g_VulkanDevice.device, currentFrame, g_CommandBuffers[currentFrame], g_VulkanWindows.swapchain, g_VulkanQueue, g_VulkanSynchronize);
#endif
    currentFrame = (currentFrame + 1) % g_VulkanWindows.framebuffers.size();
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
    setupVulkan(window);
    setup(window);
    while (!glfwWindowShouldClose(window)) {
        display(window);

        glfwPollEvents();
    }
	glfwTerminate();

    vkDeviceWaitIdle(g_VulkanDevice.device);
    cleanup();
    cleanupVulkan();
    return 0;
}
