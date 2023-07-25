#ifndef CHESSBOARD_H
#define CHESSBOARD_H
#include "chess.h"
#include "Pipeline.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
//只要绘制线框时没有加线宽偏移的话，下面的值就不应该修改。因为图形管线线宽默认1
#define CHESSBOARD_LINE_WIDTH 1
#define CHESSBOARD_GRID_SIZE 40
//大网格除了4个CHESSBOARD_GRID_SIZE还包含线条的宽度
#define CHESSBOARD_GRID_SIZE_BIG (CHESSBOARD_GRID_SIZE * 4 + CHESSBOARD_LINE_WIDTH * 3)
#define CHESSBOARD_ROW_COLUMN_INDEX(ROW_INDEX, COLUMN_INDEX, COLUMN)(ROW_INDEX * COLUMN + COLUMN_INDEX)
struct ChessboardVertex {
    glm::vec3 pos;
    glm::vec3 color;
    ChessboardVertex(const glm::vec3&pos, const glm::vec3&color){
        this->pos = pos;
        this->color = color;
    }
};
struct PushConstant{
    glm::mat4 projection;
};
//两个成员必须一样大才能一次送进uniform
struct PointUniform{
    glm::mat4 model;
    glm::uint pointSize;
};
class Chessboard{
    VkDescriptorSet mSet, mPointSet;//只需要关心颜色, 不需要纹理
    VulkanBuffer mPointVert, mPointIndex;//因为需要2种颜色的点。所以需要索引来或许同一个顶点缓冲中的下一个顶点
    VulkanBuffer mPointUniform, mWireFrameUniform;
    VulkanBuffer mWireframeVert, mWireframeIndex[2];//2个顶点索引缓冲。顶点可以共用
    GraphicsPipeline mPointPipeline, mWireframePipeline;//线框管线分左和右
    int32_t mPointMinUniformBufferOffset, mWireframeMinUniformBufferOffset;
    void CreateGraphicsPipeline(VkDevice device, uint32_t windowWidth, VkRenderPass renderpass, const std::vector<std::string>&shader, GraphicsPipeline&pipeline, const GraphicsPipelineStateInfo&pipelineState = {});
public:
    Chessboard(/* args */);
    ~Chessboard();
    //该函数只改变坐标,所以不需要关心调用的先后顺序(至少CreateVulkanResource是这样)
    // void InitChessboard();
    void Cleanup(VkDevice device);
    void UpdateSet(VkDevice device);
    void DestroyGraphicsPipeline(VkDevice device);
    void RecodeCommand(VkCommandBuffer cmd, uint32_t windowWidth);
    void CreatePipeline(VkDevice device, VkRenderPass renderpass, uint32_t windowWidth);
    void CreateVulkanResource(VkPhysicalDevice physicalDevice, VkDevice device, uint32_t windowWidth, VkQueue graphics, VulkanPool pool);
    //如果可以。试试去掉蜀国和吴国的2个兵(其他棋子也可以考虑)，已体现魏国的优势
    //绘制棋盘
    //接受棋子
    //输赢交给外部判断
    //mCirclePipeline应该交给棋子类,uniform也是
};
#endif