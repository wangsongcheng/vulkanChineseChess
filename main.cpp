#include <array>
#include <stdio.h>
#ifdef __linux
#include <unistd.h>
#include <pthread.h>
#endif
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "Chess.h"
#include "Game.h"
#include "Pipeline.hpp"
#include "VulkanChess.h"
#include "VulkanChessboard.h"
#define JOIN_AI

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

#define MAX_BYTE 0xff
//该宏重新定义在vulkanchess.h
// #define HAN_CAN_PLAY
#define INTERNET_MODE
#ifdef INTERNET_MODE
#include <array>

#include "Server.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"
#ifdef HAN_CAN_PLAY
const uint32_t g_DefaultCountryCount = MAX_COUNTRY_INDEX;
#else
const uint32_t g_DefaultCountryCount = MAX_COUNTRY_INDEX - 1;
#endif
Client g_Client;
Server g_Server;
char g_ServerIp[MAX_BYTE];
bool g_ServerAppaction, g_GameStart;
std::array<Player, g_DefaultCountryCount>g_Players;//用来保存其他玩家的信息//尽管也包含自身信息
//本来只需要1个,但服务端能改ai的势力, 所以需要加上ai的
char g_CountryItems[2][5][MAX_BYTE];
glm::vec2 g_JiangPos[MAX_COUNTRY_INDEX];//目前只用于RotateChess
uint32_t g_ClientIndex = INVALID_PLAYER_INDEX;
// uint32_t g_AIClientIndex = INVALID_PLAYER_INDEX;
#endif
struct GraphicsPipeline{
    VkPipeline pipeline;
    VkPipelineCache cache;
};
VulkanPool g_VulkanPool;
VulkanQueue g_VulkanQueue;
VulkanDevice g_VulkanDevice;
VulkanWindows g_VulkanWindows;
VulkanSynchronize g_VulkanSynchronize;
VkDebugUtilsMessengerEXT g_VulkanMessenger;

uint32_t g_WindowWidth = CHESSBOARD_BIG_RECT_SIZE * 2 + 10 * CHESSBOARD_RECT_SIZE + 11 * CHESSBOARD_LINE_WIDTH, g_WindowHeight = g_WindowWidth;

VkCommandBuffer g_CommandBuffers;

VulkanChess g_Chess;
VulkanChessboard g_VulkanChessboard;

VkPipelineLayout g_PipelineLayout;
GraphicsPipeline g_Fill, g_Wireframe, g_Font;
VkDescriptorSetLayout g_DescriptorSetLayout;

