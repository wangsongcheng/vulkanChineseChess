#ifndef VULKAN_IMAGE_H
#define VULKAN_IMAGE_H
#include "VulkanPool.h"
#include "VulkanBuffer.h"
#include "VulkanDevice.h"
struct VulkanImage{
    VkExtent3D size;
    // std::string name;
    VkImage image = VK_NULL_HANDLE;
    VkImageView view = VK_NULL_HANDLE;
    VkDeviceMemory memory = VK_NULL_HANDLE;
    VkResult CreateImage(VkDevice device, const VkExtent2D&extent, VkImageUsageFlags usage, VkFormat format, VkImageType type = VK_IMAGE_TYPE_2D);
    VkResult CreateImage(VkDevice device, const VkExtent3D&extent, VkImageUsageFlags usage, VkFormat format, VkImageType type = VK_IMAGE_TYPE_2D);
    VkResult CreateImage(VkDevice device, const VkExtent3D&extent, VkImageUsageFlags usage, VkFormat format, uint32_t arrayLayer, VkImageType type = VK_IMAGE_TYPE_2D);
    VkResult CreateImage(VkDevice device, const VkExtent3D&extent, VkImageUsageFlags usage, VkFormat format, VkImageTiling tiling, uint32_t arrayLayer = 1, VkImageType type = VK_IMAGE_TYPE_2D);
    VkResult CreateImage(VkDevice device, const VkExtent3D&extent, VkImageUsageFlags usage, VkFormat format, VkImageType type, VkImageTiling tiling, uint32_t arrayLayer = 1, VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT, VkImageLayout initialLayout = VK_IMAGE_LAYOUT_UNDEFINED);

    VkResult CreateImageView(VkDevice device, VkFormat format, VkImageViewType type = VK_IMAGE_VIEW_TYPE_2D, uint32_t arrayLayer = 1);
    VkResult CreateImageView(VkDevice device, VkFormat format, VkImageAspectFlags aspectMask, VkImageViewType type = VK_IMAGE_VIEW_TYPE_2D, uint32_t arrayLayer = 1);

    void CopyImage(VulkanDevice device, const void *datas, uint32_t width, uint32_t height, VulkanPool pool, VkQueue graphics);
    void CopyImage(VkDevice device, const VulkanBuffer &dataBuffer, uint32_t width, uint32_t height, VulkanPool pool, VkQueue graphics);
    void CopyImage(VulkanDevice device, const void*const*datas, uint32_t imageCount, uint32_t width, uint32_t height, VulkanPool pool, VkQueue graphics);
    void CopyImage(VkDevice device, const VulkanBuffer&dataBuffer, uint32_t imageCount, uint32_t width, uint32_t height, VulkanPool pool, VkQueue graphics);

    VkResult AllocateAndBindMemory(VulkanDevice device, VkMemoryPropertyFlags properties);

    void Destroy(VkDevice device);
};
#endif