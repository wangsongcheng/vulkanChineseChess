#ifndef VULKANIMGUI_H
#define VULKANIMGUI_H
#include "imgui.h"
#include "Pipeline.hpp"
#include "vulkanFrame.h"
class VulkanImgui{
    //因为imgui确实需要一条不同的管线，所以这里需要一个管线对象
    struct{
        VkPipeline pipeline;
        VkPipelineLayout layout;
    }pipelines;
    BaseGraphic mRect;
    VulkanImage mFont;
    VkDescriptorSet mSet;
    VulkanDevice mDevice;
    VkSampler mFontSampler;
    VkDescriptorSetLayout mSetLayout;
    VkDeviceSize mBufferMemoryAlignment;
    void CreatePipelineLayout(VkDevice device);
    void SetupDescriptorSetLayout(VkDevice device);
    void CreateFontsTexture(VulkanDevice device, VkQueue graphics, VulkanPool pool);
    // void CreateRect(VulkanDevice device, VkDeviceSize index_size, VkDeviceSize vertex_size);
    void SetupRenderState(VkCommandBuffer command, ImDrawData* draw_data, int fb_width, int fb_height);
    //测试
    void CreateOrResizeBuffer(VkBuffer& buffer, VkDeviceMemory& buffer_memory, VkDeviceSize& p_buffer_size, size_t new_size, VkBufferUsageFlagBits usage);
public:
    VulkanImgui(/* args */);
    ~VulkanImgui();
    void Cleanup(VkDevice device);
    void Setup(VulkanDevice device, VkQueue graphics, VulkanPool pool);

    void CreatePipeline(VkDevice device, VkRenderPass renderPass, VkPipelineCache cache = VK_NULL_HANDLE);

    void RenderDrawData(VkCommandBuffer command, ImDrawData* draw_data);
};
#endif