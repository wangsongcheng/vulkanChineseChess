#ifndef CHESS_H
#define CHESS_H
#include "Pipeline.h"
#include "vulkanFrame.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
//因为GetCountryBehind函数中的值是写死的，所以在修改颜色或国家位置后可能需要修改函数内的方向
#define WU_CHESS_COUNTRY_COLOR glm::vec3(0, 1, 0)
#define WEI_CHESS_COUNTRY_COLOR glm::vec3(0, 0, 1)
#define SHU_CHESS_COUNTRY_COLOR glm::vec3(1, 0, 0)
#define HAN_CHESS_COUNTRY_COLOR glm::vec3(1, 1, 0)

#define CHESSBOARD_LINE_WIDTH 1
#define CHESSBOARD_GRID_SIZE 40
#define ROW_TO_Y(ROW, GRIDSIZE, LINE_WIDTH)((GRIDSIZE) + (ROW) * (GRIDSIZE) + ((ROW) + 1) * LINE_WIDTH)
#define COLUMN_TO_X(COLUMN, GRIDSIZE, LINE_WIDTH)(ROW_TO_Y(COLUMN, GRIDSIZE, LINE_WIDTH))
#define CHESSBOARD_ROW_TO_Y(ROW)(ROW_TO_Y(ROW, CHESSBOARD_GRID_SIZE, CHESSBOARD_LINE_WIDTH))
// #define CHESSBOARD_ROW_TO_Y(ROW)(ROW_TO_Y(ROW, CHESSBOARD_GRID_SIZE, CHESSBOARD_LINE_WIDTH) - (CHESSBOARD_GRID_SIZE * .5 + CHESSBOARD_LINE_WIDTH))
#define CHESSBOARD_COLUMN_TO_X(COLUMN)(CHESSBOARD_ROW_TO_Y(COLUMN))

#define CHESS_WIDTH (CHESSBOARD_GRID_SIZE * .45)
#define CHESS_HEIGHT CHESS_WIDTH
#define CHESS_FONT_IMAGE_WIDTH CHESS_WIDTH
#define CHESS_FONT_ROOT_PATH "./ChessFont/"
#define CHESS_FONT_IMAGE_HEIGHT CHESS_FONT_IMAGE_WIDTH
struct Ranks{
    uint32_t row;
    uint32_t column;
    Ranks(){
    }
    Ranks(uint32_t row, uint32_t column){
        this->row = row;
        this->column = column;
    }
    ~Ranks(){
        
    }
};
struct ChessVertex {
    glm::vec3 pos;
    glm::vec3 color;
    ChessVertex(const glm::vec3&pos, const glm::vec3&color){
        this->pos = pos;
        this->color = color;
    }
};
struct ChessFontVertex {
    glm::vec3 pos;
    glm::vec2 uv;
    ChessFontVertex(const glm::vec3&pos, const glm::vec2&uv){
        this->pos = pos;
        this->uv = uv;
    }
};
class Chessboard;
class Chess{
    glm::vec2 mPos;
    VulkanBuffer mPosition;
    glm::vec3 mCountryColor;
    VulkanBuffer mFontPosition;
    VulkanBuffer mVertex, mIndex;
    VkDescriptorSet mSet, mFontSet;
    uint32_t mCircularIndicesCount;
    VulkanBuffer mFontVertex, mFontIndex;
    GraphicsPipeline mPipeline, mFontPipeline;
protected:
    VulkanImage mFont;
    uint32_t mRow, mColumn;//棋子目前所在的行列
    void DrawFont(VkCommandBuffer cmd);
    void DrawCircular(VkCommandBuffer cmd);
    void GetCircularVertex(const glm::vec3&color, std::vector<ChessVertex>&vertices);
    bool CreateTexture(VkDevice device, const char *file, VkCommandPool pool, VkQueue graphics);
    void CreateGraphicsPipeline(VkDevice device, uint32_t windowWidth, VkRenderPass renderpass, const std::vector<std::string>&shader, GraphicsPipeline&pipeline, const GraphicsPipelineStateInfo&pipelineState = {});
public:
    Chess(/* args */);
    ~Chess();
    void Cleanup(VkDevice device);
    void DestroyGraphicsPipeline(VkDevice device);
    void RecodeCommand(VkCommandBuffer cmd, uint32_t windowWidth);
    void CreatePipeline(VkDevice device, VkRenderPass renderpass, uint32_t windowWidth);
    void CreateVulkanResource(VkDevice device, const glm::vec3&color, const VulkanPool&pool, VkQueue graphics);

