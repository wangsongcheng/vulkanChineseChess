#include <string.h>
#include <iostream>
#include "VulkanChess.h"
#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"
uint32_t GetFileSize(FILE *fp){
    uint32_t size = 0;
    if(fp){
        fseek(fp, 0, SEEK_END);
        size = ftell(fp);
        fseek(fp, 0, SEEK_SET);
    }
    return size;
}
void GetFontImageData(const unsigned char *fontData, int bitmap_w, int bitmap_h, wchar_t word, unsigned char *out){
	stbtt_fontinfo info;
    const float pixels = bitmap_w;
	if(!stbtt_InitFont(&info, fontData, 0)){
		printf("in function:%s, stbtt_InitFont error\n", __FUNCTION__);
        return;
	}
	float scale = stbtt_ScaleForPixelHeight(&info, pixels);
	int ascent = 0, descent = 0, lineGap = 0;
	stbtt_GetFontVMetrics(&info, &ascent, &descent, &lineGap);
	ascent = roundf(ascent * scale);
	descent = roundf(descent * scale);
    int advanceWidth = 0, leftSideBearing = 0, c_x1, c_y1, c_x2, c_y2;
    stbtt_GetCodepointHMetrics(&info, word, &advanceWidth, &leftSideBearing);
    stbtt_GetCodepointBitmapBox(&info, word, scale, scale, &c_x1, &c_y1, &c_x2, &c_y2);
    int y = ascent +c_y1;
    int byteOffset = roundf(leftSideBearing * scale) + (y * bitmap_w);
    stbtt_MakeCodepointBitmap(&info, out + byteOffset, c_x2 - c_x1, c_y2 - c_y1, bitmap_w, scale, scale, word);
}
void VulkanChess::GetCircularVertex(const glm::vec3 &color, std::vector<Vertex> &vertices){
    Vertex v;
    v.color = color;
    vertices.emplace_back(Vertex(glm::vec3(0, 0, 0), color));
    for (float t = 0; t < 1; t += .01f){
        v.pos = glm::vec3(cos(t * 2 * glm::pi<float>()), sin(t * 2 * glm::pi<float>()), 0);
        vertices.push_back(v);
    }
}
void VulkanChess::CreateFontResource(VulkanDevice device, VulkanPool pool, VkQueue graphics){
    const uint16_t indices[] = { 0, 1, 2, 0, 3, 1 };
    const Vertex vertices[] = {
        Vertex(glm::vec3(.0f, 1.0f, .0f), glm::vec3(0.0f, 1.0f, 0)),//左下
        Vertex(glm::vec3(1.0f, .0f, .0f), glm::vec3(1.0f, 0.0f, 0)),//右上
        Vertex(glm::vec3(.0f, .0f, .0f), glm::vec3(0.0f, 0.0f, 0)), //左上
        Vertex(glm::vec3(1.0f, 1.0f, .0f), glm::vec3(1.0f, 1.0f, 0))//右下
    };
    mFont.CreateIndexBuffer(device, indices, sizeof(indices), graphics, pool);
    mFont.CreateVertexBuffer(device, vertices, sizeof(vertices), 4, graphics, pool);
    
	long int size = 0;
	unsigned char *fontBuffer = NULL;
	FILE *fontFile = fopen("fonts/SourceHanSerifCN-Bold.otf", "rb");
	if(fontFile){
		size = GetFileSize(fontFile);
		fontBuffer = (unsigned char *)malloc(size);
		fread(fontBuffer, size, 1, fontFile);
	}
	else{
		perror("open file error");
		printf("file name is fonts/SourceHanSerifCN-Bold.otf\n");
        return;
	}
    const uint32_t fontImageCount = 10;
    const uint32_t imageSize = CHESS_WIDTH * 2 * CHESS_HEIGHT * 2;
    std::vector<unsigned char *>datas(fontImageCount);
    wchar_t word[10];
    word[FONT_INDEX_MA] = L'馬';
    word[FONT_INDEX_WU] = L'吴';
    word[FONT_INDEX_SHU] = L'蜀';
    word[FONT_INDEX_WEI] = L'魏';
    word[FONT_INDEX_HAN] = L'漢';
    word[FONT_INDEX_PAO] = L'炮';
    word[FONT_INDEX_CHE] = L'車';
    word[FONT_INDEX_SHI] = L'士';
    word[FONT_INDEX_BING] = L'兵';
    word[FONT_INDEX_XIANG] = L'相';
    for (size_t i = 0; i < fontImageCount; ++i){
        datas[i] = new unsigned char[imageSize];
        memset(datas[i], 0, imageSize);
        GetFontImageData(fontBuffer, CHESS_WIDTH * 2, CHESS_HEIGHT * 2, word[i], datas[i]);
    }
    fonts.image.CreateImageArray(device, (void *const *)datas.data(), fontImageCount, CHESS_WIDTH * 2, CHESS_HEIGHT * 2, 1, graphics, pool);
    for (auto&it:datas){
        delete[]it;
    }
    fclose(fontFile);
    free(fontBuffer);
}
void VulkanChess::CreateChessResource(VulkanDevice device, VulkanPool pool, VkQueue graphics){
    //这里得准备4*2种颜色的棋子
    std::vector<Vertex> circularVertices;
    glm::vec3 countryColor[4];
    countryColor[WU_COUNTRY_INDEX] = WU_CHESS_COUNTRY_COLOR; 
    countryColor[WEI_COUNTRY_INDEX] = WEI_CHESS_COUNTRY_COLOR; 
    countryColor[SHU_COUNTRY_INDEX] = SHU_CHESS_COUNTRY_COLOR; 
    countryColor[HAN_COUNTRY_INDEX] = HAN_CHESS_COUNTRY_COLOR; 

    GetCircularVertex(countryColor[0], circularVertices);
    mChess.vertexCount = circularVertices.size();
    std::vector<uint16_t> circularIndices;
    for (size_t i = 1; i < circularVertices.size(); ++i){
        circularIndices.push_back(i);
        circularIndices.push_back(i + 1);
        circularIndices.push_back(0);
    }
    mChess.indexCount = circularIndices.size();
    for (size_t uiCountry = 1; uiCountry < sizeof(countryColor) / sizeof(glm::vec3); ++uiCountry){
        GetCircularVertex(countryColor[uiCountry], circularVertices);
    }
    for (size_t uiCountry = 0; uiCountry < sizeof(countryColor) / sizeof(glm::vec3); ++uiCountry){
        if(uiCountry != HAN_COUNTRY_INDEX){
            const glm::vec3 banColor = glm::vec3(countryColor[uiCountry].r * .6, countryColor[uiCountry].g * .6, countryColor[uiCountry].b * .6);
            GetCircularVertex(banColor, circularVertices);
        }
        else{
            GetCircularVertex(countryColor[uiCountry], circularVertices);
        }
    }
    mChess.index.CreateBuffer(device, sizeof(uint16_t) * circularIndices.size(), VK_BUFFER_USAGE_INDEX_BUFFER_BIT|VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    //8是4种颜色+亮度较低的颜色(表示不是该玩家下子)
    mChess.vertex.CreateBuffer(device, sizeof(Vertex) * circularVertices.size(), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT|VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    mChess.index.UpdateData(device, circularIndices.data(), graphics, pool);
    mChess.vertex.UpdateData(device, circularVertices.data(), graphics, pool);
}
void VulkanChess::SetupDescriptorSet(VkDevice device, VkDescriptorSetLayout layout, VulkanPool pool){
    pool.AllocateDescriptorSets(device, &layout, 1, &descriptorSet.font);
    pool.AllocateDescriptorSets(device, &layout, 1, &descriptorSet.chess);
    
    VkDescriptorSetLayoutBinding descriptorSetLayoutBindings[2] = {};
    // descriptorSetLayoutBindings[0].binding = 0;
    descriptorSetLayoutBindings[0].descriptorCount = 1;
    descriptorSetLayoutBindings[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    descriptorSetLayoutBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
    descriptorSetLayoutBindings[1].binding = 1;
    descriptorSetLayoutBindings[1].descriptorCount = 1;
    descriptorSetLayoutBindings[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    descriptorSetLayoutBindings[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    vulkanFrame::UpdateDescriptorSets(device, descriptorSetLayoutBindings, 1, {uniform.chess},{}, descriptorSet.chess);
    vulkanFrame::UpdateDescriptorSets(device, descriptorSetLayoutBindings, 2, {uniform.font},{fonts.image}, descriptorSet.font, fonts.sampler);
}
VulkanChess::VulkanChess(/* args */){
}
VulkanChess::~VulkanChess(){
}

void VulkanChess::Cleanup(VkDevice device){
    mFont.Destroy(device);
    mChess.Destroy(device);
    fonts.image.Destroy(device);
    uniform.font.Destroy(device);
    uniform.chess.Destroy(device);
    vkDestroySampler(device, fonts.sampler, VK_NULL_HANDLE);
}

void VulkanChess::Setup(VulkanDevice device, VkDescriptorSetLayout setLayout, VkQueue graphics, VulkanPool pool){
    auto&physicalDeviceProperties = device.GetPhysicalDeviceProperties();
    uint32_t minUniformBufferOffset = ALIGN(sizeof(glm::mat4), physicalDeviceProperties.limits.minUniformBufferOffsetAlignment);
    uint32_t mFontMinUniformBufferOffset = ALIGN(sizeof(FontUniform), physicalDeviceProperties.limits.minUniformBufferOffsetAlignment);

    vulkanFrame::CreateTextureSampler(device.device, fonts.sampler);
    CreateFontResource(device, pool, graphics);
    CreateChessResource(device, pool, graphics);
    
    uniform.chess.CreateBuffer(device, minUniformBufferOffset * CHESSBOARD_CHESS_TOTAL, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT|VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    uniform.chess.size = minUniformBufferOffset;

    uniform.font.CreateBuffer(device, mFontMinUniformBufferOffset * CHESSBOARD_CHESS_TOTAL, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT|VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    uniform.font.size = mFontMinUniformBufferOffset;

    SetupDescriptorSet(device.device, setLayout, pool);
}

void VulkanChess::UpdateUniform(VkDevice device, uint32_t fontIndex, const glm::vec3&pos, uint32_t dynamicOffsets){
    const glm::mat4 model = glm::scale(glm::translate(glm::mat4(1), pos), glm::vec3(CHESS_WIDTH, CHESS_HEIGHT, 1));
    uniform.chess.UpdateData(device, &model, dynamicOffsets * uniform.chess.size);

    FontUniform fontUbo;
    fontUbo.imageIndex = fontIndex;
    glm::vec3 fontPos = pos;
    fontPos.x -=  CHESS_WIDTH * .8;
    fontPos.y -= CHESS_HEIGHT * 1;
    fontUbo.model = glm::scale(glm::translate(glm::mat4(1), fontPos), glm::vec3(CHESS_WIDTH * 2, CHESS_HEIGHT * 2, 1));
    uniform.font.UpdateData(device, &fontUbo, dynamicOffsets * uniform.font.size);
}

void VulkanChess::DrawFont(VkCommandBuffer command, VkPipelineLayout layout){
    uint32_t dynamicOffsets;
    mFont.Bind(command);
    for (size_t i = 0; i < CHESSBOARD_CHESS_TOTAL; ++i){
        dynamicOffsets = i * uniform.font.size;
        vkCmdBindDescriptorSets(command, VK_PIPELINE_BIND_POINT_GRAPHICS, layout, 0, 1, &descriptorSet.font, 1, &dynamicOffsets);
        mFont.Draw(command);
    }
}

void VulkanChess::DrawChess(VkCommandBuffer command, VkPipelineLayout layout, uint32_t currentCountry){
    mChess.Bind(command);
    uint32_t dynamicOffsets = 0;
    for (size_t uiCountry = 0; uiCountry < MAX_COUNTRY_INDEX; ++uiCountry){
        for (size_t uiChess = 0; uiChess < DRAW_COUNTRY_CHESS_COUNT; ++uiChess){
            dynamicOffsets = ROW_COLUMN_TO_INDEX(uiCountry, uiChess, DRAW_COUNTRY_CHESS_COUNT) * uniform.chess.size;
            vkCmdBindDescriptorSets(command, VK_PIPELINE_BIND_POINT_GRAPHICS, layout, 0, 1, &descriptorSet.chess, 1, &dynamicOffsets);
            mChess.Draw(command, uiCountry == currentCountry?uiCountry * mChess.vertexCount:(4 + uiCountry) * mChess.vertexCount);
        }
    }
}
