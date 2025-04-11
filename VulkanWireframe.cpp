#include "VulkanWireframe.h"
VulkanWireframe::VulkanWireframe(/* args */){
}

VulkanWireframe::~VulkanWireframe(){
}

void VulkanWireframe::Cleanup(VkDevice device){
    mRect.Destroy(device);
    mUniform.Destroy(device);
}

void VulkanWireframe::Setup(VulkanDevice device, uint32_t count, VkDescriptorSetLayout setLayout, VkQueue graphics, VulkanPool pool){
    auto&physicalDeviceProperties = device.GetPhysicalDeviceProperties();
    uint32_t minUniformBufferOffset = ALIGN(sizeof(glm::mat4), physicalDeviceProperties.limits.minUniformBufferOffsetAlignment);

    mRect.CreateRect2D(device, glm::vec3(.1), graphics, pool);

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