    void UpdateSet(VkDevice device, VkSampler fontSampler);
    virtual void UpdateUniform(VkDevice device, const glm::vec2&pos);
    virtual void UpdateUniform(VkDevice device, uint32_t row, uint32_t column);
    
    bool isSelect(const glm::vec2&pos);
    inline glm::vec3 GetCountryColor()const{
        return mCountryColor;
    }
    inline bool isSelect(uint32_t row, uint32_t column)const{
        return row == mRow && column == mColumn;
    }
    // inline void GetChessRanks(uint32_t&row, uint32_t&column)const{
    //     row = mRow;
    //     column = mColumn;
    // }
    //该函数不考虑九宫格
    virtual void Selected(const Chessboard *chessboard, std::vector<Ranks>&canplays) = 0;
    virtual void CreateFontTexture(VkDevice device, VkCommandPool pool, VkQueue graphics) = 0;
};

class Wei:public Chess{
public:
    Wei();
    ~Wei();
    virtual void Selected(const Chessboard *chessboard, std::vector<Ranks>&canplays);
    // //一般情况下其他棋子不需要该函数。//重写是因为需要记录一次mFirstRow, mFirstColumn
    // virtual void UpdateUniform(VkDevice device, uint32_t row, uint32_t column);
    virtual void CreateFontTexture(VkDevice device, VkCommandPool pool, VkQueue graphics);
};

class Shu:public Chess{
public:
    Shu();
    ~Shu();
    virtual void Selected(const Chessboard *chessboard, std::vector<Ranks>&canplays);
    // //一般情况下其他棋子不需要该函数。
    // virtual void UpdateUniform(VkDevice device, uint32_t row, uint32_t column);
    virtual void CreateFontTexture(VkDevice device, VkCommandPool pool, VkQueue graphics);
};

class Wu:public Chess{
public:
    Wu();
    ~Wu();
    virtual void Selected(const Chessboard *chessboard, std::vector<Ranks>&canplays);
    // //一般情况下其他棋子不需要该函数。
    // virtual void UpdateUniform(VkDevice device, uint32_t row, uint32_t column);
    virtual void CreateFontTexture(VkDevice device, VkCommandPool pool, VkQueue graphics);
};

class Han:public Chess{
public:
    Han();
    ~Han();
    virtual void Selected(const Chessboard *chessboard, std::vector<Ranks>&canplays);
    virtual void CreateFontTexture(VkDevice device, VkCommandPool pool, VkQueue graphics);
};
class Bing:public Chess{
public:
    Bing();
    ~Bing();
    virtual void Selected(const Chessboard *chessboard, std::vector<Ranks>&canplays);
    virtual void CreateFontTexture(VkDevice device, VkCommandPool pool, VkQueue graphics);
};
class Pao:public Chess{
public:
    Pao();
    ~Pao();
    virtual void Selected(const Chessboard *chessboard, std::vector<Ranks>&canplays);
    virtual void CreateFontTexture(VkDevice device, VkCommandPool pool, VkQueue graphics);
};
class Che:public Chess{
public:
    Che();
    ~Che();
    virtual void Selected(const Chessboard *chessboard, std::vector<Ranks>&canplays);
    virtual void CreateFontTexture(VkDevice device, VkCommandPool pool, VkQueue graphics);
};
class Ma:public Chess{
public:
    Ma();
    ~Ma();
    virtual void Selected(const Chessboard *chessboard, std::vector<Ranks>&canplays);
    virtual void CreateFontTexture(VkDevice device, VkCommandPool pool, VkQueue graphics);
};
class Xiang:public Chess{
public:
    Xiang();
    ~Xiang();
    virtual void Selected(const Chessboard *chessboard, std::vector<Ranks>&canplays);
    virtual void CreateFontTexture(VkDevice device, VkCommandPool pool, VkQueue graphics);
};
class Shi:public Chess{
public:
    Shi();
    ~Shi();
    virtual void Selected(const Chessboard *chessboard, std::vector<Ranks>&canplays);
    virtual void CreateFontTexture(VkDevice device, VkCommandPool pool, VkQueue graphics);
};

// void CreateFontImage(const std::string&fontfile, const std::string&outPath = CHESS_FONT_ROOT_PATH);
// bool GenerateFontImage(int bitmap_w, int bitmap_h, wchar_t word, const char *outImageName, const unsigned char *fontData, float pixels = 30);
// bool GenerateFontImage(int bitmap_w, int bitmap_h, const wchar_t *word, uint32_t wordCount, const char *outImageName, const unsigned char *fontData, float pixels = 30);
#endif