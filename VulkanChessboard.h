#ifndef VULKAN_CHESS_BOARD_INCLUDE_H
#define VULKAN_CHESS_BOARD_INCLUDE_H
#include "Chess.h"
#include "VulkanChess.h"
#define CHESSBOARD_ROW 16
//只要绘制线框时没有加线宽偏移的话，下面的值就不应该修改。因为图形管线线宽默认1
#define CHESSBOARD_LINE_WIDTH 1
#define CHESSBOARD_BIG_GRID_COUNT 4
#ifdef HAN_CAN_PLAY
#define CHESSBOARD_WIREFRAME_COUNT 16
#else
#define CHESSBOARD_WIREFRAME_COUNT 12
#endif
#define CHESSBOARD_COLUMN CHESSBOARD_ROW
#define CHESSBOARD_GRID_SIZE (CHESS_WIDTH * 2)

// 大网格除了4个CHESSBOARD_GRID_SIZE还包含线条的宽度
#define CHESSBOARD_BIG_GRID_SIZE (CHESSBOARD_GRID_SIZE * 4 + CHESSBOARD_LINE_WIDTH * 3)
#define CHESSBOARD_BLACK_BACKGROUND_SIZE(WINDOW_WIDTH)(WINDOW_WIDTH - 2 * CHESSBOARD_GRID_SIZE)
class VulkanChessboard:public VulkanChess{
    struct{
        VkDescriptorSet grid;
        VkDescriptorSet bigGrid;
        VkDescriptorSet background;
        struct{
            VkDescriptorSet jiugongge;
            VkDescriptorSet selectChess;
        }wireframe;
    }descriptorSet;
    struct{
        VulkanBuffer grid;
        VulkanBuffer bigGrid;
        VulkanBuffer background;
        struct{
            VulkanBuffer jiugongge;
            VulkanBuffer selectChess;
        }wireframe;
    }uniforms;
    struct{
        GraphicsPipeline grid;
        GraphicsPipeline wireframe;
    }pipelines;
    //2种颜色的方格;顺便用来画线框
    BaseGraphic mRect;
    uint32_t mMinUniformBufferOffset;
    void GetRectVertices(const glm::vec3&color, Vertex *ptrVertices);
    void CreateRectResource(VkDevice device, VkCommandPool pool, VkQueue graphics);
    void SetupDescriptorSet(VkDevice device, VkDescriptorSetLayout setLayout, VkDescriptorPool pool);

    void DrawGrid(VkCommandBuffer command, const GraphicsPipeline&pipeline);
    void DrawBigGrid(VkCommandBuffer command, const GraphicsPipeline&pipeline);
    void DrawWireframe(VkCommandBuffer command, const GraphicsPipeline&pipeline);
    void DrawBackground(VkCommandBuffer command, const GraphicsPipeline&pipeline);
    void DrawSelectWireframe(VkCommandBuffer command, const GraphicsPipeline&pipeline);

    void UpdateGridUniform(VkDevice device);
    void UpdateBigGridUniform(VkDevice device);
    void UpdateJiuGongGeWireframeUniform(VkDevice device, uint32_t playerIndex);
public:
    VulkanChessboard(/* args */);
    ~VulkanChessboard();

    void Cleanup(VkDevice device);
    void Setup(VkPhysicalDevice physicalDevice, VkDevice device, VkDescriptorSetLayout setLayout, uint32_t windowWidth, VkQueue graphics, VulkanPool pool);
    void RecordCommand(VkCommandBuffer cmd, uint32_t windowWidth, uint32_t currentCountry);

    void ClearSelectWireframeUnfirom(VkDevice device);
    void UpdateUniform(VkDevice device, uint32_t playerIndex);
    void UpdateBackgroundUniform(VkDevice device, uint32_t windowWidth);
    void UpdateSelectWireframeUniform(VkDevice device, const ChessInfo *pInfo, uint32_t count);
    void UpdateChessUniform(VkDevice device, uint32_t country, uint32_t chess, const glm::vec2&pos, uint32_t fontIndex, const VkExtent2D&size);
    //chess的期望值为0~15
    void UpdateChessUniform(VkDevice device, uint32_t country, uint32_t chess, uint32_t row, uint32_t column, uint32_t fontIndex, const VkExtent2D&size);

    void DestroyGraphicsPipeline(VkDevice device);
    void CreatePipeline(VkDevice device, VkRenderPass renderpass, VkDescriptorSetLayout setLayout, VkPipelineCache pipelineCache, uint32_t windowWidth);
};
#endif