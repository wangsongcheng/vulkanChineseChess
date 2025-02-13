#include "VulkanWireframe.h"
void VulkanWireframe::CreateRect(VulkanDevice device, VkQueue graphics, VulkanPool pool){
   const float vertices[] = {
        .0f, 1.0f, .0f, 0, 0, 0,//左下
        1.0f, .0f, .0f, 0, 0, 0,//右上
        .0f, .0f, .0f, 0, 0, 0, //左上
        1.0f, 1.0f, .0f, 0, 0, 0//右下
    };
    const uint16_t indices[] = { 0, 1, 2, 0, 3, 1 };
    mRect.indexCount = 6;
    mRect.vertexCount = 4;
    mRect.index.CreateBuffer(device, sizeof(indices), VK_BUFFER_USAGE_INDEX_BUFFER_BIT|VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    //8是4种颜色+亮度较低的颜色(表示不是该玩家下子)
    mRect.vertex.CreateBuffer(device, sizeof(vertices), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT|VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    mRect.index.UpdateData(device, indices, graphics, pool);
    mRect.vertex.UpdateData(device, vertices, graphics, pool);
}
VulkanWireframe::VulkanWireframe(/* args */){
}

VulkanWireframe::~VulkanWireframe(){
}

void VulkanWireframe::Cleanup(VkDevice device){
    mRect.Destroy(device);
    mUniform.Destroy(device);
}

void VulkanWireframe::Setup(VulkanDevice device, uint32_t count, VkDescriptorSetLayout setLayout, VkQueue graphics, VulkanPool pool){
    VkPhysicalDeviceProperties physicalDeviceProperties;
    device.GetPhysicalDeviceProperties(physicalDeviceProperties);
    uint32_t minUniformBufferOffset = ALIGN(sizeof(glm::mat4), physicalDeviceProperties.limits.minUniformBufferOffsetAlignment);

    CreateRect(device, graphics, pool);

    mCount = count;
    mUniform.CreateBuffer(device, minUniformBufferOffset * count, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT|VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    mUniform.size = minUniformBufferOffset;

    pool.AllocateDescriptorSets(device.device, &setLayout, 1, &mSet);

    VkDescriptorSetLayoutBinding descriptorSetLayoutBindings[1] = {};
    // descriptorSetLayoutBindings[0].binding = 0;
    descriptorSetLayoutBindings[0].descriptorCount = 1;
    descriptorSetLayoutBindings[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    descriptorSetLayoutBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
    vulkanFrame::UpdateDescriptorSets(device.device, descriptorSetLayoutBindings, 1, { mUniform },{}, mSet);
}

void VulkanWireframe::Draw(VkCommandBuffer command, VkPipelineLayout layout){
    uint32_t dynamicOffsets;
    mRect.Bind(command);
    for (size_t i = 0; i < mCount; ++i){
        dynamicOffsets = i * mUniform.size;
        vkCmdBindDescriptorSets(command, VK_PIPELINE_BIND_POINT_GRAPHICS, layout, 0, 1, &mSet, 1, &dynamicOffsets);
        mRect.Draw(command);
    }
}

void VulkanWireframe::UpdateUniform(VkDevice device, glm::mat4 model, uint32_t offset){
    mUniform.UpdateData(device, &model, offset * mUniform.size);
}

void VulkanWireframe::UpdateUniform(VkDevice device, const std::vector<glm::mat4>&model){
    mUniform.UpdateData(device, model.size() * mUniform.size, model.data());
}