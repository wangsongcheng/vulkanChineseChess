#include "VulkanChessboard.h"
void VulkanChessboard::GetRectVertices(const glm::vec3&color, Vertex *ptrVertices){
    const Vertex v[] = {
        Vertex(glm::vec3(.0f, 1.0f, .0f), color),//左下
        Vertex(glm::vec3(1.0f, .0f, .0f), color),//右上
        Vertex(glm::vec3(.0f, .0f, .0f), color), //左上

        Vertex(glm::vec3(1.0f, 1.0f, .0f), color)//右下
    };
    memcpy(ptrVertices, v, sizeof(v));
}
void VulkanChessboard::CreateRectResource(VkDevice device, VkCommandPool pool, VkQueue graphics){
    Vertex vertices[2 * 4];
    const uint16_t indices[] = {
        0, 1, 2, 0, 3, 1,//.../
        0, 2, 3, 2, 3, 1
    };
    GetRectVertices(glm::vec3(1, .9, .5), vertices);
    GetRectVertices(glm::vec3(0), vertices + 4);
    mRect.indexCount = 6;
    mRect.vertexCount = 4;
    mRect.index.CreateBuffer(device, sizeof(indices), VK_BUFFER_USAGE_INDEX_BUFFER_BIT|VK_BUFFER_USAGE_TRANSFER_DST_BIT);
    //8是4种颜色+亮度较低的颜色(表示不是该玩家下子)
    mRect.vertex.CreateBuffer(device, sizeof(vertices), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT|VK_BUFFER_USAGE_TRANSFER_DST_BIT);
    mRect.index.AllocateAndBindMemory(device, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    mRect.vertex.AllocateAndBindMemory(device, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    vkf::tool::CopyBuffer(device, sizeof(indices), indices, graphics, pool, mRect.index);
    vkf::tool::CopyBuffer(device, sizeof(vertices), vertices, graphics, pool, mRect.vertex);
}
void VulkanChessboard::SetupDescriptorSet(VkDevice device, VkDescriptorSetLayout setLayout, VkDescriptorPool pool){
    vkf::tool::AllocateDescriptorSets(device, pool, &setLayout, 1, &descriptorSet.grid);
    vkf::tool::AllocateDescriptorSets(device, pool, &setLayout, 1, &descriptorSet.bigGrid);
    vkf::tool::AllocateDescriptorSets(device, pool, &setLayout, 1, &descriptorSet.background);
    vkf::tool::AllocateDescriptorSets(device, pool, &setLayout, 1, &descriptorSet.wireframe.jiugongge);
    vkf::tool::AllocateDescriptorSets(device, pool, &setLayout, 1, &descriptorSet.wireframe.selectChess);
    
    VkDescriptorSetLayoutBinding descriptorSetLayoutBindings[1] = {};
    // descriptorSetLayoutBindings[0].binding = 0;
    descriptorSetLayoutBindings[0].descriptorCount = 1;
    descriptorSetLayoutBindings[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    descriptorSetLayoutBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
    vkf::tool::UpdateDescriptorSets(device, 1, descriptorSetLayoutBindings, {uniforms.grid},{}, descriptorSet.grid);
    vkf::tool::UpdateDescriptorSets(device, 1, descriptorSetLayoutBindings, {uniforms.bigGrid},{}, descriptorSet.bigGrid);
    vkf::tool::UpdateDescriptorSets(device, 1, descriptorSetLayoutBindings, {uniforms.background},{}, descriptorSet.background);
    vkf::tool::UpdateDescriptorSets(device, 1, descriptorSetLayoutBindings, {uniforms.wireframe.jiugongge},{}, descriptorSet.wireframe.jiugongge);
    vkf::tool::UpdateDescriptorSets(device, 1, descriptorSetLayoutBindings, {uniforms.wireframe.selectChess},{}, descriptorSet.wireframe.selectChess);
}
void VulkanChessboard::DrawGrid(VkCommandBuffer command, const GraphicsPipeline&pipeline){
    uint32_t dynamicOffsets;
    for (size_t i = 0; i < CHESSBOARD_ROW * CHESSBOARD_COLUMN; ++i){
        dynamicOffsets = i * mMinUniformBufferOffset;
        pipeline.BindDescriptorSet(command, descriptorSet.grid, 1, &dynamicOffsets);
        vkCmdDrawIndexed(command, mRect.indexCount, 1, 0, 0, 0);
        // DrawGraphics(command, &mRect);
    }
}
void VulkanChessboard::DrawBigGrid(VkCommandBuffer command, const GraphicsPipeline&pipeline){
    uint32_t dynamicOffsets;
    for (size_t i = 0; i < CHESSBOARD_BIG_GRID_COUNT; ++i){
        dynamicOffsets = i * mMinUniformBufferOffset;
        pipeline.BindDescriptorSet(command, descriptorSet.bigGrid, 1, &dynamicOffsets);
        vkCmdDrawIndexed(command, mRect.indexCount, 1, 0, 0, 0);
        // DrawGraphics(command, &mRect);
    }
}
void VulkanChessboard::DrawWireframe(VkCommandBuffer command, const GraphicsPipeline&pipeline){
    uint32_t dynamicOffsets;
    // mRect.vertexOffset = 4;
    uint32_t indexOffset = 0;
    for (size_t i = 0; i < CHESSBOARD_WIREFRAME_COUNT; ++i){
        dynamicOffsets = i * mMinUniformBufferOffset;
        if(i < CHESSBOARD_WIREFRAME_COUNT * .5)
            indexOffset = 0;
        else
            indexOffset = mRect.indexCount;
        pipeline.BindDescriptorSet(command, descriptorSet.wireframe.jiugongge, 1, &dynamicOffsets);
        vkCmdDrawIndexed(command, mRect.indexCount, 1, indexOffset, 4, 0);
        // DrawGraphics(command, &mRect);
    }
}
void VulkanChessboard::DrawBackground(VkCommandBuffer command, const GraphicsPipeline&pipeline  ){
    uint32_t dynamicOffsets = 0;
    for (size_t i = 0; i < 2; i++){
        dynamicOffsets = i * mMinUniformBufferOffset;
        pipeline.BindDescriptorSet(command, descriptorSet.background, 1, &dynamicOffsets);
        vkCmdDrawIndexed(command, mRect.indexCount, 1, 0, i * mRect.vertexCount, 0);
        // DrawGraphics(command, &mRect);
    }    
}
void VulkanChessboard::DrawSelectWireframe(VkCommandBuffer command, const GraphicsPipeline &pipeline){
    uint32_t dynamicOffsets = 0;
    for (size_t i = 0; i < CHESSBOARD_ROW * 2; ++i){
        dynamicOffsets = i * mMinUniformBufferOffset;
        pipeline.BindDescriptorSet(command, descriptorSet.wireframe.selectChess, 1, &dynamicOffsets);
        vkCmdDrawIndexed(command, mRect.indexCount, 1, 0, mRect.vertexCount, 0);
        // DrawGraphics(command, &mRect);
    }
}
void VulkanChessboard::UpdateGridUniform(VkDevice device){
    glm::mat4 model[CHESSBOARD_ROW * CHESSBOARD_COLUMN];
    glm::vec2 gridPos[CHESSBOARD_ROW][CHESSBOARD_COLUMN];
    const uint32_t gridBigPointOffset = CHESSBOARD_GRID_SIZE;
    const glm::mat4 scale = glm::scale(glm::mat4(1), glm::vec3(CHESSBOARD_GRID_SIZE, CHESSBOARD_GRID_SIZE, 1));
    for (size_t uiRow = 0; uiRow < CHESSBOARD_ROW; ++uiRow){
        for (size_t uiColumn = 0; uiColumn < CHESSBOARD_COLUMN; ++uiColumn){
            gridPos[uiRow][uiColumn] = glm::vec2(uiRow * CHESSBOARD_GRID_SIZE + (uiRow + 1) * CHESSBOARD_LINE_WIDTH + gridBigPointOffset, uiColumn * CHESSBOARD_GRID_SIZE + (uiColumn + 1) * CHESSBOARD_LINE_WIDTH + gridBigPointOffset);
            model[ROW_COLUMN_INDEX(uiRow, uiColumn, CHESSBOARD_COLUMN)] = glm::translate(glm::mat4(1), glm::vec3(gridPos[uiRow][uiColumn], 0)) * scale;
        }
    }
    uniforms.grid.UpdateData(device, sizeof(model), model);
}
void VulkanChessboard::UpdateBigGridUniform(VkDevice device){
    glm::mat4 model[CHESSBOARD_BIG_GRID_COUNT];
    const glm::mat4 scale = glm::scale(glm::mat4(1), glm::vec3(CHESSBOARD_BIG_GRID_SIZE, CHESSBOARD_BIG_GRID_SIZE, 1));
    const glm::vec3 bigGridPos[] = {
        glm::vec3(CHESSBOARD_LINE_WIDTH + CHESSBOARD_GRID_SIZE, CHESSBOARD_LINE_WIDTH + CHESSBOARD_GRID_SIZE, 0),
        glm::vec3(9 * CHESSBOARD_GRID_SIZE + 10 * CHESSBOARD_LINE_WIDTH + CHESSBOARD_BIG_GRID_SIZE, bigGridPos[0].y, 0),
        glm::vec3(bigGridPos[0].x, bigGridPos[1].x, 0),
        glm::vec3(bigGridPos[1].x, bigGridPos[2].y, 0)
    };
    for (size_t i = 0; i < CHESSBOARD_BIG_GRID_COUNT; ++i){
        model[i] = glm::translate(glm::mat4(1.0f), bigGridPos[i]) * scale;
    }
    uniforms.bigGrid.UpdateData(device, sizeof(model), model);
}
void VulkanChessboard::UpdateJiuGongGeWireframeUniform(VkDevice device){
    const glm::mat4 gridScale = glm::scale(glm::mat4(1.0f), glm::vec3(CHESSBOARD_GRID_SIZE + CHESSBOARD_LINE_WIDTH, CHESSBOARD_GRID_SIZE + CHESSBOARD_LINE_WIDTH, 1));
    const glm::vec2 rc[] = {
        //线框[0]
        glm::vec2(8, 0), glm::vec2(rc[0].x - 1, rc[0].y + 1), glm::vec2(15, 7), glm::vec2(rc[2].x - 1, rc[2].y + 1), glm::vec2(rc[0].x, 14), glm::vec2(rc[1].x, rc[4].y + 1),
        //线框[1]
        glm::vec2(rc[1].x, rc[0].y), glm::vec2(rc[0].x, rc[1].y), glm::vec2(rc[3].x, rc[2].y), glm::vec2(rc[2].x, rc[3].y), glm::vec2(rc[1].x, rc[4].y), glm::vec2(rc[4].x, rc[5].y)
    };
    glm::mat4 model[CHESSBOARD_WIREFRAME_COUNT];
    for (size_t i = 0; i < sizeof(rc) / sizeof(glm::vec2); ++i){
        model[i] = glm::translate(glm::mat4(1), glm::vec3(COLUMN_TO_X(rc[i].x), ROW_TO_Y(rc[i].y), 0)) * gridScale;
    }
    uniforms.wireframe.jiugongge.UpdateData(device, sizeof(model), &model);
}
VulkanChessboard::VulkanChessboard(/* args */){
    
}

VulkanChessboard::~VulkanChessboard(){
}

void VulkanChessboard::Cleanup(VkDevice device){
    mRect.index.Destroy(device);
    mRect.vertex.Destroy(device);

    uniforms.grid.Destroy(device);
    uniforms.bigGrid.Destroy(device);
    uniforms.background.Destroy(device);
    uniforms.wireframe.jiugongge.Destroy(device);
    uniforms.wireframe.selectChess.Destroy(device);
}
void VulkanChessboard::Setup(VkPhysicalDevice physicalDevice, VkDevice device, VkDescriptorSetLayout setLayout, uint32_t windowWidth, VkQueue graphics, VulkanPool pool){
    VkPhysicalDeviceProperties physicalDeviceProperties;
    vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);
    mMinUniformBufferOffset = ALIGN(sizeof(glm::mat4), physicalDeviceProperties.limits.minUniformBufferOffsetAlignment);

    CreateRectResource(device, pool.commandPool, graphics);

    //绘制所有方格, 即使是被大方格覆盖
    uniforms.grid.CreateBuffer(device, mMinUniformBufferOffset * CHESSBOARD_ROW * CHESSBOARD_COLUMN, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
    uniforms.grid.AllocateAndBindMemory(device, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT|VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    uniforms.grid.size = mMinUniformBufferOffset;

    uniforms.bigGrid.CreateBuffer(device, mMinUniformBufferOffset * CHESSBOARD_BIG_GRID_COUNT, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
    uniforms.bigGrid.AllocateAndBindMemory(device, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT|VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    uniforms.bigGrid.size = mMinUniformBufferOffset;
    
    uniforms.wireframe.jiugongge.CreateBuffer(device, mMinUniformBufferOffset * CHESSBOARD_WIREFRAME_COUNT, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
    uniforms.wireframe.jiugongge.AllocateAndBindMemory(device, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT|VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    uniforms.wireframe.jiugongge.size = mMinUniformBufferOffset;

    uniforms.wireframe.selectChess.CreateBuffer(device, mMinUniformBufferOffset * CHESSBOARD_ROW * 2, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
    uniforms.wireframe.selectChess.AllocateAndBindMemory(device, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT|VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    uniforms.wireframe.selectChess.size = mMinUniformBufferOffset;

    uniforms.background.CreateBuffer(device, mMinUniformBufferOffset * 2, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
    uniforms.background.AllocateAndBindMemory(device, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT|VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    uniforms.background.size = mMinUniformBufferOffset;

    SetupDescriptorSet(device, setLayout, pool.descriptorPool);

    VulkanChess::Setup(physicalDevice, device, setLayout, pool, graphics);
}
void VulkanChessboard::RecordCommand(VkCommandBuffer cmd, uint32_t windowWidth, uint32_t currentCountry){
    //绘制顺序:非黑色背景->黑色背景->所有小方块->4个大方块->九宫格
    const glm::mat4 projection = glm::ortho(0.0f, (float)windowWidth, 0.0f, (float)windowWidth, -1.0f, 1.0f);
    pipelines.grid.BindPipeline(cmd);
    pipelines.grid.PushConstant(cmd, VK_SHADER_STAGE_VERTEX_BIT, sizeof(glm::mat4), &projection);
    
    VkDeviceSize offset = 0;
    vkCmdBindVertexBuffers(cmd, 0, 1, &mRect.vertex.buffer, &offset);
    vkCmdBindIndexBuffer(cmd, mRect.index.buffer, 0, VK_INDEX_TYPE_UINT16);

    DrawBackground(cmd, pipelines.grid);
    DrawGrid(cmd, pipelines.grid);
    DrawBigGrid(cmd, pipelines.grid);
    pipelines.wireframe.BindPipeline(cmd);
    pipelines.wireframe.PushConstant(cmd, VK_SHADER_STAGE_VERTEX_BIT, sizeof(glm::mat4), &projection);
    DrawWireframe(cmd, pipelines.wireframe);
    DrawSelectWireframe(cmd, pipelines.wireframe);
    VulkanChess::RecordCommand(cmd, windowWidth, currentCountry);
}
void VulkanChessboard::UpdateUniform(VkDevice device){
    UpdateBigGridUniform(device);

    UpdateGridUniform(device);

    UpdateJiuGongGeWireframeUniform(device);
}
void VulkanChessboard::ClearSelectWireframeUnfirom(VkDevice device){
    glm::mat4 model[CHESSBOARD_ROW * 2];
    for (size_t i = 0; i < CHESSBOARD_ROW * 2; ++i){
        model[i] = glm::translate(glm::mat4(1), glm::vec3(COLUMN_TO_X(100), ROW_TO_Y(100), 0));
    }
    uniforms.wireframe.selectChess.UpdateData(device, CHESSBOARD_ROW * 2 * mMinUniformBufferOffset, model);
}
void VulkanChessboard::UpdateBackgroundUniform(VkDevice device, uint32_t windowWidth){
    const glm::mat4 model[] = {
        glm::scale(glm::mat4(1.0f), glm::vec3(windowWidth, windowWidth, 1)), 
        glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(CHESSBOARD_GRID_SIZE, CHESSBOARD_GRID_SIZE, 0)), glm::vec3(CHESSBOARD_BLACK_BACKGROUND_SIZE(windowWidth), CHESSBOARD_BLACK_BACKGROUND_SIZE(windowWidth), 1)), 
    };
    uniforms.background.UpdateData(device, sizeof(model), model);
}
void VulkanChessboard::UpdateSelectWireframeUniform(VkDevice device, const ChessInfo *pInfo, uint32_t count){
    if(count == 0)return;
    glm::mat4 model[CHESSBOARD_ROW * 2];
    const glm::mat4 scale = glm::scale(glm::mat4(1), glm::vec3(CHESSBOARD_GRID_SIZE, CHESSBOARD_GRID_SIZE, 1));
    for (size_t i = 0; i < count; ++i){
        model[i] = glm::translate(glm::mat4(1), glm::vec3(COLUMN_TO_X(pInfo[i].column) - CHESSBOARD_GRID_SIZE * .5, ROW_TO_Y(pInfo[i].row) - CHESSBOARD_GRID_SIZE * .5, 0)) * scale;
    }
    uniforms.wireframe.selectChess.UpdateData(device, count * mMinUniformBufferOffset, model);
}
void VulkanChessboard::UpdateChessUniform(VkDevice device, uint32_t country, uint32_t chess, const glm::vec2 &pos, uint32_t fontIndex, const VkExtent2D &size){
    const uint32_t countryOffset = country * (WEI_CHESS_COUNT + 4);//因为除汉外每个国家的棋子数相同,所以直接这么用
    const uint32_t offset = countryOffset + chess;
    VulkanChess::UpdateUniform(device, pos, size, offset);
    VkExtent2D fontSize = size;
    fontSize.width *= 1.8;
    fontSize.height *= 1.8;
    const glm::vec2 newPos = glm::vec2(pos.x - CHESS_WIDTH * .65, pos.y - CHESS_HEIGHT * .9);
    // printf("国家:%d, 棋子:%d, 行:%d, 列:%d, 偏移:%d, pos:%f, %f\n", country, chess, row, column, offset, pos.x, pos.y);
    UpdateFontUniform(device, fontIndex, newPos, fontSize, offset);
}
void VulkanChessboard::UpdateChessUniform(VkDevice device, uint32_t country, uint32_t chess, uint32_t row, uint32_t column, uint32_t fontIndex, const VkExtent2D &size){
    UpdateChessUniform(device, country, chess, glm::vec2(COLUMN_TO_X(column), ROW_TO_Y(row)), fontIndex, size);
}
void VulkanChessboard::DestroyGraphicsPipeline(VkDevice device){
    pipelines.grid.DestroyLayout(device);
    pipelines.grid.DestroyPipeline(device);

    pipelines.wireframe.DestroyLayout(device);
    pipelines.wireframe.DestroyPipeline(device);

    VulkanChess::DestroyGraphicsPipeline(device);
}

void VulkanChessboard::CreatePipeline(VkDevice device, VkRenderPass renderpass, VkDescriptorSetLayout setLayout, VkPipelineCache pipelineCache, uint32_t windowWidth){
    pipelines.grid.PushScissor(windowWidth, windowWidth);
    pipelines.grid.PushViewport(windowWidth, windowWidth);

    pipelines.grid.PushShader(device, VK_SHADER_STAGE_VERTEX_BIT, "shaders/baseVert.spv");
    pipelines.grid.PushShader(device, VK_SHADER_STAGE_FRAGMENT_BIT, "shaders/baseFrag.spv");

    pipelines.grid.PushVertexInputBindingDescription(sizeof(Vertex));
    pipelines.grid.PushPushConstant(sizeof(glm::mat4), VK_SHADER_STAGE_VERTEX_BIT);
    pipelines.grid.PushVertexInputAttributeDescription(0, 0, VK_FORMAT_R32G32B32_SFLOAT);
    pipelines.grid.PushVertexInputAttributeDescription(1, offsetof(Vertex, Vertex::color), VK_FORMAT_R32G32B32_SFLOAT);
    
    pipelines.grid.CreateLayout(device, { setLayout });
    pipelines.grid.CreatePipeline(device, renderpass, pipelineCache);

    GraphicsPipelineStateInfo state;
    state.mRasterization.polygonMode = VK_POLYGON_MODE_LINE;
    pipelines.wireframe.SetStateInfo(state);
    pipelines.wireframe.PushScissor(windowWidth, windowWidth);
    pipelines.wireframe.PushViewport(windowWidth, windowWidth);

    pipelines.wireframe.PushShader(device, VK_SHADER_STAGE_VERTEX_BIT, "shaders/baseVert.spv");
    pipelines.wireframe.PushShader(device, VK_SHADER_STAGE_FRAGMENT_BIT, "shaders/baseFrag.spv");

    pipelines.wireframe.PushVertexInputBindingDescription(sizeof(Vertex));
    pipelines.wireframe.PushPushConstant(sizeof(glm::mat4), VK_SHADER_STAGE_VERTEX_BIT);
    pipelines.wireframe.PushVertexInputAttributeDescription(0, 0, VK_FORMAT_R32G32B32_SFLOAT);
    pipelines.wireframe.PushVertexInputAttributeDescription(1, offsetof(Vertex, Vertex::color), VK_FORMAT_R32G32B32_SFLOAT);
    
    pipelines.wireframe.CreateLayout(device, { setLayout });
    pipelines.wireframe.CreatePipeline(device, renderpass, pipelineCache);

    VulkanChess::CreatePipeline(device, renderpass, setLayout, pipelineCache, windowWidth);
}