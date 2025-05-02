#include "VulkanChessboard.h"
void GetRectVertices(const glm::vec3&color, float *ptrVertices){
    const float v[] = {
        .0f, 1.0f, .0f, color.x, color.y, color.z,//左下
        1.0f, .0f, .0f, color.x, color.y, color.z,//右上
        .0f, .0f, .0f, color.x, color.y, color.z, //左上
        1.0f, 1.0f, .0f, color.x, color.y, color.z//右下
    };
    memcpy(ptrVertices, v, sizeof(v));
}
void VulkanChessboard::UpdateGridUniform(VkDevice device){
    glm::mat4 model[CHESSBOARD_RECT_COUNT * CHESSBOARD_RECT_COUNT];
    glm::vec2 gridPos[CHESSBOARD_RECT_COUNT][CHESSBOARD_RECT_COUNT];
    const uint32_t gridBigPointOffset = CHESSBOARD_RECT_SIZE;
    const glm::mat4 scale = glm::scale(glm::mat4(1), glm::vec3(CHESSBOARD_RECT_SIZE, CHESSBOARD_RECT_SIZE, 1));
    for (size_t uiRow = 0; uiRow < CHESSBOARD_RECT_COUNT; ++uiRow){
        for (size_t uiColumn = 0; uiColumn < CHESSBOARD_RECT_COUNT; ++uiColumn){
            gridPos[uiRow][uiColumn] = glm::vec2(uiRow * CHESSBOARD_RECT_SIZE + (uiRow + 1) * CHESSBOARD_LINE_WIDTH + gridBigPointOffset, uiColumn * CHESSBOARD_RECT_SIZE + (uiColumn + 1) * CHESSBOARD_LINE_WIDTH + gridBigPointOffset);
            model[ROW_COLUMN_TO_INDEX(uiRow, uiColumn, CHESSBOARD_RECT_COUNT)] = glm::translate(glm::mat4(1), glm::vec3(gridPos[uiRow][uiColumn], 0)) * scale;
        }
    }
    uniforms.grid.UpdateData(device, sizeof(model), model);
}
void VulkanChessboard::UpdateBigGridUniform(VkDevice device){
    glm::mat4 model[CHESSBOARD_BIG_GRID_COUNT];
    const glm::mat4 scale = glm::scale(glm::mat4(1), glm::vec3(CHESSBOARD_BIG_RECT_SIZE, CHESSBOARD_BIG_RECT_SIZE, 1));
    const glm::vec3 bigGridPos[] = {
        glm::vec3(CHESSBOARD_LINE_WIDTH + CHESSBOARD_RECT_SIZE, CHESSBOARD_LINE_WIDTH + CHESSBOARD_RECT_SIZE, 0),
        //最上边还有一个小矩形和一条线，所以需要加上
        glm::vec3(CHESSBOARD_RECT_SIZE + CHESSBOARD_LINE_WIDTH + (CHESSBOARD_BOUNDARY_CENTER_RECT_COUNT + CHESSBOARD_BING_GRID_DENSITY) * CHESSBOARD_RECT_SIZE +  (CHESSBOARD_BOUNDARY_CENTER_RECT_COUNT + CHESSBOARD_BING_GRID_DENSITY) * CHESSBOARD_LINE_WIDTH, bigGridPos[0].y, 0),
        glm::vec3(bigGridPos[0].x, bigGridPos[1].x, 0),
        glm::vec3(bigGridPos[1].x, bigGridPos[2].y, 0)
    };
    for (size_t i = 0; i < CHESSBOARD_BIG_GRID_COUNT; ++i){
        model[i] = glm::translate(glm::mat4(1.0f), bigGridPos[i]) * scale;
    }
    uniforms.bigGrid.UpdateData(device, sizeof(model), model);
}
void VulkanChessboard::UpdateJiuGongGeWireframeUniform(VkDevice device){
    const glm::mat4 gridScale = glm::scale(glm::mat4(1.0f), glm::vec3(CHESSBOARD_RECT_SIZE + CHESSBOARD_LINE_WIDTH, CHESSBOARD_RECT_SIZE + CHESSBOARD_LINE_WIDTH, 1));
    //COLUMN_TO_X宏已经算好坐标，这里只需要计算行列即可
    const glm::vec2 wu_jiugongge[] = {
        //因为要退2格，所以减2
        //wu_jiugongge[0]在左下角
        glm::vec2(CHESSBOARD_COLUMN - 2, CHESSBOARD_ROW / 2), glm::vec2(wu_jiugongge[0].x + 1, wu_jiugongge[0].y - 1),
        glm::vec2(wu_jiugongge[0].x, wu_jiugongge[1].y), glm::vec2(wu_jiugongge[1].x, wu_jiugongge[0].y)
    };
    const glm::vec2 wei_jiugongge[] = {
        //wei_jiugongge[0]在右上角
        glm::vec2(CHESSBOARD_COLUMN / 2, 0), glm::vec2(wei_jiugongge[0].x - 1, wei_jiugongge[0].y + 1),
        glm::vec2(wei_jiugongge[0].x, wei_jiugongge[1].y), glm::vec2(wei_jiugongge[1].x, wei_jiugongge[0].y)
    };
    const glm::vec2 shu_jiugongge[] = {
        //shu_jiugongge[0]在右上角
        glm::vec2(CHESSBOARD_COLUMN / 2, CHESSBOARD_ROW - 2), glm::vec2(shu_jiugongge[0].x - 1, shu_jiugongge[0].y + 1),
        glm::vec2(shu_jiugongge[1].x, shu_jiugongge[0].y), glm::vec2(shu_jiugongge[0].x, shu_jiugongge[1].y)
    };
    const glm::vec2 han_jiugongge[] = {
        //han_jiugongge[0]在右下角
        glm::vec2(0, CHESSBOARD_COLUMN / 2), glm::vec2(han_jiugongge[0].x + 1, han_jiugongge[0].y - 1),
        glm::vec2(han_jiugongge[1].x, han_jiugongge[0].y), glm::vec2(han_jiugongge[0].x, han_jiugongge[1].y)
    };
    const glm::vec2 jiugonggePos[] = {
        //线框[0]
        wei_jiugongge[0], wei_jiugongge[1], shu_jiugongge[0], shu_jiugongge[1], wu_jiugongge[0], wu_jiugongge[1], han_jiugongge[0], han_jiugongge[1],
        //线框[1]
        wei_jiugongge[2], wei_jiugongge[3], shu_jiugongge[2], shu_jiugongge[3], wu_jiugongge[2], wu_jiugongge[3], han_jiugongge[2], han_jiugongge[3]
    };
    glm::mat4 model[CHESSBOARD_PALACE_COUNT];
    for (size_t i = 0; i < CHESSBOARD_PALACE_COUNT; ++i){
        model[i] = glm::translate(glm::mat4(1), glm::vec3(WIREFRAME_COLUMN_TO_X(jiugonggePos[i].x), WIREFRAME_ROW_TO_Y(jiugonggePos[i].y), 0)) * gridScale;
    }
    uniforms.wireframe.jiugongge.UpdateData(device, sizeof(model), &model);
}
void VulkanChessboard::UpdateBackgroundUniform(VkDevice device, uint32_t windowWidth){
    const glm::mat4 model[] = {
        glm::scale(glm::mat4(1.0f), glm::vec3(windowWidth, windowWidth, 1)), 
        glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(CHESSBOARD_RECT_SIZE, CHESSBOARD_RECT_SIZE, 0)), glm::vec3(CHESSBOARD_BACKGROUND_SIZE(windowWidth), CHESSBOARD_BACKGROUND_SIZE(windowWidth), 1)), 
    };
    uniforms.background.UpdateData(device, sizeof(model), model);
}
void VulkanChessboard::CreateFontResource(VulkanDevice device, VkQueue graphics, VulkanPool pool){
    const uint16_t indices[] = { 0, 1, 2, 0, 3, 1 };
    const float vertices[] = {
        .0f, 1.0f, .0f, 0.0f, 1.0f, 0,//左下
        1.0f, .0f, .0f, 1.0f, 0.0f, 0,//右上
        .0f, .0f, .0f, 0.0f, 0.0f, 0, //左上
        1.0f, 1.0f, .0f, 1.0f, 1.0f, 0//右下
    };
    fonts.font.CreateIndexBuffer(device, indices, sizeof(indices), graphics, pool);
    fonts.font.CreateVertexBuffer(device, vertices, sizeof(vertices), 4, graphics, pool);
	long int size = 0;
	unsigned char *fontBuffer = NULL;
	FILE *fontFile = fopen("fonts/SourceHanSerifCN-Bold.otf", "rb");
	if(fontFile){
		size = font::GetFileSize(fontFile);
		fontBuffer = (unsigned char *)malloc(size);
		fread(fontBuffer, size, 1, fontFile);
	}
	else{
		perror("open file error");
		printf("file name is fonts/SourceHanSerifCN-Bold.otf\n");
        return;
	}
    const uint32_t imageSize = ALLIANCE_POINT_FONT_WIDTH * ALLIANCE_POINT_FONT_HEIGHT;
    wchar_t word[3];
    word[ALLIANCE_POINT_FONT_INDEX_WU] = L'吴';
    word[ALLIANCE_POINT_FONT_INDEX_WEI] = L'魏';
    word[ALLIANCE_POINT_FONT_INDEX_SHU] = L'蜀';
    const uint32_t fontImageCount = sizeof(word) / sizeof(wchar_t);
    std::vector<unsigned char *>datas(fontImageCount);
    for (size_t i = 0; i < fontImageCount; ++i){
        datas[i] = new unsigned char[imageSize];
        memset(datas[i], 0, imageSize);
        font::GetFontImageData(fontBuffer, ALLIANCE_POINT_FONT_WIDTH, word[i], datas[i]);
    }
    fonts.image.CreateImageArray(device, (void *const *)datas.data(), fontImageCount, ALLIANCE_POINT_FONT_WIDTH, ALLIANCE_POINT_FONT_HEIGHT, 1, graphics, pool);
    for (auto&it:datas){
        delete[]it;
    }
    fclose(fontFile);
    free(fontBuffer);
}
void VulkanChessboard::CreateRectResource(VulkanDevice device, VulkanPool pool, VkQueue graphics){
    float vertices[2 * 4 * 6];
    const uint16_t indices[] = {
        0, 1, 2, 0, 3, 1,//.../
        0, 2, 3, 2, 3, 1
    };
    GetRectVertices(glm::vec3(.7, .5, .3), vertices);
    GetRectVertices(glm::vec3(0), vertices + 4 * 6);
    mRect.indexCount = 6;
    mRect.vertexCount = 4;
    mRect.CreateVertexBuffer(device, vertices, sizeof(vertices), mRect.vertexCount, graphics, pool);
    mRect.index.CreateBuffer(device, sizeof(indices), VK_BUFFER_USAGE_INDEX_BUFFER_BIT|VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    mRect.index.UpdateData(device, indices, graphics, pool);
}
void VulkanChessboard::UpdateDescriptorSet(VkDevice device){
    VkDescriptorSetLayoutBinding descriptorSetLayoutBindings[2] = {};
    // descriptorSetLayoutBindings[0].binding = 0;
    descriptorSetLayoutBindings[0].descriptorCount = 1;
    descriptorSetLayoutBindings[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    descriptorSetLayoutBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
    descriptorSetLayoutBindings[1].binding = 1;
    descriptorSetLayoutBindings[1].descriptorCount = 1;
    descriptorSetLayoutBindings[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    descriptorSetLayoutBindings[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    vulkanFrame::UpdateDescriptorSets(device, descriptorSetLayoutBindings, 1, {uniforms.grid},{}, descriptorSet.grid);
    vulkanFrame::UpdateDescriptorSets(device, descriptorSetLayoutBindings, 1, {uniforms.bigGrid},{}, descriptorSet.bigGrid);
    vulkanFrame::UpdateDescriptorSets(device, descriptorSetLayoutBindings, 1, {uniforms.alliance},{}, descriptorSet.alliance);
    vulkanFrame::UpdateDescriptorSets(device, descriptorSetLayoutBindings, 1, {uniforms.background},{}, descriptorSet.background);
    vulkanFrame::UpdateDescriptorSets(device, descriptorSetLayoutBindings, 1, {uniforms.wireframe.jiugongge},{}, descriptorSet.wireframe.jiugongge);
    vulkanFrame::UpdateDescriptorSets(device, descriptorSetLayoutBindings, 2, { fonts.uniforms.alliance},{ fonts.image }, fonts.descriptrorSet.alliance, fonts.sampler);
}

VulkanChessboard::VulkanChessboard(/* args */){
}

VulkanChessboard::~VulkanChessboard(){
}
void VulkanChessboard::Cleanup(VkDevice device){
    mRect.Destroy(device);
    mCircle.Destroy(device);
    uniforms.grid.Destroy(device);
    uniforms.bigGrid.Destroy(device);
    uniforms.alliance.Destroy(device);
    uniforms.background.Destroy(device);
    uniforms.wireframe.jiugongge.Destroy(device);

    fonts.font.Destroy(device);
    fonts.image.Destroy(device);
    fonts.uniforms.alliance.Destroy(device);
    vkDestroySampler(device, fonts.sampler, VK_NULL_HANDLE);
}

void VulkanChessboard::Draw(VkCommandBuffer command, VkPipelineLayout layout){
    uint32_t dynamicOffsets;
    mRect.Bind(command);
    for (size_t i = 0; i < 2; i++){
        dynamicOffsets = i * uniforms.grid.size;
        vkCmdBindDescriptorSets(command, VK_PIPELINE_BIND_POINT_GRAPHICS, layout, 0, 1, &descriptorSet.background, 1, &dynamicOffsets);
        mRect.Draw(command, i * mRect.vertexCount);
    }
    for (size_t i = 0; i < CHESSBOARD_RECT_COUNT * CHESSBOARD_RECT_COUNT; ++i){
        dynamicOffsets = i * uniforms.grid.size;
        vkCmdBindDescriptorSets(command, VK_PIPELINE_BIND_POINT_GRAPHICS, layout, 0, 1, &descriptorSet.grid, 1, &dynamicOffsets);
        mRect.Draw(command);
    }
    for (size_t i = 0; i < CHESSBOARD_BIG_GRID_COUNT; ++i){
        dynamicOffsets = i * uniforms.grid.size;
        vkCmdBindDescriptorSets(command, VK_PIPELINE_BIND_POINT_GRAPHICS, layout, 0, 1, &descriptorSet.bigGrid, 1, &dynamicOffsets);
        mRect.Draw(command);
    }
    mCircle.Bind(command);
    for (size_t i = 0; i < MAX_COUNTRY_INDEX - 1; ++i){
        dynamicOffsets = i * uniforms.alliance.size;
        vkCmdBindDescriptorSets(command, VK_PIPELINE_BIND_POINT_GRAPHICS, layout, 0, 1, &descriptorSet.alliance, 1, &dynamicOffsets);
        mCircle.Draw(command);        
    }

}

void VulkanChessboard::DrawWireframe(VkCommandBuffer command, VkPipelineLayout layout){
    uint32_t indexOffset = 0;
    uint32_t dynamicOffsets;
    mRect.Bind(command);
    for (size_t i = 0; i < CHESSBOARD_PALACE_COUNT; ++i){
        dynamicOffsets = i * uniforms.grid.size;
        if(i < CHESSBOARD_PALACE_COUNT * .5)
            indexOffset = 0;
        else
            indexOffset = mRect.indexCount;
        vkCmdBindDescriptorSets(command, VK_PIPELINE_BIND_POINT_GRAPHICS, layout, 0, 1, &descriptorSet.wireframe.jiugongge, 1, &dynamicOffsets);
        mRect.Draw(command, 4, indexOffset);
    }
}

void VulkanChessboard::DrawAllianceFont(VkCommandBuffer command, VkPipelineLayout layout){
    uint32_t dynamicOffsets;
    fonts.font.Bind(command);
    for (size_t i = 0; i < MAX_COUNTRY_INDEX - 1; ++i){
        dynamicOffsets = i * fonts.uniforms.alliance.size;
        vkCmdBindDescriptorSets(command, VK_PIPELINE_BIND_POINT_GRAPHICS, layout, 0, 1, &fonts.descriptrorSet.alliance, 1, &dynamicOffsets);
        fonts.font.Draw(command);
    }
}

void VulkanChessboard::Setup(VulkanDevice device, VkDescriptorSetLayout setLayout, VkQueue graphics, VulkanPool pool){
    auto&physicalDeviceProperties = device.GetPhysicalDeviceProperties();
    uint32_t minUniformBufferOffset = ALIGN(sizeof(glm::mat4), physicalDeviceProperties.limits.minUniformBufferOffsetAlignment);
    uint32_t minFontUniformBufferOffset = ALIGN(sizeof(FontUniform), physicalDeviceProperties.limits.minUniformBufferOffsetAlignment);

    vulkanFrame::CreateTextureSampler(device.device, fonts.sampler);

    CreateRectResource(device, pool, graphics);
    CreateFontResource(device, graphics, pool);
    mCircle.CreateCircie(device, glm::vec3(0.1), graphics, pool);

    //绘制所有小矩形, 即使是被大矩形覆盖
    uniforms.grid.CreateBuffer(device, minUniformBufferOffset * CHESSBOARD_RECT_COUNT * CHESSBOARD_RECT_COUNT, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT|VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    uniforms.grid.size = minUniformBufferOffset;

    uniforms.bigGrid.CreateBuffer(device, minUniformBufferOffset * CHESSBOARD_BIG_GRID_COUNT, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT|VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    uniforms.bigGrid.size = minUniformBufferOffset;
    
    uniforms.wireframe.jiugongge.CreateBuffer(device, minUniformBufferOffset * CHESSBOARD_PALACE_COUNT, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT|VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    uniforms.wireframe.jiugongge.size = minUniformBufferOffset;

    uniforms.alliance.CreateBuffer(device, minUniformBufferOffset * 3, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT|VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    uniforms.alliance.size = minUniformBufferOffset;

    fonts.uniforms.alliance.CreateBuffer(device, minFontUniformBufferOffset * 3, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT|VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    fonts.uniforms.alliance.size = minFontUniformBufferOffset;

    uniforms.background.CreateBuffer(device, minUniformBufferOffset * 2, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT|VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    uniforms.background.size = minUniformBufferOffset;

    pool.AllocateDescriptorSets(device.device, &setLayout, 1, &descriptorSet.grid);
    pool.AllocateDescriptorSets(device.device, &setLayout, 1, &descriptorSet.bigGrid);
    pool.AllocateDescriptorSets(device.device, &setLayout, 1, &descriptorSet.alliance);
    pool.AllocateDescriptorSets(device.device, &setLayout, 1, &descriptorSet.background);
    pool.AllocateDescriptorSets(device.device, &setLayout, 1, &fonts.descriptrorSet.alliance);
    pool.AllocateDescriptorSets(device.device, &setLayout, 1, &descriptorSet.wireframe.jiugongge);

    UpdateDescriptorSet(device.device);
}

void VulkanChessboard::UpdateUniform(VkDevice device, uint32_t windowWidth, uint32_t playerCountry){
    //因为棋盘里面的坐标都是固定的, 所以可以直接在类里全画出来
    //但棋子位置不固定...
    UpdateBigGridUniform(device);

    UpdateGridUniform(device);

    UpdateJiuGongGeWireframeUniform(device);

    UpdateBackgroundUniform(device, windowWidth);

    //在确定位置前，绝对不能看出来在哪里
    UpdateFontUniform(device, playerCountry);
}

void VulkanChessboard::UpdateFontUniform(VkDevice device, int32_t playerCountry){
    glm::vec2 center;
    if(playerCountry == WU_COUNTRY_INDEX){
        center =glm::vec2(CHESSBOARD_COLUMN / 2, 1);
    }
    else if(playerCountry == WEI_COUNTRY_INDEX){
        center = glm::vec2(CHESSBOARD_COLUMN - 1, CHESSBOARD_ROW / 2);
    }
    else if(playerCountry == SHU_COUNTRY_INDEX){
        center = glm::vec2(1, CHESSBOARD_COLUMN / 2);
    }
    else if(playerCountry == HAN_COUNTRY_INDEX){
        center = glm::vec2(CHESSBOARD_COLUMN / 2, CHESSBOARD_ROW - 1);
    }
    std::array<glm::vec2, 3>pos = { glm::vec2(1000, 1000), glm::vec2(1000, 1000), glm::vec2(1000, 1000) };
    if(playerCountry != INVALID_COUNTRY_INDEX){
        //吴在九宫格的中心点
        pos[WU_COUNTRY_INDEX] = glm::vec2(CHESS_COLUMN_TO_X(center.x), CHESS_ROW_TO_Y(center.y));
        if(center.x == 1){
            //汉在汉的位置上
            pos[WEI_COUNTRY_INDEX] = glm::vec2(CHESS_COLUMN_TO_X(center.x - 1), CHESS_ROW_TO_Y(center.y + 1));
            pos[SHU_COUNTRY_INDEX] = glm::vec2(CHESS_COLUMN_TO_X(center.x - 1), CHESS_ROW_TO_Y(center.y - 1));
        }
        else if(center.y == 1){
            //汉在魏的位置上
            pos[WEI_COUNTRY_INDEX] = glm::vec2(CHESS_COLUMN_TO_X(center.x - 1), CHESS_ROW_TO_Y(center.y - 1));
            pos[SHU_COUNTRY_INDEX] = glm::vec2(CHESS_COLUMN_TO_X(center.x + 1), CHESS_ROW_TO_Y(center.y - 1));
        }
        else if(center.y == CHESSBOARD_ROW - 1){
            //汉在蜀的位置上....理论上不可能，但算了，还是写上吧
            pos[WEI_COUNTRY_INDEX] = glm::vec2(CHESS_COLUMN_TO_X(center.x + 1), CHESS_ROW_TO_Y(center.y + 1));
            pos[SHU_COUNTRY_INDEX] = glm::vec2(CHESS_COLUMN_TO_X(center.x - 1), CHESS_ROW_TO_Y(center.y + 1));
        }
        else if(center.x == CHESSBOARD_ROW - 1){
            //汉在吴的位置
            pos[WEI_COUNTRY_INDEX] = glm::vec2(CHESS_COLUMN_TO_X(center.x + 1), CHESS_ROW_TO_Y(center.y - 1));
            pos[SHU_COUNTRY_INDEX] = glm::vec2(CHESS_COLUMN_TO_X(center.x + 1), CHESS_ROW_TO_Y(center.y + 1));
        }
    }
    glm::mat4 model[3];
    FontUniform fUbo[3];
    for (size_t i = 0; i < sizeof(model) / sizeof(glm::mat4); ++i){
        model[i] = glm::scale(glm::translate(glm::mat4(1), glm::vec3(pos[i].x, pos[i].y, 0)), glm::vec3(ALLIANCE_POINT_WIDTH, ALLIANCE_POINT_HEIGHT, 1));
        fUbo[i].imageIndex = i;
        fUbo[i].model = glm::scale(glm::translate(glm::mat4(1), glm::vec3(pos[i].x - ALLIANCE_POINT_FONT_WIDTH * .45, pos[i].y - ALLIANCE_POINT_FONT_HEIGHT * .5, 0)), glm::vec3(ALLIANCE_POINT_FONT_WIDTH, ALLIANCE_POINT_FONT_HEIGHT, 1));
    }
    uniforms.alliance.UpdateData(device, sizeof(model), model);
    fonts.uniforms.alliance.UpdateData(device, sizeof(fUbo), fUbo);
}
