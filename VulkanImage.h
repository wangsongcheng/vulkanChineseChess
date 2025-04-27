#ifndef VULKAN_IMAGE_H
#define VULKAN_IMAGE_H
#include <cstring>
#include <assert.h>
#include "VulkanBuffer.h"
//在写这份代码的机器上，提示不支持"VK_FORMAT_R8G8B8_UNORM和VK_IMAGE_TYPE_2D以及一些其他格式一起用"
struct VulkanImage{
    VkExtent3D size;
    VkFormat format;
    uint32_t channels = 4;
    uint32_t mipLevels = 1;
    VkImage image = VK_NULL_HANDLE;
    VkImageView view = VK_NULL_HANDLE;
    VkDeviceMemory memory = VK_NULL_HANDLE;
    VkImageSubresourceRange subresourceRange = {};
    VkImageLayout layout = VK_IMAGE_LAYOUT_UNDEFINED;
    VkResult AllocateAndBindMemory(VulkanDevice device, VkMemoryPropertyFlags properties);

    VkResult CreateImage(VkDevice device, const VkExtent3D&extent, VkImageUsageFlags usage, VkFormat format, VkImageType type = VK_IMAGE_TYPE_2D);
    VkResult CreateImage(VkDevice device, const VkExtent3D&extent, VkImageUsageFlags usage, VkFormat format, uint32_t arrayLayer, VkImageType type = VK_IMAGE_TYPE_2D);
    VkResult CreateImage(VkDevice device, const VkExtent3D&extent, VkImageUsageFlags usage, VkFormat format, VkImageTiling tiling, uint32_t arrayLayer = 1, VkImageType type = VK_IMAGE_TYPE_2D);
    VkResult CreateImage(VkDevice device, const VkExtent3D&extent, VkImageUsageFlags usage, VkFormat format, VkImageType type, VkImageTiling tiling, uint32_t arrayLayer = 1, VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT, VkImageLayout initialLayout = VK_IMAGE_LAYOUT_UNDEFINED);

    VkResult CreateImageView(VkDevice device, VkImageViewType type = VK_IMAGE_VIEW_TYPE_2D, uint32_t arrayLayer = 1);
    VkResult CreateImageView(VkDevice device, VkImageAspectFlags aspectMask, VkImageViewType type = VK_IMAGE_VIEW_TYPE_2D, uint32_t arrayLayer = 1);

    void CopyImage(VkCommandBuffer command, VulkanImage&image);
    void CopyImage(VulkanDevice device, const void *datas, VulkanPool pool, VkQueue graphics);
    void CopyImage(VulkanDevice device, const void*const*datas, uint32_t imageCount, VulkanPool pool, VkQueue graphics);

    void Destroy(VkDevice device);

    void SetImageLayout(VkCommandBuffer cmd, VkImageLayout newLayout, VkPipelineStageFlags srcStage, VkPipelineStageFlags dstStage, VkAccessFlags srcAccessMask, VkAccessFlags dstAccessMask);
    // //通过map更新图片数据
    // void UpdateImage(VkDevice device, const void *datas, uint32_t imageCount = 1);
private:
    VkCommandBuffer BeginSingleTimeCommands(VkDevice device, VulkanPool pool);

    void CopyImage(VkCommandBuffer command, const VulkanBuffer &dataBuffer);
    void CopyImage(VkCommandBuffer command, const VulkanBuffer&dataBuffer, uint32_t imageCount);

    void EndSingleTimeCommands(VkDevice device, VkCommandPool pool, VkQueue graphics, VkCommandBuffer command);
};
//可以分出灰度图片、纹理图片、字体图片以及各种数组等
struct VulkanGrayImage:VulkanImage{
    void CreateImage(VulkanDevice device, uint32_t width, uint32_t height, uint32_t arrayLayers = 1);
    void CreateImage(VulkanDevice device, const void *data, uint32_t width, uint32_t height, VkQueue graphics, VulkanPool pool);
    void CreateImageArray(VulkanDevice device, const void *const *datas, uint32_t imageCount, uint32_t width, uint32_t height, VkQueue graphics, VulkanPool pool);
};
struct VulkanFontImage:VulkanImage{
    void CreateImage(VulkanDevice device, uint32_t width, uint32_t height, uint32_t arrayLayers = 1);
    void CreateImage(VulkanDevice device, const void *data, uint32_t width, uint32_t height, uint32_t channels, VkQueue graphics, VulkanPool pool);
    void CreateImageArray(VulkanDevice device, const void *const *datas, uint32_t imageCount, uint32_t width, uint32_t height, uint32_t channels, VkQueue graphics, VulkanPool pool);
};
struct VulkanTextureImage:VulkanImage{
    void CreateImage(VulkanDevice device, uint32_t width, uint32_t height, uint32_t arrayLayers = 1);
    void CreateImage(VulkanDevice device, const void *data, uint32_t width, uint32_t height, VkQueue graphics, VulkanPool pool);
    void CreateImageArray(VulkanDevice device, const void *const *datas, uint32_t imageCount, uint32_t width, uint32_t height, VkQueue graphics, VulkanPool pool);
};
struct VulkanCubeImage:VulkanImage{
    void CreateImage(VulkanDevice device, uint32_t width);
    void CreateImage(VulkanDevice device, const void *const *datas, uint32_t width, VkQueue graphics, VulkanPool pool);
    //立方体贴图本身就是图片数组, 那么立方体纹理数组要怎么写?arraylayer = imagecount*6?
    // void CreateImageArray(VulkanDevice device, const void *const *datas, uint32_t imageCount, uint32_t width, VkQueue graphics, VulkanPool pool);
};
#endif