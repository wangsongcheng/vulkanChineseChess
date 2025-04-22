#ifndef VULKAN_CHESSBOARD_INCLUDE
#define VULKAN_CHESSBOARD_INCLUDE
#include "glm/glm.hpp"
#include "vulkanFrame.h"
#include "BaseGraphic.hpp"
#include "glm/gtc/matrix_transform.hpp"
#define CHESSBOARD_ROW 16
#define CHESSBOARD_COLUMN CHESSBOARD_ROW

// #define CHESSBOARD_START_ROW_OFFSET 0
// #define CHESSBOARD_START_COLUMN_OFFSET CHESSBOARD_START_ROW_OFFSET

#define CHESSBOARD_WIREFRAME_COUNT 16

#define CHESSBOARD_RECT_SIZE 45
#define CHESSBOARD_LINE_WIDTH 1
#define CHESSBOARD_BING_GRID_DENSITY (CHESSBOARD_ROW / 4)
#define CHESSBOARD_BIG_GRID_COUNT (CHESSBOARD_ROW / 8u * 2)
#define CHESSBOARD_BIG_RECT_SIZE (CHESSBOARD_RECT_SIZE * CHESSBOARD_BING_GRID_DENSITY + CHESSBOARD_LINE_WIDTH * CHESSBOARD_BING_GRID_DENSITY - 1)
#define CHESSBOARD_BACKGROUND_SIZE(WINDOW_WIDTH)(WINDOW_WIDTH - 2 * CHESSBOARD_RECT_SIZE)
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
class VulkanChessboard{
    struct{
        VkDescriptorSet grid;
        VkDescriptorSet bigGrid;
        VkDescriptorSet background;
        struct{
            VkDescriptorSet jiugongge;
            // VkDescriptorSet selectChess;
        }wireframe;
    }descriptorSet;
    struct{
        VulkanBuffer grid;
        VulkanBuffer bigGrid;
        VulkanBuffer background;
        struct{
            VulkanBuffer jiugongge;
            // VulkanWireframe jiugongge;
            // VulkanBuffer selectChess;
        }wireframe;
    }uniforms;
    Rect mRect;
    void UpdateGridUniform(VkDevice device);
    void UpdateBigGridUniform(VkDevice device);
    void UpdateJiuGongGeWireframeUniform(VkDevice device);
    void UpdateBackgroundUniform(VkDevice device, uint32_t windowWidth);
    void CreateRectResource(VulkanDevice device, VulkanPool pool, VkQueue graphics);
    void SetupDescriptorSet(VkDevice device, VkDescriptorSetLayout setLayout, VulkanPool pool);
public:
    VulkanChessboard(/* args */);
    ~VulkanChessboard();
    void Cleanup(VkDevice device);
    void Setup(VulkanDevice device, VkDescriptorSetLayout setLayout, VkQueue graphics, VulkanPool pool);

    void UpdateUniform(VkDevice device, uint32_t windowWidth);
    // void UpdateSelectChessWireframeUniform(VkDevice device, const glm::mat4 *pModel, uint32_t count = CHESSBOARD_ROW + CHESSBOARD_COLUMN);

    void Draw(VkCommandBuffer command, const VkPipelineLayout&layout);
    void DrawWireframe(VkCommandBuffer command, const VkPipelineLayout&layout);
};
#endif