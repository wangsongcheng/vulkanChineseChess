#ifndef VULKAN_CHESSBOARD_INCLUDE
#define VULKAN_CHESSBOARD_INCLUDE
#include <array>
#include "glm/glm.hpp"
#include "vulkanFrame.h"
#include "BaseGraphic.hpp"
#include "glm/gtc/matrix_transform.hpp"
// #define ENABLE_BATTLE_FAN_GUANYU

#define CHESSBOARD_WIREFRAME_COUNT 16

#define CHESSBOARD_RECT_SIZE 45
#define CHESSBOARD_LINE_WIDTH 1
#define CHESSBOARD_BACKGROUND_SIZE(WINDOW_WIDTH)(WINDOW_WIDTH - 2 * CHESSBOARD_RECT_SIZE)
//数量当然能随意改，但只改数量意义不大，还需要把它们的坐标等信息通过uniform传给着色器
//除汉外, 每个国家的棋子数
#define PAO_CHESS_COUNT 2
#ifdef ENABLE_BATTLE_FAN_GUANYU
#define CHE_CHESS_COUNT 0
#define BING_CHESS_COUNT 3
#define XIANG_CHESS_COUNT 2
#else
#define CHE_CHESS_COUNT 2
#define BING_CHESS_COUNT 5
#define XIANG_CHESS_COUNT 2
#endif
#define MA_CHESS_COUNT 2
#define SHI_CHESS_COUNT 2
#define JIANG_CHESS_COUNT 1

#define HAN_CHE_CHESS_COUNT 3
#define HAN_PAO_CHESS_COUNT 1
#define HAN_JIANG_CHESS_COUNT 1

#define WU_CHESS_COUNT (BING_CHESS_COUNT + PAO_CHESS_COUNT + CHE_CHESS_COUNT + MA_CHESS_COUNT + XIANG_CHESS_COUNT + SHI_CHESS_COUNT + JIANG_CHESS_COUNT)
#define HAN_CHESS_COUNT WU_CHESS_COUNT
#define WEI_CHESS_COUNT WU_CHESS_COUNT
#define SHU_CHESS_COUNT WU_CHESS_COUNT

#define COUNTRY_CHESS_COUNT WEI_CHESS_COUNT

#define DRAW_COUNTRY_CHESS_COUNT (COUNTRY_CHESS_COUNT * 3)

#define CHESSBOARD_CHESS_TOTAL (DRAW_COUNTRY_CHESS_COUNT * 4)
#ifdef ENABLE_BATTLE_FAN_GUANYU
#define CHESSBOARD_BING_GRID_DENSITY 3
#else
#define CHESSBOARD_BING_GRID_DENSITY 4
#endif

#define CHESSBOARD_BOUNDARY_CHESS_COUNT (CHE_CHESS_COUNT + MA_CHESS_COUNT + XIANG_CHESS_COUNT + SHI_CHESS_COUNT + JIANG_CHESS_COUNT)

#define CHESSBOARD_ROW (CHESSBOARD_BOUNDARY_CHESS_COUNT + CHESSBOARD_BING_GRID_DENSITY * 2 - 1)
#define CHESSBOARD_COLUMN CHESSBOARD_ROW

#define CHESSBOARD_BIG_GRID_COUNT 4
#define CHESSBOARD_BIG_RECT_SIZE (CHESSBOARD_RECT_SIZE * CHESSBOARD_BING_GRID_DENSITY + CHESSBOARD_LINE_WIDTH * CHESSBOARD_BING_GRID_DENSITY - 1)

// #define CHESSBOARD_START_ROW_OFFSET 0
// #define CHESSBOARD_START_COLUMN_OFFSET CHESSBOARD_START_ROW_OFFSET