Game g_Game;
uint32_t g_Player, g_CurrentCountry;
#ifdef JOIN_AI
void *AiPlayChess(void *userData);
#endif
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
void SetupVulkan(GLFWwindow *window){
    uint32_t count;
    const char** instanceExtension = glfwGetRequiredInstanceExtensions(&count);
    std::vector<const char*> extensions(instanceExtension, instanceExtension + count);
    VK_CHECK(vkf::CreateInstance(extensions, g_VulkanDevice.instance));
    g_VulkanDevice.physicalDevice = vkf::GetPhysicalDevices(g_VulkanDevice.instance);
    vkf::CreateDebugUtilsMessenger(g_VulkanDevice.instance, g_VulkanMessenger, debugUtilsMessenger);
    glfwCreateWindowSurface(g_VulkanDevice.instance, window, nullptr, &g_VulkanWindows.surface);
    vkf::CreateDevice(g_VulkanDevice.physicalDevice, {}, g_VulkanWindows.surface, g_VulkanDevice.device);
    uint32_t queueFamilies[2];
    vkf::tool::GetGraphicAndPresentQueueFamiliesIndex(g_VulkanDevice.physicalDevice, g_VulkanWindows.surface, queueFamilies);
    vkGetDeviceQueue(g_VulkanDevice.device, queueFamilies[0], 0, &g_VulkanQueue.graphics);
    vkGetDeviceQueue(g_VulkanDevice.device, queueFamilies[1], 0, &g_VulkanQueue.present);
    vkf::CreateSwapchain(g_VulkanDevice.physicalDevice, g_VulkanDevice.device, g_VulkanWindows);
    vkf::CreateRenderPassForSwapchain(g_VulkanDevice.device, g_VulkanWindows.swapchainInfo.format, g_VulkanWindows.renderpass, true);
    vkf::CreateCommandPool(g_VulkanDevice.physicalDevice, g_VulkanDevice.device, g_VulkanPool.command);
    vkf::CreateFrameBufferForSwapchain(g_VulkanDevice.device, g_VulkanWindows, true);
    vkf::CreateSemaphoreAndFenceForSwapchain(g_VulkanDevice.device, g_VulkanWindows.swapchainImages.size(), g_VulkanSynchronize);
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
    for (size_t i = 0; i < g_VulkanSynchronize.fences.size(); ++i){
        vkDestroyFence(g_VulkanDevice.device, g_VulkanSynchronize.fences[i], nullptr);
        vkDestroySemaphore(g_VulkanDevice.device, g_VulkanSynchronize.imageAcquired[i], nullptr);
        vkDestroySemaphore(g_VulkanDevice.device, g_VulkanSynchronize.renderComplete[i], nullptr);
    }

    CleanupSwapchain(g_VulkanDevice.device, g_VulkanWindows);

    vkDestroyCommandPool(g_VulkanDevice.device, g_VulkanPool.command, nullptr);
    vkDestroyDescriptorPool(g_VulkanDevice.device, g_VulkanPool.descriptor, nullptr);

    vkDestroySurfaceKHR(g_VulkanDevice.instance, g_VulkanWindows.surface, nullptr);

    vkf::DestoryDebugUtilsMessenger(g_VulkanDevice.instance, g_VulkanMessenger);

    vkDestroyDevice(g_VulkanDevice.device, nullptr);
    vkDestroyInstance(g_VulkanDevice.instance, nullptr);
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
    vkDestroyPipeline(g_VulkanDevice.device, g_Fill.pipeline, VK_NULL_HANDLE);
    vkDestroyPipeline(g_VulkanDevice.device, g_Font.pipeline, VK_NULL_HANDLE);
    vkDestroyPipeline(g_VulkanDevice.device, g_Wireframe.pipeline, VK_NULL_HANDLE);
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
    VkGraphicsPipelineCreateInfo pipelineCreateInfo = Pipeline::initializers::pipelineCreateInfo(layout, g_VulkanWindows.renderpass, 0);

    VkPipelineColorBlendAttachmentState blendAttachmentState = Pipeline::initializers::pipelineColorBlendAttachmentState(0xf, VK_FALSE);
    VkPipelineColorBlendStateCreateInfo colorBlendState = Pipeline::initializers::pipelineColorBlendStateCreateInfo(1, &blendAttachmentState);
    VkPipelineMultisampleStateCreateInfo multisampleState = Pipeline::initializers::pipelineMultisampleStateCreateInfo(VK_SAMPLE_COUNT_1_BIT, 0);
    VkPipelineDepthStencilStateCreateInfo depthStencilState = Pipeline::initializers::pipelineDepthStencilStateCreateInfo(VK_FALSE, VK_FALSE, VK_COMPARE_OP_LESS_OR_EQUAL);
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyState = Pipeline::initializers::pipelineInputAssemblyStateCreateInfo(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, 0, VK_FALSE);
    VkPipelineRasterizationStateCreateInfo rasterizationState = Pipeline::initializers::pipelineRasterizationStateCreateInfo(VK_POLYGON_MODE_FILL, VK_CULL_MODE_NONE, VK_FRONT_FACE_CLOCKWISE, 0);
    // std::array<VkDynamicState> dynamicStateEnables = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
    // VkPipelineDynamicStateCreateInfo dynamicState = Pipeline::initializers::pipelineDynamicStateCreateInfo(dynamicStateEnables.data(), static_cast<uint32_t>(dynamicStateEnables.size()), 0);
    VkRect2D scissors = Pipeline::rect2D(g_WindowWidth, g_WindowHeight);
    VkViewport viewport = Pipeline::viewport(g_WindowWidth, g_WindowHeight);
    viewportState.pScissors = &scissors;
    viewportState.pViewports = &viewport;

    inputState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    inputState.vertexBindingDescriptionCount = 1;
    inputState.pVertexBindingDescriptions = &bindingDescriptions;
    inputState.pVertexAttributeDescriptions = attributeDescriptions.data();
    inputState.vertexAttributeDescriptionCount = attributeDescriptions.size();

    shaderStages[0] = Pipeline::LoadShader(VK_SHADER_STAGE_VERTEX_BIT);
    shaderStages[1] = Pipeline::LoadShader(VK_SHADER_STAGE_FRAGMENT_BIT);
    Pipeline::CreateShaderModule(device, "shaders/baseVert.spv", shaderStages[0].module);
    Pipeline::CreateShaderModule(device, "shaders/baseFrag.spv", shaderStages[1].module);

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
    VK_CHECK(vkCreateGraphicsPipelines(device, g_Fill.cache, 1, &pipelineCreateInfo, nullptr, &g_Fill.pipeline));
    rasterizationState = Pipeline::initializers::pipelineRasterizationStateCreateInfo(VK_POLYGON_MODE_LINE, VK_CULL_MODE_NONE, VK_FRONT_FACE_CLOCKWISE, 0);
    VK_CHECK(vkCreateGraphicsPipelines(device, g_Wireframe.cache, 1, &pipelineCreateInfo, nullptr, &g_Wireframe.pipeline));
    vkDestroyShaderModule(device, shaderStages[0].module, VK_NULL_HANDLE);
    vkDestroyShaderModule(device, shaderStages[1].module, VK_NULL_HANDLE);
    rasterizationState = Pipeline::initializers::pipelineRasterizationStateCreateInfo(VK_POLYGON_MODE_FILL, VK_CULL_MODE_NONE, VK_FRONT_FACE_CLOCKWISE, 0);
    Pipeline::CreateShaderModule(device, "shaders/baseFontVert.spv", shaderStages[0].module);
    Pipeline::CreateShaderModule(device, "shaders/baseFontFrag.spv", shaderStages[1].module);
    VK_CHECK(vkCreateGraphicsPipelines(device, g_Font.cache, 1, &pipelineCreateInfo, nullptr, &g_Font.pipeline));
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
    vkf::CreateDescriptorSetLayout(device, 2, setlayoutBindings, &g_DescriptorSetLayout);
}
void UpdateChessUniform(VkDevice device, Chess *pChess[MAX_COUNTRY_INDEX][DRAW_COUNTRY_CHESS_COUNT]){
    for (uint32_t uiCoutry = 0; uiCoutry < MAX_COUNTRY_INDEX; ++uiCoutry){
        for(uint32_t uiChess = 0; uiChess < DRAW_COUNTRY_CHESS_COUNT; ++uiChess){
            const Chess *pc = pChess[uiCoutry][uiChess];
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
void ClearSelectWireframeUnfirom(glm::mat4 model[CHESSBOARD_ROW * 2]){
    for (size_t i = 0; i < CHESSBOARD_ROW * 2; ++i){
        model[i] = glm::translate(glm::mat4(1), glm::vec3(COLUMN_TO_X(100), ROW_TO_Y(100), 0));
    }
}
void UpdateSelectChessUniform(VkDevice device, std::vector<Chess>&canplays){
    //本来, 为了点击棋子之后有效果...是会改变棋子大小的..但如今发现..或许不需要了
    glm::mat4 model[CHESSBOARD_ROW * 2];
    const glm::mat4 scale = glm::scale(glm::mat4(1), glm::vec3(CHESSBOARD_RECT_SIZE, CHESSBOARD_RECT_SIZE, 1));
    for (size_t i = 0; i < canplays.size(); ++i){
        model[i] = glm::translate(glm::mat4(1), glm::vec3(COLUMN_TO_X(canplays[i].GetColumn()) - CHESSBOARD_RECT_SIZE * .5, ROW_TO_Y(canplays[i].GetRow()) - CHESSBOARD_RECT_SIZE * .5, 0)) * scale;
    }
    g_VulkanChessboard.UpdateSelectChessWireframeUniform(device, model, CHESSBOARD_ROW * 2);
}
//以下2个函数只处理显示的线框
void UnSelectChess(VkDevice device){
    glm::mat4 model[CHESSBOARD_ROW * 2];
    ClearSelectWireframeUnfirom(model);
    g_VulkanChessboard.UpdateSelectChessWireframeUniform(device, model, CHESSBOARD_ROW * 2);
}
void SelectChess(VkDevice device, const Chess *pChess){
    std::vector<Chess>canplays;
    pChess->Selected(g_Game.GetChess(), canplays);
    UpdateSelectChessUniform(device, canplays);
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
bool GameOver(uint32_t countryCount){
    uint32_t deathCount = 0;
    for (size_t i = 0; i < countryCount; ++i){
        if(g_Game.IsDeath(i)){
            ++deathCount;
        }
    }
    return deathCount > 1;
}
uint32_t GetNextCountry(uint32_t currentCountry){
    // currentCountry = g_ClientIndex;
    do{
        currentCountry = (currentCountry + 1) % g_DefaultCountryCount;
    } while (g_Game.IsDeath(currentCountry));
    //"将"警告
    return currentCountry;
}
void PlayChess(Chess *pChess, uint32_t dstRow, uint32_t dstColumn){
    UnSelectChess(g_VulkanDevice.device);
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
    UpdateChessUniform(g_VulkanDevice.device, g_Game.GetChess());
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
    if(g_CurrentCountry != g_Player){
        EnableAi();
    }
#endif
#endif
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
#ifdef INTERNET_MODE
void InitJiangPos(){
    const Chess *pChess = g_Game.GetChess()[WEI_COUNTRY_INDEX][JIANG_CHESS_INDEX];
    g_JiangPos[WEI_COUNTRY_INDEX] = glm::vec2(pChess->GetColumn(), pChess->GetRow());
    pChess = g_Game.GetChess()[SHU_COUNTRY_INDEX][JIANG_CHESS_INDEX];
    g_JiangPos[SHU_COUNTRY_INDEX] = glm::vec2(pChess->GetColumn(), pChess->GetRow());
    pChess = g_Game.GetChess()[WU_COUNTRY_INDEX][JIANG_CHESS_INDEX];
    g_JiangPos[WU_COUNTRY_INDEX] = glm::vec2(pChess->GetColumn(), pChess->GetRow());
    pChess = g_Game.GetChess()[HAN_COUNTRY_INDEX][JIANG_CHESS_INDEX];
    g_JiangPos[HAN_COUNTRY_INDEX] = glm::vec2(pChess->GetColumn(), pChess->GetRow());
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
    uint32_t countryIndex[MAX_COUNTRY_INDEX];
    const char *countryName[MAX_COUNTRY_INDEX];
    countryName[WU_COUNTRY_INDEX] = "吴";
    countryName[WEI_COUNTRY_INDEX] = "魏";
    countryName[SHU_COUNTRY_INDEX] = "蜀";
#ifdef HAN_CAN_PLAY
    countryName[HAN_COUNTRY_INDEX] = "汉";
#endif
    RandomNumber(g_DefaultCountryCount, g_DefaultCountryCount, countryIndex);
    for (auto it = g_Players.begin(); it != g_Players.end(); ++it){
        uint32_t uiCountry = countryToindex(it->country);
        if(!it->random && g_ClientIndex < g_DefaultCountryCount - 1){
            for (size_t i = g_ClientIndex + 1; i < g_DefaultCountryCount; ++i){
                if(uiCountry == countryIndex[i]){
                    Swap(&countryIndex[g_ClientIndex], &countryIndex[i]);
                    break;
                }
            }
        }
    }
    uint32_t playerIndex = 0;
    for (auto it = g_Players.begin(); it != g_Players.end(); ++it, ++playerIndex){
        strcpy(it->country, countryName[countryIndex[playerIndex]]);
        it->index = countryIndex[playerIndex];
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
    message.clientIndex = g_CurrentCountry;
    message.event = CURRENT_COUNTRY_GAME_EVENT;
    g_Server.SendToAllClient(&message, sizeof(message));
}
void SendPlayChessMessage(const Player&game, const Chess *pSelect, const Chess *pTarget){
    GameMessage message;
    message.event = PLAY_CHESS_GAME_EVENT;
    message.player = game;
    // message.mousePos = mousePos;
    if(pSelect){
        memcpy(&message.select, pSelect, sizeof(Chess));
    }
    if(pTarget){
        memcpy(&message.target, pTarget, sizeof(Chess));
    }
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
int32_t GetAiPlayerIndex(uint32_t start = 0){
    int32_t index = INVALID_PLAYER_INDEX;
    for (size_t i = start; i < g_Players.size(); ++i){
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
#ifndef HAN_CAN_PLAY
    static uint32_t socketIndex[] = { 0, 1, 2 };
#else
    static uint32_t socketIndex[] = { 0, 1, 2, 3 };
#endif
    for (uint32_t i = 0; i < g_DefaultCountryCount; ++i){
        if(INVALID_SOCKET != g_Server.AcceptClient(i, &message, sizeof(message))){
            // printf("in function %s:socket index %d\n", __FUNCTION__, i);
            if(message.event == AI_JOIN_GAME_GAME_EVENT){
                g_Players[i].ai = true;
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
//country为下棋的国家
void RotateChess(uint32_t country, uint32_t row, uint32_t column, uint32_t&newRow, uint32_t&newColumn){
    int32_t aiIndex[2] = { INVALID_PLAYER_INDEX, INVALID_PLAYER_INDEX};
    for (size_t i = 0; i < 2; ++i){
        aiIndex[i] = GetAiPlayerIndex(aiIndex[0] + 1);
        if(aiIndex[i] != INVALID_PLAYER_INDEX && country == g_Players[aiIndex[i]].index && g_ServerAppaction){
            return;
        }
    }
    if(country != g_Players[g_ClientIndex].index){
        //因为ai下棋的位置和服务端一样，所以必须用服务端的国家索引
        uint32_t uiCountry = country;
        for (size_t i = 0; i < 2; ++i){
            if(aiIndex[i] != INVALID_PLAYER_INDEX && country == g_Players[aiIndex[i]].index){
                uiCountry = g_Players[0].index;
                break;
            }
        }
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
    // // if(count){
    // //     glm::vec3 point;
    // //     if(row < 8 || column < 8){
    // //         count += 2;
    // //         point = glm::vec3(column + 8, row + 8, .0f);
    // //     }
    // //     else{
    // //         point = glm::vec3(column - 8, row - 8, .0f);
    // //     }
    // //     const float angle = glm::radians(90.0f *  count) * -1;
    // //     const glm::vec3 zAxis = glm::vec3(0, 0, 1);
    // //     glm::mat4 rotate = glm::rotate(glm::mat4(1), angle, zAxis);
    // //     point = rotate * glm::vec4(point, 1.0);
    // //     point.x += 8;
    // //     point.y += 8;
    // //     //取小数部分
    // //     float y = point.y - (uint32_t)point.y;
    // //     float x = point.x - (uint32_t)point.x;
    // //     newRow = point.y;
    // //     newColumn = point.x;
    // //     newRow = y > .5?newRow + 1:newRow;
    // //     newColumn = x > .5?newColumn + 1:newColumn;
    // // }
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
            g_Player = g_Players[g_ClientIndex].index;
            g_Game.InitializeChess(g_Player);
            UpdateChessUniform(g_VulkanDevice.device, g_Game.GetChess());

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
            auto pChess = g_Game.GetChess();
            uint32_t dstRow = message.target.GetRow(), dstColumn = message.target.GetColumn();
            Chess *pStart = pChess[message.select.GetCountry()][message.select.GetChess()], *pTarget = nullptr;
            if(message.target.GetCountry() >= MAX_COUNTRY_INDEX){
                RotateChess(g_CurrentCountry, dstRow, dstColumn,dstRow, dstColumn);
            }
            else{
                pTarget = pChess[message.target.GetCountry()][message.target.GetChess()];
                dstRow = pTarget->GetRow();
                dstColumn = pTarget->GetColumn();
            }
            printf("country %d, chess %d, row %d, column %d\n", message.target.GetCountry(), message.target.GetChess(), dstRow, dstColumn);
            PlayChess(pStart, dstRow, dstColumn);
        }
        else if(message.event == GAME_OVER_GAME_EVENT){
            printf("int function %s:game over\n", __FUNCTION__);
            break;
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
void ShowPlayerCountryCombo(uint32_t comboIndex, const Player *it){
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
                uint32_t aiCount = 0;
                for (auto it = g_Players.begin(); it != g_Players.end(); it++){
                    if(!strcmp(it->name, "ai")){
                        ++aiCount;
                        break;
                    }
                }
                //减掉一个服务端玩家和一个客户端玩家
                if(aiCount < g_DefaultCountryCount - 2)AddAi(g_ServerIp);
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
    }
    ImGui::End();
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
#ifdef JOIN_AI
#ifdef __linux
pthread_t g_AiPlayChess;
#endif
#ifdef WIN32
HANDLE g_AiPlayChess;
#endif
void aiPlay(){
    uint32_t dstRow, dstColumn;
    Chess *pSelect = nullptr, *pTarget = nullptr;
    g_AI.GetPlayChess(g_CurrentCountry, &pSelect, &pTarget, &dstRow, &dstColumn, &g_Game);
    Chess target(dstRow, dstColumn);
    // SelectChess(g_VulkanDevice.device, pSelect);
#ifdef INTERNET_MODE
    if(pTarget){
        SendPlayChessMessage(g_Players[g_CurrentCountry], pSelect, pTarget);
    }
    else{
        target.SetCountry(MAX_COUNTRY_INDEX);
        SendPlayChessMessage(g_Players[g_CurrentCountry], pSelect, &target);
    }
#else
    PlayChess(pSelect, dstRow, dstColumn);
#endif
    // UnSelectChess(g_VulkanDevice.device);
}
void *AiPlayChess(void *userData){
#ifdef __linux__
#ifdef INTERNET_MODE
    int32_t aiIndex, __value = 0;
    for (size_t i = 0; i < 2; ++i){
        aiIndex = GetAiPlayerIndex(aiIndex + 1);
        if(aiIndex != INVALID_PLAYER_INDEX && g_Players[aiIndex].index == g_CurrentCountry){
            __value = 1;
            break;
        }
    }
    sem_init(&g_AiSemaphore, 0, __value);
#else
    sem_init(&g_AiSemaphore, 0, g_Player != g_CurrentCountry);
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
void RecordCommand(VkCommandBuffer command, VkFramebuffer frame){
    vkf::tool::BeginCommands(command, VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT);
    vkf::tool::BeginRenderPassGeneral(command, frame, g_VulkanWindows.renderpass, g_WindowWidth, g_WindowHeight);

    const glm::mat4 projection = glm::ortho(0.0f, (float)g_WindowWidth, 0.0f, (float)g_WindowHeight, -1.0f, 1.0f);
    vkCmdBindPipeline(command, VK_PIPELINE_BIND_POINT_GRAPHICS, g_Fill.pipeline);
    vkCmdPushConstants(command, g_PipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(glm::mat4), &projection);

    g_VulkanChessboard.Draw(command,g_PipelineLayout);

    vkCmdBindPipeline(command, VK_PIPELINE_BIND_POINT_GRAPHICS, g_Wireframe.pipeline);
    g_VulkanChessboard.DrawWireframe(command, g_PipelineLayout);

    vkCmdBindPipeline(command, VK_PIPELINE_BIND_POINT_GRAPHICS, g_Fill.pipeline);
    g_Chess.DrawChess(command, g_PipelineLayout, g_CurrentCountry);
    vkCmdBindPipeline(command, VK_PIPELINE_BIND_POINT_GRAPHICS, g_Font.pipeline);
    g_Chess.DrawFont(command, g_PipelineLayout);

#ifdef INTERNET_MODE
    UpdateImguiWidget();
#endif

    vkCmdEndRenderPass(command);
    vkEndCommandBuffer(command);
}
#ifndef INTERNET_MODE
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
    return nullptr;
}
#endif
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
    const Chess *pSelected = nullptr;
    pSelected = g_Game.GetChess(country, mousePos);
    if(pSelected && pSelected->GetCountry() == g_CurrentCountry){
        SelectChess(g_VulkanDevice.device, pSelected);
        return pSelected;
    }
    return nullptr;
}
void PreparePlayChess(const Chess *pSelect, const glm::vec2&mousePos){
#ifndef INTERNET_MODE
    static PPC ppc;
#endif
    std::vector<Chess>canplays;
    pSelect->Selected(g_Game.GetChess(), canplays);
    int32_t index = GetCanPlay(mousePos, canplays);

    UnSelectChess(g_VulkanDevice.device);
    Chess *pTarget = GetChess(mousePos);
    if(pTarget){
        //选到的是棋子，不论是否自己的
        if(pTarget->GetCountry() != pSelect->GetCountry() && -1 != index){
#ifdef INTERNET_MODE
                //单机和局域网联机差不多。无非就是要接收和发送消息
                SendPlayChessMessage(g_Players[g_ClientIndex], pSelect, pTarget);
#else
                ppc.dstRow = pTarget->GetRow();
                ppc.srcRow = pSelect->GetRow();
                ppc.srcCountry = g_CurrentCountry;
                ppc.dstColumn = pTarget->GetColumn();
                ppc.dstCountry = pTarget->GetCountry();
                ppc.srcColumn = pSelect->GetColumn();
                CreateThread(PlayChessFun, &ppc);
#endif
        }
    }
    else{
        if(index != -1){
#ifdef INTERNET_MODE
                //单机和局域网联机差不多。无非就是要接收和发送消息
                Chess target;
                target.SetCountry(MAX_COUNTRY_INDEX);
                target.SetPos(canplays[index].GetRow(), canplays[index].GetColumn());
                SendPlayChessMessage(g_Players[g_ClientIndex], pSelect, &target);
#else
                ppc.srcRow = pSelect->GetRow();
                ppc.srcCountry = g_CurrentCountry;
                ppc.srcColumn = pSelect->GetColumn();
                ppc.dstRow = canplays[index].GetRow();
                ppc.dstColumn = canplays[index].GetColumn();
                CreateThread(PlayChessFun, &ppc);
#endif
        }
    }
}
const Chess *g_Selected = nullptr;
void mousebutton(GLFWwindow *window,int button,int action,int mods){
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    const glm::vec2 mousePos = glm::vec2(xpos, ypos);
    if(action == GLFW_RELEASE && button == GLFW_MOUSE_BUTTON_LEFT){
        if(g_Selected){
            PreparePlayChess(g_Selected, mousePos);
            g_Selected = nullptr;
        }
        else{
            g_Selected = SelectChess(g_Player, mousePos);
        }
    }
}
#ifdef JOIN_AI
bool isEnableAi;
uint32_t g_BackupPlayer;
#endif
void keybutton(GLFWwindow *window,int key, int scancode, int action, int mods){
    if(action == GLFW_RELEASE){
#ifndef INTERNET_MODE
#ifdef JOIN_AI
        if(key == GLFW_KEY_SPACE){
            if(g_Player != INVALID_COUNTRY_INDEX)g_BackupPlayer = g_Player;
            //玩家不参与游戏
            g_Player = INVALID_COUNTRY_INDEX;
            if(!isEnableAi){
                g_AI.CreatePthread(AiPlayChess, nullptr);
                isEnableAi = true;
                printf("启用ai并且玩家无法参与\n");
            }
        }
        else if(key == GLFW_KEY_ENTER){
            if(g_Player == INVALID_COUNTRY_INDEX)g_Player = g_BackupPlayer;
            if(!isEnableAi){
                g_AI.CreatePthread(AiPlayChess, nullptr);
                isEnableAi = true;
                printf("启用ai并且玩家可以参与\n");
            }
        }
#endif
#endif
    }
}
void Setup(GLFWwindow *window){
    glfwSetKeyCallback(window, keybutton);
    glfwSetMouseButtonCallback(window, mousebutton);

    SetupDescriptorSetLayout(g_VulkanDevice.device);
    
    Pipeline::CreatePipelineCache(g_VulkanDevice.device, "FillPipelineCache", g_Fill.cache);
    Pipeline::CreatePipelineCache(g_VulkanDevice.device, "FontPipelineCache", g_Font.cache);
    Pipeline::CreatePipelineCache(g_VulkanDevice.device, "WireframePipelineCache", g_Wireframe.cache);

    CreatePipelineLayout(g_VulkanDevice.device, g_DescriptorSetLayout);
    
    CreateGraphicsPipeline(g_VulkanDevice.device, g_PipelineLayout);

    g_Chess.Setup(g_VulkanDevice.physicalDevice, g_VulkanDevice.device, g_DescriptorSetLayout, g_VulkanQueue.graphics, g_VulkanPool);
    g_VulkanChessboard.Setup(g_VulkanDevice.physicalDevice, g_VulkanDevice.device, g_DescriptorSetLayout, g_WindowWidth, g_VulkanQueue.graphics, g_VulkanPool);

    g_VulkanChessboard.UpdateUniform(g_VulkanDevice.device, g_WindowWidth);
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
#endif
    vkf::tool::AllocateCommandBuffers(g_VulkanDevice.device, g_VulkanPool.command, 1, &g_CommandBuffers);
#ifndef INTERNET_MODE
    g_Player = rand()%g_DefaultCountryCount;
    g_CurrentCountry = rand()%g_DefaultCountryCount;    
    g_Game.InitializeChess(g_Player);
    UpdateChessUniform(g_VulkanDevice.device, g_Game.GetChess());
#endif
}

void Cleanup(){
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
    Pipeline::DestroyPipelineCache(g_VulkanDevice.device, "FillPipelineCache", g_Fill.cache);
    Pipeline::DestroyPipelineCache(g_VulkanDevice.device, "FontPipelineCache", g_Font.cache);
    Pipeline::DestroyPipelineCache(g_VulkanDevice.device, "WireframePipelineCache", g_Wireframe.cache);

    vkDestroyDescriptorSetLayout(g_VulkanDevice.device, g_DescriptorSetLayout, VK_NULL_HANDLE);
    vkDestroyPipelineLayout(g_VulkanDevice.device, g_PipelineLayout, VK_NULL_HANDLE);

    DestroyGraphicsPipeline(g_VulkanDevice.device);

    g_VulkanChessboard.Cleanup(g_VulkanDevice.device);
    g_Chess.Cleanup(g_VulkanDevice.device);
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
        DestroyGraphicsPipeline(g_VulkanDevice.device);
        CreateGraphicsPipeline(g_VulkanDevice.device, g_PipelineLayout);
    }
}
void display(GLFWwindow* window){
    static size_t currentFrame;
    vkDeviceWaitIdle(g_VulkanDevice.device);
    RecordCommand(g_CommandBuffers, g_VulkanWindows.framebuffers[currentFrame]);
    if(VK_ERROR_OUT_OF_DATE_KHR == vkf::DrawFrame(g_VulkanDevice.device, currentFrame, g_CommandBuffers, g_VulkanWindows.swapchain, g_VulkanQueue, g_VulkanSynchronize, RecreateSwapchain, window)){
        //重建交换链后, 即使currentFrame不为0, 获取的交换链图片索引不一定和currentFrame相同
        currentFrame = 0;
        //那重建交换链后, 那重建后有没有可能得到的索引不是0?
    }
    else{
        currentFrame = (currentFrame + 1) % g_VulkanWindows.framebuffers.size();
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
    while (!glfwWindowShouldClose(window)) {
        display(window);

        glfwPollEvents();
    }
	glfwTerminate();

    Cleanup();
    CleanupVulkan();
    return 0;
}
