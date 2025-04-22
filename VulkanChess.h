#ifndef VULKAN_CHESS_INCLUDE
#define VULKAN_CHESS_INCLUDE
#include "glm/glm.hpp"
#include "vulkanFrame.h"
#include "BaseGraphic.hpp"
#include "glm/gtc/matrix_transform.hpp"

#define WU_CHESS_COUNTRY_COLOR glm::vec3(0, 1, 0)
#define WEI_CHESS_COUNTRY_COLOR glm::vec3(0, 0, 1)
#define SHU_CHESS_COUNTRY_COLOR glm::vec3(1, 0, 0)
#define HAN_CHESS_COUNTRY_COLOR glm::vec3(1, 1, 0)

#ifndef ROW_COLUMN_TO_INDEX
#define ROW_COLUMN_TO_INDEX(ROW_INDEX, COLUMN_INDEX, COLUMN)((ROW_INDEX) * (COLUMN) + (COLUMN_INDEX))
#endif

//字体大小不一定要和棋子大小一样
#define FONT_WIDTH 40
#define FONT_HEIGHT FONT_WIDTH

#define FONT_INDEX_MA 7
#define FONT_INDEX_PAO 5
#define FONT_INDEX_CHE 6
#define FONT_INDEX_SHI 9
#define FONT_INDEX_BING 4
#define FONT_INDEX_XIANG 8

#define FONT_INDEX_WU 0
#define FONT_INDEX_SHU 1
#define FONT_INDEX_WEI 2
#define FONT_INDEX_HAN 3
#ifndef ALIGN
//如果val比alignment小，则返回alignment，否则如果val大于alignment但小于alignment*2则返回alignment*2以此类推
#define ALIGN(val, alignment)((val + alignment - 1) & ~(alignment - 1))
#endif
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
        VulkanFontImage image;//魏, 蜀, 吴, 漢, 兵, 炮, 車, 馬, 相, 士
    }fonts;
    struct{
        uint32_t chessCount;
        uint32_t countryCount;
    }game;
    Rect mFont;
    Circle mChess;
    void UpdateDescriptorSet(VkDevice device);
    void GetCircularVertex(const glm::vec3&color, std::vector<Vertex>&vertices);
    void CreateFontResource(VulkanDevice device, VulkanPool pool, VkQueue graphics);
    void CreateChessResource(VulkanDevice device, VulkanPool pool, VkQueue graphics);
public:
    VulkanChess(/* args */);
    ~VulkanChess();
    void Cleanup(VkDevice device);
    void Setup(VulkanDevice device, uint32_t countryCount, uint32_t chessCount, VkDescriptorSetLayout layout, VkQueue graphics, VulkanPool pool);

    void UpdateUniform(VkDevice device, uint32_t fontIndex, const glm::vec2&pos, uint32_t width, uint32_t height, uint32_t dynamicOffsets);
    //应该拆成独立函数，供外部调用
    void DrawFont(VkCommandBuffer command, VkPipelineLayout layout);
    void DrawChess(VkCommandBuffer command, VkPipelineLayout layout, uint32_t currentCountry);
};
#endif