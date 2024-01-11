#ifndef VULKAN_CHESS_INCLUDE_H
#define VULKAN_CHESS_INCLUDE_H
#include <array>
#include "Pipeline.h"
#include "glm/glm.hpp"
#include "vulkanFrame.h"
#include "glm/gtc/matrix_transform.hpp"
#ifdef WIN32
#define M_PI 3.14159265358979323846
#endif
#define WU_COUNTRY_INDEX 2
#define WEI_COUNTRY_INDEX 0
#define SHU_COUNTRY_INDEX 1
#define HAN_COUNTRY_INDEX 3
#define INVALID_COUNTRY_INDEX -1
//这个值已经用到了至少一个循环, 修改的时候需要注意
#define MAX_COUNTRY_INDEX 4
//除汉外, 每个国家的棋子数
#define BING_CHESS_COUNT 5
#define PAO_CHESS_COUNT 2
#define CHE_CHESS_COUNT 2
#define MA_CHESS_COUNT 2
#define XIANG_CHESS_COUNT 2
#define SHI_CHESS_COUNT 2
#define JIANG_CHESS_COUNT 1

#define HAN_COUNTRY_CHE_CHESS_COUNT 3
#define HAN_COUNTRY_PAO_CHESS_COUNT 1
#define HAN_COUNTRY_JIANG_CHESS_COUNT 1

#define WU_CHESS_COUNTRY_COLOR glm::vec3(0, 1, 0)
#define WEI_CHESS_COUNTRY_COLOR glm::vec3(0, 0, 1)
#define SHU_CHESS_COUNTRY_COLOR glm::vec3(1, 0, 0)
#define HAN_CHESS_COUNTRY_COLOR glm::vec3(1, 1, 0)
#define ALIGN(val, alignment)((val + alignment - 1) & ~(alignment - 1))
#define ROW_COLUMN_INDEX(ROW_INDEX, COLUMN_INDEX, COLUMN)((ROW_INDEX) * (COLUMN) + (COLUMN_INDEX))

#define HAN_CHESS_COUNT (HAN_COUNTRY_CHE_CHESS_COUNT + HAN_COUNTRY_PAO_CHESS_COUNT + HAN_COUNTRY_JIANG_CHESS_COUNT)
#define WU_CHESS_COUNT (BING_CHESS_COUNT + PAO_CHESS_COUNT + CHE_CHESS_COUNT + MA_CHESS_COUNT + XIANG_CHESS_COUNT + SHI_CHESS_COUNT + JIANG_CHESS_COUNT)

#define WEI_CHESS_COUNT WU_CHESS_COUNT
#define SHU_CHESS_COUNT WU_CHESS_COUNT

//多出来的空间是为了容纳奖励棋子//为了后面方便一点, 别省空间了
#define COUNTRY_CHESS_COUNT (WEI_CHESS_COUNT + HAN_CHESS_COUNT)
// #define COUNTRY_CHESS_COUNT (WEI_CHESS_COUNT * 2 + HAN_CHESS_COUNT)
#define CHESSBOARD_CHESS_TOTAL (COUNTRY_CHESS_COUNT * 4)

#define FONT_INDEX_WU 2
#define FONT_INDEX_MA 7
#define FONT_INDEX_WEI 0
#define FONT_INDEX_SHU 1
#define FONT_INDEX_HAN 3
#define FONT_INDEX_PAO 5
#define FONT_INDEX_CHE 6
#define FONT_INDEX_SHI 9
#define FONT_INDEX_BING 4
#define FONT_INDEX_XIANG 8
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
struct FontVertex {
    glm::vec3 pos;
    glm::vec2 uv;
    FontVertex(const glm::vec3&pos, const glm::vec2&uv){
        this->pos = pos;
        this->uv = uv;
    }
};
struct BaseGraphics{
    VulkanBuffer index;
    VulkanBuffer vertex;
    uint32_t indexCount;
    uint32_t vertexCount;
    // uint32_t indexOffset;
    // uint32_t vertexOffset;
};
struct FontUniform{
    glm::mat4 model;
    float imageIndex;
};
//只创建一个，后面也只是用来绘制
class VulkanChess{
    struct{
        VkDescriptorSet font;
        VkDescriptorSet chess;
    }descriptorSet;
    struct{
        VulkanBuffer font;
        VulkanBuffer chess;
    }uniform;
    struct{
        GraphicsPipeline font;
        GraphicsPipeline chess;
    }pipelines;
    struct{
        VkSampler sampler;
        VulkanImage image;//魏, 蜀, 吴, 漢, 兵, 炮, 車, 馬, 相, 士
    }fonts;
    //mChess保存4种颜色的圆以及颜色较深的4种圆(魏蜀吴汉'不到该玩家颜色')
    BaseGraphics mChess, mFont;
    uint32_t mMinUniformBufferOffset;
    uint32_t mFontMinUniformBufferOffset;
    void DestroyFontResource(VkDevice device);
    void DestroyChessResource(VkDevice device);
    void DrawFont(VkCommandBuffer command, const GraphicsPipeline&pipeline);
    void GetCircularVertex(const glm::vec3&color, std::vector<Vertex>&vertices);
    void CreateFontResource(VkDevice device, VkCommandPool pool, VkQueue graphics);
    void CreateChessResource(VkDevice device, VkCommandPool pool, VkQueue graphics);
    void SetupDescriptorSet(VkDevice device, VkDescriptorSetLayout setLayout, VkDescriptorPool pool);
    void DrawChess(VkCommandBuffer command, uint32_t currentCountry, const GraphicsPipeline&pipeline);
protected:
    // void DrawGraphics(VkCommandBuffer command, const BaseGraphics *ptrGraphics);
public:
    VulkanChess(/* args */);
    ~VulkanChess();
    void Cleanup(VkDevice device);
    void Setup(VkPhysicalDevice physicalDevice, VkDevice device, VkDescriptorSetLayout setLayout, const VulkanPool&pool, VkQueue graphics);
    
    void RecordCommand(VkCommandBuffer cmd, uint32_t windowWidth, uint32_t currentCountry);

    void UpdateUniform(VkDevice device, const glm::vec2&pos, const VkExtent2D&size, uint32_t offset);
    void UpdateFontUniform(VkDevice device, uint32_t fontIndex, const glm::vec2&pos, const VkExtent2D&size, uint32_t offset);
    
    void DestroyGraphicsPipeline(VkDevice device);
    void CreatePipeline(VkDevice device, VkRenderPass renderpass, VkDescriptorSetLayout setLayout, VkPipelineCache pipelineCache, uint32_t windowWidth);
};
#endif