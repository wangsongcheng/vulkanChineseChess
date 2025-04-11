#ifndef VULKANIMGUI_H
#define VULKANIMGUI_H
#include "imgui.h"
#include "Pipeline.hpp"
#include "vulkanFrame.h"
#include "BaseGraphic.hpp"
#ifdef _MSC_VER
#undef CreateFont
#endif
class VulkanImGui{
    //因为imgui确实需要一条不同的管线，所以这里需要一个管线对象
    struct{
        VkPipeline pipeline;
        VkPipelineLayout layout;
    }pipelines;
    BaseGraphic mRect;
    VkDescriptorSet mSet;
    VulkanDevice mDevice;
    VulkanFontImage mFont;
    VkSampler mFontSampler;
    VkDescriptorSetLayout mSetLayout;
    VkDeviceSize mBufferMemoryAlignment;
    void CreatePipelineLayout(VkDevice device);
    void SetupDescriptorSetLayout(VkDevice device);
    void CreateFontsTexture(VulkanDevice device, VkQueue graphics, VulkanPool pool);
    // void CreateRect(VulkanDevice device, VkDeviceSize index_size, VkDeviceSize vertex_size);
    void CreateOrResizeBuffer(VulkanBuffer&buffer, size_t new_size, VkBufferUsageFlagBits usage);
    void SetupRenderState(VkCommandBuffer command, ImDrawData* draw_data, int fb_width, int fb_height);
public:
    VulkanImGui(/* args */);
    ~VulkanImGui();
    void Cleanup(VkDevice device);
    void Setup(VulkanDevice device, VulkanPool pool);

    void CreatePipeline(VkDevice device, VkRenderPass renderPass, VkPipelineCache cache = VK_NULL_HANDLE);

    void CreateFont(VulkanDevice device, const char *font, VkQueue graphics, VulkanPool pool);

    void RenderDrawData(VkCommandBuffer command, ImDrawData* draw_data);
};
#endif