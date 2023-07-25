#ifndef VULKAN_FRAME_H
#define VULKAN_FRAME_H
#include <string>
#include <vector>
#include <vulkan/vulkan.h>
struct VulkanImage{
    VkImage image;
    VkExtent3D size;
    std::string name;
    VkImageView view;
    VkDeviceMemory memory;
    //使用这些函数前需要先给size赋值
    VkResult CreateImage(VkDevice device, VkImageUsageFlags usage, VkFormat format, VkImageType type = VK_IMAGE_TYPE_2D);
    VkResult CreateImage(VkDevice device, VkImageUsageFlags usage, VkFormat format, uint32_t arrayLayer, VkImageType type = VK_IMAGE_TYPE_2D);
    VkResult CreateImage(VkDevice device, VkImageUsageFlags usage, VkFormat format, VkImageTiling tiling, uint32_t arrayLayer = 1, VkImageType type = VK_IMAGE_TYPE_2D);
    VkResult CreateImage(VkDevice device, VkImageUsageFlags usage, VkFormat format, VkImageType type, VkImageTiling tiling, uint32_t arrayLayer = 1, VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT, VkImageLayout initialLayout = VK_IMAGE_LAYOUT_UNDEFINED);
    
    VkResult CreateImage(VkDevice device, const VkExtent2D&extent, VkImageUsageFlags usage, VkFormat format, VkImageType type = VK_IMAGE_TYPE_2D);
    VkResult CreateImage(VkDevice device, const VkExtent3D&extent, VkImageUsageFlags usage, VkFormat format, VkImageType type = VK_IMAGE_TYPE_2D);
    VkResult CreateImage(VkDevice device, const VkExtent3D&extent, VkImageUsageFlags usage, VkFormat format, uint32_t arrayLayer, VkImageType type = VK_IMAGE_TYPE_2D);
    VkResult CreateImage(VkDevice device, const VkExtent3D&extent, VkImageUsageFlags usage, VkFormat format, VkImageTiling tiling, uint32_t arrayLayer = 1, VkImageType type = VK_IMAGE_TYPE_2D);
    VkResult CreateImage(VkDevice device, const VkExtent3D&extent, VkImageUsageFlags usage, VkFormat format, VkImageType type, VkImageTiling tiling, uint32_t arrayLayer = 1, VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT, VkImageLayout initialLayout = VK_IMAGE_LAYOUT_UNDEFINED);

    VkResult CreateImageView(VkDevice device, VkFormat format, VkImageViewType type = VK_IMAGE_VIEW_TYPE_2D, uint32_t arrayLayer = 1);
    VkResult CreateImageView(VkDevice device, VkFormat format, VkImageAspectFlags aspectMask, VkImageViewType type = VK_IMAGE_VIEW_TYPE_2D, uint32_t arrayLayer = 1);

    void AllocateAndBindMemory(VkDevice device, VkMemoryPropertyFlags properties);

    void Destroy(VkDevice device);
};

struct VulkanBuffer{
    VkBuffer buffer;
    VkDeviceSize size;
    VkDeviceMemory memory;
    VkResult CreateBuffer(VkDevice device, VkDeviceSize size, VkBufferUsageFlags usage);
    void AllocateAndBindMemory(VkDevice device, VkMemoryPropertyFlags properties);
    void UpdateData(VkDevice device, const void * pData, VkDeviceSize offset = 0);
    void UpdateData(VkDevice device, VkDeviceSize size, const void * pData, VkDeviceSize offset = 0);

    void Destroy(VkDevice device);
};
struct VulkanDevice{
    VkDevice device;
    VkInstance instance;
    VkPhysicalDevice physicalDevice;
};
struct VulkanWindows{
    VkSurfaceKHR surface;
    VulkanImage depthImage;
    VkRenderPass renderpass;
    VkSwapchainKHR swapchain;
    VkExtent2D swapchainExtent;
    // std::vector<VkImage>swapchainImages;
    std::vector<VkFramebuffer>framebuffers;
    std::vector<VkImageView>swapchainImageViews;
};
struct VulkanPool{
    VkCommandPool commandPool;
    VkDescriptorPool descriptorPool;
};
struct VulkanQueue{
    VkQueue present;
    VkQueue graphics;
};
struct VulkanSynchronize{
    std::vector<VkFence>fences;
    std::vector<VkSemaphore>imageAcquired;
    std::vector<VkSemaphore>renderComplete;
};
namespace vkf{
    static VkPhysicalDeviceMemoryProperties gMemoryProperties;

    VkPhysicalDevice GetPhysicalDevices(VkInstance instance, VkPhysicalDeviceType deviceType = VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU);
    VkResult CreateDevice(VkPhysicalDevice physicalDevice, const std::vector<const char*>& deviceExtensions, VkSurfaceKHR surface, VkDevice&device);
    VkResult CreateDebugUtilsMessenger(VkInstance instance, VkDebugUtilsMessengerEXT&messenger, PFN_vkDebugUtilsMessengerCallbackEXT debugUtilsMessenger);
    void DestoryDebugUtilsMessenger(VkInstance instance, VkDebugUtilsMessengerEXT&messenger);
    VkResult CreateInstance(const std::vector<const char*>&instanceExtensions, VkInstance&instance, bool enableValidation = true, VkApplicationInfo *pApplicationInfo = nullptr);

