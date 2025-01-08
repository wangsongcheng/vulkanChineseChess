#ifndef VULKAN_WINDOW_H
#define VULKAN_WINDOW_H
#include "VulkanImage.h"
#include "VulkanDevice.h"
#define SWAPCHAIN_FORMAT VK_FORMAT_B8G8R8A8_UNORM
#define SWAPCHAIN_COLOR_SPACE VK_COLOR_SPACE_SRGB_NONLINEAR_KHR
struct VulkanSwapchain{
    VkFormat format;
    VkExtent2D extent;
    VkSwapchainKHR swapchain;
    std::vector<VulkanImage>images;
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
    void CreateFrameBuffer(VulkanDevice device, bool createDepthImage);
    VkResult CreateRenderPass(VkDevice device, bool useDepthImage = true);
private:
    void CreateDepthImage(VulkanDevice device, const VkExtent2D&swapchainExtent, VulkanImage&image);
};
#endif