#ifndef CHESSBOARD_H
#define CHESSBOARD_H
#include "chess.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#define WU_CHESS_INDEX 2
#define WEI_CHESS_INDEX 0
#define SHU_CHESS_INDEX 1
#define HAN_CHESS_INDEX 3
#define UNKNOW_CHESS_INDEX 4

#define SELECT_CHESS_UNIFORM_MAX 70

//只要绘制线框时没有加线宽偏移的话，下面的值就不应该修改。因为图形管线线宽默认1
#define CHESSBOARD_LINE_WIDTH 1//chess.h包含了该值,修改后可能需要修改chess.h的值//目前先这样
#define CHESSBOARD_GRID_SIZE 40//chess.h包含了该值,修改后可能需要修改chess.h的值//目前先这样
#define ALIGN(val, alignment)((val + alignment - 1) & ~(alignment - 1))
// 大网格除了4个CHESSBOARD_GRID_SIZE还包含线条的宽度
#define CHESSBOARD_GRID_SIZE_BIG (CHESSBOARD_GRID_SIZE * 4 + CHESSBOARD_LINE_WIDTH * 3)
#define CHESSBOARD_BLACK_BACKGROUND_SIZE(WINDOW_WIDTH)(WINDOW_WIDTH - 2 * CHESSBOARD_GRID_SIZE)
#define CHESSBOARD_ROW_COLUMN_INDEX(ROW_INDEX, COLUMN_INDEX, COLUMN)(ROW_INDEX * COLUMN + COLUMN_INDEX)

#define ROW_TO_Y(ROW, GRIDSIZE, LINE_WIDTH)((GRIDSIZE) + (ROW) * (GRIDSIZE) + ((ROW) + 1) * LINE_WIDTH)
#define COLUMN_TO_X(COLUMN, GRIDSIZE, LINE_WIDTH)(ROW_TO_Y(COLUMN, GRIDSIZE, LINE_WIDTH))

// #define ROW_COLUMN_TO_COORD(ROW, COLUMN, GRIDSIZE)()
//除非增加新棋,否则以下棋子的值应该在16以下
#define JIANG_CHESS 0
#define MA_CHESS_1 1
#define MA_CHESS_2 2
#define SHI_CHESS_1 3
#define SHI_CHESS_2 4
#define BING_CHESS_1 5
#define BING_CHESS_2 6
#define BING_CHESS_3 7
#define BING_CHESS_4 8
#define BING_CHESS_5 9
#define XIANG_CHESS_1 10
#define XIANG_CHESS_2 11
#define PAO_CHESS_1 12
#define PAO_CHESS_2 13
#define PAO_CHESS_3 14
#define CHE_CHESS_1 15
#define CHE_CHESS_2 16
#define CHE_CHESS_3 17
#define CHE_CHESS_4 18
#define CHE_CHESS_5 19
struct ChessboardVertex {
    glm::vec3 pos;
    glm::vec3 color;
    glm::uint pointSize;
    ChessboardVertex(){
    }
    ChessboardVertex(const glm::vec3&pos, const glm::vec3&color, const glm::uint&pointSize){
        this->pos = pos;
        this->color = color;
        this->pointSize = pointSize;
    }
};
struct ChessboardRectVertex {
    glm::vec3 pos;
    glm::vec3 color;
    ChessboardRectVertex(){
    }
    ChessboardRectVertex(const glm::vec3&pos, const glm::vec3&color){
        this->pos = pos;
        this->color = color;
    }
};
struct ChessboardChess{
    //原棋子数为16,因为汉需要3个車,比原棋子多1个, 所以为17
    //在占领漢后, 需要3个車和1个炮来获取漢的車和炮//因为非漢势力有一个車的位置用不到，所以只需要增加2个車
    Chess *chess[20];
    // Chess *jiang;//实际上是魏蜀吴汉
    // Chess *ma[2];
    // Chess *pao[2];
    // Chess *che[2];
    // Chess *shi[2];
    // Chess *bing[5];
    // Chess *xiang[2];
};
// struct PushConstant{
//     glm::mat4 projection;
// };
//两个成员必须一样大才能一次送进uniform
// struct PointUniform{
//     glm::mat4 model;
//     // glm::uint pointSize;
// };
struct ChessboardWireframe{
    VkDescriptorSet set;
    VulkanBuffer uniform;
    GraphicsPipeline pipeline;
    VulkanBuffer vert, index[2];//2个顶点索引缓冲。顶点可以共用//0为/
};
//某些设备绘制的点大小有限制。为了保证效果, 背景一律用矩形绘制
struct ChessboardBackground{
    VulkanBuffer pos;
    VkDescriptorSet set;
    VulkanBuffer vert[2];//1个棋盘颜色，1个黑色
    GraphicsPipeline pipeline;
};
struct ChessboardGrid{
    VulkanBuffer vert;
    VkDescriptorSet set;
    VulkanBuffer uniform;
    GraphicsPipeline pipeline;
};
struct ChessboardSelectChess{
    VkDescriptorSet set;
    VulkanBuffer uniform;
    void UpdateUniform(VkDevice device, const std::vector<Ranks>&canplays){
        glm::mat4 model[SELECT_CHESS_UNIFORM_MAX];
        const uint32_t uiCanplays = canplays.size();
        if(uiCanplays > sizeof(model) / sizeof(glm::mat4)){
            printf("%s函数:uiCanplays的数量:%d, 大于model[%d]的数量:%d\n", __FUNCTION__, uiCanplays, sizeof(model) / sizeof(glm::mat4), sizeof(model) / sizeof(glm::mat4));
            return;
        }
        const glm::mat4 scale = glm::scale(glm::mat4(1), glm::vec3(CHESSBOARD_GRID_SIZE, CHESSBOARD_GRID_SIZE, 1));
        for (size_t i = 0; i < sizeof(model) / sizeof(glm::mat4); ++i){
            model[i] = glm::translate(glm::mat4(1), glm::vec3(COLUMN_TO_X(100, CHESSBOARD_GRID_SIZE, CHESSBOARD_LINE_WIDTH), ROW_TO_Y(100, CHESSBOARD_GRID_SIZE, CHESSBOARD_LINE_WIDTH), 0)) * scale;
        }
        for (size_t i = 0; i < canplays.size(); ++i){
            model[i] = glm::translate(glm::mat4(1), glm::vec3(COLUMN_TO_X(canplays[i].column, CHESSBOARD_GRID_SIZE, CHESSBOARD_LINE_WIDTH) - CHESSBOARD_GRID_SIZE * .5, ROW_TO_Y(canplays[i].row, CHESSBOARD_GRID_SIZE, CHESSBOARD_LINE_WIDTH) - CHESSBOARD_GRID_SIZE * .5, 0)) * scale;
        }
        uniform.UpdateData(device, sizeof(model), model);
    }
};
class Chessboard{
    Chess *mSelected;
    // float mPointSizeRange;
    Ranks mPalacePos[5];
    //魏蜀吴汉
    ChessboardChess mChess[4];

