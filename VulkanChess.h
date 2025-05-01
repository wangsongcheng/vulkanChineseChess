#ifndef VULKAN_CHESS_INCLUDE
#define VULKAN_CHESS_INCLUDE
#include "stdafx.h"
#include "BaseGraphic.hpp"

#define WU_CHESS_COUNTRY_COLOR glm::vec3(0, 1, 0)
#define WEI_CHESS_COUNTRY_COLOR glm::vec3(0, 0, 1)
#define SHU_CHESS_COUNTRY_COLOR glm::vec3(1, 0, 0)
#define HAN_CHESS_COUNTRY_COLOR glm::vec3(1, 1, 0)

class VulkanChess{
    struct Vertex {
        glm::vec3 pos;
        glm::vec3 color;
        Vertex(){
        }
        Vertex(const glm::vec3&pos, const glm::vec3&color){
            this->pos = pos;
            this->color = color;
        }
    };
    struct FontUniform{
        glm::mat4 model;
        float imageIndex;
    };
    struct{
        VkDescriptorSet font;
        VkDescriptorSet chess;
    }descriptorSet;
    struct{
        VulkanBuffer font;
        VulkanBuffer chess;
    }uniform;
    // struct{
    //     GraphicsPipeline font;
    //     GraphicsPipeline chess;
    // }pipelines;
    struct{
        VkSampler sampler;
        VulkanFontImage image;//魏, 蜀, 吴, 漢, 兵, 炮, 車, 馬, 相, 士
    }fonts;
    VulkanRect mFont;
    VulkanCircle mChess;
    void UpdateDescriptorSet(VkDevice device);
    void GetCircularVertex(const glm::vec3&color, std::vector<Vertex>&vertices);
    void CreateFontResource(VulkanDevice device, VulkanPool pool, VkQueue graphics);
    void CreateChessResource(VulkanDevice device, VulkanPool pool, VkQueue graphics);
public:
    VulkanChess(/* args */);
    ~VulkanChess();
    void Cleanup(VkDevice device);
    void Setup(VulkanDevice device, VkDescriptorSetLayout layout, VkQueue graphics, VulkanPool pool);

    void UpdateUniform(VkDevice device, uint32_t fontIndex, const glm::vec2&pos, uint32_t dynamicOffsets);
    //应该拆成独立函数，供外部调用
    void DrawFont(VkCommandBuffer command, VkPipelineLayout layout);
    void DrawChess(VkCommandBuffer command, VkPipelineLayout layout, uint32_t currentCountry);
};
#endif