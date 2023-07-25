#include "Chessboard.h"
void Chessboard::CreateGraphicsPipeline(VkDevice device, uint32_t windowWidth, VkRenderPass renderpass, const std::vector<std::string>&shader, GraphicsPipeline&pipeline, const GraphicsPipelineStateInfo&pipelineState){
    std::vector<VkShaderStageFlagBits> stage = { VK_SHADER_STAGE_VERTEX_BIT, VK_SHADER_STAGE_FRAGMENT_BIT };
    pipeline.PushDescriptorSetLayoutBinding(0, VK_SHADER_STAGE_VERTEX_BIT, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC);
    std::vector<uint32_t>cacheData;
    vkf::tool::GetFileContent("GraphicsPipelineCache", cacheData);
    for (size_t i = 0; i < shader.size(); ++i){
        pipeline.PushShader(device, stage[i], shader[i]);
    }
    pipeline.SetStateInfo(pipelineState);
    pipeline.PushScissor(windowWidth, windowWidth);
    pipeline.PushViewport(windowWidth, windowWidth);
    pipeline.SetVertexInputBindingDescription(sizeof(ChessboardVertex));
    pipeline.PushPushConstant(sizeof(PushConstant), VK_SHADER_STAGE_VERTEX_BIT);
    pipeline.PushVertexInputAttributeDescription(0, 0, VK_FORMAT_R32G32B32_SFLOAT);
    pipeline.PushVertexInputAttributeDescription(1, 3 * sizeof(float), VK_FORMAT_R32G32B32_SFLOAT);
    pipeline.CreateCache(device, cacheData);
    pipeline.CreateDescriptorSetLayout(device);
    pipeline.CreateLayout(device);
    pipeline.CreatePipeline(device, renderpass);
}
Chessboard::Chessboard(/* args */){
}

