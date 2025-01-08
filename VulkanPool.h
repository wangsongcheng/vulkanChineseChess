#ifndef VULKAN_POOL_H
#define VULKAN_POOL_H
#include "VulkanDevice.h"
struct VulkanPool{
    VkCommandPool command;
    VkDescriptorPool descriptor;
    void Cleanup(VkDevice device);
    void CreatePool(VulkanDevice device, uint32_t descriptorCount, VkCommandPoolCreateFlags flags = 0);

    VkResult AllocateDescriptorSets(VkDevice device, const VkDescriptorSetLayout *setlayout, uint32_t count, VkDescriptorSet *pDescriptorSet);
    VkResult AllocateCommandBuffers(VkDevice device, uint32_t count, VkCommandBuffer *pCommandBuffers, VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);
};
#endif