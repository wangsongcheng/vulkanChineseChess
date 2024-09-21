#ifndef VULKAN_CHESS_INCLUDE
#define VULKAN_CHESS_INCLUDE
#include "glm/glm.hpp"
#include "vulkanFrame.h"
#include "glm/gtc/matrix_transform.hpp"
#define CHESS_WIDTH 20
#define CHESS_HEIGHT CHESS_WIDTH

#define WU_CHESS_COUNTRY_COLOR glm::vec3(0, 1, 0)
#define WEI_CHESS_COUNTRY_COLOR glm::vec3(0, 0, 1)
#define SHU_CHESS_COUNTRY_COLOR glm::vec3(1, 0, 0)
#define HAN_CHESS_COUNTRY_COLOR glm::vec3(1, 1, 0)

#ifndef ALIGN
//如果val比alignment小，则返回alignment，否则如果val大于alignment但小于alignment*2则返回alignment*2以此类推
#define ALIGN(val, alignment)((val + alignment - 1) & ~(alignment - 1))
#endif
//这个值已经用到了, 修改的时候需要注意
#define MAX_COUNTRY_INDEX 4
//修改势力索引的时候需要该字的顺序
#define WU_COUNTRY_INDEX 1
#define WEI_COUNTRY_INDEX 2
#define SHU_COUNTRY_INDEX 0
#define HAN_COUNTRY_INDEX 3
#define INVALID_COUNTRY_INDEX -1
//除汉外, 每个国家的棋子数
#define BING_CHESS_COUNT 5
#define PAO_CHESS_COUNT 2
#define CHE_CHESS_COUNT 2
#define MA_CHESS_COUNT 2
#define XIANG_CHESS_COUNT 2
#define SHI_CHESS_COUNT 2
#define JIANG_CHESS_COUNT 1

#define HAN_CHE_CHESS_COUNT 3
#define HAN_PAO_CHESS_COUNT 1
#define HAN_JIANG_CHESS_COUNT 1

#define WU_CHESS_COUNT (BING_CHESS_COUNT + PAO_CHESS_COUNT + CHE_CHESS_COUNT + MA_CHESS_COUNT + XIANG_CHESS_COUNT + SHI_CHESS_COUNT + JIANG_CHESS_COUNT)
#ifdef HAN_CAN_PLAY
#define HAN_CHESS_COUNT WU_CHESS_COUNT
#else
#define HAN_CHESS_COUNT (HAN_CHE_CHESS_COUNT + HAN_PAO_CHESS_COUNT + HAN_JIANG_CHESS_COUNT)
#endif
#define WEI_CHESS_COUNT WU_CHESS_COUNT
#define SHU_CHESS_COUNT WU_CHESS_COUNT

#define COUNTRY_CHESS_COUNT WEI_CHESS_COUNT

#define CHESSBOARD_CHESS_TOTAL (DRAW_COUNTRY_CHESS_COUNT * 4)

#define DRAW_COUNTRY_CHESS_COUNT (COUNTRY_CHESS_COUNT + HAN_CHESS_COUNT)

#ifndef ROW_COLUMN_INDEX
#define ROW_COLUMN_INDEX(ROW_INDEX, COLUMN_INDEX, COLUMN)((ROW_INDEX) * (COLUMN) + (COLUMN_INDEX))
#endif

//索引值理论可以随意修改;但因为数组有限,所以最大索引应该小于数组大小 
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
        float imageIndex;
    };
    struct{
        VkDescriptorSet font;
        VkDescriptorSet chess;
    }descriptorSet;
    struct{
        VulkanBuffer font;
        VulkanBuffer chess;
    }uniform;
    // struct{
    //     GraphicsPipeline font;
    //     GraphicsPipeline chess;
    // }pipelines;
    struct{
        VkSampler sampler;
        VulkanImage image;//魏, 蜀, 吴, 漢, 兵, 炮, 車, 馬, 相, 士
    }fonts;
    BaseGraphic mChess, mFont;
    uint32_t mMinUniformBufferOffset;
    uint32_t mFontMinUniformBufferOffset;
    void GetCircularVertex(const glm::vec3&color, std::vector<Vertex>&vertices);
    void CreateFontResource(VkDevice device, VkCommandPool pool, VkQueue graphics);
    void CreateChessResource(VkDevice device, VkCommandPool pool, VkQueue graphics);
    void SetupDescriptorSet(VkDevice device, VkDescriptorSetLayout layout, VkDescriptorPool pool);
public:
    VulkanChess(/* args */);
    ~VulkanChess();
    void Cleanup(VkDevice device);
    void Setup(VkPhysicalDevice physicalDevice, VkDevice device, VkDescriptorSetLayout setLayout, VkQueue graphics, VulkanPool pool);

    void UpdateUniform(VkDevice device, uint32_t fontIndex, const glm::vec3&pos, uint32_t dynamicOffsets);

    void DrawFont(VkCommandBuffer command, VkPipelineLayout layout);
    void DrawChess(VkCommandBuffer command, VkPipelineLayout layout, uint32_t currentCountry);
};
#endif