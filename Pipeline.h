#ifndef PIPELINE_H 
#define PIPELINE_H
#include <vector>
#include <string>
#include <cstring>
#include "vulkanFrame.h"
// #include <glm/glm.hpp>
// #include <glm/gtc/matrix_transform.hpp>

//// #include "vulkanFrame.h"
//struct VertexInputAttributeDescription{
//    std::string mName;
//    VkVertexInputAttributeDescription mAttributeDescriptions;
//};
//struct VertexInput{
//    
//    
//};
//struct DescriptorSet{
//    uint32_t uiSet;
//    uint32_t uiSize;
//    std::string mName;
//    // bufferInfo uniform;
//    // bufferInfo storage;
//    // ImageInfo sampledImage;
//    VkDescriptorSetLayoutBinding binding;
//};
struct Shader{
    std::string mEntryName;
    VkShaderModule mModule;
    VkShaderStageFlags mStage;
    Shader() {
        
    }
    ~Shader() {

    }
    //    Shader(const std::string&name, VkShaderStageFlagBits stage){
//        mName = name;
//        mStage = stage;
//        // mShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
//        // mShaderStageInfo.pName = "main";
//        // mShaderStageInfo.module = VK_NULL_HANDLE;
//        // mShaderStageInfo.stage = stage;
//    }
//    VkResult CreateShaderModule(VkDevice device, const std::vector<uint32_t>&code){
//        VkShaderModuleCreateInfo info{};
//        info.codeSize = code.size() * sizeof(uint32_t);
//        info.pCode = code.data();
//        return vkCreateShaderModule(device, &info, nullptr, &mModule);
//    }
//    void DestroyShaderModule(VkDevice device){
//        vkDestroyShaderModule(device, mModule, nullptr);
//    }
};
class Pipeline{
protected:
    VkPipeline mPipeline;
    VkPipelineCache mCache;
    VkPipelineLayout mLayout;
    std::vector<Shader>mShaders;
    std::vector<uint32_t>mSetLayoutBindingIndex;
    std::vector<VkDescriptorSetLayout>mSetLayouts;
    std::vector<VkPushConstantRange>mPushConstants;
    std::vector<VkDescriptorSetLayoutBinding>mSetLayoutBindings;
    // void DestroyDescriptorSet(VkDevice device, DescriptorSet&descriptorSet);
    //void GetDescriptorSetBinding(const spirv_cross::CompilerGLSL&glsl, const spirv_cross::SmallVector<spirv_cross::Resource>&resource, VkDescriptorType type, std::vector<DescriptorSet>&out);
    //auto AddShader(VkDevice device, const std::vector<uint32_t>&code);
public:
    Pipeline();
    ~Pipeline();
    void cleanup(){
        mShaders.clear();
        mPushConstants.clear();
        mSetLayoutBindings.clear();
        mSetLayoutBindingIndex.clear();
    }
    virtual void BindDescriptorSet(VkCommandBuffer cmd, VkDescriptorSet&set, uint32_t firstSet = 0, uint32_t descriptorSetCount = 1);
    virtual void BindDescriptorSet(VkCommandBuffer cmd, VkDescriptorSet&set, uint32_t dynamicOffsetCount, const uint32_t *pDynamicOffsets, uint32_t descriptorSetCount = 1);
    virtual void BindDescriptorSet(VkCommandBuffer cmd, VkPipelineBindPoint pipelineBindPoint, VkDescriptorSet&set, uint32_t firstSet = 0, uint32_t dynamicOffsetCount = 0, const uint32_t *pDynamicOffsets = nullptr, uint32_t descriptorSetCount = 1);
    virtual VkResult CreateCache(VkDevice device, const std::vector<uint32_t>&cacheData);
    virtual VkResult CreateDescriptorSetLayout(VkDevice device);
    virtual VkResult CreateLayout(VkDevice device);
    //virtual void DeleteShader(VkDevice device, VkShaderStageFlags stage);
    virtual void DestroyCache(VkDevice device, std::vector<uint32_t>&cacheData);
    virtual void DestroyLayout(VkDevice device);
    virtual void DestroyPipeline(VkDevice device);
    virtual void DestroySetLayout(VkDevice device);
    //virtual void DestrotyShader(VkDevice device);
    //virtual void DestrotyShader(VkDevice device, const std::vector<Shader>::iterator&it);
    virtual void GetCacheData(VkDevice device, std::vector<uint32_t>&cacheData);
    inline void SetCache(VkPipelineCache&cache){
        mCache = cache;
    }
    inline VkPipelineCache GetCache(){
        return mCache;
    }
    inline auto ShaderBegin()const{
        return mShaders.begin();
    }
    inline auto ShaderEnd()const{
        return mShaders.end();
    }
    inline auto ShaderBegin(){
        return mShaders.begin();
    }
    inline auto ShaderEnd(){
        return mShaders.end();
    }
    // inline VkPipelineLayout GetLayout(){
    //     return mLayout;
    // }
    //std::vector<Shader>::iterator GetShaders(VkShaderStageFlags stage);
    //VkShaderStageFlags GetShaderStageFlags(const spv::ExecutionModel&executionModel);
    //std::string GetShaderStageName(VkShaderStageFlags stage);
    
