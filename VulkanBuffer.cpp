#include <cstring>
#include "VulkanBuffer.h"
VkResult VulkanBuffer::CreateBuffer(VulkanDevice device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties){
    this->size = size;
    VkBufferCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    info.size = size;
    info.usage = usage;
    info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    VkResult result = vkCreateBuffer(device.device, &info, nullptr, &buffer);
    // VK_MEMORY_PROPERTY_HOST_COHERENT_BIT可以保证数据写入后立即更新到buffer。但会导致性能下降
    // 这种方式只使用于VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT应该用vkCmdCopyBuffer
	VkMemoryRequirements memoryRequirements;
	vkGetBufferMemoryRequirements(device.device, buffer, &memoryRequirements);
    device.AllocateMemory(memoryRequirements.size, memoryRequirements.memoryTypeBits, properties, memory);
	vkBindBufferMemory(device.device, buffer, memory, 0);
    return result;
}

void VulkanBuffer::UpdateData(VulkanDevice device, const void *pData, VkQueue graphics, VulkanPool pool){
    VulkanBuffer temporary;
    temporary.CreateBuffer(device, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    temporary.UpdateData(device.device, size, pData);
    VkCommandBuffer command;
    VkBufferCopy regions = {};
    regions.size = size;
    pool.AllocateCommandBuffers(device.device, 1, &command);
	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	vkBeginCommandBuffer(command, &beginInfo);

    vkCmdCopyBuffer(command, temporary.buffer, buffer, 1, &regions);

    // Create fence to ensure that the command buffer has finished executing
    VkFence fence;
    VkFenceCreateInfo fenceInfo = {};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    vkCreateFence(device.device, &fenceInfo, nullptr, &fence);

	vkEndCommandBuffer(command);
    
	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &command;

	vkQueueSubmit(graphics, 1, &submitInfo, fence);
	vkQueueWaitIdle(graphics);
    
    // Wait for the fence to signal that command buffer has finished executing
    vkWaitForFences(device.device, 1, &fence, VK_TRUE, UINT64_MAX);
    vkDestroyFence(device.device, fence, nullptr);

	vkFreeCommandBuffers(device.device, pool.command, 1, &command);

    temporary.Destroy(device.device);
}

void VulkanBuffer::UpdateData(VkDevice device, const void *pData, VkDeviceSize offset){
    UpdateData(device, size, pData, offset);
}

void VulkanBuffer::UpdateData(VkDevice device, VkDeviceSize size, const void *pData, VkDeviceSize offset){
	void* data;
	vkMapMemory(device, memory, offset, size, 0, &data);
	memcpy(data, pData, size);
	vkUnmapMemory(device, memory);
}

void VulkanBuffer::Destroy(VkDevice device){
    if(buffer != VK_NULL_HANDLE)vkDestroyBuffer(device, buffer, nullptr);
    if(memory != VK_NULL_HANDLE)vkFreeMemory(device, memory, nullptr);
    buffer = VK_NULL_HANDLE;
    memory = VK_NULL_HANDLE;
}
