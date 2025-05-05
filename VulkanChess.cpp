#include "font.h"
#include <string.h>
#include "VulkanChess.h"
#include "Game.h"
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
	FILE *fontFile = fopen("fonts/ukai.ttc", "rb");
	if(fontFile){
		size = font::GetFileSize(fontFile);
		fontBuffer = (unsigned char *)malloc(size);
		fread(fontBuffer, size, 1, fontFile);
	}
	else{
		perror("open file error");
		printf("file name is fonts/ukai.ttc\n");
        return;
	}
    const uint32_t imageSize = FONT_WIDTH * FONT_HEIGHT;
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
    const uint32_t fontImageCount = sizeof(word) / sizeof(wchar_t);
    std::vector<unsigned char *>datas(fontImageCount);
    for (size_t i = 0; i < fontImageCount; ++i){
        datas[i] = new unsigned char[imageSize];
        memset(datas[i], 0, imageSize);
        font::GetFontImageData(fontBuffer, FONT_WIDTH, word[i], datas[i]);
    }
    fonts.image.CreateImageArray(device, (void *const *)datas.data(), fontImageCount, FONT_WIDTH, FONT_HEIGHT, 1, graphics, pool);
    for (auto&it:datas){
        delete[]it;
    }
    fclose(fontFile);
    free(fontBuffer);
}
void VulkanChess::CreateChessResource(VulkanDevice device, VulkanPool pool, VkQueue graphics){
    std::vector<Vertex> circularVertices;
    GetCircularVertex(glm::vec3(0), circularVertices);
    mChess.vertexCount = circularVertices.size();
    std::vector<uint16_t> circularIndices;
    for (size_t i = 1; i < circularVertices.size(); ++i){
        circularIndices.push_back(i);
        circularIndices.push_back(i + 1);
        circularIndices.push_back(0);
    }
    GetCircularVertex(glm::vec3(.8), circularVertices);

    // //这里得准备4*2种颜色的棋子
    // std::vector<Vertex> circularVertices;
    // glm::vec3 countryColor[] = { SHU_COUNTRY_COLOR, WU_COUNTRY_COLOR, WEI_COUNTRY_COLOR, HAN_COUNTRY_COLOR };

    // GetCircularVertex(countryColor[0], circularVertices);
    // mChess.vertexCount = circularVertices.size();
    // std::vector<uint16_t> circularIndices;
    // for (size_t i = 1; i < circularVertices.size(); ++i){
    //     circularIndices.push_back(i);
    //     circularIndices.push_back(i + 1);
    //     circularIndices.push_back(0);
    // }
    // mChess.indexCount = circularIndices.size();
    // for (size_t uiCountry = 1; uiCountry < sizeof(countryColor) / sizeof(glm::vec3); ++uiCountry){
    //     GetCircularVertex(countryColor[uiCountry], circularVertices);
    // }
    // for (size_t uiCountry = 0; uiCountry < sizeof(countryColor) / sizeof(glm::vec3); ++uiCountry){
    //     const glm::vec3 banColor = glm::vec3(countryColor[uiCountry].r * .5, countryColor[uiCountry].g * .5, countryColor[uiCountry].b * .5);
    //     GetCircularVertex(banColor, circularVertices);
    // }
    mChess.CreateIndexBuffer(device, circularIndices.data(), sizeof(uint16_t) * circularIndices.size(), graphics, pool);
    //8是4种颜色+亮度较低的颜色(表示不是该玩家下子)
    mChess.CreateVertexBuffer(device, circularVertices.data(), sizeof(Vertex) * circularVertices.size(), mChess.vertexCount, graphics, pool);
}
void VulkanChess::UpdateDescriptorSet(VkDevice device){
    VkDescriptorSetLayoutBinding descriptorSetLayoutBindings[2] = {};
    // descriptorSetLayoutBindings[0].binding = 0;
    descriptorSetLayoutBindings[0].descriptorCount = 1;
    descriptorSetLayoutBindings[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    descriptorSetLayoutBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
    descriptorSetLayoutBindings[1].binding = 1;
    descriptorSetLayoutBindings[1].descriptorCount = 1;
    descriptorSetLayoutBindings[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    descriptorSetLayoutBindings[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    vulkanFrame::UpdateDescriptorSets(device, descriptorSetLayoutBindings, 1, {uniform.chess.white},{}, descriptorSet.chess.white);
    vulkanFrame::UpdateDescriptorSets(device, descriptorSetLayoutBindings, 1, {uniform.chess.black},{}, descriptorSet.chess.black);
    vulkanFrame::UpdateDescriptorSets(device, descriptorSetLayoutBindings, 1, {uniform.chess.basewhite},{}, descriptorSet.chess.basewhite);
    // vulkanFrame::UpdateDescriptorSets(device, descriptorSetLayoutBindings, 1, {uniform.chess},{}, descriptorSet.chess);
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
    uniform.chess.basewhite.Destroy(device);
    uniform.chess.white.Destroy(device);
    uniform.chess.black.Destroy(device);
    // uniform.chess.Destroy(device);
    vkDestroySampler(device, fonts.sampler, VK_NULL_HANDLE);
}

void VulkanChess::Setup(VulkanDevice device, VkDescriptorSetLayout layout, VkQueue graphics, VulkanPool pool){
    auto&physicalDeviceProperties = device.GetPhysicalDeviceProperties();
    uint32_t minUniformBufferOffset = ALIGN(sizeof(glm::mat4), physicalDeviceProperties.limits.minUniformBufferOffsetAlignment);
    uint32_t mFontMinUniformBufferOffset = ALIGN(sizeof(FontUniform), physicalDeviceProperties.limits.minUniformBufferOffsetAlignment);

    vulkanFrame::CreateTextureSampler(device.device, fonts.sampler);
    CreateFontResource(device, pool, graphics);
    CreateChessResource(device, pool, graphics);

    uniform.chess.white.CreateBuffer(device, minUniformBufferOffset * DRAW_CHESS_COUNT * MAX_COUNTRY_INDEX, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT|VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    uniform.chess.white.size = minUniformBufferOffset;
    uniform.chess.black.CreateBuffer(device, minUniformBufferOffset * DRAW_CHESS_COUNT * MAX_COUNTRY_INDEX, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT|VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    uniform.chess.black.size = minUniformBufferOffset;
    uniform.chess.basewhite.CreateBuffer(device, minUniformBufferOffset * DRAW_CHESS_COUNT * MAX_COUNTRY_INDEX, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT|VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    uniform.chess.basewhite.size = minUniformBufferOffset;

    // uniform.chess.CreateBuffer(device, minUniformBufferOffset * DRAW_CHESS_COUNT * MAX_COUNTRY_INDEX, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT|VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    // uniform.chess.size = minUniformBufferOffset;

    uniform.font.CreateBuffer(device, mFontMinUniformBufferOffset * DRAW_CHESS_COUNT * MAX_COUNTRY_INDEX, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT|VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    uniform.font.size = mFontMinUniformBufferOffset;

    pool.AllocateDescriptorSets(device.device, &layout, 1, &descriptorSet.font);
    pool.AllocateDescriptorSets(device.device, &layout, 1, &descriptorSet.chess.white);
    pool.AllocateDescriptorSets(device.device, &layout, 1, &descriptorSet.chess.black);
    pool.AllocateDescriptorSets(device.device, &layout, 1, &descriptorSet.chess.basewhite);
    // pool.AllocateDescriptorSets(device.device, &layout, 1, &descriptorSet.chess);

    UpdateDescriptorSet(device.device);
}

void VulkanChess::UpdateUniform(VkDevice device, uint32_t fontIndex, uint32_t country, const glm::vec2&pos, uint32_t dynamicOffsets){
    UpdateUniform(device, fontIndex, country, pos, CHESS_WIDTH,CHESS_HEIGHT, dynamicOffsets);
}

void VulkanChess::UpdateUniform(VkDevice device, uint32_t fontIndex, uint32_t country, const glm::vec2 &pos, uint32_t width, uint32_t height, uint32_t dynamicOffsets){
    glm::mat4 model = glm::scale(glm::translate(glm::mat4(1), glm::vec3(pos.x, pos.y, 0)), glm::vec3(width * 1.1, height * 1.1, 1));
    uniform.chess.basewhite.UpdateData(device, &model, dynamicOffsets * uniform.chess.basewhite.size);
    model = glm::scale(glm::translate(glm::mat4(1), glm::vec3(pos.x, pos.y, 0)), glm::vec3(width * .95, height * .95, 1));
    uniform.chess.black.UpdateData(device, &model, dynamicOffsets * uniform.chess.black.size);
    model = glm::scale(glm::translate(glm::mat4(1), glm::vec3(pos.x, pos.y, 0)), glm::vec3(width * .85, height * .85, 1));
    uniform.chess.white.UpdateData(device, &model, dynamicOffsets * uniform.chess.white.size);
    // uniform.chess.UpdateData(device, &model, dynamicOffsets * uniform.chess.size);

    FontUniform fontUbo;
    fontUbo.imageIndex = fontIndex;
    glm::vec3 fontPos = glm::vec3(pos.x, pos.y, 0);
    if(fontIndex == FONT_INDEX_SHI){
        fontPos.x -=  width * .8;
    }
    else{
        fontPos.x -=  width * .8;
    }
    fontPos.y -= height;
    glm::vec3 countryColor[] = { SHU_COUNTRY_COLOR, WU_COUNTRY_COLOR, WEI_COUNTRY_COLOR, HAN_COUNTRY_COLOR };
    fontUbo.color = countryColor[country];
    fontUbo.model = glm::scale(glm::translate(glm::mat4(1), fontPos), glm::vec3(width * 1.8, height * 1.8, 1));
    uniform.font.UpdateData(device, &fontUbo, dynamicOffsets * uniform.font.size);
}

void VulkanChess::DrawFont(VkCommandBuffer command, VkPipelineLayout layout){
    uint32_t dynamicOffsets;
    mFont.Bind(command);
    for (size_t i = 0; i < DRAW_CHESS_COUNT * MAX_COUNTRY_INDEX; ++i){
        dynamicOffsets = i * uniform.font.size;
        vkCmdBindDescriptorSets(command, VK_PIPELINE_BIND_POINT_GRAPHICS, layout, 0, 1, &descriptorSet.font, 1, &dynamicOffsets);
        mFont.Draw(command);
    }
}
void VulkanChess::DrawChess(VkCommandBuffer command, VkPipelineLayout layout){
    mChess.Bind(command);
    uint32_t dynamicOffsets = 0;
    for (size_t uiCountry = 0; uiCountry < MAX_COUNTRY_INDEX; ++uiCountry){
        for (size_t uiChess = 0; uiChess < DRAW_CHESS_COUNT; ++uiChess){
            dynamicOffsets = ROW_COLUMN_TO_INDEX(uiCountry, uiChess, DRAW_CHESS_COUNT) * uniform.chess.basewhite.size;
            vkCmdBindDescriptorSets(command, VK_PIPELINE_BIND_POINT_GRAPHICS, layout, 0, 1, &descriptorSet.chess.basewhite, 1, &dynamicOffsets);
            mChess.Draw(command, mChess.vertexCount);
            // mChess.Draw(command, uiCountry == currentCountry?uiCountry * mChess.vertexCount:(MAX_COUNTRY_INDEX + uiCountry) * mChess.vertexCount);
        }
    }
    for (size_t uiCountry = 0; uiCountry < MAX_COUNTRY_INDEX; ++uiCountry){
        for (size_t uiChess = 0; uiChess < DRAW_CHESS_COUNT; ++uiChess){
            dynamicOffsets = ROW_COLUMN_TO_INDEX(uiCountry, uiChess, DRAW_CHESS_COUNT) * uniform.chess.black.size;
            vkCmdBindDescriptorSets(command, VK_PIPELINE_BIND_POINT_GRAPHICS, layout, 0, 1, &descriptorSet.chess.black, 1, &dynamicOffsets);
            mChess.Draw(command);
        }
    }
    for (size_t uiCountry = 0; uiCountry < MAX_COUNTRY_INDEX; ++uiCountry){
        for (size_t uiChess = 0; uiChess < DRAW_CHESS_COUNT; ++uiChess){
            dynamicOffsets = ROW_COLUMN_TO_INDEX(uiCountry, uiChess, DRAW_CHESS_COUNT) * uniform.chess.white.size;
            vkCmdBindDescriptorSets(command, VK_PIPELINE_BIND_POINT_GRAPHICS, layout, 0, 1, &descriptorSet.chess.white, 1, &dynamicOffsets);
            mChess.Draw(command, mChess.vertexCount);
        }
    }
}