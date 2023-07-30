#ifndef CHESS_H
#define CHESS_H
#include "Pipeline.h"
#include "vulkanFrame.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#define CHESS_WIDTH 30
#define CHESS_HEIGHT CHESS_WIDTH
#define CHESS_FONT_IMAGE_WIDTH 30
#define CHESS_FONT_ROOT_PATH "./ChessFont/"
#define CHESS_FONT_IMAGE_HEIGHT CHESS_FONT_IMAGE_WIDTH
struct ChessVertex {
    glm::vec3 pos;
    glm::vec3 color;
    ChessVertex(const glm::vec3&pos, const glm::vec3&color){
        this->pos = pos;
        this->color = color;
    }
};
class Chess{
    glm::vec2 mPos;
    VulkanBuffer mPosition;
    VulkanBuffer mVertex, mIndex;
    VkDescriptorSet mSet, mFontSet;
    std::vector<uint16_t> mCircularIndices;
protected:
    VulkanImage mFont;
    void GetCircularVertex(const glm::vec3&color, std::vector<ChessVertex>&vertices);
    bool CreateTexture(VkDevice device, const char *file, VkCommandPool pool, VkQueue graphics);
public:
    Chess(/* args */);
    ~Chess();
    void Cleanup(VkDevice device);
    void CreateVertexAndIndex(VkDevice device, const glm::vec3&color, VkCommandPool pool, VkQueue graphics);
    //提供绘制圆形函数。字则由子类最后绘制
    void DrawFont(VkCommandBuffer cmd, GraphicsPipeline font);
    void DrawCircular(VkCommandBuffer cmd, GraphicsPipeline circular);

    virtual void UpdateUniform(VkDevice device, const glm::vec2&pos);
    void UpdateSet(VkDevice device, VkDescriptorPool pool, GraphicsPipeline font, GraphicsPipeline circular, VkSampler textureSampler);
    
    bool isSelect(const glm::vec2&pos);

    //能下的位置从参数返回
    virtual void Selected(std::vector<glm::vec2>&pos) = 0;
    virtual void CreateFontTexture(VkDevice device, VkCommandPool pool, VkQueue graphics) = 0;
};

class Wei:public Chess{
    glm::vec2 mFirstPos;
public:
    Wei();
    ~Wei();
    virtual void Selected(std::vector<glm::vec2>&pos);
    //一般情况下其他棋子不需要该函数。
    virtual void UpdateUniform(VkDevice device, const glm::vec2&pos);
    virtual void CreateFontTexture(VkDevice device, VkCommandPool pool, VkQueue graphics);
};

class Shu:public Chess{
    glm::vec2 mFirstPos;
public:
    Shu();
    ~Shu();
    virtual void Selected(std::vector<glm::vec2>&pos);
    //一般情况下其他棋子不需要该函数。
    virtual void UpdateUniform(VkDevice device, const glm::vec2&pos);
    virtual void CreateFontTexture(VkDevice device, VkCommandPool pool, VkQueue graphics);
};

class Wu:public Chess{
    glm::vec2 mFirstPos;
public:
    Wu();
    ~Wu();
    virtual void Selected(std::vector<glm::vec2>&pos);
    //一般情况下其他棋子不需要该函数。
    virtual void UpdateUniform(VkDevice device, const glm::vec2&pos);
    virtual void CreateFontTexture(VkDevice device, VkCommandPool pool, VkQueue graphics);
};

class Han:public Chess{
    glm::vec2 mFirstPos;
public:
    Han();
    ~Han();
    virtual void Selected(std::vector<glm::vec2>&pos);
    //一般情况下其他棋子不需要该函数。
    virtual void UpdateUniform(VkDevice device, const glm::vec2&pos);
    virtual void CreateFontTexture(VkDevice device, VkCommandPool pool, VkQueue graphics);
};
class Bing:public Chess{
    glm::vec2 mFirstPos;
public:
    Bing();
    ~Bing();
    virtual void Selected(std::vector<glm::vec2>&pos);
    virtual void CreateFontTexture(VkDevice device, VkCommandPool pool, VkQueue graphics);
};
class Pao:public Chess{
    glm::vec2 mFirstPos;
public:
    Pao();
    ~Pao();
    virtual void Selected(std::vector<glm::vec2>&pos);
    virtual void CreateFontTexture(VkDevice device, VkCommandPool pool, VkQueue graphics);
};
class Che:public Chess{
    glm::vec2 mFirstPos;
public:
    Che();
    ~Che();
    virtual void Selected(std::vector<glm::vec2>&pos);
    virtual void CreateFontTexture(VkDevice device, VkCommandPool pool, VkQueue graphics);
};
class Ma:public Chess{
    glm::vec2 mFirstPos;
public:
    Ma();
    ~Ma();
    virtual void Selected(std::vector<glm::vec2>&pos);
    virtual void CreateFontTexture(VkDevice device, VkCommandPool pool, VkQueue graphics);
};
class Xiang:public Chess{
    glm::vec2 mFirstPos;
public:
    Xiang();
    ~Xiang();
    virtual void Selected(std::vector<glm::vec2>&pos);
    virtual void CreateFontTexture(VkDevice device, VkCommandPool pool, VkQueue graphics);
};
class Shi:public Chess{
    glm::vec2 mFirstPos;
public:
    Shi();
    ~Shi();
    virtual void Selected(std::vector<glm::vec2>&pos);
    virtual void CreateFontTexture(VkDevice device, VkCommandPool pool, VkQueue graphics);
};

// void CreateFontImage(const std::string&fontfile, const std::string&outPath = CHESS_FONT_ROOT_PATH);
// bool GenerateFontImage(int bitmap_w, int bitmap_h, wchar_t word, const char *outImageName, const unsigned char *fontData, float pixels = 30);
// bool GenerateFontImage(int bitmap_w, int bitmap_h, const wchar_t *word, uint32_t wordCount, const char *outImageName, const unsigned char *fontData, float pixels = 30);
#endif