#ifndef VULKAN_FRAME_H
#define VULKAN_FRAME_H
#include <assert.h>
#include "VulkanPool.h"
#include "VulkanBuffer.h"
#include "VulkanImage.h"
#include "VulkanDevice.h"
#define VK_CHECK(x)                                                 \
do{                                                               \
        VkResult err = x;                                           \
        if (err != VK_SUCCESS){                                       \
                printf("vulkan error:in function %s line %d information %s\n", __FUNCTION__, __LINE__, vulkanFrame::cvmx_chip_type_to_string(err)); \
                assert(0);           \
        }                                                    \
} while (0)
struct VulkanSynchronize{
    std::vector<VkFence>fences;
    std::vector<VkSemaphore>imageAcquired;
    std::vector<VkSemaphore>renderComplete;
    void Cleanup(VkDevice device);
    void CreateSynchronize(VkDevice device, uint32_t swapchainImageCount);
};
struct BaseGraphic{
    VulkanBuffer index;
    VulkanBuffer vertex;
    uint32_t indexCount;
    uint32_t vertexCount;
    void Destroy(VkDevice device){
        vertex.Destroy(device);
        index.Destroy(device);
    }
    void Bind(VkCommandBuffer command, VkIndexType indexType = VK_INDEX_TYPE_UINT16)const{
        VkDeviceSize offset = 0;
        if(vertex.buffer != VK_NULL_HANDLE)vkCmdBindVertexBuffers(command, 0, 1, &vertex.buffer, &offset);
        if(index.buffer != VK_NULL_HANDLE)vkCmdBindIndexBuffer(command, index.buffer, offset, indexType);
    }
    void Draw(VkCommandBuffer command, uint32_t vertexOffset = 0, uint32_t firstIndex = 0)const{
        if(index.buffer != VK_NULL_HANDLE){
            vkCmdDrawIndexed(command, indexCount, 1, firstIndex, vertexOffset, 0);
        }
        else if(vertex.buffer != VK_NULL_HANDLE){
            vkCmdDraw(command, vertexCount, 1, vertexOffset, 0);
        }
    }
};
namespace vulkanFrame{
    const char *cvmx_chip_type_to_string(VkResult type);
    void CopyImage(VkDevice device, VulkanImage&src, VulkanImage&dst, VulkanPool pool, VkQueue graphics);
    // void CopyImage(VulkanDevice device, const void *datas, uint32_t imageCount, uint32_t width, uint32_t height, VulkanImage&image, VkCommandPool pool, VkQueue graphics);

    VkResult CreateDescriptorSetLayout(VkDevice device, const VkDescriptorSetLayoutBinding *pBinding, uint32_t count, VkDescriptorSetLayout *pSetLayout);

    void BeginRenderPassGeneral(VkCommandBuffer command, VkFramebuffer frame, VkRenderPass renderpass, uint32_t windowWidth, uint32_t windowHeight);
    void BeginRenderPass(VkCommandBuffer command, VkFramebuffer frame, VkRenderPass renderpass, uint32_t windowWidth, uint32_t windowHeight, uint32_t clearValueCount, VkClearValue *pClearValues, int32_t renderAreaWidthOffset = 0, int32_t renderAreaHeightOffset = 0);

    VkResult BeginCommands(VkCommandBuffer command, VkCommandBufferUsageFlags flags);
    VkResult BeginSingleTimeCommands(VkDevice device, VulkanPool pool, VkCommandBuffer&commandBuffer);
    void EndSingleTimeCommands(VkDevice device, VkCommandPool pool, VkQueue graphics, VkCommandBuffer commandBuffer);

    void SetImageLayout(VkCommandBuffer cmdbuffer, VkImage image, VkImageAspectFlags aspectMask, VkImageLayout oldImageLayout, VkImageLayout newImageLayout, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask);
    void SetImageLayout(VkCommandBuffer cmdbuffer, VkImage image, VkImageLayout oldImageLayout, VkImageLayout newImageLayout, VkImageSubresourceRange subresourceRange, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask);
    void SetImageLayout(VkCommandBuffer cmdbuffer, VkImage image, VkImageAspectFlags aspectMask, VkImageLayout oldImageLayout, VkImageLayout newImageLayout, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, uint32_t imageCount);
    void InsertImageMemoryBarrier(VkCommandBuffer cmdbuffer, VkImage image, VkAccessFlags srcAccessMask, VkAccessFlags dstAccessMask, VkImageLayout oldImageLayout, VkImageLayout newImageLayout, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, VkImageSubresourceRange subresourceRange);

