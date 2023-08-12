#include "chess.h"
#define STB_IMAGE_IMPLEMENTATION
#define STB_TRUETYPE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_truetype.h"
#include "stb_image_write.h"

#include "Chessboard.h"
#define IM_COL32_R_SHIFT    0
#define IM_COL32_G_SHIFT    8
#define IM_COL32_B_SHIFT    16
#define IM_COL32_A_SHIFT    24
#define IM_COL32(R,G,B,A)    (((unsigned int)(A)<<IM_COL32_A_SHIFT) | ((unsigned int)(B)<<IM_COL32_B_SHIFT) | ((unsigned int)(G)<<IM_COL32_G_SHIFT) | ((unsigned int)(R)<<IM_COL32_R_SHIFT))
// bool GenerateFontImage(int bitmap_w, int bitmap_h, const wchar_t *word, uint32_t wordCount, const char *outImageName, const unsigned char *fontData, float pixels = 20){
// 	stbtt_fontinfo info;
// 	if(!stbtt_InitFont(&info, fontData, 0)){
// 		printf("in function:%s, stbtt_InitFont error\n", __FUNCTION__);
// 		return false;
// 	}
// 	unsigned char *bitmap = (unsigned char *)malloc(bitmap_w * bitmap_h);
// 	memset(bitmap, 0, bitmap_w * bitmap_h);
// 	// const int *word = L"世界你好";
// 	// printf("%x\n", (unsigned int *)word[0]);
// 	// printf("strlen(%s) = %d\n", c, strlen(c));
// 	// int word[20] = { L'你', L'好', L'世', L'界' };//字母的编码
// 	// int word[20] = { 0x4f60, 0x597d, 0x4e16, 0x754c };//字母的编码
// 	float scale = stbtt_ScaleForPixelHeight(&info, pixels);
// 	int ascent = 0, descent = 0, lineGap = 0;
// 	stbtt_GetFontVMetrics(&info, &ascent, &descent, &lineGap);
// 	ascent = roundf(ascent * scale);
// 	descent = roundf(descent * scale);
// 	int x = 0;
//     for (int i = 0; i < wordCount; ++i){
//         int advanceWidth = 0, leftSideBearing = 0, c_x1, c_y1, c_x2, c_y2;
//         stbtt_GetCodepointHMetrics(&info, word[i], &advanceWidth, &leftSideBearing);
//         stbtt_GetCodepointBitmapBox(&info, word[i], scale, scale, &c_x1, &c_y1, &c_x2, &c_y2);
//         int y = ascent +c_y1;
//         int byteOffset = x + roundf(leftSideBearing * scale) + (y * bitmap_w);
//         stbtt_MakeCodepointBitmap(&info, bitmap + byteOffset, c_x2 - c_x1, c_y2 - c_y1, bitmap_w, scale, scale, word[i]);
//         x += roundf(advanceWidth * scale);
//         int kern;
//         kern = stbtt_GetCodepointKernAdvance(&info, word[i], word[i + 1]);
//         x += roundf(kern * scale);
//     }
// 	stbi_write_png(outImageName, bitmap_w, bitmap_h, 1, bitmap, 0);
// 	free(bitmap);
//     return true;
// }
void GetFontImageData(const unsigned char *fontData, int bitmap_w, int bitmap_h, wchar_t word, unsigned char *out){
	stbtt_fontinfo info;
    const float pixels = 20;
	if(!stbtt_InitFont(&info, fontData, 0)){
		printf("in function:%s, stbtt_InitFont error\n", __FUNCTION__);
        return;
	}
	// unsigned char *bitmap = (unsigned char *)malloc(bitmap_w * bitmap_h);
	float scale = stbtt_ScaleForPixelHeight(&info, pixels);
	int ascent = 0, descent = 0, lineGap = 0;
	stbtt_GetFontVMetrics(&info, &ascent, &descent, &lineGap);
	ascent = roundf(ascent * scale);
	descent = roundf(descent * scale);
	int x = 0;
    int advanceWidth = 0, leftSideBearing = 0, c_x1, c_y1, c_x2, c_y2;
    stbtt_GetCodepointHMetrics(&info, word, &advanceWidth, &leftSideBearing);
    stbtt_GetCodepointBitmapBox(&info, word, scale, scale, &c_x1, &c_y1, &c_x2, &c_y2);
    int y = ascent +c_y1;
    int byteOffset = x + roundf(leftSideBearing * scale) + (y * bitmap_w);
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
// bool generateFontImage(const unsigned char *fontData){
// 	stbtt_fontinfo info;
// 	if(!stbtt_InitFont(&info, fontData, 0)){
// 		printf("in function:%s, stbtt_InitFont error\n", __FUNCTION__);
// 		return false;
// 	}
// 	int bitmap_w = 30, bitmap_h = 30;
// 	unsigned char *bitmap = malloc(bitmap_w * bitmap_h);
// 	memset(bitmap, 0, bitmap_w * bitmap_h);
// 	char word = '魏';
// 	//wchar_t word = L'魏';
// 	//const int *word = L"世界你好";
// 	// printf("%x\n", (unsigned int *)word[0]);
// 	// printf("strlen(%s) = %d\n", c, strlen(c));
// 	// int word[20] = { L'你', L'好', L'世', L'界' };//字母的编码
// 	// int word[20] = { 0x4f60, 0x597d, 0x4e16, 0x754c };//字母的编码
// 	float pixels = 30.0;//字号
// 	float scale = stbtt_ScaleForPixelHeight(&info, pixels);
// 	int ascent = 0, descent = 0, lineGap = 0;
// 	stbtt_GetFontVMetrics(&info, &ascent, &descent, &lineGap);
// 	ascent = roundf(ascent * scale);
// 	descent = roundf(descent * scale);
// 	int x = 0;
// 	int advanceWidth = 0, leftSideBearing = 0, c_x1, c_y1, c_x2, c_y2;
// 	stbtt_GetCodepointHMetrics(&info, word, &advanceWidth, &leftSideBearing);
// 	stbtt_GetCodepointBitmapBox(&info, word, scale, scale, &c_x1, &c_y1, &c_x2, &c_y2);
// 	int y = ascent +c_y1;
// 	int byteOffset = x + roundf(leftSideBearing * scale) + (y * bitmap_w);
// 	stbtt_MakeCodepointBitmap(&info, bitmap + byteOffset, c_x2 - c_x1, c_y2 - c_y1, bitmap_w, scale, scale, word);
// 	x += roundf(advanceWidth * scale);
// 	int kern;
// 	kern = stbtt_GetCodepointKernAdvance(&info, word, word);
// 	x += roundf(kern * scale);
// 	stbi_write_png("stb.png", bitmap_w, bitmap_h, 1, bitmap, 0);
// 	free(bitmap);
// }
void GetFontImageData(const std::string&fontfile, wchar_t word, unsigned char *bitmap){
	long int size = 0;
	unsigned char *fontBuffer = NULL;
	FILE *fontFile = fopen(fontfile.c_str(), "rb");
	if(fontFile){
		size = vkf::tool::GetFileSize(fontFile);
		fontBuffer = (unsigned char *)malloc(size);
		fread(fontBuffer, size, 1, fontFile);
		fclose(fontFile);
        GetFontImageData(fontBuffer, CHESS_FONT_IMAGE_WIDTH, CHESS_FONT_IMAGE_HEIGHT, word, bitmap);
		free(fontBuffer);
	}
	else{
		perror("open file error");
		printf("file name is %s\n", fontfile.c_str());
	}
}
// void *GenerateFontImage(int bitmap_w, int bitmap_h, wchar_t word, const char *outImageName, const unsigned char *fontData, float pixels = 20){
// 	stbtt_fontinfo info;
// 	if(!stbtt_InitFont(&info, fontData, 0)){
// 		printf("in function:%s, stbtt_InitFont error\n", __FUNCTION__);
// 		return false;
// 	}
// 	unsigned char *bitmap = (unsigned char *)malloc(bitmap_w * bitmap_h);
// 	memset(bitmap, 0, bitmap_w * bitmap_h);
// 	// const int *word = L"世界你好";
// 	// printf("%x\n", (unsigned int *)word[0]);
// 	// printf("strlen(%s) = %d\n", c, strlen(c));
// 	// int word[20] = { L'你', L'好', L'世', L'界' };//字母的编码
// 	// int word[20] = { 0x4f60, 0x597d, 0x4e16, 0x754c };//字母的编码
// 	float scale = stbtt_ScaleForPixelHeight(&info, pixels);
// 	int ascent = 0, descent = 0, lineGap = 0;
// 	stbtt_GetFontVMetrics(&info, &ascent, &descent, &lineGap);
// 	ascent = roundf(ascent * scale);
// 	descent = roundf(descent * scale);
// 	int x = 0;
//     int advanceWidth = 0, leftSideBearing = 0, c_x1, c_y1, c_x2, c_y2;
//     stbtt_GetCodepointHMetrics(&info, word, &advanceWidth, &leftSideBearing);
//     stbtt_GetCodepointBitmapBox(&info, word, scale, scale, &c_x1, &c_y1, &c_x2, &c_y2);
//     int y = ascent +c_y1;
//     int byteOffset = x + roundf(leftSideBearing * scale) + (y * bitmap_w);
//     stbtt_MakeCodepointBitmap(&info, bitmap + byteOffset, c_x2 - c_x1, c_y2 - c_y1, bitmap_w, scale, scale, word);
//     // x += roundf(advanceWidth * scale);
//     // int kern;
//     // kern = stbtt_GetCodepointKernAdvance(&info, word, word);
//     // x += roundf(kern * scale);
// 	// stbi_write_png(outImageName, bitmap_w, bitmap_h, 1, bitmap, 0);
// 	free(bitmap);
//     return true;
// }
// void CreateFontImage(const std::string&fontfile, const std::string&outPath = CHESS_FONT_ROOT_PATH){
// 	long int size = 0;
// 	unsigned char *fontBuffer = NULL;
// 	FILE *fontFile = fopen(fontfile.c_str(), "rb");
// 	if(fontFile){
// 		size = vkf::tool::GetFileSize(fontFile);
// 		fontBuffer = (unsigned char *)malloc(size);
// 		fread(fontBuffer, size, 1, fontFile);
// 		fclose(fontFile);
//         const wchar_t word[] = { L'魏', L'蜀', L'吴', L'漢', L'兵', L'炮', L'車', L'馬', L'相', L'士' };
//         for (size_t i = 0; i < sizeof(word) / sizeof(int); ++i){
//             std::string path;
//             if(word[i] == L'魏'){
//                 path = outPath + "魏";
//             }
//             else if(word[i] == L'蜀'){
//                 path = outPath + "蜀";
//             }
//             else if(word[i] == L'吴'){
//                 path = outPath + "吴";
//             }
//             else if(word[i] == L'漢'){
//                 path = outPath + "漢";
//             }
//             else if(word[i] == L'兵'){
//                 path = outPath + "兵";
//             }
//             else if(word[i] == L'炮'){
//                 path = outPath + "炮";
//             }
//             else if(word[i] == L'車'){
//                 path = outPath + "車";
//             }
//             else if(word[i] == L'馬'){
//                 path = outPath + "馬";
//             }
//             else if(word[i] == L'相'){
//                 path = outPath + "相";
//             }
//             else if(word[i] == L'士'){
//                 path = outPath + "士";
//             }
//             path += ".png";
// 		    GenerateFontImage(CHESS_FONT_IMAGE_WIDTH, CHESS_FONT_IMAGE_HEIGHT, word[i], path.c_str(), fontBuffer);
//         }
// 		free(fontBuffer);
// 	}
// 	else{
// 		perror("open file error");
// 		printf("file name is %s\n", fontfile.c_str());
// 	}
// }

void Chess::DrawFont(VkCommandBuffer cmd){
    VkDeviceSize offset = 0;
    vkCmdBindVertexBuffers(cmd, 0, 1, &mFontVertex.buffer, &offset);
    vkCmdBindIndexBuffer(cmd, mFontIndex.buffer, 0, VK_INDEX_TYPE_UINT16);
    mFontPipeline.BindDescriptorSet(cmd, mFontSet);
    vkCmdDrawIndexed(cmd, 6, 1, 0, 0, 0);
}
void Chess::DrawCircular(VkCommandBuffer cmd){
    VkDeviceSize offset = 0;
    vkCmdBindVertexBuffers(cmd, 0, 1, &mVertex.buffer, &offset);
    vkCmdBindIndexBuffer(cmd, mIndex.buffer, 0, VK_INDEX_TYPE_UINT16);
    mPipeline.BindDescriptorSet(cmd, mSet);
    vkCmdDrawIndexed(cmd, mCircularIndicesCount, 1, 0, 0, 0);
}
void Chess::GetCircularVertex(const glm::vec3&color, std::vector<ChessVertex>&vertices){
    ChessVertex v = ChessVertex(glm::vec3(.0f), color);
    vertices.push_back(v);
    for (float i = 1; i < 360.0f; ++i){
        float r = i * M_PI / 180.0f;
        v.pos = glm::vec3(sin(r), cos(r), .0f);
        vertices.push_back(v);
    }
}
bool Chess::CreateTexture(VkDevice device, const char *file, VkCommandPool pool, VkQueue graphics){
    int nrComponents;
    stbi_uc* data = stbi_load(file, (int *)&mFont.size.width, (int *)&mFont.size.height, &nrComponents, STBI_rgb_alpha);
    if(data == nullptr){
        printf("load picture error:%s\n", file);
        return false;
    }       
    vkf::CreateTextureImage(device, data, mFont.size.width, mFont.size.height, mFont, pool, graphics);
    stbi_image_free(data);
    return true;
}
void Chess::CreateGraphicsPipeline(VkDevice device, uint32_t windowWidth, VkRenderPass renderpass, const std::vector<std::string>&shader, GraphicsPipeline&pipeline, const GraphicsPipelineStateInfo&pipelineState){
    std::vector<VkShaderStageFlagBits> stage = { VK_SHADER_STAGE_VERTEX_BIT, VK_SHADER_STAGE_FRAGMENT_BIT };
    std::vector<uint32_t>cacheData;
    vkf::tool::GetFileContent("ChessGraphicsPipelineCache", cacheData);
    for (size_t i = 0; i < shader.size(); ++i){
        pipeline.PushShader(device, stage[i], shader[i]);
    }
    pipeline.SetStateInfo(pipelineState);
    pipeline.PushScissor(windowWidth, windowWidth);
    pipeline.PushViewport(windowWidth, windowWidth);
    // pipeline.SetVertexInputBindingDescription(sizeof(ChessVertex));
    pipeline.PushPushConstant(sizeof(glm::mat4), VK_SHADER_STAGE_VERTEX_BIT);
    // pipeline.PushVertexInputAttributeDescription(0, 0, VK_FORMAT_R32G32B32_SFLOAT);
    // pipeline.PushVertexInputAttributeDescription(1, 3 * sizeof(float), VK_FORMAT_R32G32B32_SFLOAT);
    pipeline.CreateCache(device, cacheData);
    pipeline.CreateDescriptorSetLayout(device);
    pipeline.CreateLayout(device);
    pipeline.CreatePipeline(device, renderpass);
    pipeline.GetCacheData(device, cacheData);
    vkf::tool::WriteFileContent("ChessGraphicsPipelineCache", cacheData.data(), cacheData.size() * sizeof(uint32_t)); 
}
Chess::Chess(/* args */){
    
}

Chess::~Chess(){

}
void Chess::Cleanup(VkDevice device){
    mFont.Destroy(device);
    mIndex.Destroy(device);
    mVertex.Destroy(device);
    mPosition.Destroy(device);
    mFontIndex.Destroy(device);
    mFontVertex.Destroy(device);
    mFontPosition.Destroy(device);
    DestroyGraphicsPipeline(device);
}
void Chess::DestroyGraphicsPipeline(VkDevice device){
    std::vector<uint32_t>cacheData;
    mPipeline.DestroyCache(device, cacheData);
    vkf::tool::WriteFileContent("ChessGraphicsPipelineCache", cacheData.data(), cacheData.size() * sizeof(uint32_t)); 
    mPipeline.DestroySetLayout(device);
    mPipeline.DestroyLayout(device);
    mPipeline.DestroyPipeline(device);

    mFontPipeline.DestroyCache(device, cacheData);
    mFontPipeline.DestroySetLayout(device);
    mFontPipeline.DestroyLayout(device);
    mFontPipeline.DestroyPipeline(device);
}
void Chess::RecodeCommand(VkCommandBuffer cmd, uint32_t windowWidth){
    const glm::mat4 projection = glm::ortho(0.0f, (float)windowWidth, 0.0f, (float)windowWidth, -1.0f, 1.0f);
    mPipeline.BindPipeline(cmd);
    mPipeline.PushPushConstant(cmd, VK_SHADER_STAGE_VERTEX_BIT, sizeof(glm::mat4), &projection);
    DrawCircular(cmd);
    mFontPipeline.BindPipeline(cmd);
    mFontPipeline.PushPushConstant(cmd, VK_SHADER_STAGE_VERTEX_BIT, sizeof(glm::mat4), &projection);
    DrawFont(cmd);
}
void Chess::CreatePipeline(VkDevice device, VkRenderPass renderpass, uint32_t windowWidth){
    mPipeline.SetVertexInputBindingDescription(sizeof(ChessVertex));
    mPipeline.PushVertexInputAttributeDescription(0, 0, VK_FORMAT_R32G32B32_SFLOAT);
    mPipeline.PushVertexInputAttributeDescription(1, 3 * sizeof(float), VK_FORMAT_R32G32B32_SFLOAT);
    mPipeline.PushDescriptorSetLayoutBinding(0, VK_SHADER_STAGE_VERTEX_BIT, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
    CreateGraphicsPipeline(device, windowWidth, renderpass, { "shaders/baseVert.spv", "shaders/baseFrag.spv" }, mPipeline);

    mFontPipeline.SetVertexInputBindingDescription(sizeof(ChessFontVertex));
    mFontPipeline.PushVertexInputAttributeDescription(0, 0, VK_FORMAT_R32G32B32_SFLOAT);
    mFontPipeline.PushVertexInputAttributeDescription(1, 3 * sizeof(float), VK_FORMAT_R32G32_SFLOAT);
    mFontPipeline.PushDescriptorSetLayoutBinding(0, VK_SHADER_STAGE_VERTEX_BIT, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
    mFontPipeline.PushDescriptorSetLayoutBinding(1, VK_SHADER_STAGE_FRAGMENT_BIT, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
    CreateGraphicsPipeline(device, windowWidth, renderpass, { "shaders/baseFontVert.spv", "shaders/baseFontFrag.spv" }, mFontPipeline);
}
void Chess::CreateVulkanResource(VkDevice device, const glm::vec3&color, const VulkanPool&pool, VkQueue graphics){
    mCountryColor = color;
    std::vector<ChessVertex> circularVertices;
    GetCircularVertex(color, circularVertices);
    std::vector<uint16_t> circularIndices;
    for (size_t i = 1; i <= circularVertices.size(); ++i){
        circularIndices.push_back(0);
        circularIndices.push_back(i);
        circularIndices.push_back(i + 1);
    }
    mCircularIndicesCount = circularIndices.size();
    const ChessFontVertex vertices[] = {
        ChessFontVertex(glm::vec3(.0f, 1.0f, .0f), glm::vec2(0.0f, 1.0f)),//左下
        ChessFontVertex(glm::vec3(1.0f, .0f, .0f), glm::vec2(1.0f, 0.0f)),//右上
        ChessFontVertex(glm::vec3(.0f, .0f, .0f), glm::vec2(0.0f, 0.0f)), //左上
        ChessFontVertex(glm::vec3(1.0f, 1.0f, .0f), glm::vec2(1.0f, 1.0f))//右下
    };
    const uint16_t indices[] = { 0, 1, 2, 0, 3, 1 };
    mFontIndex.CreateBuffer(device, sizeof(indices), VK_BUFFER_USAGE_INDEX_BUFFER_BIT|VK_BUFFER_USAGE_TRANSFER_DST_BIT);
    mIndex.CreateBuffer(device, sizeof(uint16_t) * circularIndices.size(), VK_BUFFER_USAGE_INDEX_BUFFER_BIT|VK_BUFFER_USAGE_TRANSFER_DST_BIT);
    mFontVertex.CreateBuffer(device, sizeof(vertices), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT|VK_BUFFER_USAGE_TRANSFER_DST_BIT);
    mVertex.CreateBuffer(device, sizeof(ChessVertex) * circularVertices.size(), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT|VK_BUFFER_USAGE_TRANSFER_DST_BIT);
    mIndex.AllocateAndBindMemory(device, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    mFontIndex.AllocateAndBindMemory(device, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    mVertex.AllocateAndBindMemory(device, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    mFontVertex.AllocateAndBindMemory(device, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    vkf::tool::CopyBuffer(device, sizeof(indices), indices, graphics, pool.commandPool, mFontIndex);
    vkf::tool::CopyBuffer(device, sizeof(uint16_t) * circularIndices.size(), circularIndices.data(), graphics, pool.commandPool, mIndex);
    vkf::tool::CopyBuffer(device, sizeof(vertices), vertices, graphics, pool.commandPool, mFontVertex);
    vkf::tool::CopyBuffer(device, sizeof(ChessVertex) * circularVertices.size(), circularVertices.data(), graphics, pool.commandPool, mVertex);

    mPosition.CreateBuffer(device, sizeof(glm::mat4), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
    mFontPosition.CreateBuffer(device, sizeof(glm::mat4), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
    mPosition.AllocateAndBindMemory(device, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT|VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    mFontPosition.AllocateAndBindMemory(device, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT|VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    mPipeline.AllocateDescriptorSets(device, pool.descriptorPool, 1, &mSet);
    mFontPipeline.AllocateDescriptorSets(device, pool.descriptorPool, 1, &mFontSet);
}
void Chess::UpdateSet(VkDevice device, VkSampler fontSampler){
    mPipeline.UpdateDescriptorSets(device, { mPosition }, {}, mSet);
    mFontPipeline.UpdateDescriptorSets(device, { mFontPosition }, { mFont }, mFontSet, fontSampler);
}
void Chess::UpdateUniform(VkDevice device, const glm::vec2&pos){
    mPos = pos;
    glm::mat4 model = glm::scale(glm::translate(glm::mat4(1), glm::vec3(mPos, 0)), glm::vec3(CHESS_WIDTH, CHESS_HEIGHT,1));
    mPosition.UpdateData(device, &model);
    model = glm::scale(glm::translate(glm::mat4(1), glm::vec3(mPos.x - CHESS_WIDTH * .65, mPos.y - CHESS_HEIGHT * .9, 0)), glm::vec3(CHESS_WIDTH * 1.5, CHESS_HEIGHT * 1.5,1));
    mFontPosition.UpdateData(device, &model);
}
void Chess::UpdateUniform(VkDevice device, uint32_t row, uint32_t column){
    mRow = row;
    mColumn = column;
    UpdateUniform(device, glm::vec2(CHESSBOARD_COLUMN_TO_X(column), CHESSBOARD_ROW_TO_Y(row)));
}
// bool Chess::isSelect(uint32_t row, uint32_t column){
//     const glm::vec2 select = glm::vec2(CHESSBOARD_COLUMN_TO_X(column), CHESSBOARD_ROW_TO_Y(row));
//     return abs(select.x - mPos.x) < CHESS_WIDTH * .5 && abs(select.y - mPos.y) < CHESS_WIDTH * .5;
// }
bool Chess::isSelect(const glm::vec2&pos){
    return abs(pos.x - mPos.x) < CHESS_WIDTH * .5 && abs(pos.y - mPos.y) < CHESS_WIDTH * .5;
}

Wei::Wei(){

}
Wei::~Wei(){

}
void Wei::Selected(const Chessboard *chessboard, std::vector<Ranks>&canplays){
    const Ranks ranks[] = { Ranks(mRow + 1, mColumn), Ranks(mRow - 1, mColumn), Ranks(mRow, mColumn + 1), Ranks(mRow, mColumn - 1) };
    //将不能出九宫格
    for(uint32_t i = 0; i < sizeof(ranks) / sizeof(Ranks); ++i){
        if(!chessboard->IsBoundary(ranks[i].row, ranks[i].column)){
            const Chess *chess = chessboard->GetChess(ranks[i].row, ranks[i].column);
            if(chessboard->IsInPalace(ranks[i].row, ranks[i].column)){
                if(!chess || chess->GetCountryColor() != GetCountryColor()){
                    canplays.push_back(Ranks(ranks[i].row, ranks[i].column));
                }
            }
        }
    }
    chessboard->SelectChessInPalace(this, canplays);
}
// void Wei::UpdateUniform(VkDevice device, uint32_t row, uint32_t column){
//     if(!mRow && mColumn){
//         mRow = row;
//         mColumn = column;
//     }
//     Chess::UpdateUniform(device, row, column);
// }
void Wei::CreateFontTexture(VkDevice device, VkCommandPool pool, VkQueue graphics){
    unsigned char *bitmap = (unsigned char *)malloc(CHESS_FONT_IMAGE_WIDTH * CHESS_FONT_IMAGE_HEIGHT);
    memset(bitmap, 0, CHESS_FONT_IMAGE_WIDTH * CHESS_FONT_IMAGE_HEIGHT);
    GetFontImageData("fonts/SourceHanSerifCN-Bold.otf", L'魏', bitmap);
    vkf::CreateFontImage(device, bitmap, CHESS_FONT_IMAGE_WIDTH, CHESS_FONT_IMAGE_HEIGHT, mFont, pool, graphics);
    free(bitmap);
    // if(!CreateTexture(device, CHESS_FONT_ROOT_PATH"魏.png", pool, graphics)){
    //     CreateFontImage();
    //     CreateTexture(device, CHESS_FONT_ROOT_PATH"魏.png", pool, graphics);
    // }
}

Shu::Shu(){

}
Shu::~Shu(){

}
void Shu::Selected(const Chessboard *chessboard, std::vector<Ranks>&canplays){
    const Ranks ranks[] = { Ranks(mRow + 1, mColumn), Ranks(mRow - 1, mColumn), Ranks(mRow, mColumn + 1), Ranks(mRow, mColumn - 1) };
    for(uint32_t i = 0; i < sizeof(ranks) / sizeof(Ranks); ++i){
        if(!chessboard->IsBoundary(ranks[i].row, ranks[i].column)){
            const Chess *chess = chessboard->GetChess(ranks[i].row, ranks[i].column);
            if(chessboard->IsInPalace(ranks[i].row, ranks[i].column)){
                if(!chess || chess->GetCountryColor() != GetCountryColor()){
                    canplays.push_back(Ranks(ranks[i].row, ranks[i].column));
                }
            }
        }
    }
    chessboard->SelectChessInPalace(this, canplays);
}
// void Shu::UpdateUniform(VkDevice device, uint32_t row, uint32_t column){
//     if(!mRow && mColumn){
//         mRow = row;
//         mColumn = column;
//     }
//     Chess::UpdateUniform(device, row, column);
// }
void Shu::CreateFontTexture(VkDevice device, VkCommandPool pool, VkQueue graphics){
    unsigned char *bitmap = (unsigned char *)malloc(CHESS_FONT_IMAGE_WIDTH * CHESS_FONT_IMAGE_HEIGHT);
    memset(bitmap, 0, CHESS_FONT_IMAGE_WIDTH * CHESS_FONT_IMAGE_HEIGHT);
    GetFontImageData("fonts/SourceHanSerifCN-Bold.otf", L'蜀', bitmap);
    vkf::CreateFontImage(device, bitmap, CHESS_FONT_IMAGE_WIDTH, CHESS_FONT_IMAGE_HEIGHT, mFont, pool, graphics);
    free(bitmap);

    // if(!CreateTexture(device, CHESS_FONT_ROOT_PATH"蜀.png", pool, graphics)){
    //     // CreateFontImage("fonts/SourceHanSerifCN-Bold.otf");
    //     // CreateTexture(device, CHESS_FONT_ROOT_PATH"蜀.png", pool, graphics);
    // }
}

Wu::Wu(){

}
Wu::~Wu(){

}
void Wu::Selected(const Chessboard *chessboard, std::vector<Ranks>&canplays){
    const Ranks ranks[] = { Ranks(mRow, mColumn - 1), Ranks(mRow + 1, mColumn), Ranks(mRow - 1, mColumn), Ranks(mRow, mColumn + 1) };
    for(uint32_t i = 0; i < sizeof(ranks) / sizeof(Ranks); ++i){
        if(!chessboard->IsBoundary(ranks[i].row, ranks[i].column)){
            const Chess *chess = chessboard->GetChess(ranks[i].row, ranks[i].column);
            if(chessboard->IsInPalace(ranks[i].row, ranks[i].column)){
                if(!chess || chess->GetCountryColor() != GetCountryColor()){
                    canplays.push_back(Ranks(ranks[i].row, ranks[i].column));
                }
            }
        }
    }
    chessboard->SelectChessInPalace(this, canplays);
}
// void Wu::UpdateUniform(VkDevice device, uint32_t row, uint32_t column){
//     if(!mRow && mColumn){
//         mRow = row;
//         mColumn = column;
//     }
//     Chess::UpdateUniform(device, row, column);
// }
void Wu::CreateFontTexture(VkDevice device, VkCommandPool pool, VkQueue graphics){
    unsigned char *bitmap = (unsigned char *)malloc(CHESS_FONT_IMAGE_WIDTH * CHESS_FONT_IMAGE_HEIGHT);
    memset(bitmap, 0, CHESS_FONT_IMAGE_WIDTH * CHESS_FONT_IMAGE_HEIGHT);
    GetFontImageData("fonts/SourceHanSerifCN-Bold.otf", L'吴', bitmap);
    vkf::CreateFontImage(device, bitmap, CHESS_FONT_IMAGE_WIDTH, CHESS_FONT_IMAGE_HEIGHT, mFont, pool, graphics);
    free(bitmap);

    // if(!CreateTexture(device, CHESS_FONT_ROOT_PATH"吴.png", pool, graphics)){
    //     // CreateFontImage("fonts/SourceHanSerifCN-Bold.otf");
    //     // CreateTexture(device, CHESS_FONT_ROOT_PATH"吴.png", pool, graphics);
    // }
}

Han::Han(){

}
Han::~Han(){

}
void Han::Selected(const Chessboard *chessboard, std::vector<Ranks>&canplays){
    //漢不能被选择
    const Ranks ranks[] = { Ranks(mRow + 1, mColumn), Ranks(mRow - 1, mColumn), Ranks(mRow, mColumn + 1), Ranks(mRow, mColumn - 1) };
    for(uint32_t i = 0; i < sizeof(ranks) / sizeof(Ranks); ++i){
        if(!chessboard->IsBoundary(ranks[i].row, ranks[i].column)){
            const Chess *chess = chessboard->GetChess(ranks[i].row, ranks[i].column);
            if(!chess || chess->GetCountryColor() != GetCountryColor()){
                canplays.push_back(Ranks(ranks[i].row, ranks[i].column));
            }
        }
    }
    chessboard->SelectChessInPalace(this, canplays);
}
void Han::CreateFontTexture(VkDevice device, VkCommandPool pool, VkQueue graphics){
    unsigned char *bitmap = (unsigned char *)malloc(CHESS_FONT_IMAGE_WIDTH * CHESS_FONT_IMAGE_HEIGHT);
    memset(bitmap, 0, CHESS_FONT_IMAGE_WIDTH * CHESS_FONT_IMAGE_HEIGHT);
    GetFontImageData("fonts/SourceHanSerifCN-Bold.otf", L'漢', bitmap);
    vkf::CreateFontImage(device, bitmap, CHESS_FONT_IMAGE_WIDTH, CHESS_FONT_IMAGE_HEIGHT, mFont, pool, graphics);
    free(bitmap);

    // if(!CreateTexture(device, CHESS_FONT_ROOT_PATH"漢.png", pool, graphics)){
    //     // CreateFontImage("fonts/SourceHanSerifCN-Bold.otf");
    //     // CreateTexture(device, CHESS_FONT_ROOT_PATH"漢.png", pool, graphics);
    // }
}

Bing::Bing(){

}
Bing::~Bing(){

}
void Bing::Selected(const Chessboard *chessboard, std::vector<Ranks>&canplays){
    //兵不能后退
    const glm::vec2 behind = chessboard->GetCountryBehind(GetCountryColor());
    const glm::vec2 ranks[] = { glm::vec2(0, 1), glm::vec2(0, -1), glm::vec2(1, 0), glm::vec2(-1, 0) };
    // const Ranks ranks[] = { Ranks(mRow + 1, mColumn), Ranks(mRow - 1, mColumn), Ranks(mRow, mColumn + 1), Ranks(mRow, mColumn - 1) };
    for(uint32_t i = 0; i < sizeof(ranks) / sizeof(Ranks); ++i){
        if(ranks[i].x == behind.x && ranks[i].y == behind.y)continue;
        const glm::vec2 currentPos = glm::vec2(mColumn, mRow) + ranks[i];
        if(!chessboard->IsBoundary(currentPos.y, currentPos.x)){
            const Chess *chess = chessboard->GetChess(currentPos.y, currentPos.x);
            if(!chess || chess->GetCountryColor() != GetCountryColor()){
                canplays.push_back(Ranks(currentPos.y, currentPos.x));
            }
        }
    }
    chessboard->SelectChessInPalace(this, canplays);
}
void Bing::CreateFontTexture(VkDevice device, VkCommandPool pool, VkQueue graphics){
    unsigned char *bitmap = (unsigned char *)malloc(CHESS_FONT_IMAGE_WIDTH * CHESS_FONT_IMAGE_HEIGHT);
    memset(bitmap, 0, CHESS_FONT_IMAGE_WIDTH * CHESS_FONT_IMAGE_HEIGHT);
    GetFontImageData("fonts/SourceHanSerifCN-Bold.otf", L'兵', bitmap);
    vkf::CreateFontImage(device, bitmap, CHESS_FONT_IMAGE_WIDTH, CHESS_FONT_IMAGE_HEIGHT, mFont, pool, graphics);
    free(bitmap);

    // if(!CreateTexture(device, CHESS_FONT_ROOT_PATH"兵.png", pool, graphics)){
    //     // CreateFontImage("fonts/SourceHanSerifCN-Bold.otf");
    //     // CreateTexture(device, CHESS_FONT_ROOT_PATH"兵.png", pool, graphics);
    // }
}

Pao::Pao(){

}
Pao::~Pao(){
    
}
void Pao::Selected(const Chessboard *chessboard, std::vector<Ranks>&canplays){
    // bool bCapture = false;
    const glm::vec2 direction[] = { glm::vec2(1, 0), glm::vec2(-1, 0), glm::vec2(0, 1), glm::vec2(0, -1) };
    for(uint32_t i = 0; i < sizeof(direction) / sizeof(glm::vec2); ++i){
        const glm::vec2 currentPos = glm::vec2(mColumn, mRow);
        glm::vec2 pos = currentPos + direction[i];
        while(!chessboard->IsBoundary(pos.y, pos.x)){
            const Chess *chess = chessboard->GetChess(pos.y, pos.x);
            if(chess){
                pos += direction[i];
                while(!chessboard->IsBoundary(pos.y, pos.x)){
                    const Chess *chess = chessboard->GetChess(pos.y, pos.x);
                    if(chess && chess->GetCountryColor() != GetCountryColor()){
                        canplays.push_back(Ranks(pos.y, pos.x));
                        break;
                    }
                    pos += direction[i];
                }
                break;
            }
            else{
                canplays.push_back(Ranks(pos.y, pos.x));
            }
            pos += direction[i];
        }
    }
    chessboard->SelectChessInPalace(this, canplays);
}
void Pao::CreateFontTexture(VkDevice device, VkCommandPool pool, VkQueue graphics){
    unsigned char *bitmap = (unsigned char *)malloc(CHESS_FONT_IMAGE_WIDTH * CHESS_FONT_IMAGE_HEIGHT);
    memset(bitmap, 0, CHESS_FONT_IMAGE_WIDTH * CHESS_FONT_IMAGE_HEIGHT);
    GetFontImageData("fonts/SourceHanSerifCN-Bold.otf", L'炮', bitmap);
    vkf::CreateFontImage(device, bitmap, CHESS_FONT_IMAGE_WIDTH, CHESS_FONT_IMAGE_HEIGHT, mFont, pool, graphics);
    free(bitmap);

    // if(!CreateTexture(device, CHESS_FONT_ROOT_PATH"炮.png", pool, graphics)){
    //     // CreateFontImage("fonts/SourceHanSerifCN-Bold.otf");
    //     // CreateTexture(device, CHESS_FONT_ROOT_PATH"炮.png", pool, graphics);
    // }
}

Che::Che(){

}
Che::~Che(){
    
}
void Che::Selected(const Chessboard *chessboard, std::vector<Ranks>&canplays){
    const glm::vec2 direction[] = { glm::vec2(1, 0), glm::vec2(-1, 0), glm::vec2(0, 1), glm::vec2(0, -1) };
    for(uint32_t i = 0; i < sizeof(direction) / sizeof(glm::vec2); ++i){
        const glm::vec2 currentPos = glm::vec2(mColumn, mRow);
        glm::vec2 pos = currentPos + direction[i];
        while(!chessboard->IsBoundary(pos.y, pos.x)){
            const Chess *chess = chessboard->GetChess(pos.y, pos.x);
            if(!chess || chess->GetCountryColor() != GetCountryColor()){
                canplays.push_back(Ranks(pos.y, pos.x));
            }
            if(chess){
                if(chess->GetCountryColor() != GetCountryColor())canplays.push_back(Ranks(pos.y, pos.x));
                break;
            }
            pos += direction[i];
        }
    }
    chessboard->SelectChessInPalace(this, canplays);
}
void Che::CreateFontTexture(VkDevice device, VkCommandPool pool, VkQueue graphics){
    unsigned char *bitmap = (unsigned char *)malloc(CHESS_FONT_IMAGE_WIDTH * CHESS_FONT_IMAGE_HEIGHT);
    memset(bitmap, 0, CHESS_FONT_IMAGE_WIDTH * CHESS_FONT_IMAGE_HEIGHT);
    GetFontImageData("fonts/SourceHanSerifCN-Bold.otf", L'車', bitmap);
    vkf::CreateFontImage(device, bitmap, CHESS_FONT_IMAGE_WIDTH, CHESS_FONT_IMAGE_HEIGHT, mFont, pool, graphics);
    free(bitmap);

    // if(!CreateTexture(device, CHESS_FONT_ROOT_PATH"車.png", pool, graphics)){
    //     // CreateFontImage("fonts/SourceHanSerifCN-Bold.otf");
    //     // CreateTexture(device, CHESS_FONT_ROOT_PATH"車.png", pool, graphics);
    // }
}

Ma::Ma(){

}
Ma::~Ma(){
    
}
void Ma::Selected(const Chessboard *chessboard, std::vector<Ranks>&canplays){
    const Ranks ranks[] = {
        Ranks(mRow - 1, mColumn - 2), Ranks(mRow - 2, mColumn - 1), Ranks(mRow - 2, mColumn + 1), Ranks(mRow - 1, mColumn + 2),
        Ranks(mRow + 1, mColumn + 2), Ranks(mRow + 2, mColumn + 1), Ranks(mRow + 2, mColumn - 1), Ranks(mRow + 1, mColumn - 2)
    };
    for(uint32_t i = 0; i < sizeof(ranks) / sizeof(Ranks); ++i){
        if(!chessboard->IsBoundary(ranks[i].row, ranks[i].column)){//这里的马和相不会被"蹩马腿",所以不需要判断
            const Chess *chess = chessboard->GetChess(ranks[i].row, ranks[i].column);
            if(!chess || chess->GetCountryColor() != GetCountryColor()){
                canplays.push_back(Ranks(ranks[i].row, ranks[i].column));
            }
        }
    }
}
void Ma::CreateFontTexture(VkDevice device, VkCommandPool pool, VkQueue graphics){
    unsigned char *bitmap = (unsigned char *)malloc(CHESS_FONT_IMAGE_WIDTH * CHESS_FONT_IMAGE_HEIGHT);
    memset(bitmap, 0, CHESS_FONT_IMAGE_WIDTH * CHESS_FONT_IMAGE_HEIGHT);
    GetFontImageData("fonts/SourceHanSerifCN-Bold.otf", L'馬', bitmap);
    vkf::CreateFontImage(device, bitmap, CHESS_FONT_IMAGE_WIDTH, CHESS_FONT_IMAGE_HEIGHT, mFont, pool, graphics);
    free(bitmap);

    // if(!CreateTexture(device, CHESS_FONT_ROOT_PATH"馬.png", pool, graphics)){
    //     // CreateFontImage("fonts/SourceHanSerifCN-Bold.otf");
    //     // CreateTexture(device, CHESS_FONT_ROOT_PATH"馬.png", pool, graphics);
    // }
}
Xiang::Xiang(){

}
Xiang::~Xiang(){
    
}
void Xiang::Selected(const Chessboard *chessboard, std::vector<Ranks>&canplays){
    const Ranks ranks[] = { Ranks(mRow + 2, mColumn + 2), Ranks(mRow - 2, mColumn - 2), Ranks(mRow - 2, mColumn + 2), Ranks(mRow + 2, mColumn - 2) };
    for(uint32_t i = 0; i < sizeof(ranks) / sizeof(Ranks); ++i){
        if(!chessboard->IsBoundary(ranks[i].row, ranks[i].column)){
            const Chess *chess = chessboard->GetChess(ranks[i].row, ranks[i].column);
            if(!chess || chess->GetCountryColor() != GetCountryColor()){
                canplays.push_back(Ranks(ranks[i].row, ranks[i].column));
            }
        }
    }
}
void Xiang::CreateFontTexture(VkDevice device, VkCommandPool pool, VkQueue graphics){
    unsigned char *bitmap = (unsigned char *)malloc(CHESS_FONT_IMAGE_WIDTH * CHESS_FONT_IMAGE_HEIGHT);
    memset(bitmap, 0, CHESS_FONT_IMAGE_WIDTH * CHESS_FONT_IMAGE_HEIGHT);
    GetFontImageData("fonts/SourceHanSerifCN-Bold.otf", L'相', bitmap);
    vkf::CreateFontImage(device, bitmap, CHESS_FONT_IMAGE_WIDTH, CHESS_FONT_IMAGE_HEIGHT, mFont, pool, graphics);
    free(bitmap);

    // if(!CreateTexture(device, CHESS_FONT_ROOT_PATH"相.png", pool, graphics)){
    //     // CreateFontImage("fonts/SourceHanSerifCN-Bold.otf");
    //     // CreateTexture(device, CHESS_FONT_ROOT_PATH"相.png", pool, graphics);
    // }
}
Shi::Shi(){

}
Shi::~Shi(){
    
}
void Shi::Selected(const Chessboard *chessboard, std::vector<Ranks>&canplays){
    chessboard->SelectChessInPalace(this, canplays);
}
void Shi::CreateFontTexture(VkDevice device, VkCommandPool pool, VkQueue graphics){
    unsigned char *bitmap = (unsigned char *)malloc(CHESS_FONT_IMAGE_WIDTH * CHESS_FONT_IMAGE_HEIGHT);
    memset(bitmap, 0, CHESS_FONT_IMAGE_WIDTH * CHESS_FONT_IMAGE_HEIGHT);
    GetFontImageData("fonts/SourceHanSerifCN-Bold.otf", L'士', bitmap);
    vkf::CreateFontImage(device, bitmap, CHESS_FONT_IMAGE_WIDTH, CHESS_FONT_IMAGE_HEIGHT, mFont, pool, graphics);
    free(bitmap);

    // if(!CreateTexture(device, CHESS_FONT_ROOT_PATH"士.png", pool, graphics)){
    //     // CreateFontImage("fonts/SourceHanSerifCN-Bold.otf");
    //     // CreateTexture(device, CHESS_FONT_ROOT_PATH"士.png", pool, graphics);
    // }
}