    std::vector<Ranks>mCanplays;

    ChessboardGrid mGrid;
    uint32_t mWindowWidth;
    ChessboardWireframe mWireframe;
    ChessboardBackground mBackground;
    ChessboardSelectChess mSelectChess;
    int32_t mPointMinUniformBufferOffset, mRectMinUniformBufferOffset;
    // //棋类的管线//在没有更好的办法前先这么写
    // GraphicsPipeline mCircularPipeline, mFontPipeline;
    void DrawChessboard(VkCommandBuffer cmd);
    void DrawSelectChessWireframe(VkCommandBuffer cmd);
    void DrawChessboardBackground(VkCommandBuffer cmd);
    void DrawChess(VkCommandBuffer cmd, const glm::vec3&nowCountryColor);
    void GetRectVertices(const glm::vec3&color, ChessboardVertex vertices[]);
    void InitChess(VkDevice device, uint32_t country);
    void InitChessForVulkan(VkDevice device, uint32_t country, uint32_t windowWidth, VkRenderPass renderpass, VulkanPool pool, VkQueue graphics, VkSampler fontSampler);
    void CreateGraphicsPipeline(VkDevice device, uint32_t windowWidth, VkRenderPass renderpass, const std::vector<std::string>&shader, GraphicsPipeline&pipeline, const GraphicsPipelineStateInfo&pipelineState = {});
public:
    Chessboard(/* args */);
    ~Chessboard();
    //该函数只改变坐标,所以不需要关心调用的先后顺序(至少CreateVulkanResource是这样)
    void Cleanup(VkDevice device);
    void UpdateSet(VkDevice device);
    void InitChessboard(VkDevice device);
    void DestroyGraphicsPipeline(VkDevice device);
    void DestroyChess(VkDevice device, uint32_t country);
    void RecodeCommand(VkCommandBuffer cmd, const glm::vec3&nowCountryColor);
    void CreatePipeline(VkDevice device, VkRenderPass renderpass, uint32_t windowWidth);
    void CreateVulkanResource(VkPhysicalDevice physicalDevice, VkDevice device, uint32_t windowWidth, VkQueue graphics, VulkanPool pool, VkRenderPass renderpass, VkSampler fontSampler);

    //调用这个函数前应该先调用select函数。该函数会将select选中的棋子移动到row,column
    // void Select(uint32_t row, uint32_t column);
    void UnSelect(VkDevice device);
    void Select(VkDevice device, const glm::vec2&pos);
    bool IsInPalace(uint32_t row, uint32_t column)const;
    bool IsPalaceCenter(uint32_t row, uint32_t column)const;
    //如果返回真, country表示被灭亡的势力。否则该值无效
    bool Play(VkDevice device, const glm::vec2&pos, uint32_t&country, bool&next);
    void SelectChessInPalace(const Chess *ptrChess, std::vector<Ranks>&canplays)const;
    bool Play(VkDevice device, uint32_t row, uint32_t column, uint32_t&country, bool&next);
    //如果返回真, 表示目标势力灭亡
    bool CaptureChess(VkDevice device, const Chess *src, uint32_t dstCountry, uint32_t destChess);

    inline bool IsPerish(uint32_t country)const{
        return !mChess[country].chess[JIANG_CHESS];
    }
    //返回真表示在边界上，不能下棋
    bool IsBoundary(uint32_t row, uint32_t column)const;
    //如果返回null,说明该位置没有棋子
    Chess *GetChess(const glm::vec2&pos)const;
    Chess *GetChess(uint32_t row, uint32_t column)const;

    inline bool IsSelfChess(const glm::vec3&color){
        return mSelected->GetCountryColor() == color;
    }
    inline bool IsSelected(){
        return mSelected;
    }
    // inline glm::vec2 GetCountryFront(){
        
    // }
    //返回下个势力
    // inline glm::vec3 NextCountry(){
    //     const glm::vec3 color[] = { WEI_CHESS_COUNTRY_COLOR, SHU_CHESS_COUNTRY_COLOR, WU_CHESS_COUNTRY_COLOR };
    //     static uint32_t index = 0;
    //     const glm::vec3 next = color[index];
    //     index = (index + 1) % sizeof(color) / sizeof(glm::vec2);
    //     return next;
    // }
    glm::vec2 GetCountryBehind(const glm::vec3&countryColor)const;
    //如果可以。试试去掉蜀国和吴国的2个兵(其他棋子也可以考虑)，已体现魏国的优势
    //输赢交给外部判断
};
#endif