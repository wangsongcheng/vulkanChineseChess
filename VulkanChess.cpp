#include "Chess.h"
#include "VulkanChess.h"
#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"
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
void VulkanChess::DestroyFontResource(VkDevice device){
    mFont.Destroy(device);
    fonts.image.Destroy(device);
    uniform.font.Destroy(device);

    vkDestroySampler(device, fonts.sampler, nullptr);
}
void VulkanChess::DestroyChessResource(VkDevice device){
    mChess.Destroy(device);
    uniform.chess.Destroy(device);
}
void VulkanChess::DrawFont(VkCommandBuffer command, const GraphicsPipeline &pipeline){
    uint32_t dynamicOffsets;
    for (size_t i = 0; i < CHESSBOARD_CHESS_TOTAL; ++i){
        dynamicOffsets = i * mFontMinUniformBufferOffset;
        pipeline.BindDescriptorSet(command, descriptorSet.font, 1, &dynamicOffsets);
        mFont.Draw(command);
    }
}
void VulkanChess::GetCircularVertex(const glm::vec3 &color, std::vector<Vertex> &vertices){
    Vertex v = Vertex(glm::vec3(.0f), color);
    vertices.push_back(v);
    for (float i = 0; i < 360.0f; ++i){
        const float r = glm::radians(i);
        v.pos = glm::vec3(sin(r), cos(r), .0f);
        vertices.push_back(v);
    }
}
void VulkanChess::CreateFontResource(VkDevice device, VkCommandPool pool, VkQueue graphics){
    const uint16_t indices[] = { 0, 1, 2, 0, 3, 1 };
    const FontVertex vertices[] = {
        FontVertex(glm::vec3(.0f, 1.0f, .0f), glm::vec2(0.0f, 1.0f)),//左下
        FontVertex(glm::vec3(1.0f, .0f, .0f), glm::vec2(1.0f, 0.0f)),//右上
        FontVertex(glm::vec3(.0f, .0f, .0f), glm::vec2(0.0f, 0.0f)), //左上
        FontVertex(glm::vec3(1.0f, 1.0f, .0f), glm::vec2(1.0f, 1.0f))//右下
    };
    mFont.indexCount = 6;
    mFont.index.CreateBuffer(device, sizeof(indices), VK_BUFFER_USAGE_INDEX_BUFFER_BIT|VK_BUFFER_USAGE_TRANSFER_DST_BIT);
    mFont.vertex.CreateBuffer(device, sizeof(vertices), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT|VK_BUFFER_USAGE_TRANSFER_DST_BIT);
    mFont.index.AllocateAndBindMemory(device, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    mFont.vertex.AllocateAndBindMemory(device, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    vkf::tool::CopyBuffer(device, sizeof(indices), indices, graphics, pool, mFont.index);
    vkf::tool::CopyBuffer(device, sizeof(vertices), vertices, graphics, pool, mFont.vertex);

    uniform.font.CreateBuffer(device, mFontMinUniformBufferOffset * CHESSBOARD_CHESS_TOTAL, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
    uniform.font.AllocateAndBindMemory(device, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT|VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    uniform.font.size = mFontMinUniformBufferOffset;

	long int size = 0;
	unsigned char *fontBuffer = NULL;
	FILE *fontFile = fopen("fonts/SourceHanSerifCN-Bold.otf", "rb");
	if(fontFile){
		size = vkf::tool::GetFileSize(fontFile);
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
    unsigned char *datas = new unsigned char[imageSize * fontImageCount];
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
    memset(datas, 0, imageSize * fontImageCount);
    for (size_t i = 0; i < fontImageCount; ++i){
        GetFontImageData(fontBuffer, CHESS_WIDTH * 2, CHESS_HEIGHT * 2, word[i], datas + i * imageSize);
    }
    vkf::CreateFontImageArray(device, datas, fontImageCount, CHESS_WIDTH * 2, CHESS_HEIGHT * 2, fonts.image, pool, graphics);
    // delete[]temp;
    delete[]datas;
    fclose(fontFile);
    free(fontBuffer);
}
void VulkanChess::CreateChessResource(VkDevice device, VkCommandPool pool, VkQueue graphics){
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
        circularIndices.push_back(0);
        circularIndices.push_back(i);
        circularIndices.push_back(i + 1);
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
    mChess.index.CreateBuffer(device, sizeof(uint16_t) * circularIndices.size(), VK_BUFFER_USAGE_INDEX_BUFFER_BIT|VK_BUFFER_USAGE_TRANSFER_DST_BIT);
    //8是4种颜色+亮度较低的颜色(表示不是该玩家下子)
    mChess.vertex.CreateBuffer(device, sizeof(Vertex) * circularVertices.size() * 5, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT|VK_BUFFER_USAGE_TRANSFER_DST_BIT);
    mChess.index.AllocateAndBindMemory(device, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    mChess.vertex.AllocateAndBindMemory(device, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    vkf::tool::CopyBuffer(device, sizeof(uint16_t) * circularIndices.size(), circularIndices.data(), graphics, pool, mChess.index);
    vkf::tool::CopyBuffer(device, sizeof(Vertex) * circularVertices.size(), circularVertices.data(), graphics, pool, mChess.vertex);

    uniform.chess.CreateBuffer(device, mMinUniformBufferOffset * CHESSBOARD_CHESS_TOTAL, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
    uniform.chess.AllocateAndBindMemory(device, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT|VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    uniform.chess.size = mMinUniformBufferOffset;
}
void VulkanChess::SetupDescriptorSet(VkDevice device, VkDescriptorSetLayout setLayout, VkDescriptorPool pool){
    vkf::tool::AllocateDescriptorSets(device, pool, &setLayout, 1, &descriptorSet.font);
    vkf::tool::AllocateDescriptorSets(device, pool, &setLayout, 1, &descriptorSet.chess);
    
    VkDescriptorSetLayoutBinding descriptorSetLayoutBindings[2] = {};
    // descriptorSetLayoutBindings[0].binding = 0;
    descriptorSetLayoutBindings[0].descriptorCount = 1;
    descriptorSetLayoutBindings[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    descriptorSetLayoutBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
    descriptorSetLayoutBindings[1].binding = 1;
    descriptorSetLayoutBindings[1].descriptorCount = 1;
    descriptorSetLayoutBindings[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    descriptorSetLayoutBindings[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    vkf::tool::UpdateDescriptorSets(device, 1, descriptorSetLayoutBindings, {uniform.chess},{}, descriptorSet.chess);
    vkf::tool::UpdateDescriptorSets(device, 2, descriptorSetLayoutBindings, {uniform.font},{fonts.image}, descriptorSet.font, fonts.sampler);
}
void VulkanChess::DrawChess(VkCommandBuffer command, uint32_t currentCountry, const GraphicsPipeline&pipeline){
    uint32_t dynamicOffsets = 0;
    for (size_t uiCountry = 0; uiCountry < 4; ++uiCountry){
        for (size_t uiChess = 0; uiChess < COUNTRY_CHESS_COUNT; ++uiChess){
            dynamicOffsets = ROW_COLUMN_INDEX(uiCountry, uiChess, COUNTRY_CHESS_COUNT) * mMinUniformBufferOffset;
            pipeline.BindDescriptorSet(command, descriptorSet.chess, 1, &dynamicOffsets);
            mChess.Draw(command, uiCountry == currentCountry?uiCountry * mChess.vertexCount:(4 + uiCountry) * mChess.vertexCount);
        }
    }
    // for (size_t uiChess = 0; uiChess < HAN_CHESS_COUNT; ++uiChess){
    //     dynamicOffsets = ROW_COLUMN_INDEX(HAN_COUNTRY_INDEX, uiChess, countryChess[0]) * mMinUniformBufferOffset;
    //     pipeline.BindDescriptorSet(command, descriptorSet.chess, 1, &dynamicOffsets);
    //     vkCmdDrawIndexed(command, mChess.indexCount, 1, 0, HAN_COUNTRY_INDEX * mChess.vertexCount, 0);
    //     // DrawGraphics(command, &mChess);
    // }
}
// void VulkanChess::DrawGraphics(VkCommandBuffer command, const BaseGraphics *ptrGraphics){
//     VkDeviceSize offset = 0;
//     if(ptrGraphics){
//         vkCmdBindVertexBuffers(command, 0, 1, &ptrGraphics->vertex.buffer, &offset);
//         if(ptrGraphics->index.buffer != VK_NULL_HANDLE){
//             vkCmdBindIndexBuffer(command, ptrGraphics->index.buffer, 0, VK_INDEX_TYPE_UINT16);
//             vkCmdDrawIndexed(command, ptrGraphics->indexCount, 1, ptrGraphics->indexOffset, ptrGraphics->vertexOffset, 0);
//         }
//         else{
//             vkCmdDraw(command, ptrGraphics->vertexCount, 1, ptrGraphics->vertexOffset, 0);
//         }
//     }
// }

VulkanChess::VulkanChess(/* args */){
}

VulkanChess::~VulkanChess(){

}

void VulkanChess::Cleanup(VkDevice device){
    DestroyFontResource(device);
    DestroyChessResource(device);
}
void VulkanChess::Setup(VkPhysicalDevice physicalDevice, VkDevice device, VkDescriptorSetLayout setLayout, const VulkanPool &pool, VkQueue graphics){
    VkPhysicalDeviceProperties physicalDeviceProperties;
    vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);
    mMinUniformBufferOffset = ALIGN(sizeof(glm::mat4), physicalDeviceProperties.limits.minUniformBufferOffsetAlignment);
    mFontMinUniformBufferOffset = ALIGN(sizeof(FontUniform), physicalDeviceProperties.limits.minUniformBufferOffsetAlignment);

    vkf::CreateTextureSampler(device, fonts.sampler);
    CreateFontResource(device, pool.command, graphics);
    CreateChessResource(device, pool.command, graphics);

    SetupDescriptorSet(device, setLayout, pool.descriptor);
}

void VulkanChess::RecordCommand(VkCommandBuffer cmd, uint32_t windowWidth, uint32_t currentCountry){
    const glm::mat4 projection = glm::ortho(0.0f, (float)windowWidth, 0.0f, (float)windowWidth, -1.0f, 1.0f);
    pipelines.chess.BindPipeline(cmd);
    pipelines.chess.PushConstant(cmd, VK_SHADER_STAGE_VERTEX_BIT, sizeof(glm::mat4), &projection);
    
    mChess.Bind(cmd);
    DrawChess(cmd, currentCountry, pipelines.chess);

    pipelines.font.BindPipeline(cmd);
    pipelines.font.PushConstant(cmd, VK_SHADER_STAGE_VERTEX_BIT, sizeof(glm::mat4), &projection);

    mFont.Bind(cmd);
    DrawFont(cmd, pipelines.font);
}

void VulkanChess::UpdateUniform(VkDevice device, const glm::vec2 &pos, const VkExtent2D &size, uint32_t offset){
    const glm::mat4 model = glm::scale(glm::translate(glm::mat4(1), glm::vec3(pos, 0)), glm::vec3(size.width, size.height, 1));
    uniform.chess.UpdateData(device, &model, offset * mMinUniformBufferOffset);
}

void VulkanChess::UpdateFontUniform(VkDevice device, uint32_t fontIndex, const glm::vec2 &pos, const VkExtent2D &size, uint32_t offset){
    FontUniform fontUbo;
    fontUbo.imageIndex = fontIndex;
    fontUbo.model = glm::scale(glm::translate(glm::mat4(1), glm::vec3(pos, 0)), glm::vec3(size.width, size.height, 1));
    uniform.font.UpdateData(device, &fontUbo, offset * mFontMinUniformBufferOffset);
}

void VulkanChess::DestroyGraphicsPipeline(VkDevice device){
    pipelines.chess.DestroyLayout(device);
    pipelines.chess.DestroyPipeline(device);

    pipelines.font.DestroyLayout(device);
    pipelines.font.DestroyPipeline(device);
}

void VulkanChess::CreatePipeline(VkDevice device, VkRenderPass renderpass, VkDescriptorSetLayout setLayout, VkPipelineCache pipelineCache, uint32_t windowWidth){
    pipelines.chess.PushScissor(windowWidth, windowWidth);
    pipelines.chess.PushViewport(windowWidth, windowWidth);

    pipelines.chess.PushShader(device, VK_SHADER_STAGE_VERTEX_BIT, "shaders/baseVert.spv");
    pipelines.chess.PushShader(device, VK_SHADER_STAGE_FRAGMENT_BIT, "shaders/baseFrag.spv");

    pipelines.chess.PushVertexInputBindingDescription(sizeof(Vertex));
    pipelines.chess.PushPushConstant(sizeof(glm::mat4), VK_SHADER_STAGE_VERTEX_BIT);
    pipelines.chess.PushVertexInputAttributeDescription(0, 0, VK_FORMAT_R32G32B32_SFLOAT);
    pipelines.chess.PushVertexInputAttributeDescription(1, offsetof(Vertex, Vertex::color), VK_FORMAT_R32G32B32_SFLOAT);
    
    pipelines.chess.CreateLayout(device, {setLayout});
    pipelines.chess.CreatePipeline(device, renderpass, pipelineCache);

    pipelines.font.PushScissor(windowWidth, windowWidth);
    pipelines.font.PushViewport(windowWidth, windowWidth);

    pipelines.font.PushShader(device, VK_SHADER_STAGE_VERTEX_BIT, "shaders/baseFontVert.spv");
    pipelines.font.PushShader(device, VK_SHADER_STAGE_FRAGMENT_BIT, "shaders/baseFontFrag.spv");

    pipelines.font.PushVertexInputBindingDescription(sizeof(FontVertex));
    pipelines.font.PushPushConstant(sizeof(glm::mat4), VK_SHADER_STAGE_VERTEX_BIT);
    pipelines.font.PushVertexInputAttributeDescription(0, 0, VK_FORMAT_R32G32B32_SFLOAT);
    pipelines.font.PushVertexInputAttributeDescription(1, offsetof(FontVertex, FontVertex::uv), VK_FORMAT_R32G32_SFLOAT);
    
    pipelines.font.CreateLayout(device, {setLayout});
    pipelines.font.CreatePipeline(device, renderpass, pipelineCache);
}