    VkResult CreateRenderPassForSwapchain(VkDevice device, VkRenderPass&renderpass, bool useDepthImage = false);
    void CreateSemaphoreAndFenceForSwapchain(VkDevice device, uint32_t swapchainImageCount, VulkanSynchronize&vulkanWindows);
    VkResult CreateSwapchain(VkPhysicalDevice physicalDevice, VkDevice device, VkSurfaceKHR surface, VkSwapchainKHR&swapchain);
    void CreateFrameBufferForSwapchain(VkDevice device, const VkExtent2D&swapchainExtent, VulkanWindows&vulkanWindows, VkCommandPool pool, VkQueue graphics, bool createDepthImage = false);
    VkResult CreateFrameBuffer(VkDevice device, VkRenderPass renderPass, const VkExtent2D&extent, const std::vector<VkImageView>&attachments, VkFramebuffer&frameBuffer, uint32_t layers = 1);
    VkResult CreateRenderPass(VkDevice device, const std::vector<VkSubpassDescription>&subpassDescription, const std::vector<VkSubpassDependency>&subpassDependency, const std::vector<VkAttachmentDescription>&attachmentDescriptions, VkRenderPass&renderpass);

    VkResult CreateCommandPool(VkPhysicalDevice physicalDevice, VkDevice device, VkCommandPool&pool, VkCommandPoolCreateFlags flags = 0);
    VkResult CreateDescriptorPool(VkDevice device, uint32_t descriptorCount, VkDescriptorPool&pool);

    void CreateDepthImage(VkDevice device, const VkExtent2D&swapchainExtent, VulkanImage&image);

    void CreateTextureImage(VkDevice device, const void *datas, uint32_t width, uint32_t height, VulkanImage&image, VkCommandPool pool, VkQueue graphics);
    void CreateTextureImage(VkDevice device, const VulkanBuffer&dataBuffer, uint32_t width, uint32_t height, VulkanImage&image, VkCommandPool pool, VkQueue graphics);
    
    void CreateImageArray(VkDevice device, const void * const * datas, uint32_t imageCount, uint32_t width, uint32_t height, VulkanImage&image, VkCommandPool pool, VkQueue graphics);
    void CreateImageArray(VkDevice device, const VulkanBuffer&dataBuffer, uint32_t imageCount, uint32_t width, uint32_t height, VulkanImage&image, VkCommandPool pool, VkQueue graphics);

    VkResult CreateTextureSampler(VkDevice device, VkSampler&sampler);

    VkResult RenderFrame(VkDevice device, const VkCommandBuffer&commandbuffers, VkQueue graphics, const VkSemaphore&imageAcquired, const VkSemaphore&renderComplete, const VkFence&fence = VK_NULL_HANDLE);
    uint32_t PrepareFrame(VkDevice device, VkSwapchainKHR swapchain, const VkSemaphore&imageAcquired, void(*recreateSwapchain)(void* userData) = nullptr, void* userData = nullptr, const VkFence&fence = VK_NULL_HANDLE);
    VkResult SubmitFrame(VkDevice device, uint32_t imageIndex, VkSwapchainKHR swapchain, const VkQueue present, const VkSemaphore&renderComplete, void(*recreateSwapchain)(void* userData) = nullptr, void* userData = nullptr);
    void DrawFrame(VkDevice device, uint32_t currentFrame, const VkCommandBuffer& commandbuffers, VkSwapchainKHR swapchain, const VulkanQueue&vulkanQueue, const VulkanSynchronize&vulkanSynchronize, void(*recreateSwapchain)(void* userData) = nullptr, void* userData = nullptr);

    namespace tool{
        uint32_t GetFileSize(FILE *fp);
        uint32_t GetFileContent(const std::string&file, std::vector<uint32_t>&data);
        bool WriteFileContent(const std::string&file, const void *data, uint32_t size);

        void GetGraphicAndPresentQueueFamiliesIndex(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, int queueFamilies[2]);
        uint32_t findMemoryTypeIndex(uint32_t typeFilter, VkMemoryPropertyFlags properties);

        void AllocateMemory(VkDevice device, VkDeviceSize size, uint32_t typeFilter, VkMemoryPropertyFlags properties, VkDeviceMemory&memory);
        VkResult AllocateCommandBuffers(VkDevice device, VkCommandPool pool, uint32_t count, VkCommandBuffer *pCommandBuffers, VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);

        void CopyBuffer(VkDevice device, VkDeviceSize size, const void *pData, VkQueue graphics, VkCommandPool pool, VulkanBuffer&buffer);
        
        VkResult BeginSingleTimeCommands(VkDevice device, VkCommandPool commandPool, VkCommandBuffer&commandBuffer);
        void EndSingleTimeCommands(VkDevice device, VkCommandPool commandPool, VkQueue graphics, VkCommandBuffer commandBuffer);

        void SetImageLayout(VkCommandBuffer cmdbuffer, VkImage image, VkImageAspectFlags aspectMask, VkImageLayout oldImageLayout, VkImageLayout newImageLayout, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask);
        void SetImageLayout(VkCommandBuffer cmdbuffer, VkImage image, VkImageLayout oldImageLayout, VkImageLayout newImageLayout, VkImageSubresourceRange subresourceRange, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask);
        void SetImageLayout(VkCommandBuffer cmdbuffer, VkImage image, VkImageAspectFlags aspectMask, VkImageLayout oldImageLayout, VkImageLayout newImageLayout, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, uint32_t imageCount);
    }
}
// #include "vulkanDevice.h"
// class vulkanFrame:vulkanDevice{
//     VkQueue mGraphics, mPresent;
// public:
//     vulkanFrame(/* args */);
//     ~vulkanFrame();
//     void CreateVulkan(const std::vector<const char*>&instanceExtensions, const std::vector<const char*>&deviceExtensions, void(*createSurfaceFun)(VkInstance, VkSurfaceKHR&, void*), void *userData = nullptr);
// };
#endif