Chessboard::~Chessboard(){
}
void Chessboard::Cleanup(VkDevice device){
    mPointVert.Destroy(device);
    mPointIndex.Destroy(device);
    mPointUniform.Destroy(device);
    mWireframeVert.Destroy(device);
    mWireFrameUniform.Destroy(device);
    for (size_t i = 0; i < sizeof(mWireframeIndex) / sizeof(VulkanBuffer); ++i){
        mWireframeIndex[i].Destroy(device);
    }
    DestroyGraphicsPipeline(device);
}
void Chessboard::UpdateSet(VkDevice device){
    mPointPipeline.UpdateDescriptorSets(device, { mPointUniform }, {}, mPointSet);
    mWireframePipeline.UpdateDescriptorSets(device, { mWireFrameUniform }, {}, mSet);
}
void Chessboard::DestroyGraphicsPipeline(VkDevice device){
    std::vector<uint32_t>cacheData;
    mPointPipeline.DestroyCache(device, cacheData);
    vkf::tool::WriteFileContent("GraphicsPipelineCache", cacheData.data(), cacheData.size() * sizeof(uint32_t)); 
    mPointPipeline.DestroySetLayout(device);
    mPointPipeline.DestroyLayout(device);
    mPointPipeline.DestroyPipeline(device);

    mWireframePipeline.DestroySetLayout(device);
    mWireframePipeline.DestroyLayout(device);
    mWireframePipeline.DestroyPipeline(device);
}
void Chessboard::RecodeCommand(VkCommandBuffer cmd, uint32_t windowWidth){
    PushConstant pc;
    pc.projection = glm::ortho(0.0f, (float)windowWidth, 0.0f, (float)windowWidth, -1.0f, 1.0f);
    VkDeviceSize offset = 0;
    vkCmdBindVertexBuffers(cmd, 0, 1, &mPointVert.buffer, &offset);
    vkCmdBindIndexBuffer(cmd, mPointIndex.buffer, 0, VK_INDEX_TYPE_UINT16);
    mPointPipeline.PushPushConstant(cmd, VK_SHADER_STAGE_VERTEX_BIT, sizeof(pc), &pc);
    mPointPipeline.BindPipeline(cmd);
    uint32_t dynamicOffset = 0;
    mPointPipeline.BindDescriptorSet(cmd, mPointSet, 1, &dynamicOffset);
    vkCmdDrawIndexed(cmd, 1, 1, 0, 0, 0);

    dynamicOffset =  mPointMinUniformBufferOffset;
    mPointPipeline.BindDescriptorSet(cmd, mPointSet, 1, &dynamicOffset);
    vkCmdDrawIndexed(cmd, 1, 1, 0, 1, 0);

    for (size_t i = 0; i < 4; ++i){
        dynamicOffset = (i + 2) * mPointMinUniformBufferOffset;
        mPointPipeline.BindDescriptorSet(cmd, mPointSet, 1, &dynamicOffset);
        vkCmdDrawIndexed(cmd, 1, 1, 0, 0, 0);
    }
    for (uint32_t uiRow = 0; uiRow < 4; ++uiRow){
        for (uint32_t uiColumn = 0; uiColumn < 8; ++uiColumn){
            dynamicOffset = (CHESSBOARD_ROW_COLUMN_INDEX(uiRow, uiColumn, 8) + 6) * mPointMinUniformBufferOffset;
            mPointPipeline.BindDescriptorSet(cmd, mPointSet, 1, &dynamicOffset);
            vkCmdDrawIndexed(cmd, 1, 1, 0, 0, 0);
        }
    }
    for (uint32_t uiRow = 0; uiRow < 8; ++uiRow){
        for (uint32_t uiColumn = 0; uiColumn < 16; ++uiColumn){
            dynamicOffset = (CHESSBOARD_ROW_COLUMN_INDEX(uiRow, uiColumn, 16) + 38) * mPointMinUniformBufferOffset;
            mPointPipeline.BindDescriptorSet(cmd, mPointSet, 1, &dynamicOffset);
            vkCmdDrawIndexed(cmd, 1, 1, 0, 0, 0);
        }
    }
    for (uint32_t uiRow = 0; uiRow < 4; ++uiRow){
        for (uint32_t uiColumn = 0; uiColumn < 8; ++uiColumn){
            dynamicOffset = (CHESSBOARD_ROW_COLUMN_INDEX(uiRow, uiColumn, 8) + 166) * mPointMinUniformBufferOffset;
            mPointPipeline.BindDescriptorSet(cmd, mPointSet, 1, &dynamicOffset);
            vkCmdDrawIndexed(cmd, 1, 1, 0, 0, 0);
        }
    }
    //线框
    vkCmdBindVertexBuffers(cmd, 0, 1, &mWireframeVert.buffer, &offset);
    mWireframePipeline.PushPushConstant(cmd, VK_SHADER_STAGE_VERTEX_BIT, sizeof(pc), &pc);
    mWireframePipeline.BindPipeline(cmd);

    for (uint32_t i = 0; i < 2; ++i){
        vkCmdBindIndexBuffer(cmd, mWireframeIndex[i].buffer, 0, VK_INDEX_TYPE_UINT16);
        for (uint32_t j = 0; j < 6; ++j){
            dynamicOffset = (CHESSBOARD_ROW_COLUMN_INDEX(i, j, 6)) * mWireframeMinUniformBufferOffset;
            mWireframePipeline.BindDescriptorSet(cmd, mSet, 1, &dynamicOffset);
            vkCmdDrawIndexed(cmd, 6, 1, 0, 0, 0);
        }
    }
    
}
void Chessboard::CreatePipeline(VkDevice device, VkRenderPass renderpass, uint32_t windowWidth){
    GraphicsPipelineStateInfo state;
    state.mInputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
    CreateGraphicsPipeline(device, windowWidth, renderpass, { "shaders/basePointVert.spv", "shaders/baseFrag.spv" }, mPointPipeline, state);
    state.mRasterization.polygonMode = VK_POLYGON_MODE_LINE;
    state.mInputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    CreateGraphicsPipeline(device, windowWidth, renderpass, { "shaders/baseVert.spv", "shaders/baseFrag.spv" }, mWireframePipeline, state);
}
void Chessboard::CreateVulkanResource(VkPhysicalDevice physicalDevice, VkDevice device, uint32_t windowWidth, VkQueue graphics, VulkanPool pool){
    VkPhysicalDeviceProperties physicalDeviceProperties;
    vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);
    mPointMinUniformBufferOffset = ALIGN(sizeof(PointUniform), physicalDeviceProperties.limits.minUniformBufferOffsetAlignment);
    mWireframeMinUniformBufferOffset = ALIGN(sizeof(glm::mat4), physicalDeviceProperties.limits.minUniformBufferOffsetAlignment);
    const ChessboardVertex vertices[] = {
        ChessboardVertex(glm::vec3(.0f, 1.0f, .0f), glm::vec3(0)),//左下
        ChessboardVertex(glm::vec3(1.0f, .0f, .0f), glm::vec3(0)),//右上
        ChessboardVertex(glm::vec3(.0f, .0f, .0f), glm::vec3(0)), //左上

        ChessboardVertex(glm::vec3(1.0f, 1.0f, .0f), glm::vec3(0))//右下
    };
    const ChessboardVertex pointVertices[] = {
        ChessboardVertex(glm::vec3(.0f), glm::vec3(1, .9, .5)),
        // ChessboardVertex(glm::vec3(.0f), glm::vec3(50, 215, 90.0f)),
        ChessboardVertex(glm::vec3(.0f), glm::vec3(0))
    };
    const uint16_t pointIndices[] = { 0 };
    const uint16_t indices[] = { 0, 1, 2, 0, 3, 1 };
    const uint16_t mirrorIndices[] = { 0, 2, 3, 2, 3, 1 };
    mPointIndex.CreateBuffer(device, sizeof(pointIndices), VK_BUFFER_USAGE_INDEX_BUFFER_BIT|VK_BUFFER_USAGE_TRANSFER_DST_BIT);
    mPointVert.CreateBuffer(device, sizeof(pointVertices), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT|VK_BUFFER_USAGE_TRANSFER_DST_BIT);
    mWireframeIndex[0].CreateBuffer(device, sizeof(indices), VK_BUFFER_USAGE_INDEX_BUFFER_BIT|VK_BUFFER_USAGE_TRANSFER_DST_BIT);
    mWireframeIndex[1].CreateBuffer(device, sizeof(mirrorIndices), VK_BUFFER_USAGE_INDEX_BUFFER_BIT|VK_BUFFER_USAGE_TRANSFER_DST_BIT);
    mWireframeVert.CreateBuffer(device, sizeof(vertices), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT|VK_BUFFER_USAGE_TRANSFER_DST_BIT);
    mPointVert.AllocateAndBindMemory(device, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    mPointIndex.AllocateAndBindMemory(device, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    mWireframeVert.AllocateAndBindMemory(device, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    for (size_t i = 0; i < sizeof(mWireframeIndex) / sizeof(VulkanBuffer); i++){
        mWireframeIndex[i].AllocateAndBindMemory(device, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    }
    vkf::tool::CopyBuffer(device, sizeof(indices), indices, graphics, pool.commandPool, mWireframeIndex[0]);
    vkf::tool::CopyBuffer(device, sizeof(pointIndices), pointIndices, graphics, pool.commandPool, mPointIndex);
    vkf::tool::CopyBuffer(device, sizeof(mirrorIndices), mirrorIndices, graphics, pool.commandPool, mWireframeIndex[1]);
    vkf::tool::CopyBuffer(device, sizeof(vertices), vertices, graphics, pool.commandPool, mWireframeVert);
    vkf::tool::CopyBuffer(device, sizeof(pointVertices), pointVertices, graphics, pool.commandPool, mPointVert);
    // 中间那部分(8 * 16) + 汉和吴(8 * 8) + 4个大一些的矩形 + 2个背景
    //8 * 16 + 8 * 8 + 4 + 1 + 1//其中一个1是黑色背景
    mWireFrameUniform.CreateBuffer(device, mWireframeMinUniformBufferOffset * 12, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
    mPointUniform.CreateBuffer(device, mPointMinUniformBufferOffset * (8 * 16 + 70), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
    mPointUniform.AllocateAndBindMemory(device, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT|VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    mWireFrameUniform.AllocateAndBindMemory(device, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT|VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    mPointUniform.size = mPointMinUniformBufferOffset;
    mWireFrameUniform.size = mWireframeMinUniformBufferOffset;

    // glm::mat4 pos = glm::mat4(1.0f);
    PointUniform pointPos;
    const uint32_t pointOffset = windowWidth * .5;
    pointPos.pointSize = windowWidth;
    pointPos.model = glm::translate(glm::mat4(1.0f), glm::vec3(pointOffset, pointOffset, 0));
    mPointUniform.UpdateData(device, &pointPos);
    pointPos.pointSize = windowWidth - 2 * CHESSBOARD_GRID_SIZE;
    pointPos.model = glm::translate(glm::mat4(1.0f), glm::vec3(pointPos.pointSize * .5 + CHESSBOARD_GRID_SIZE, pointPos.pointSize * .5 + CHESSBOARD_GRID_SIZE, 0));
    mPointUniform.UpdateData(device, &pointPos, mPointMinUniformBufferOffset);
    //4个大网格的大小
    const uint32_t gridBigPointOffset = CHESSBOARD_GRID_SIZE_BIG * .5 + CHESSBOARD_LINE_WIDTH;
    const glm::vec3 bigGridPos[] = {
        glm::vec3(gridBigPointOffset + CHESSBOARD_LINE_WIDTH + CHESSBOARD_GRID_SIZE, gridBigPointOffset + CHESSBOARD_LINE_WIDTH + CHESSBOARD_GRID_SIZE, 0),
        glm::vec3(gridBigPointOffset + 9 * CHESSBOARD_GRID_SIZE + 10 * CHESSBOARD_LINE_WIDTH + CHESSBOARD_GRID_SIZE_BIG, bigGridPos[0].y, 0),
        glm::vec3(bigGridPos[0].x, bigGridPos[1].x, 0),
        glm::vec3(bigGridPos[1].x, bigGridPos[2].y, 0)
    };
    pointPos.pointSize = CHESSBOARD_GRID_SIZE_BIG;
    for (size_t i = 0; i < 4; ++i){
        pointPos.model = glm::translate(glm::mat4(1.0f), bigGridPos[i]);
        mPointUniform.UpdateData(device, &pointPos, (i + 2) * mPointMinUniformBufferOffset);
    }
    //魏
    pointPos.pointSize = CHESSBOARD_GRID_SIZE;
    const uint32_t gridPointOffset = CHESSBOARD_GRID_SIZE * .5 + CHESSBOARD_LINE_WIDTH;
    uint32_t xOffset = gridPointOffset + CHESSBOARD_GRID_SIZE + CHESSBOARD_LINE_WIDTH + CHESSBOARD_GRID_SIZE_BIG, yOffset = gridPointOffset + CHESSBOARD_GRID_SIZE;
    for (uint32_t uiRow = 0; uiRow < 4; ++uiRow){
        for (uint32_t uiColumn = 0; uiColumn < 8; ++uiColumn){
            pointPos.model = glm::translate(glm::mat4(1.0f), glm::vec3(xOffset + uiColumn * (CHESSBOARD_LINE_WIDTH + CHESSBOARD_GRID_SIZE), yOffset + uiRow * (CHESSBOARD_LINE_WIDTH + CHESSBOARD_GRID_SIZE), 0));
            mPointUniform.UpdateData(device, &pointPos, (CHESSBOARD_ROW_COLUMN_INDEX(uiRow, uiColumn, 8) + 6) * mPointMinUniformBufferOffset);
        }
    }
    //汉 吴
    uint32_t temp = xOffset;
    xOffset = yOffset;
    yOffset = temp;
    pointPos.pointSize = CHESSBOARD_GRID_SIZE;
    for (uint32_t uiRow = 0; uiRow < 8; ++uiRow){
        for (uint32_t uiColumn = 0; uiColumn < 16; ++uiColumn){
            pointPos.model = glm::translate(glm::mat4(1.0f), glm::vec3(xOffset + uiColumn * (CHESSBOARD_LINE_WIDTH + CHESSBOARD_GRID_SIZE), yOffset + uiRow * (CHESSBOARD_LINE_WIDTH + CHESSBOARD_GRID_SIZE), 0));
            mPointUniform.UpdateData(device, &pointPos, (CHESSBOARD_ROW_COLUMN_INDEX(uiRow, uiColumn, 16) + 38) * mPointMinUniformBufferOffset);
        }
    }
    temp = xOffset;
    xOffset = yOffset;
    yOffset = temp;
    pointPos.pointSize = CHESSBOARD_GRID_SIZE;
    for (uint32_t uiRow = 0; uiRow < 4; ++uiRow){
        for (uint32_t uiColumn = 0; uiColumn < 8; ++uiColumn){
            pointPos.model = glm::translate(glm::mat4(1.0f), glm::vec3(xOffset + uiColumn * (CHESSBOARD_LINE_WIDTH + CHESSBOARD_GRID_SIZE), yOffset + (uiRow + 12) * (CHESSBOARD_LINE_WIDTH + CHESSBOARD_GRID_SIZE), 0));
            mPointUniform.UpdateData(device, &pointPos, (CHESSBOARD_ROW_COLUMN_INDEX(uiRow, uiColumn, 8) + 166) * mPointMinUniformBufferOffset);
        }
    }
    //线框[0]
    const glm::mat4 gridScale = glm::scale(glm::mat4(1.0f), glm::vec3(CHESSBOARD_GRID_SIZE + CHESSBOARD_LINE_WIDTH, CHESSBOARD_GRID_SIZE + CHESSBOARD_LINE_WIDTH, 1));

    glm::mat4 pos = glm::translate(glm::mat4(1), glm::vec3(5 * CHESSBOARD_GRID_SIZE + 6 * CHESSBOARD_LINE_WIDTH + CHESSBOARD_GRID_SIZE_BIG, CHESSBOARD_GRID_SIZE + CHESSBOARD_LINE_WIDTH, 0)) * gridScale;
    mWireFrameUniform.UpdateData(device, &pos);
    pos = glm::translate(glm::mat4(1), glm::vec3(4 * CHESSBOARD_GRID_SIZE + 5 * CHESSBOARD_LINE_WIDTH + CHESSBOARD_GRID_SIZE_BIG, 2 * CHESSBOARD_GRID_SIZE + 2 * CHESSBOARD_LINE_WIDTH, 0)) * gridScale;
    mWireFrameUniform.UpdateData(device, &pos, mWireframeMinUniformBufferOffset);
    
    pos = glm::translate(glm::mat4(1), glm::vec3(5 * CHESSBOARD_GRID_SIZE + 6 * CHESSBOARD_LINE_WIDTH + CHESSBOARD_GRID_SIZE_BIG, 11 * CHESSBOARD_GRID_SIZE + 12 * CHESSBOARD_LINE_WIDTH + CHESSBOARD_GRID_SIZE_BIG, 0)) * gridScale;
    mWireFrameUniform.UpdateData(device, &pos, 2 * mWireframeMinUniformBufferOffset);
    pos = glm::translate(glm::mat4(1), glm::vec3(4 * CHESSBOARD_GRID_SIZE + 5 * CHESSBOARD_LINE_WIDTH + CHESSBOARD_GRID_SIZE_BIG, 12 * CHESSBOARD_GRID_SIZE + 13 * CHESSBOARD_LINE_WIDTH + CHESSBOARD_GRID_SIZE_BIG, 0)) * gridScale;
    mWireFrameUniform.UpdateData(device, &pos, 3 * mWireframeMinUniformBufferOffset);

    pos = glm::translate(glm::mat4(1), glm::vec3(11 * CHESSBOARD_GRID_SIZE + 12 * CHESSBOARD_LINE_WIDTH + CHESSBOARD_GRID_SIZE_BIG, 5 * CHESSBOARD_GRID_SIZE + 6 * CHESSBOARD_LINE_WIDTH + CHESSBOARD_GRID_SIZE_BIG, 0)) * gridScale;
    mWireFrameUniform.UpdateData(device, &pos, 4 * mWireframeMinUniformBufferOffset);
    pos = glm::translate(glm::mat4(1), glm::vec3(12 * CHESSBOARD_GRID_SIZE + 13 * CHESSBOARD_LINE_WIDTH + CHESSBOARD_GRID_SIZE_BIG, 4 * CHESSBOARD_GRID_SIZE + 5 * CHESSBOARD_LINE_WIDTH + CHESSBOARD_GRID_SIZE_BIG, 0)) * gridScale;
    mWireFrameUniform.UpdateData(device, &pos, 5 * mWireframeMinUniformBufferOffset);
    //线框[1]
    pos = glm::translate(glm::mat4(1), glm::vec3(4 * CHESSBOARD_GRID_SIZE + 5 * CHESSBOARD_LINE_WIDTH + CHESSBOARD_GRID_SIZE_BIG, CHESSBOARD_GRID_SIZE + CHESSBOARD_LINE_WIDTH, 0)) * gridScale;
    mWireFrameUniform.UpdateData(device, &pos, 6 * mWireframeMinUniformBufferOffset);
    pos = glm::translate(glm::mat4(1), glm::vec3(5 * CHESSBOARD_GRID_SIZE + 6 * CHESSBOARD_LINE_WIDTH + CHESSBOARD_GRID_SIZE_BIG, 2 * CHESSBOARD_GRID_SIZE + 2 * CHESSBOARD_LINE_WIDTH, 0)) * gridScale;
    mWireFrameUniform.UpdateData(device, &pos, 7 * mWireframeMinUniformBufferOffset);
    
    pos = glm::translate(glm::mat4(1), glm::vec3(4 * CHESSBOARD_GRID_SIZE + 5 * CHESSBOARD_LINE_WIDTH + CHESSBOARD_GRID_SIZE_BIG, 11 * CHESSBOARD_GRID_SIZE + 12 * CHESSBOARD_LINE_WIDTH + CHESSBOARD_GRID_SIZE_BIG, 0)) * gridScale;
    mWireFrameUniform.UpdateData(device, &pos, 8 * mWireframeMinUniformBufferOffset);
    pos = glm::translate(glm::mat4(1), glm::vec3(5 * CHESSBOARD_GRID_SIZE + 6 * CHESSBOARD_LINE_WIDTH + CHESSBOARD_GRID_SIZE_BIG, 12 * CHESSBOARD_GRID_SIZE + 13 * CHESSBOARD_LINE_WIDTH + CHESSBOARD_GRID_SIZE_BIG, 0)) * gridScale;
    mWireFrameUniform.UpdateData(device, &pos, 9 * mWireframeMinUniformBufferOffset);

    pos = glm::translate(glm::mat4(1), glm::vec3(12 * CHESSBOARD_GRID_SIZE + 13 * CHESSBOARD_LINE_WIDTH + CHESSBOARD_GRID_SIZE_BIG, 5 * CHESSBOARD_GRID_SIZE + 6 * CHESSBOARD_LINE_WIDTH + CHESSBOARD_GRID_SIZE_BIG, 0)) * gridScale;
    mWireFrameUniform.UpdateData(device, &pos, 10 * mWireframeMinUniformBufferOffset);
    pos = glm::translate(glm::mat4(1), glm::vec3(11 * CHESSBOARD_GRID_SIZE + 12 * CHESSBOARD_LINE_WIDTH + CHESSBOARD_GRID_SIZE_BIG, 4 * CHESSBOARD_GRID_SIZE + 5 * CHESSBOARD_LINE_WIDTH + CHESSBOARD_GRID_SIZE_BIG, 0)) * gridScale;
    mWireFrameUniform.UpdateData(device, &pos, 11 * mWireframeMinUniformBufferOffset);

    mWireframePipeline.AllocateDescriptorSets(device, pool.descriptorPool, 1, &mSet);
    mPointPipeline.AllocateDescriptorSets(device, pool.descriptorPool, 1, &mPointSet);
}