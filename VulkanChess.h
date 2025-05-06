#ifndef VULKAN_CHESS_INCLUDE
#define VULKAN_CHESS_INCLUDE
#include "stdafx.h"
#include "BaseGraphic.hpp"

#define WU_COUNTRY_COLOR glm::vec3(0, 1, 0)
#define WEI_COUNTRY_COLOR glm::vec3(0, 0, 1)
#define SHU_COUNTRY_COLOR glm::vec3(1, 0, 0)
#define HAN_COUNTRY_COLOR glm::vec3(.5, .5, 0)

#define FONT_INDEX_MA 7
#define FONT_INDEX_PAO 5
#define FONT_INDEX_CHE 6
#define FONT_INDEX_SHI 9
#define FONT_INDEX_BING 4
#define FONT_INDEX_XIANG 8

#define FONT_INDEX_WU WU_COUNTRY_INDEX
#define FONT_INDEX_SHU SHU_COUNTRY_INDEX
#define FONT_INDEX_WEI WEI_COUNTRY_INDEX
#define FONT_INDEX_HAN HAN_COUNTRY_INDEX

//字体宏一般情况下需要修改
//字体大小不一定要和棋子大小一样
#define FONT_WIDTH 70
#define FONT_HEIGHT FONT_WIDTH

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
        glm::vec3 color;
        float imageIndex;
    };
    struct{
        VkDescriptorSet font;
        struct{
            VkDescriptorSet basewhite;
            VkDescriptorSet white;
            VkDescriptorSet black;
        }chess;
        // VkDescriptorSet chess;
    }descriptorSet;
    struct{
        VulkanBuffer font;
        struct{
            VulkanBuffer basewhite;
            VulkanBuffer white;
            VulkanBuffer black;
        }chess;
        // VulkanBuffer chess;
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

    void UpdateUniform(VkDevice device, uint32_t fontIndex, uint32_t country, const glm::vec2&pos, uint32_t dynamicOffsets);
    void UpdateUniform(VkDevice device, uint32_t fontIndex, uint32_t country, const glm::vec2&pos, uint32_t width, uint32_t height, uint32_t dynamicOffsets);
    //应该拆成独立函数，供外部调用
    void DrawFont(VkCommandBuffer command, VkPipelineLayout layout);
    void DrawChess(VkCommandBuffer command, VkPipelineLayout layout);
};
#endif