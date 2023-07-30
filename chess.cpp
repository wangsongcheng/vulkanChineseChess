#include "chess.h"
#define STB_IMAGE_IMPLEMENTATION
#define STB_TRUETYPE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_truetype.h"
#include "stb_image_write.h"
bool GenerateFontImage(int bitmap_w, int bitmap_h, const wchar_t *word, uint32_t wordCount, const char *outImageName, const unsigned char *fontData, float pixels = 30){
	stbtt_fontinfo info;
	if(!stbtt_InitFont(&info, fontData, 0)){
		printf("in function:%s, stbtt_InitFont error\n", __FUNCTION__);
		return false;
	}
	unsigned char *bitmap = (unsigned char *)malloc(bitmap_w * bitmap_h);
	memset(bitmap, 0, bitmap_w * bitmap_h);
	// const int *word = L"世界你好";
	// printf("%x\n", (unsigned int *)word[0]);
	// printf("strlen(%s) = %d\n", c, strlen(c));
	// int word[20] = { L'你', L'好', L'世', L'界' };//字母的编码
	// int word[20] = { 0x4f60, 0x597d, 0x4e16, 0x754c };//字母的编码
	float scale = stbtt_ScaleForPixelHeight(&info, pixels);
	int ascent = 0, descent = 0, lineGap = 0;
	stbtt_GetFontVMetrics(&info, &ascent, &descent, &lineGap);
	ascent = roundf(ascent * scale);
	descent = roundf(descent * scale);
	int x = 0;
    for (int i = 0; i < wordCount; ++i){
        int advanceWidth = 0, leftSideBearing = 0, c_x1, c_y1, c_x2, c_y2;
        stbtt_GetCodepointHMetrics(&info, word[i], &advanceWidth, &leftSideBearing);
        stbtt_GetCodepointBitmapBox(&info, word[i], scale, scale, &c_x1, &c_y1, &c_x2, &c_y2);
        int y = ascent +c_y1;
        int byteOffset = x + roundf(leftSideBearing * scale) + (y * bitmap_w);
        stbtt_MakeCodepointBitmap(&info, bitmap + byteOffset, c_x2 - c_x1, c_y2 - c_y1, bitmap_w, scale, scale, word[i]);
        x += roundf(advanceWidth * scale);
        int kern;
        kern = stbtt_GetCodepointKernAdvance(&info, word[i], word[i + 1]);
        x += roundf(kern * scale);
    }
	stbi_write_png(outImageName, bitmap_w, bitmap_h, 1, bitmap, 0);
	free(bitmap);
    return true;
}
bool GenerateFontImage(int bitmap_w, int bitmap_h, wchar_t word, const char *outImageName, const unsigned char *fontData, float pixels = 30){
	stbtt_fontinfo info;
	if(!stbtt_InitFont(&info, fontData, 0)){
		printf("in function:%s, stbtt_InitFont error\n", __FUNCTION__);
		return false;
	}
	unsigned char *bitmap = (unsigned char *)malloc(bitmap_w * bitmap_h);
	memset(bitmap, 0, bitmap_w * bitmap_h);
	// const int *word = L"世界你好";
	// printf("%x\n", (unsigned int *)word[0]);
	// printf("strlen(%s) = %d\n", c, strlen(c));
	// int word[20] = { L'你', L'好', L'世', L'界' };//字母的编码
	// int word[20] = { 0x4f60, 0x597d, 0x4e16, 0x754c };//字母的编码
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
    stbtt_MakeCodepointBitmap(&info, bitmap + byteOffset, c_x2 - c_x1, c_y2 - c_y1, bitmap_w, scale, scale, word);
    x += roundf(advanceWidth * scale);
    int kern;
    kern = stbtt_GetCodepointKernAdvance(&info, word, word);
    x += roundf(kern * scale);
	stbi_write_png(outImageName, bitmap_w, bitmap_h, 1, bitmap, 0);
	free(bitmap);
    return true;
}
void CreateFontImage(const std::string&fontfile, const std::string&outPath = CHESS_FONT_ROOT_PATH){
	long int size = 0;
	unsigned char *fontBuffer = NULL;
	FILE *fontFile = fopen(fontfile.c_str(), "rb");
	if(fontFile){
		size = vkf::tool::GetFileSize(fontFile);
		fontBuffer = (unsigned char *)malloc(size);
		fread(fontBuffer, size, 1, fontFile);
		fclose(fontFile);
        const wchar_t word[] = { L'魏', L'蜀', L'吴', L'漢', L'兵', L'炮', L'車', L'馬', L'相', L'士' };
        for (size_t i = 0; i < sizeof(word) / sizeof(int); ++i){
            std::string path;
            if(word[i] == L'魏'){
                path = outPath + "魏";
            }
            else if(word[i] == L'蜀'){
                path = outPath + "蜀";
            }
            else if(word[i] == L'吴'){
                path = outPath + "吴";
            }
            else if(word[i] == L'漢'){
                path = outPath + "漢";
            }
            else if(word[i] == L'兵'){
                path = outPath + "兵";
            }
            else if(word[i] == L'炮'){
                path = outPath + "炮";
            }
            else if(word[i] == L'車'){
                path = outPath + "車";
            }
            else if(word[i] == L'馬'){
                path = outPath + "馬";
            }
            else if(word[i] == L'相'){
                path = outPath + "相";
            }
            else if(word[i] == L'士'){
                path = outPath + "士";
            }
            path += ".png";
		    GenerateFontImage(CHESS_FONT_IMAGE_WIDTH, CHESS_FONT_IMAGE_HEIGHT, word[i], path.c_str(), fontBuffer);
        }
		free(fontBuffer);
	}
	else{
		perror("open file error");
		printf("file name is %s\n", fontfile.c_str());
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
Chess::Chess(/* args */){
    
}

Chess::~Chess(){

}
void Chess::Cleanup(VkDevice device){
    mFont.Destroy(device);
    mIndex.Destroy(device);
    mVertex.Destroy(device);
    mPosition.Destroy(device);
}
void Chess::GetCircularVertex(const glm::vec3&color, std::vector<ChessVertex>&vertices){
    ChessVertex v = ChessVertex(glm::vec3(.0f), color);
    vertices.push_back(v);
    for (float i = 1; i < 362.0f; ++i){
        float r = i * M_PI / 180.0f;
        v.pos = glm::vec3(sin(r), cos(r), .0f);
        vertices.push_back(v);
    }
}

void Chess::CreateVertexAndIndex(VkDevice device, const glm::vec3&color, VkCommandPool pool, VkQueue graphics){
    std::vector<ChessVertex> circularVertices;
    GetCircularVertex(color, circularVertices);
    for (size_t i = 1; i <= circularVertices.size() * .5f; ++i){
        mCircularIndices.push_back(0);
        mCircularIndices.push_back(i);
        mCircularIndices.push_back(i + 1);
    }
    mIndex.CreateBuffer(device, sizeof(uint16_t) * mCircularIndices.size(), VK_BUFFER_USAGE_INDEX_BUFFER_BIT|VK_BUFFER_USAGE_TRANSFER_DST_BIT);
    mVertex.CreateBuffer(device, sizeof(ChessVertex) * circularVertices.size(), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT|VK_BUFFER_USAGE_TRANSFER_DST_BIT);
    mIndex.AllocateAndBindMemory(device, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    mVertex.AllocateAndBindMemory(device, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    vkf::tool::CopyBuffer(device, sizeof(uint16_t) * mCircularIndices.size(), mCircularIndices.data(), graphics, pool, mIndex);
    vkf::tool::CopyBuffer(device, sizeof(ChessVertex) * circularVertices.size(), mCircularIndices.data(), graphics, pool, mVertex);

    mPosition.CreateBuffer(device, sizeof(glm::mat4), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
    mPosition.AllocateAndBindMemory(device, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT|VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    UpdateUniform(device, mPos);
}
void Chess::DrawFont(VkCommandBuffer cmd, GraphicsPipeline font){
    // font.BindDescriptorSet(cmd);
}
void Chess::DrawCircular(VkCommandBuffer cmd, GraphicsPipeline circular){
    // circular.BindDescriptorSet(cmd);
    VkDeviceSize offset = 0;
    vkCmdBindVertexBuffers(cmd, 0, 1, &mVertex.buffer, &offset);
    // vkCmdDrawIndexed()
    circular.BindDescriptorSet(cmd, mSet);
}
void Chess::UpdateUniform(VkDevice device, const glm::vec2&pos){
    mPos = pos;
    const glm::mat4 model = glm::scale(glm::translate(glm::mat4(1), glm::vec3(mPos, 0)), glm::vec3(CHESS_WIDTH, CHESS_HEIGHT,1));
    mPosition.UpdateData(device, &model);    
}
void Chess::UpdateSet(VkDevice device, VkDescriptorPool pool, GraphicsPipeline font, GraphicsPipeline circular, VkSampler textureSampler){
    //一般都不会更新多次,所以可以在这里分配
    circular.AllocateDescriptorSets(device, pool, 1, &mSet);
    font.AllocateDescriptorSets(device, pool, 1, &mFontSet);

    circular.UpdateDescriptorSets(device, { mPosition }, {}, mSet);
    font.UpdateDescriptorSets(device, { mPosition }, { mFont }, mFontSet, textureSampler);
}
bool Chess::isSelect(const glm::vec2&pos){
    
}
Wei::Wei(){

}
Wei::~Wei(){

}
void Wei::Selected(std::vector<glm::vec2>&pos){

}
void Wei::UpdateUniform(VkDevice device, const glm::vec2&pos){
    mFirstPos = pos;
    Chess::UpdateUniform(device, pos);
}
void Wei::CreateFontTexture(VkDevice device, VkCommandPool pool, VkQueue graphics){
    if(!CreateTexture(device, CHESS_FONT_ROOT_PATH"魏.png", pool, graphics)){
        CreateFontImage("fonts/SourceHanSerifCN-Bold.otf");
    }
}

Shu::Shu(){

}
Shu::~Shu(){

}
void Shu::Selected(std::vector<glm::vec2>&pos){

}
void Shu::UpdateUniform(VkDevice device, const glm::vec2&pos){
    mFirstPos = pos;
    Chess::UpdateUniform(device, pos);
}
void Shu::CreateFontTexture(VkDevice device, VkCommandPool pool, VkQueue graphics){
    if(!CreateTexture(device, CHESS_FONT_ROOT_PATH"蜀.png", pool, graphics)){
        CreateFontImage("fonts/SourceHanSerifCN-Bold.otf");
    }
}

Wu::Wu(){

}
Wu::~Wu(){

}
void Wu::Selected(std::vector<glm::vec2>&pos){

}
void Wu::UpdateUniform(VkDevice device, const glm::vec2&pos){
    mFirstPos = pos;
    Chess::UpdateUniform(device, pos);
}
void Wu::CreateFontTexture(VkDevice device, VkCommandPool pool, VkQueue graphics){
    if(!CreateTexture(device, CHESS_FONT_ROOT_PATH"吴.png", pool, graphics)){
        CreateFontImage("fonts/SourceHanSerifCN-Bold.otf");
    }
}

Han::Han(){

}
Han::~Han(){

}
void Han::Selected(std::vector<glm::vec2>&pos){

}
void Han::UpdateUniform(VkDevice device, const glm::vec2&pos){
    mFirstPos = pos;
    Chess::UpdateUniform(device, pos);
}
void Han::CreateFontTexture(VkDevice device, VkCommandPool pool, VkQueue graphics){
    if(!CreateTexture(device, CHESS_FONT_ROOT_PATH"吴.png", pool, graphics)){
        CreateFontImage("fonts/SourceHanSerifCN-Bold.otf");
    }
}

Bing::Bing(){

}
Bing::~Bing(){

}
void Bing::Selected(std::vector<glm::vec2>&pos){

}
void Bing::CreateFontTexture(VkDevice device, VkCommandPool pool, VkQueue graphics){

}

Pao::Pao(){

}
Pao::~Pao(){
    
}
void Pao::Selected(std::vector<glm::vec2>&pos){

}
void Pao::CreateFontTexture(VkDevice device, VkCommandPool pool, VkQueue graphics){

}

Che::Che(){

}
Che::~Che(){
    
}
void Che::Selected(std::vector<glm::vec2>&pos){

}
void Che::CreateFontTexture(VkDevice device, VkCommandPool pool, VkQueue graphics){

}

Ma::Ma(){

}
Ma::~Ma(){
    
}
void Ma::Selected(std::vector<glm::vec2>&pos){

}
void Ma::CreateFontTexture(VkDevice device, VkCommandPool pool, VkQueue graphics){

}
Xiang::Xiang(){

}
Xiang::~Xiang(){
    
}
void Xiang::Selected(std::vector<glm::vec2>&pos){

}
void Xiang::CreateFontTexture(VkDevice device, VkCommandPool pool, VkQueue graphics){

}
Shi::Shi(){

}
Shi::~Shi(){
    
}
void Shi::Selected(std::vector<glm::vec2>&pos){

}
void Shi::CreateFontTexture(VkDevice device, VkCommandPool pool, VkQueue graphics){

}