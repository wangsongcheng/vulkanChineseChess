#include "VulkanPool.h"

void VulkanPool::Cleanup(VkDevice device){
    vkDestroyCommandPool(device, command, nullptr);
    vkDestroyDescriptorPool(device, descriptor, nullptr);
}

void VulkanPool::CreatePool(VulkanDevice device, uint32_t descriptorCount, VkCommandPoolCreateFlags flags){
    std::vector<VkDescriptorPoolSize> descriptorPoolSize = {
        { VK_DESCRIPTOR_TYPE_SAMPLER, descriptorCount },
        { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, descriptorCount },
        { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, descriptorCount },
        { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, descriptorCount },
        { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, descriptorCount },
        { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, descriptorCount },
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, descriptorCount },
        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, descriptorCount },
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, descriptorCount },
        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, descriptorCount },
        { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, descriptorCount}
	};
	VkDescriptorPoolCreateInfo descriptorPoolInfo;
	descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	descriptorPoolInfo.flags = 0;
	descriptorPoolInfo.pNext = nullptr;
	descriptorPoolInfo.maxSets = descriptorCount;
	descriptorPoolInfo.poolSizeCount = descriptorPoolSize.size();
	descriptorPoolInfo.pPoolSizes = descriptorPoolSize.data();
	vkCreateDescriptorPool(device.device, &descriptorPoolInfo, nullptr, &descriptor);
    uint32_t graphicsFamily;
    device.GetGraphicAndPresentQueueFamiliesIndex(VK_NULL_HANDLE, &graphicsFamily);
	VkCommandPoolCreateInfo commandPoolInfo = {};
	commandPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	commandPoolInfo.flags = flags;
	commandPoolInfo.queueFamilyIndex = graphicsFamily;
	commandPoolInfo.flags = flags | VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
	commandPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	vkCreateCommandPool(device.device, &commandPoolInfo, nullptr, &command);
}

VkResult VulkanPool::AllocateDescriptorSets(VkDevice device, const VkDescriptorSetLayout *setlayout, uint32_t count, VkDescriptorSet *pDescriptorSet){
    VkDescriptorSetAllocateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    info.pSetLayouts = setlayout;
    info.descriptorPool = descriptor;
    info.descriptorSetCount = count;
    return vkAllocateDescriptorSets(device, &info, pDescriptorSet);
}

VkResult VulkanPool::AllocateCommandBuffers(VkDevice device, uint32_t count, VkCommandBuffer *pCommandBuffers, VkCommandBufferLevel level){
    VkCommandBufferAllocateInfo allocateInfo = {};
    allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocateInfo.level = level;
    allocateInfo.commandPool = command;
    allocateInfo.commandBufferCount = count;
    return vkAllocateCommandBuffers(device, &allocateInfo, pCommandBuffers);
}