#ifndef ROW_COLUMN_TO_INDEX
#define ROW_COLUMN_TO_INDEX(ROW_INDEX, COLUMN_INDEX, COLUMN)((ROW_INDEX) * (COLUMN) + (COLUMN_INDEX))
#endif
#ifndef ALIGN
//如果val比alignment小，则返回alignment，否则如果val大于alignment但小于alignment*2则返回alignment*2以此类推
#define ALIGN(val, alignment)((val + alignment - 1) & ~(alignment - 1))
#endif
#ifndef COLUMN_TO_X
#define COLUMN_TO_X(COLUMN)((CHESSBOARD_RECT_SIZE) + (COLUMN) * (CHESSBOARD_RECT_SIZE) + ((COLUMN) + 1) * CHESSBOARD_LINE_WIDTH)
#endif
#ifndef ROW_TO_Y
#define ROW_TO_Y(ROW)(COLUMN_TO_X(ROW))
#endif
#define INVALID_CHESS_INDEX -1
#ifndef WU_COUNTRY_INDEX
#define WU_COUNTRY_INDEX 0
#define WEI_COUNTRY_INDEX 1
#define SHU_COUNTRY_INDEX 2
#define HAN_COUNTRY_INDEX 3

#define INVALID_COUNTRY_INDEX -1
//最好不要修改该宏的值
#define MAX_COUNTRY_INDEX 4
#endif

#define ALLIANCE_POINT_FONT_WIDTH 25
#define ALLIANCE_POINT_FONT_HEIGHT ALLIANCE_POINT_FONT_WIDTH
#define ALLIANCE_POINT_FONT_INDEX_WU 0
#define ALLIANCE_POINT_FONT_INDEX_WEI 1
#define ALLIANCE_POINT_FONT_INDEX_SHU 2
#define ALLIANCE_POINT_WIDTH 15
#define ALLIANCE_POINT_HEIGHT ALLIANCE_POINT_WIDTH

#define JIANG_CHESS_INDEX 0
#define MA_CHESS_INDEX (JIANG_CHESS_INDEX + JIANG_CHESS_COUNT)
#define PAO_CHESS_INDEX (MA_CHESS_INDEX + MA_CHESS_COUNT)
#define CHE_CHESS_INDEX (PAO_CHESS_INDEX + PAO_CHESS_COUNT)
#define SHI_CHESS_INDEX (CHE_CHESS_INDEX + CHE_CHESS_COUNT)
#define XIANG_CHESS_INDEX (SHI_CHESS_INDEX + SHI_CHESS_COUNT)
#define BING_CHESS_INDEX (XIANG_CHESS_INDEX + XIANG_CHESS_COUNT)
class VulkanChessboard{
    struct FontUniform{
        glm::mat4 model;
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
    void UpdateBigGridUniform(VkDevice device);
    void UpdateJiuGongGeWireframeUniform(VkDevice device);
    void UpdateBackgroundUniform(VkDevice device, uint32_t windowWidth);
    void CreateFontResource(VulkanDevice device, VkQueue graphics, VulkanPool pool);
    void CreateRectResource(VulkanDevice device, VulkanPool pool, VkQueue graphics);
    void SetupDescriptorSet(VkDevice device, VkDescriptorSetLayout setLayout, VulkanPool pool);
public:
    VulkanChessboard(/* args */);
    ~VulkanChessboard();
    void Cleanup(VkDevice device);
    void Setup(VulkanDevice device, VkDescriptorSetLayout setLayout, VkQueue graphics, VulkanPool pool);

    void UpdateUniform(VkDevice device, uint32_t windowWidth);
    void UpdateFontUniform(VkDevice device, uint32_t playerCountry);
    // void UpdateSelectChessWireframeUniform(VkDevice device, const glm::mat4 *pModel, uint32_t count = CHESSBOARD_ROW + CHESSBOARD_COLUMN);

    void Draw(VkCommandBuffer command, const VkPipelineLayout&layout);
    void DrawWireframe(VkCommandBuffer command, const VkPipelineLayout&layout);
    void DrawAllianceFont(VkCommandBuffer command, const VkPipelineLayout&layout);
};
#endif