    virtual void PushPushConstant(const VkPushConstantRange& pc);
    virtual void PushPushConstant(uint32_t size, VkShaderStageFlags stage, uint32_t offset = 0);
    virtual void PushPushConstant(VkCommandBuffer cmd, VkShaderStageFlags stage, uint32_t size, const void *pData, uint32_t offset = 0);

    virtual void PushDescriptorSetLayoutBinding(uint32_t binding, VkShaderStageFlags stage, VkDescriptorType descriptorType, uint32_t SetLayoutIndex = 0);
    virtual VkResult AllocateDescriptorSets(VkDevice device, VkDescriptorPool pool, uint32_t setCount, VkDescriptorSet *pDescriptorSet, uint32_t setLayoutIndex = 0);
    virtual void UpdateDescriptorSets(VkDevice device, const std::vector<VulkanBuffer>&descriptorBuffer, const std::vector<VulkanImage>&descriptorImage, VkDescriptorSet&destSet, const VkSampler&textureSampler = VK_NULL_HANDLE);

    virtual void PushShader(const Shader&shader);
    virtual void PushShader(VkShaderStageFlags stage, const VkShaderModule& Module);
    virtual void PushShader(VkDevice device, VkShaderStageFlags stage, const std::string&file);
    virtual void PushShader(VkDevice device, VkShaderStageFlags stage, const std::vector<uint32_t>& code);

    virtual VkResult CreatePipeline(VkDevice device, VkRenderPass& renderPass) = 0;

