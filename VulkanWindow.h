#ifndef VULKAN_WINDOW_H
#define VULKAN_WINDOW_H
// #include <algorithm>
#include "VulkanImage.h"
#include "VulkanDevice.h"
#define PRESENT_QUEUE_INDEX 1
#define GRAPHICS_QUEUE_INDEX 0
// #define SWAPCHAIN_FORMAT VK_FORMAT_B8G8R8A8_UNORM
// #define SWAPCHAIN_COLOR_SPACE VK_COLOR_SPACE_SRGB_NONLINEAR_KHR
struct VulkanSwapchain{
    VkExtent2D extent;
    VkSwapchainKHR swapchain;
    std::vector<VulkanImage>images;
    VkSurfaceFormatKHR surfaceFormat;
    VkResult CreateSwapchain(VulkanDevice device, VkSurfaceKHR surface);

    void Cleanup(VkDevice device);
};
struct VulkanWindow{
    VkSurfaceKHR surface;
    VulkanImage depthImage;
    VkRenderPass renderPass;
    VulkanSwapchain swapchain;
    std::vector<VkFramebuffer>framebuffers;
    void Cleanup(VulkanDevice device);
    void CleanupSwapchain(VkDevice device);
    VkResult CreateRenderPass(VkDevice device, bool useDepthImage = true);
    void CreateFrameBuffer(VulkanDevice device, bool createDepthImage = true);
private:
    void CreateDepthImage(VulkanDevice device, const VkExtent3D&swapchainExtent, VulkanImage&image);
};
#endif
