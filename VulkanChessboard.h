#ifndef VULKAN_CHESSBOAERD_H
#define VULKAN_CHESSBOAERD_H
#include "font.h"
#include "stdafx.h"

#include "BaseGraphic.hpp"
#define INDEX_TO_ROW(INDEX, COLUMN)(INDEX / COLUMN)
#define INDEX_TO_COLUMN(INDEX, COLUMN)(INDEX % COLUMN)

#define WIREFRAME_COLUMN_TO_X(COLUMN)CHESS_COLUMN_TO_X(COLUMN)
#define WIREFRAME_ROW_TO_Y(ROW)WIREFRAME_COLUMN_TO_X(ROW)
class VulkanChessboard{
    struct FontUniform{
        glm::mat4 model;
        glm::vec3 color;
        float imageIndex;
    };
    struct{
        VkDescriptorSet grid;
        VkDescriptorSet bigGrid;
        VkDescriptorSet alliance;
        VkDescriptorSet background;
        struct{
            VkDescriptorSet jiugongge;
            // VkDescriptorSet selectChess;
        }wireframe;
    }descriptorSet;
    struct{
        VulkanBuffer grid;
        VulkanBuffer bigGrid;
        VulkanBuffer alliance;
        VulkanBuffer background;
        struct{
            VulkanBuffer jiugongge;
            // VulkanWireframe jiugongge;
            // VulkanBuffer selectChess;
        }wireframe;
    }uniforms;
    struct{
        VkSampler sampler;
        struct{
            VulkanBuffer alliance;
        }uniforms;
        struct{
            VkDescriptorSet alliance;
        }descriptrorSet;
        VulkanRect font;
        VulkanFontImage image;//魏, 蜀, 吴
    }fonts;
    VulkanRect mRect;
    VulkanCircle mCircle;
    void UpdateGridUniform(VkDevice device);
    void UpdateDescriptorSet(VkDevice device);
    void UpdateBigGridUniform(VkDevice device);
    void UpdateJiuGongGeWireframeUniform(VkDevice device);
    void UpdateBackgroundUniform(VkDevice device, uint32_t windowWidth);
    void CreateFontResource(VulkanDevice device, VkQueue graphics, VulkanPool pool);
    void CreateRectResource(VulkanDevice device, VulkanPool pool, VkQueue graphics);
public:
    VulkanChessboard(/* args */);
    ~VulkanChessboard();

    void Cleanup(VkDevice device);

    void Draw(VkCommandBuffer command, VkPipelineLayout layout);
    void DrawWireframe(VkCommandBuffer command, VkPipelineLayout layout);
    void DrawAllianceFont(VkCommandBuffer command, VkPipelineLayout layout);

    void Setup(VulkanDevice device, VkDescriptorSetLayout setLayout, VkQueue graphics, VulkanPool pool);

    void UpdateUniform(VkDevice device, uint32_t windowWidth, Country player = Invald_Country);
    void UpdateFontUniform(VkDevice device, Country player);
};
#endif