    virtual void BindPipeline(VkCommandBuffer cmd) = 0; 
};
struct GraphicsPipelineStateInfo{
    //std::string name;
    VkPipelineColorBlendAttachmentState mColorBlend{};
    VkPipelineMultisampleStateCreateInfo mMultisample{};
    VkPipelineDepthStencilStateCreateInfo mDepthStencil{};
    VkPipelineTessellationStateCreateInfo mTessellation{};
    VkPipelineRasterizationStateCreateInfo mRasterization{};
    VkPipelineInputAssemblyStateCreateInfo mInputAssembly{};
    GraphicsPipelineStateInfo(){
        mRasterization.lineWidth = 1.0f;
        mMultisample.minSampleShading = 1.0f;
        mMultisample.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        mInputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        mMultisample.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        mTessellation.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
        mDepthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        mRasterization.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        mInputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        mColorBlend.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    }
    ~GraphicsPipelineStateInfo(){

    }
};
class GraphicsPipeline:public Pipeline{
    std::vector<VkRect2D>mScissor;
    std::vector<VkViewport>mViewport;
    GraphicsPipelineStateInfo mState;
    std::vector<VkDynamicState>mDynamics;
    // uint32_t mScissorCount, mViewportCount;
    VkVertexInputBindingDescription mBindingDescription;
    std::vector<VkVertexInputAttributeDescription>mVertexInputAttributeDescription;
public:
    GraphicsPipeline();
    GraphicsPipeline(const GraphicsPipelineStateInfo&stateInfo);
    ~GraphicsPipeline();
    void cleanup(){
        mScissor.clear();
        mViewport.clear();
        mVertexInputAttributeDescription.clear();

        Pipeline::cleanup();
    }
    virtual void BindPipeline(VkCommandBuffer cmd);
    void SetVertexInputBindingDescription(uint32_t stride, uint32_t binding = 0, VkVertexInputRate inputRate = VK_VERTEX_INPUT_RATE_VERTEX) {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.stride = stride;
        bindingDescription.binding = binding;
        bindingDescription.inputRate = inputRate;
        SetVertexInputBindingDescription(bindingDescription);
    }
    inline void SetVertexInputBindingDescription(const VkVertexInputBindingDescription&bindingDescription) {
        mBindingDescription = bindingDescription;
    }
    inline void PushVertexInputAttributeDescription(uint32_t location, uint32_t offset, VkFormat format, uint32_t binding = 0) {
        VkVertexInputAttributeDescription vertexInputAttributeDescription{};
        vertexInputAttributeDescription.format = format;
        vertexInputAttributeDescription.offset = offset;
        vertexInputAttributeDescription.binding = binding;
        vertexInputAttributeDescription.location = location;
        PushVertexInputAttributeDescription(vertexInputAttributeDescription);
    }
    inline void PushVertexInputAttributeDescription(const VkVertexInputAttributeDescription&vertexInputAttributeDescription) {
        mVertexInputAttributeDescription.push_back(vertexInputAttributeDescription);
    }
    inline void SetStateInfo(const GraphicsPipelineStateInfo&stateInfo){
        mState = stateInfo;
    }
    //如果pRect为null,则将启用动态裁剪视口,因此记录命令时应该加上vkCmdSetScissor
    inline void PushScissor(const VkRect2D *pRect){
        if(pRect){
            mScissor.push_back(*pRect);
        }
        // ++mScissorCount;
    }
    //如果pViewport为null,则将启用动态裁剪视口,因此记录命令时应该加上vkCmdSetViewport
    inline void PushViewport(const VkViewport *pViewport){
        if(pViewport){
            mViewport.push_back(*pViewport);
        }
        // ++mViewportCount;
    }
    inline void PushScissor(uint32_t width, uint32_t height, VkOffset2D offset = {}){
        VkRect2D rect;
        rect.offset = offset;
        rect.extent.width = width;
        rect.extent.height = height;
        PushScissor(&rect);
    }
    inline void PushViewport(uint32_t width, uint32_t height, float x = 0, float y = 0, float maxDepth = 1, float minDepth = 0){
        VkViewport viewport;
        viewport.x = x;
        viewport.y = y;
        viewport.width = width;
        viewport.height = height;
        viewport.maxDepth = maxDepth;
        viewport.minDepth = minDepth;
        PushViewport(&viewport);
    }
    //VK_DYNAMIC_STATE_SCISSOR和VK_DYNAMIC_STATE_VIEWPORT在推送了null的视角后自动添加。也可以不推送视角然后直接从这里添加
    inline void PushDynamic(const VkDynamicState&dynamicState){
        mDynamics.push_back(dynamicState);
    }

    // virtual bool operator==(const GraphicsPipeline&pipeline);
    //uiSet为0
    // void BindDescriptorSets(VkCommandBuffer cmd, const VkDescriptorSet *pDescriptorSets, uint32_t descriptorSetCount = 1);
    // void BindDescriptorSets(VkCommandBuffer cmd, uint32_t uiSet, const VkDescriptorSet *pDescriptorSets, uint32_t descriptorSetCount = 1);
    // void PushConstant(VkCommandBuffer cmd);
    // void PushConstant(VkCommandBuffer cmd, const std::vector<glm::mat4>&data);

    // virtual void DrawUI();
    // virtual void DrawShaderUI(const ShaderInfo&shader);
    virtual VkResult CreatePipeline(VkDevice device, VkRenderPass&renderPass);
    //virtual void IncreaseShader(VkDevice device, const std::vector<uint32_t>&code);
};
#endif