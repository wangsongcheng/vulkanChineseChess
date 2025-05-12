#ifndef VULKAN_CHESSBOAERD_H
#define VULKAN_CHESSBOAERD_H
#include "font.h"
#include "stdafx.h"

#include "BaseGraphic.hpp"
//联盟相关宏, 一般情况下需要修改
#define ALLIANCE_POINT_FONT_WIDTH 25
#define ALLIANCE_POINT_FONT_HEIGHT ALLIANCE_POINT_FONT_WIDTH
#define ALLIANCE_POINT_FONT_INDEX_WU Wu_Country
#define ALLIANCE_POINT_FONT_INDEX_WEI Wei_Country
#define ALLIANCE_POINT_FONT_INDEX_SHU Shu_Country
#define ALLIANCE_POINT_WIDTH 15
#define ALLIANCE_POINT_HEIGHT ALLIANCE_POINT_WIDTH

#define FONT_INDEX_INDEX_NUMBER_0 3
#define FONT_INDEX_INDEX_NUMBER_1 (FONT_INDEX_INDEX_NUMBER_0 + 1)
#define FONT_INDEX_INDEX_NUMBER_2 (FONT_INDEX_INDEX_NUMBER_0 + 2)
#define FONT_INDEX_INDEX_NUMBER_3 (FONT_INDEX_INDEX_NUMBER_0 + 3)
#define FONT_INDEX_INDEX_NUMBER_4 (FONT_INDEX_INDEX_NUMBER_0 + 4)
#define FONT_INDEX_INDEX_NUMBER_5 (FONT_INDEX_INDEX_NUMBER_0 + 5)
#define FONT_INDEX_INDEX_NUMBER_6 (FONT_INDEX_INDEX_NUMBER_0 + 6)
#define FONT_INDEX_INDEX_NUMBER_7 (FONT_INDEX_INDEX_NUMBER_0 + 7)
#define FONT_INDEX_INDEX_NUMBER_8 (FONT_INDEX_INDEX_NUMBER_0 + 8)
#define FONT_INDEX_INDEX_NUMBER_9 (FONT_INDEX_INDEX_NUMBER_0 + 9)

#define FONT_INDEX_COUNT 13

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