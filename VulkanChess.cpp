#include "chess.h"
#include "VulkanChess.h"
#include "imgui_impl_vulkan.h"
#define STB_TRUETYPE_IMPLEMENTATION
// #define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_truetype.h"
// #include "stb_image_write.h"
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
    // stbtt_MakeCodepointBitmap(&info, bitmap + byteOffset, c_x2 - c_x1, c_y2 - c_y1, bitmap_w, scale, scale, word);

    // // unsigned int *TexPixelsRGBA32 = (unsigned int*)malloc(bitmap_w * bitmap_h * 4);
    // const unsigned char* src = bitmap;
    // // unsigned int* dst = TexPixelsRGBA32;
    // unsigned int* dst = (unsigned int *)*out;
    // for (int n = bitmap_w * bitmap_h; n > 0; n--)
    //     *dst++ = IM_COL32(255, 255, 255, (unsigned int)(*src++));
    // *out = (unsigned char *)TexPixelsRGBA32;
}
void VulkanChess::DestroyFontResource(VkDevice device){
    mFont.index.Destroy(device);
    mFont.vertex.Destroy(device);

    uniform.font.Destroy(device);

    fonts.image.Destroy(device);

    vkDestroySampler(device, fonts.sampler, nullptr);
}
void VulkanChess::DestroyChessResource(VkDevice device){
    mChess.index.Destroy(device);
    mChess.vertex.Destroy(device);

    uniform.chess.Destroy(device);
}
void VulkanChess::DrawFont(VkCommandBuffer command, const GraphicsPipeline &pipeline){
    uint32_t dynamicOffsets;
    for (size_t i = 0; i < CHESSBOARD_CHESS_TOTAL + 12; ++i){
        dynamicOffsets = i * mFontMinUniformBufferOffset;
        pipeline.BindDescriptorSet(command, descriptorSet.font, 1, &dynamicOffsets);
        vkCmdDrawIndexed(command, mFont.indexCount, 1, 0, 0, 0);
        // DrawGraphics(command, &mFont);
    }
}
void VulkanChess::GetCircularVertex(const glm::vec3 &color, std::vector<Vertex> &vertices){
    Vertex v = Vertex(glm::vec3(.0f), color);
    vertices.push_back(v);
    for (float i = 1; i < 360.0f; ++i){
        float r = i * M_PI / 180.0f;
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
    mFont.vertexCount = 0;
    mFont.index.CreateBuffer(device, sizeof(indices), VK_BUFFER_USAGE_INDEX_BUFFER_BIT|VK_BUFFER_USAGE_TRANSFER_DST_BIT);
    mFont.vertex.CreateBuffer(device, sizeof(vertices), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT|VK_BUFFER_USAGE_TRANSFER_DST_BIT);
    mFont.index.AllocateAndBindMemory(device, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    mFont.vertex.AllocateAndBindMemory(device, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    vkf::tool::CopyBuffer(device, sizeof(indices), indices, graphics, pool, mFont.index);
    vkf::tool::CopyBuffer(device, sizeof(vertices), vertices, graphics, pool, mFont.vertex);
    uniform.font.CreateBuffer(device, mFontMinUniformBufferOffset * (CHESSBOARD_CHESS_TOTAL + 12), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
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
    const wchar_t word[] = { L'魏', L'蜀', L'吴', L'漢', L'兵', L'炮', L'車', L'馬', L'相', L'士' };
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
    const glm::vec3 countryColor[] = { WEI_CHESS_COUNTRY_COLOR, SHU_CHESS_COUNTRY_COLOR, WU_CHESS_COUNTRY_COLOR, HAN_CHESS_COUNTRY_COLOR };
    GetCircularVertex(countryColor[0], circularVertices);
    mChess.vertexCount = circularVertices.size();
    std::vector<uint16_t> circularIndices;
    for (size_t i = 1; i <= circularVertices.size(); ++i){
        circularIndices.push_back(0);
        circularIndices.push_back(i);
        circularIndices.push_back(i + 1);
    }
    mChess.indexCount = circularIndices.size();
    for (size_t uiChess = 1; uiChess < sizeof(countryColor) / sizeof(glm::vec3); ++uiChess){
        GetCircularVertex(countryColor[uiChess], circularVertices);
    }
    for (size_t uiChess = 0; uiChess < sizeof(countryColor) / sizeof(glm::vec3); ++uiChess){
        const glm::vec3 banColor = glm::vec3(countryColor[uiChess].r * .6, countryColor[uiChess].g * .6, countryColor[uiChess].b * .6);
        GetCircularVertex(banColor, circularVertices);
    }
    mChess.index.CreateBuffer(device, sizeof(uint16_t) * circularIndices.size(), VK_BUFFER_USAGE_INDEX_BUFFER_BIT|VK_BUFFER_USAGE_TRANSFER_DST_BIT);
    //8是4种颜色+亮度较低的颜色(表示不是该玩家下子)
    mChess.vertex.CreateBuffer(device, sizeof(Vertex) * circularVertices.size() * 5, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT|VK_BUFFER_USAGE_TRANSFER_DST_BIT);
    mChess.index.AllocateAndBindMemory(device, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    mChess.vertex.AllocateAndBindMemory(device, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    vkf::tool::CopyBuffer(device, sizeof(uint16_t) * circularIndices.size(), circularIndices.data(), graphics, pool, mChess.index);
    vkf::tool::CopyBuffer(device, sizeof(Vertex) * circularVertices.size(), circularVertices.data(), graphics, pool, mChess.vertex);

    //多4个是用来放消灭汉后奖励的棋子//但因为有3个势力, 所以+12//60
    uniform.chess.CreateBuffer(device, mMinUniformBufferOffset * (CHESSBOARD_CHESS_TOTAL + 12), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
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
    uint32_t dynamicOffsets, country = 0;
    
    const uint32_t countryChess[] = { WEI_CHESS_COUNT + 4, SHU_CHESS_COUNT + 4, WU_CHESS_COUNT + 4 };
    for (size_t uiCountry = 0; uiCountry < sizeof(countryChess) / sizeof(uint32_t); ++uiCountry){
        for (size_t uiChess = 0; uiChess < countryChess[uiCountry]; ++uiChess){
            dynamicOffsets = ROW_COLUMN_INDEX(uiCountry, uiChess, countryChess[uiCountry]) * mMinUniformBufferOffset;
            pipeline.BindDescriptorSet(command, descriptorSet.chess, 1, &dynamicOffsets);
            vkCmdDrawIndexed(command, mChess.indexCount, 1, 0, uiCountry == currentCountry?uiCountry * mChess.vertexCount:(4 + uiCountry) * mChess.vertexCount, 0);
            // DrawGraphics(command, &mChess);
        }
    }
    for (size_t uiChess = 0; uiChess < HAN_CHESS_COUNT; ++uiChess){
        dynamicOffsets = ROW_COLUMN_INDEX(HAN_COUNTRY_INDEX, uiChess, countryChess[0]) * mMinUniformBufferOffset;
        pipeline.BindDescriptorSet(command, descriptorSet.chess, 1, &dynamicOffsets);
        vkCmdDrawIndexed(command, mChess.indexCount, 1, 0, HAN_COUNTRY_INDEX * mChess.vertexCount, 0);
        // DrawGraphics(command, &mChess);
    }
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
    CreateFontResource(device, pool.commandPool, graphics);
    CreateChessResource(device, pool.commandPool, graphics);

    SetupDescriptorSet(device, setLayout, pool.descriptorPool);
}

void VulkanChess::RecordCommand(VkCommandBuffer cmd, uint32_t windowWidth, uint32_t currentCountry){
    const glm::mat4 projection = glm::ortho(0.0f, (float)windowWidth, 0.0f, (float)windowWidth, -1.0f, 1.0f);
    pipelines.chess.BindPipeline(cmd);
    pipelines.chess.PushConstant(cmd, VK_SHADER_STAGE_VERTEX_BIT, sizeof(glm::mat4), &projection);
    
    VkDeviceSize offset = 0;
    vkCmdBindVertexBuffers(cmd, 0, 1, &mChess.vertex.buffer, &offset);
    vkCmdBindIndexBuffer(cmd, mChess.index.buffer, 0, VK_INDEX_TYPE_UINT16);

    DrawChess(cmd, currentCountry, pipelines.chess);

    pipelines.font.BindPipeline(cmd);
    pipelines.font.PushConstant(cmd, VK_SHADER_STAGE_VERTEX_BIT, sizeof(glm::mat4), &projection);

    vkCmdBindVertexBuffers(cmd, 0, 1, &mFont.vertex.buffer, &offset);
    vkCmdBindIndexBuffer(cmd, mFont.index.buffer, 0, VK_INDEX_TYPE_UINT16);
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