    VkResult CreateTextureSampler(VkDevice device, VkSampler &sampler);
    //setlayoutBindings可以用偏移
    void UpdateDescriptorSets(VkDevice device, const VkDescriptorSetLayoutBinding *pBindings, uint32_t count, const std::vector<VulkanBuffer>&descriptorBuffer, const std::vector<VulkanImage>&descriptorImage, VkDescriptorSet&destSet, const VkSampler&textureSampler = VK_NULL_HANDLE);

    void CreateFontImage(VulkanDevice device, const void *datas, uint32_t width, uint32_t height, VulkanImage&image, VulkanPool pool, VkQueue graphics);
    void CreateTextureImage(VulkanDevice device, const void *datas, uint32_t width, uint32_t height, VulkanImage&image, VulkanPool pool, VkQueue graphics);
    void CreateFontImage(VulkanDevice device, const VulkanBuffer&dataBuffer, uint32_t width, uint32_t height, VulkanImage&image, VulkanPool pool, VkQueue graphics);
    void CreateTextureImage(VulkanDevice device, const VulkanBuffer&dataBuffer, uint32_t width, uint32_t height, VulkanImage&image, VulkanPool pool, VkQueue graphics);
    void CreateFontImageArray(VulkanDevice device, const void* datas, uint32_t imageCount, uint32_t width, uint32_t height, VulkanImage& image, VulkanPool pool, VkQueue graphics);
    void CreateFontImageArray(VulkanDevice device, const VulkanBuffer& dataBuffer, uint32_t imageCount, uint32_t width, uint32_t height, VulkanImage& image, VulkanPool pool, VkQueue graphics);

//     void CreateCubeImage(VkDevice device, const void *datas, uint32_t width, VulkanImage&image, VkCommandPool pool, VkQueue graphics);
    void CreateCubeImage(VulkanDevice device, const void*const*datas, uint32_t width, VulkanImage&image, VulkanPool pool, VkQueue graphics);
    void CreateCubeImage(VulkanDevice device, const VulkanBuffer&dataBuffer, uint32_t width, VulkanImage&image, VulkanPool pool, VkQueue graphics);
    
    // void CreateImageArray(VulkanDevice device, const void *datas, uint32_t imageCount, uint32_t width, uint32_t height, VulkanImage&image, VulkanPool pool, VkQueue graphics, VkImageViewType type = VK_IMAGE_VIEW_TYPE_2D_ARRAY);
    void CreateImageArray(VulkanDevice device, const void*const*datas, uint32_t imageCount, uint32_t width, uint32_t height, VulkanImage&image, VulkanPool pool, VkQueue graphics, VkImageViewType type = VK_IMAGE_VIEW_TYPE_2D_ARRAY);
    void CreateImageArray(VulkanDevice device, const VulkanBuffer&dataBuffer, uint32_t imageCount, uint32_t width, uint32_t height, VulkanImage&image, VulkanPool pool, VkQueue graphics, VkImageViewType type = VK_IMAGE_VIEW_TYPE_2D_ARRAY);

    VkResult Submit(VkDevice device, const VkCommandBuffer&commandbuffers, VkQueue graphics, const VkSemaphore&imageAcquired, const VkSemaphore&renderComplete, const VkFence&fence = VK_NULL_HANDLE);
    // VkResult Prepare(VkDevice device, uint32_t *imageIndex, VkSwapchainKHR swapchain, const VkSemaphore&imageAcquired);
    uint32_t Prepare(VkDevice device, VkSwapchainKHR swapchain, const VkSemaphore&imageAcquired, void(*recreateSwapchain)(void* userData), void* userData);
    VkResult Present(VkDevice device, uint32_t imageIndex, VkSwapchainKHR swapchain, const VkQueue present, const VkSemaphore&renderComplete, void(*recreateSwapchain)(void* userData) = nullptr, void* userData = nullptr);
    //返回vkQueuePresentKHR的执行结果
    VkResult Render(VkDevice device, uint32_t currentFrame, const VkCommandBuffer& commandbuffers, VkSwapchainKHR swapchain, const VulkanQueue&vulkanQueue, const VulkanSynchronize&vulkanSynchronize, void(*recreateSwapchain)(void* userData) = nullptr, void* userData = nullptr);
};
#endif