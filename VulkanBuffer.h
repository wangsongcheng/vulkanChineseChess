#ifndef VULKAN_BUFFER_H
#define VULKAN_BUFFER_H
#include "VulkanPool.h"
#include "VulkanDevice.h"
struct VulkanBuffer{
    VkDeviceSize size;
    VkBuffer buffer = VK_NULL_HANDLE;
    VkDeviceMemory memory = VK_NULL_HANDLE;
    VkResult CreateBuffer(VulkanDevice device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);

    void UpdateData(VulkanDevice device, const void *pData, VkQueue graphics, VulkanPool pool);

    void UpdateData(VkDevice device, const void * pData, VkDeviceSize offset = 0);
    void UpdateData(VkDevice device, VkDeviceSize size, const void * pData, VkDeviceSize offset = 0);

    void Destroy(VkDevice device);
};
#endif