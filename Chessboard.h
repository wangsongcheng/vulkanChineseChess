#ifndef CHESSBOARD_H
#define CHESSBOARD_H
#include "chess.h"
#define WU_CHESS_INDEX 2
#define WEI_CHESS_INDEX 0
#define SHU_CHESS_INDEX 1
#define HAN_CHESS_INDEX 3
//只要绘制线框时没有加线宽偏移的话，下面的值就不应该修改。因为图形管线线宽默认1
#define CHESSBOARD_LINE_WIDTH 1
#define CHESSBOARD_GRID_SIZE 40
#define ALIGN(val, alignment)((val + alignment - 1) & ~(alignment - 1))
//大网格除了4个CHESSBOARD_GRID_SIZE还包含线条的宽度
#define CHESSBOARD_GRID_SIZE_BIG (CHESSBOARD_GRID_SIZE * 4 + CHESSBOARD_LINE_WIDTH * 3)
#define CHESSBOARD_BLACK_BACKGROUND_SIZE(WINDOW_WIDTH)(WINDOW_WIDTH - 2 * CHESSBOARD_GRID_SIZE)
#define CHESSBOARD_ROW_COLUMN_INDEX(ROW_INDEX, COLUMN_INDEX, COLUMN)(ROW_INDEX * COLUMN + COLUMN_INDEX)
// #define ROW_COLUMN_TO_COORD(ROW, COLUMN, GRIDSIZE)()
#define ROW_TO_Y(ROW, GRIDSIZE, LINE_WIDTH)((GRIDSIZE) + (ROW) * (GRIDSIZE) + ((ROW) + 1) * LINE_WIDTH)
#define COLUMN_TO_X(COLUMN, GRIDSIZE, LINE_WIDTH)(ROW_TO_Y(COLUMN, GRIDSIZE, LINE_WIDTH))
// //除非增加新棋,否则以下棋子的值应该在16以下
// #define JIANG_CHESS 0
// #define MA_CHESS_1 1
// #define MA_CHESS_2 2
struct ChessboardVertex {
    glm::vec3 pos;
    glm::vec3 color;
    ChessboardVertex(){
    }
    ChessboardVertex(const glm::vec3&pos, const glm::vec3&color){
        this->pos = pos;
        this->color = color;
    }
};
struct ChessboardChess{
    // Chess *chess[16];
    Chess *jiang;//实际上是魏蜀吴汉
    Chess *ma[2];
    Chess *pao[2];
    Chess *che[2];
    Chess *shi[2];
    Chess *bing[5];
    Chess *xiang[2];
};
// struct PushConstant{
//     glm::mat4 projection;
// };
//两个成员必须一样大才能一次送进uniform
struct PointUniform{
    glm::mat4 model;
    glm::uint pointSize;
};
//某些设备绘制的点大小有限制。为了保证效果, 背景一律用矩形绘制
struct ChessboardBackground{
    VulkanBuffer pos;
    VulkanBuffer vert[2];//1个棋盘颜色，1个黑色
    VkDescriptorSet mSet;
    GraphicsPipeline pipeline;
};
class Chessboard{
    Chess *mSelected;
    // float mPointSizeRange;
    //魏蜀吴汉
    ChessboardChess mChess[4];

    VulkanBuffer mPointVert;
    VkDescriptorSet mSet, mPointSet;
    ChessboardBackground mBackground;
    VulkanBuffer mPointUniform, mWireFrameUniform;
    VulkanBuffer mWireframeVert, mWireframeIndex[2];//2个顶点索引缓冲。顶点可以共用//0为/
    GraphicsPipeline mPointPipeline, mWireframePipeline;
    int32_t mPointMinUniformBufferOffset, mRectMinUniformBufferOffset;
    //棋类的管线//在没有更好的办法前先这么写
    GraphicsPipeline mCircularPipeline, mFontPipeline;
    void InitChessboard(VkDevice device, uint32_t country);
    void DrawChessboard(VkCommandBuffer cmd, uint32_t windowWidth);
    void DrawChessboardBackground(VkCommandBuffer cmd, uint32_t windowWidth);
    void GetRectVertices(const glm::vec3&color, ChessboardVertex vertices[]);
    void CreateGraphicsPipeline(VkDevice device, uint32_t windowWidth, VkRenderPass renderpass, const std::vector<std::string>&shader, GraphicsPipeline&pipeline, const GraphicsPipelineStateInfo&pipelineState = {});
public:
    Chessboard(/* args */);
    ~Chessboard();
    //该函数只改变坐标,所以不需要关心调用的先后顺序(至少CreateVulkanResource是这样)
    void Cleanup(VkDevice device);
    void UpdateSet(VkDevice device);
    void InitChessboard(VkDevice device);
    void DestroyGraphicsPipeline(VkDevice device);
    void RecodeCommand(VkCommandBuffer cmd, uint32_t windowWidth);
    void CreatePipeline(VkDevice device, VkRenderPass renderpass, uint32_t windowWidth);
    void CreateVulkanResource(VkPhysicalDevice physicalDevice, VkDevice device, uint32_t windowWidth, VkQueue graphics, VulkanPool pool);

    //调用这个函数前应该先调用select函数。该函数会将select选中的棋子移动到row,column
    void Play(uint32_t row, uint32_t column);
    void Select(uint32_t row, uint32_t column);
    void UnSelect(uint32_t row, uint32_t column);
    //如果可以。试试去掉蜀国和吴国的2个兵(其他棋子也可以考虑)，已体现魏国的优势
    //绘制棋盘
    //接受棋子
    //输赢交给外部判断
    //应该交给棋子类,uniform也是
};
#endif