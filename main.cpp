#include <stdio.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "Chessboard.h"
#include "vulkanFrame.h"
VulkanPool g_VulkanPool;
VulkanQueue g_VulkanQueue;
VulkanDevice g_VulkanDevice;
VulkanWindows g_VulkanWindows;
VulkanSynchronize g_VulkanSynchronize;
VkDebugUtilsMessengerEXT g_VulkanMessenger;
//                              163 * 2 + 400 + 11
const uint32_t g_WindowWidth = CHESSBOARD_GRID_SIZE_BIG * 2 + 10 * CHESSBOARD_GRID_SIZE + 11 * CHESSBOARD_LINE_WIDTH, g_WindowHeight = g_WindowWidth;

std::vector<VkCommandBuffer>g_CommandBuffers;

Chessboard g_Chessboard;
glm::vec3 g_CurrentCountry;

VkSampler g_TextureSampler;

// void createSurface(VkInstance instance, VkSurfaceKHR&surface, void* userData){
//     glfwCreateWindowSurface(instance, (GLFWwindow *)userData, nullptr, &surface);
// }
glm::vec3 NextCountry(){
    const glm::vec3 color[] = { WEI_CHESS_COUNTRY_COLOR, SHU_CHESS_COUNTRY_COLOR, WU_CHESS_COUNTRY_COLOR };
    static uint32_t index = 0;
    const glm::vec3 next = color[index];
    index = (index + 1) % (sizeof(color) / sizeof(glm::vec3));
    return next;
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
#ifdef DRAW_CUBE
    vkf::CreateRenderPassForSwapchain(g_VulkanDevice.device, g_VulkanWindows.renderpass, true);
#else
    vkf::CreateRenderPassForSwapchain(g_VulkanDevice.device, g_VulkanWindows.renderpass);
#endif
    vkf::CreateCommandPool(g_VulkanDevice.physicalDevice, g_VulkanDevice.device, g_VulkanPool.commandPool, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
#ifdef DRAW_CUBE
    vkf::CreateFrameBufferForSwapchain(g_VulkanDevice.device, { g_WindowWidth, g_WindowHeight }, g_VulkanWindows, g_VulkanPool.commandPool, g_VulkanQueue.graphics, true);
#else
    vkf::CreateFrameBufferForSwapchain(g_VulkanDevice.device, { g_WindowWidth, g_WindowHeight }, g_VulkanWindows, g_VulkanPool.commandPool, g_VulkanQueue.graphics);
#endif
    vkf::CreateSemaphoreAndFenceForSwapchain(g_VulkanDevice.device, 3, g_VulkanSynchronize);
    vkf::CreateDescriptorPool(g_VulkanDevice.device, 147, g_VulkanPool.descriptorPool);
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
void recodeCommand(uint32_t currentFrame){
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

    g_Chessboard.RecodeCommand(g_CommandBuffers[currentFrame], g_CurrentCountry);

    vkCmdEndRenderPass(g_CommandBuffers[currentFrame]);
    vkEndCommandBuffer(g_CommandBuffers[currentFrame]);
}
void mousebutton(GLFWwindow *windows, int button, int action, int mods){
    double xpos, ypos;
    glfwGetCursorPos(windows, &xpos, &ypos);
    if(action == GLFW_RELEASE){
        if(button == GLFW_MOUSE_BUTTON_LEFT){
            const Chess *chess = g_Chessboard.GetChess(glm::vec2(xpos, ypos));
            if(!g_Chessboard.IsSelected() || (chess && g_Chessboard.IsSelfChess(chess->GetCountryColor()))){
                if(chess){
                    if(g_CurrentCountry == chess->GetCountryColor()){
                        g_Chessboard.Select(g_VulkanDevice.device, glm::vec2(xpos, ypos));
                    }
                }
            }
            else{
                bool bNext;
                uint32_t country;
                if(g_Chessboard.Play(g_VulkanDevice.device, glm::vec2(xpos, ypos), country, bNext)){
                    vkDeviceWaitIdle(g_VulkanDevice.device);
                    g_Chessboard.DestroyChess(g_VulkanDevice.device, country);
                }
                if(bNext){
                    g_CurrentCountry = NextCountry();
                }
                vkDeviceWaitIdle(g_VulkanDevice.device);
                for (size_t i = 0; i < g_VulkanWindows.framebuffers.size(); ++i){
                    recodeCommand(i);
                }
            }
        }
    }
}
void setup(GLFWwindow *windows){
    glfwSetMouseButtonCallback(windows, mousebutton);
    vkf::CreateTextureSampler(g_VulkanDevice.device, g_TextureSampler);

    g_Chessboard.CreatePipeline(g_VulkanDevice.device, g_VulkanWindows.renderpass, g_WindowWidth);
    g_Chessboard.CreateVulkanResource(g_VulkanDevice.physicalDevice, g_VulkanDevice.device, g_WindowWidth, g_VulkanQueue.graphics, g_VulkanPool);
    g_Chessboard.InitChessboard(g_VulkanDevice.device, g_WindowWidth, g_VulkanWindows.renderpass, g_VulkanPool, g_VulkanQueue.graphics, g_TextureSampler);

    g_Chessboard.UpdateSet(g_VulkanDevice.device);

    g_CurrentCountry = NextCountry();

    g_CommandBuffers.resize(g_VulkanWindows.framebuffers.size());
    vkf::tool::AllocateCommandBuffers(g_VulkanDevice.device, g_VulkanPool.commandPool, g_CommandBuffers.size(), g_CommandBuffers.data());
    for (size_t i = 0; i < g_VulkanWindows.framebuffers.size(); ++i){
        recodeCommand(i);
    }
}
void cleanup(){
    vkDeviceWaitIdle(g_VulkanDevice.device);

    g_Chessboard.Cleanup(g_VulkanDevice.device);
    vkDestroySampler(g_VulkanDevice.device, g_TextureSampler, nullptr);
}
void display(GLFWwindow* window){
    static size_t currentFrame;
    vkDeviceWaitIdle(g_VulkanDevice.device);
    // recodeCommand(currentFrame);
    vkf::DrawFrame(g_VulkanDevice.device, currentFrame, g_CommandBuffers[currentFrame], g_VulkanWindows.swapchain, g_VulkanQueue, g_VulkanSynchronize);
    currentFrame = (currentFrame + 1) % g_VulkanWindows.framebuffers.size();
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
