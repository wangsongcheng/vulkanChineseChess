#ifndef VULKANWIREFRAME_H
#define VULKANWIREFRAME_H
#include "glm/glm.hpp"
#include "vulkanFrame.h"
#include "BaseGraphic.hpp"
#include "glm/gtc/matrix_transform.hpp"
#ifndef ALIGN
//如果val比alignment小，则返回alignment，否则如果val大于alignment但小于alignment*2则返回alignment*2以此类推
#define ALIGN(val, alignment)((val + alignment - 1) & ~(alignment - 1))
#endif
class VulkanWireframe{
    uint32_t mCount;
    VulkanRect mRect;
    VkDescriptorSet mSet;
    VulkanBuffer mUniform;
public:
    VulkanWireframe(/* args */);
    ~VulkanWireframe();
    void Cleanup(VkDevice device);
    void Setup(VulkanDevice device, uint32_t count, VkDescriptorSetLayout setLayout, VkQueue graphics, VulkanPool pool);

    void Draw(VkCommandBuffer command, VkPipelineLayout layout);

    void UpdateUniform(VkDevice device, glm::mat4 model, uint32_t offset);
    void UpdateUniform(VkDevice device, const std::vector<glm::mat4>&model);
};
#endif