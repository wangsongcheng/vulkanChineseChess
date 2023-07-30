#include "Chessboard.h"
void Chessboard::InitChessboard(VkDevice device, uint32_t country){
    if(country == WU_CHESS_INDEX)mChess[country].jiang = new Wu;
    if(country == WEI_CHESS_INDEX)mChess[country].jiang = new Wei;
    if(country == SHU_CHESS_INDEX)mChess[country].jiang = new Shu;
    if(country == HAN_CHESS_INDEX)mChess[country].jiang = new Han;

    //ROW_TO_Y
    // mChess[country].jiang->UpdateUniform(device, );
    // for (size_t j = 0; j < sizeof(mChess->bing) / sizeof(Chess); ++j){
    //     mChess[i].bing[j]->;
    // }
    // for (size_t j = 0; j < sizeof(mChess->ma) / sizeof(Chess); ++j){
    //     mChess[i].ma[j]->;
    // }
    // for (size_t j = 0; j < sizeof(mChess->che) / sizeof(Chess); ++j){
    //     mChess[i].che[j]->;
    // }
    // for (size_t j = 0; j < sizeof(mChess->pao) / sizeof(Chess); ++j){
    //     mChess[i].pao[j]->;
    // }
    // for (size_t j = 0; j < sizeof(mChess->shi) / sizeof(Chess); ++j){
    //     mChess[i].shi[j]->;
    // }
    // for (size_t j = 0; j < sizeof(mChess->xiang) / sizeof(Chess); ++j){
    //     mChess[i].xiang[j]->;
    // }
}
void Chessboard::DrawChessboard(VkCommandBuffer cmd, uint32_t windowWidth){
    VkDeviceSize offset = 0;
    uint32_t dynamicOffset = 0;
    const glm::mat4 projection = glm::ortho(0.0f, (float)windowWidth, 0.0f, (float)windowWidth, -1.0f, 1.0f);
    mPointPipeline.PushPushConstant(cmd, VK_SHADER_STAGE_VERTEX_BIT, sizeof(glm::mat4), &projection);
    mPointPipeline.BindPipeline(cmd);
    vkCmdBindVertexBuffers(cmd, 0, 1, &mPointVert.buffer, &offset);
    for (size_t i = 0; i < 4; ++i){
        dynamicOffset = i * mPointMinUniformBufferOffset;
        mPointPipeline.BindDescriptorSet(cmd, mPointSet, 1, &dynamicOffset);
        vkCmdDraw(cmd, 1, 1, 0, 0);
        // vkCmdDrawIndexed(cmd, 1, 1, 0, 0, 0);
    }
    for (uint32_t uiRow = 0; uiRow < 4; ++uiRow){
        for (uint32_t uiColumn = 0; uiColumn < 8; ++uiColumn){
            dynamicOffset = (CHESSBOARD_ROW_COLUMN_INDEX(uiRow, uiColumn, 8) + 4) * mPointMinUniformBufferOffset;
            mPointPipeline.BindDescriptorSet(cmd, mPointSet, 1, &dynamicOffset);
            vkCmdDraw(cmd, 1, 1, 0, 0);
            // vkCmdDrawIndexed(cmd, 1, 1, 0, 0, 0);
        }
    }
    for (uint32_t uiRow = 0; uiRow < 8; ++uiRow){
        for (uint32_t uiColumn = 0; uiColumn < 16; ++uiColumn){
            dynamicOffset = (CHESSBOARD_ROW_COLUMN_INDEX(uiRow, uiColumn, 16) + 36) * mPointMinUniformBufferOffset;
            mPointPipeline.BindDescriptorSet(cmd, mPointSet, 1, &dynamicOffset);
            vkCmdDraw(cmd, 1, 1, 0, 0);
            // vkCmdDrawIndexed(cmd, 1, 1, 0, 0, 0);
        }
    }
    for (uint32_t uiRow = 0; uiRow < 4; ++uiRow){
        for (uint32_t uiColumn = 0; uiColumn < 8; ++uiColumn){
            dynamicOffset = (CHESSBOARD_ROW_COLUMN_INDEX(uiRow, uiColumn, 8) + 164) * mPointMinUniformBufferOffset;
            mPointPipeline.BindDescriptorSet(cmd, mPointSet, 1, &dynamicOffset);
            vkCmdDraw(cmd, 1, 1, 0, 0);
            // vkCmdDrawIndexed(cmd, 1, 1, 0, 0, 0);
        }
    }
    //线框
    vkCmdBindVertexBuffers(cmd, 0, 1, &mWireframeVert.buffer, &offset);
    mWireframePipeline.PushPushConstant(cmd, VK_SHADER_STAGE_VERTEX_BIT, sizeof(glm::mat4), &projection);
    mWireframePipeline.BindPipeline(cmd);

    for (uint32_t i = 0; i < 2; ++i){
        vkCmdBindIndexBuffer(cmd, mWireframeIndex[i].buffer, 0, VK_INDEX_TYPE_UINT16);
        for (uint32_t j = 0; j < 6; ++j){
            dynamicOffset = (CHESSBOARD_ROW_COLUMN_INDEX(i, j, 6)) * mRectMinUniformBufferOffset;
            mWireframePipeline.BindDescriptorSet(cmd, mSet, 1, &dynamicOffset);
            vkCmdDrawIndexed(cmd, 6, 1, 0, 0, 0);
        }
    }
}
void Chessboard::DrawChessboardBackground(VkCommandBuffer cmd, uint32_t windowWidth){
    VkDeviceSize offset = 0;
    uint32_t dynamicOffset = 0;
    const glm::mat4 projection = glm::ortho(0.0f, (float)windowWidth, 0.0f, (float)windowWidth, -1.0f, 1.0f);
    mBackground.pipeline.BindPipeline(cmd);
    mBackground.pipeline.PushPushConstant(cmd, VK_SHADER_STAGE_VERTEX_BIT, sizeof(glm::mat4), &projection);
    mBackground.pipeline.BindDescriptorSet(cmd, mBackground.mSet, 1, &dynamicOffset);
    vkCmdBindVertexBuffers(cmd, 0, 1, &mBackground.vert[0].buffer, &offset);
    vkCmdBindIndexBuffer(cmd, mWireframeIndex[0].buffer, 0, VK_INDEX_TYPE_UINT16);
    vkCmdDrawIndexed(cmd, 6, 1, 0, 0, 0);
    dynamicOffset = mRectMinUniformBufferOffset;
    mBackground.pipeline.BindDescriptorSet(cmd, mBackground.mSet, 1, &dynamicOffset);
    vkCmdBindVertexBuffers(cmd, 0, 1, &mBackground.vert[1].buffer, &offset);
    vkCmdDrawIndexed(cmd, 6, 1, 0, 0, 0);
}
void Chessboard::GetRectVertices(const glm::vec3&color, ChessboardVertex vertices[]){
    const ChessboardVertex v[] = {
        ChessboardVertex(glm::vec3(.0f, 1.0f, .0f), color),//左下
        ChessboardVertex(glm::vec3(1.0f, .0f, .0f), color),//右上
        ChessboardVertex(glm::vec3(.0f, .0f, .0f), color), //左上

        ChessboardVertex(glm::vec3(1.0f, 1.0f, .0f), color)//右下
    };
    memcpy(vertices, v, sizeof(v));
}
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
    pipeline.PushPushConstant(sizeof(glm::mat4), VK_SHADER_STAGE_VERTEX_BIT);
    pipeline.PushVertexInputAttributeDescription(0, 0, VK_FORMAT_R32G32B32_SFLOAT);
    pipeline.PushVertexInputAttributeDescription(1, 3 * sizeof(float), VK_FORMAT_R32G32B32_SFLOAT);
    pipeline.CreateCache(device, cacheData);
    pipeline.CreateDescriptorSetLayout(device);
    pipeline.CreateLayout(device);
    pipeline.CreatePipeline(device, renderpass);
    pipeline.GetCacheData(device, cacheData);
    vkf::tool::WriteFileContent("ChessboardPipelineCache", cacheData.data(), cacheData.size() * sizeof(uint32_t)); 
}
Chessboard::Chessboard(/* args */){
}

Chessboard::~Chessboard(){
}
void Chessboard::Cleanup(VkDevice device){
    mPointVert.Destroy(device);
    // mPointIndex.Destroy(device);
    mPointUniform.Destroy(device);
    mWireframeVert.Destroy(device);
    mWireFrameUniform.Destroy(device);
    for (size_t i = 0; i < sizeof(mBackground.vert) / sizeof(VulkanBuffer); ++i){
        mBackground.vert[i].Destroy(device);
    }
    for (size_t i = 0; i < sizeof(mWireframeIndex) / sizeof(VulkanBuffer); ++i){
        mWireframeIndex[i].Destroy(device);
    }
    DestroyGraphicsPipeline(device);
}
void Chessboard::UpdateSet(VkDevice device){
    mPointPipeline.UpdateDescriptorSets(device, { mPointUniform }, {}, mPointSet);
    mWireframePipeline.UpdateDescriptorSets(device, { mWireFrameUniform }, {}, mSet);
    mBackground.pipeline.UpdateDescriptorSets(device, { mBackground.pos }, {}, mBackground.mSet);
}
void Chessboard::InitChessboard(VkDevice device){
    for (size_t i = 0; i < sizeof(mChess) / sizeof(ChessboardChess); i++){
        InitChessboard(device, i);
    }   
}
void Chessboard::DestroyGraphicsPipeline(VkDevice device){
    std::vector<uint32_t>cacheData;
    mPointPipeline.DestroyCache(device, cacheData);
    vkf::tool::WriteFileContent("ChessboardPipelineCache", cacheData.data(), cacheData.size() * sizeof(uint32_t)); 
    mPointPipeline.DestroySetLayout(device);
    mPointPipeline.DestroyLayout(device);
    mPointPipeline.DestroyPipeline(device);

    mWireframePipeline.DestroyCache(device, cacheData);
    mWireframePipeline.DestroySetLayout(device);
    mWireframePipeline.DestroyLayout(device);
    mWireframePipeline.DestroyPipeline(device);

    mBackground.pipeline.DestroyCache(device, cacheData);
    mBackground.pipeline.DestroySetLayout(device);
    mBackground.pipeline.DestroyLayout(device);
    mBackground.pipeline.DestroyPipeline(device);
}
void Chessboard::RecodeCommand(VkCommandBuffer cmd, uint32_t windowWidth){
    DrawChessboardBackground(cmd, windowWidth);
    DrawChessboard(cmd, windowWidth);
    //没有更好的办法前先这么写
    // mCircularPipeline.BindPipeline(cmd);
    // const glm::mat4 projection = glm::ortho(0.0f, (float)windowWidth, 0.0f, (float)windowWidth, -1.0f, 1.0f);
    // mCircularPipeline.PushPushConstant(cmd, VK_SHADER_STAGE_VERTEX_BIT, sizeof(glm::mat4), &projection);
    // for (size_t i = 0; i < sizeof(mChess) / sizeof(ChessboardChess); ++i){
    //     mChess[i].jiang->DrawCircular(cmd, mCircularPipeline);
    //     for (size_t j = 0; j < sizeof(mChess->bing) / sizeof(Chess); ++j){
    //         if(mChess[i].bing[j])mChess[i].bing[j]->DrawCircular(cmd, mCircularPipeline);
    //     }
    //     for (size_t j = 0; j < sizeof(mChess->ma) / sizeof(Chess); ++j){
    //         if(mChess[i].ma[j])mChess[i].ma[j]->DrawCircular(cmd, mCircularPipeline);
    //     }
    //     for (size_t j = 0; j < sizeof(mChess->che) / sizeof(Chess); ++j){
    //         if(mChess[i].che[j])mChess[i].che[j]->DrawCircular(cmd, mCircularPipeline);
    //     }
    //     for (size_t j = 0; j < sizeof(mChess->pao) / sizeof(Chess); ++j){
    //         if(mChess[i].pao[j])mChess[i].pao[j]->DrawCircular(cmd, mCircularPipeline);
    //     }
    //     for (size_t j = 0; j < sizeof(mChess->shi) / sizeof(Chess); ++j){
    //         if(mChess[i].shi[j])mChess[i].shi[j]->DrawCircular(cmd, mCircularPipeline);
    //     }
    //     for (size_t j = 0; j < sizeof(mChess->xiang) / sizeof(Chess); ++j){
    //         if(mChess[i].xiang[j])mChess[i].xiang[j]->DrawCircular(cmd, mCircularPipeline);
    //     }
    // }
    // mFontPipeline.BindPipeline(cmd);
    // mFontPipeline.PushPushConstant(cmd, VK_SHADER_STAGE_VERTEX_BIT, sizeof(glm::mat4), &projection);
    // for (size_t i = 0; i < sizeof(mChess) / sizeof(ChessboardChess); ++i){
    //     mChess[i].jiang->DrawFont(cmd, mFontPipeline);
    //     for (size_t j = 0; j < sizeof(mChess->bing) / sizeof(Chess); ++j){
    //         if(mChess[j].bing[j])mChess[i].bing[j]->DrawFont(cmd, mFontPipeline);
    //     }
    //     for (size_t j = 0; j < sizeof(mChess->ma) / sizeof(Chess); ++j){
    //         if(mChess[j].ma[j])mChess[i].ma[j]->DrawCircular(cmd, mFontPipeline);
    //     }
    //     for (size_t j = 0; j < sizeof(mChess->che) / sizeof(Chess); ++j){
    //         if(mChess[j].che[j])mChess[i].che[j]->DrawCircular(cmd, mFontPipeline);
    //     }
    //     for (size_t j = 0; j < sizeof(mChess->pao) / sizeof(Chess); ++j){
    //         if(mChess[j].pao[j])mChess[i].pao[j]->DrawCircular(cmd, mFontPipeline);
    //     }
    //     for (size_t j = 0; j < sizeof(mChess->shi) / sizeof(Chess); ++j){
    //         if(mChess[j].shi[j])mChess[i].shi[j]->DrawCircular(cmd, mFontPipeline);
    //     }
    //     for (size_t j = 0; j < sizeof(mChess->xiang) / sizeof(Chess); ++j){
    //         if(mChess[j].xiang[j])mChess[i].xiang[j]->DrawCircular(cmd, mFontPipeline);
    //     }
    // }
}
void Chessboard::CreatePipeline(VkDevice device, VkRenderPass renderpass, uint32_t windowWidth){
    GraphicsPipelineStateInfo state;
    state.mRasterization.lineWidth = CHESSBOARD_LINE_WIDTH;
    state.mInputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
    CreateGraphicsPipeline(device, windowWidth, renderpass, { "shaders/basePointVert.spv", "shaders/baseFrag.spv" }, mPointPipeline, state);
    state.mRasterization.polygonMode = VK_POLYGON_MODE_LINE;
    state.mInputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    CreateGraphicsPipeline(device, windowWidth, renderpass, { "shaders/baseVert.spv", "shaders/baseFrag.spv" }, mWireframePipeline, state);
    // if(mPointSizeRange < CHESSBOARD_BLACK_BACKGROUND_SIZE(windowWidth)){
    state.mRasterization.polygonMode = VK_POLYGON_MODE_FILL;
    CreateGraphicsPipeline(device, windowWidth, renderpass, { "shaders/baseVert.spv", "shaders/baseFrag.spv" }, mBackground.pipeline, state);
    // }

    // //没有更好的办法前先这么写
    // state.mRasterization.polygonMode = VK_POLYGON_MODE_FILL;
    // //以下函数内没 图片相关的描述符类型
    // CreateGraphicsPipeline(device, windowWidth, renderpass, { "shaders/baseVert.spv", "shaders/baseFrag.spv" }, mCircularPipeline, state);
    // CreateGraphicsPipeline(device, windowWidth, renderpass, { "shaders/baseFontVert.spv", "shaders/baseFontFrag.spv" }, mFontPipeline, state);
}
void Chessboard::CreateVulkanResource(VkPhysicalDevice physicalDevice, VkDevice device, uint32_t windowWidth, VkQueue graphics, VulkanPool pool){
    VkPhysicalDeviceProperties physicalDeviceProperties;
    vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);
    // mPointSizeRange = physicalDeviceProperties.limits.pointSizeRange[1];
    mPointMinUniformBufferOffset = ALIGN(sizeof(PointUniform), physicalDeviceProperties.limits.minUniformBufferOffsetAlignment);
    mRectMinUniformBufferOffset = ALIGN(sizeof(glm::mat4), physicalDeviceProperties.limits.minUniformBufferOffsetAlignment);
    ChessboardVertex vertices[4];
    const ChessboardVertex pointVertices[] = {
        ChessboardVertex(glm::vec3(.0f), glm::vec3(1, .9, .5)),
        // ChessboardVertex(glm::vec3(.0f), glm::vec3(50, 215, 90.0f)),
        ChessboardVertex(glm::vec3(.0f), glm::vec3(0))
    };
    // const uint16_t pointIndices[] = { 0 };
    const uint16_t indices[] = { 0, 1, 2, 0, 3, 1 };
    const uint16_t mirrorIndices[] = { 0, 2, 3, 2, 3, 1 };
    GetRectVertices(glm::vec3(0), vertices);
    // mPointIndex.CreateBuffer(device, sizeof(pointIndices), VK_BUFFER_USAGE_INDEX_BUFFER_BIT|VK_BUFFER_USAGE_TRANSFER_DST_BIT);
    mPointVert.CreateBuffer(device, sizeof(pointVertices), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT|VK_BUFFER_USAGE_TRANSFER_DST_BIT);
    mWireframeIndex[0].CreateBuffer(device, sizeof(indices), VK_BUFFER_USAGE_INDEX_BUFFER_BIT|VK_BUFFER_USAGE_TRANSFER_DST_BIT);
    mWireframeIndex[1].CreateBuffer(device, sizeof(mirrorIndices), VK_BUFFER_USAGE_INDEX_BUFFER_BIT|VK_BUFFER_USAGE_TRANSFER_DST_BIT);
    mWireframeVert.CreateBuffer(device, sizeof(vertices), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT|VK_BUFFER_USAGE_TRANSFER_DST_BIT);
    mPointVert.AllocateAndBindMemory(device, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    // mPointIndex.AllocateAndBindMemory(device, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    mWireframeVert.AllocateAndBindMemory(device, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    for (size_t i = 0; i < sizeof(mWireframeIndex) / sizeof(VulkanBuffer); i++){
        mWireframeIndex[i].AllocateAndBindMemory(device, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    }
    vkf::tool::CopyBuffer(device, sizeof(indices), indices, graphics, pool.commandPool, mWireframeIndex[0]);
    // vkf::tool::CopyBuffer(device, sizeof(pointIndices), pointIndices, graphics, pool.commandPool, mPointIndex);
    vkf::tool::CopyBuffer(device, sizeof(mirrorIndices), mirrorIndices, graphics, pool.commandPool, mWireframeIndex[1]);
    vkf::tool::CopyBuffer(device, sizeof(vertices), vertices, graphics, pool.commandPool, mWireframeVert);
    vkf::tool::CopyBuffer(device, sizeof(pointVertices), pointVertices, graphics, pool.commandPool, mPointVert);
    // 中间那部分(8 * 16) + 汉和吴(8 * 8) + 4个大一些的矩形
    //8 * 16 + 8 * 8 + 4
    mBackground.pos.CreateBuffer(device, mRectMinUniformBufferOffset * 2, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
    mWireFrameUniform.CreateBuffer(device, mRectMinUniformBufferOffset * 12, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
    mPointUniform.CreateBuffer(device, mPointMinUniformBufferOffset * (8 * 16 + 68), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
    mPointUniform.AllocateAndBindMemory(device, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT|VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    mBackground.pos.AllocateAndBindMemory(device, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT|VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    mWireFrameUniform.AllocateAndBindMemory(device, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT|VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    mPointUniform.size = mPointMinUniformBufferOffset;
    mBackground.pos.size = mWireFrameUniform.size = mRectMinUniformBufferOffset;

    //如果比黑色低层还小，那一定比最低层的小
    // if(mPointSizeRange < CHESSBOARD_BLACK_BACKGROUND_SIZE(windowWidth)){
    // printf("设备pointSizeRange[1]的值为%f, 无法绘制指定大小的点。请更换设备\n");
    glm::vec3 color[] = { glm::vec3(1, .9, .5), glm::vec3(0) };
    for (size_t i = 0; i < sizeof(mBackground.vert) / sizeof(VulkanBuffer); ++i){
        GetRectVertices(color[i], vertices);
        mBackground.vert[i].CreateBuffer(device, sizeof(vertices), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT|VK_BUFFER_USAGE_TRANSFER_DST_BIT);
        mBackground.vert[i].AllocateAndBindMemory(device, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
        vkf::tool::CopyBuffer(device, sizeof(vertices), vertices, graphics, pool.commandPool, mBackground.vert[i]);
    }
    // }
    glm::mat4 model = glm::scale(glm::mat4(1.0f), glm::vec3(windowWidth, windowWidth, 1));
    mBackground.pos.UpdateData(device, &model);
    model = glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(CHESSBOARD_GRID_SIZE, CHESSBOARD_GRID_SIZE, 0)), glm::vec3(CHESSBOARD_BLACK_BACKGROUND_SIZE(windowWidth), CHESSBOARD_BLACK_BACKGROUND_SIZE(windowWidth), 1));
    mBackground.pos.UpdateData(device, &model, mRectMinUniformBufferOffset);

    PointUniform pointPos;
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
        mPointUniform.UpdateData(device, &pointPos, i * mPointMinUniformBufferOffset);
    }
    //魏
    pointPos.pointSize = CHESSBOARD_GRID_SIZE;
    const uint32_t gridPointOffset = CHESSBOARD_GRID_SIZE * .5;
    for (uint32_t uiRow = 0; uiRow < 4; ++uiRow){
        for (uint32_t uiColumn = 0; uiColumn < 8; ++uiColumn){
            pointPos.model = glm::translate(glm::mat4(1.0f), glm::vec3(gridPointOffset + COLUMN_TO_X(uiColumn + 4, CHESSBOARD_GRID_SIZE, CHESSBOARD_LINE_WIDTH), gridPointOffset + ROW_TO_Y(uiRow, CHESSBOARD_GRID_SIZE, CHESSBOARD_LINE_WIDTH), 0));
            mPointUniform.UpdateData(device, &pointPos, (CHESSBOARD_ROW_COLUMN_INDEX(uiRow, uiColumn, 8) + 4) * mPointMinUniformBufferOffset);
        }
    }
    //汉 吴
    pointPos.pointSize = CHESSBOARD_GRID_SIZE;
    for (uint32_t uiRow = 0; uiRow < 8; ++uiRow){
        for (uint32_t uiColumn = 0; uiColumn < 16; ++uiColumn){
            pointPos.model = glm::translate(glm::mat4(1.0f), glm::vec3(gridPointOffset + COLUMN_TO_X(uiColumn, CHESSBOARD_GRID_SIZE, CHESSBOARD_LINE_WIDTH), gridPointOffset + ROW_TO_Y(uiRow + 4, CHESSBOARD_GRID_SIZE, CHESSBOARD_LINE_WIDTH), 0));
            mPointUniform.UpdateData(device, &pointPos, (CHESSBOARD_ROW_COLUMN_INDEX(uiRow, uiColumn, 16) + 36) * mPointMinUniformBufferOffset);
        }
    }
    pointPos.pointSize = CHESSBOARD_GRID_SIZE;
    for (uint32_t uiRow = 0; uiRow < 4; ++uiRow){
        for (uint32_t uiColumn = 0; uiColumn < 8; ++uiColumn){
            pointPos.model = glm::translate(glm::mat4(1.0f), glm::vec3(gridPointOffset + COLUMN_TO_X(uiColumn + 4, CHESSBOARD_GRID_SIZE, CHESSBOARD_LINE_WIDTH), gridPointOffset + ROW_TO_Y(uiRow + 12, CHESSBOARD_GRID_SIZE, CHESSBOARD_LINE_WIDTH), 0));
            mPointUniform.UpdateData(device, &pointPos, (CHESSBOARD_ROW_COLUMN_INDEX(uiRow, uiColumn, 8) + 164) * mPointMinUniformBufferOffset);
        }
    }
    const glm::mat4 gridScale = glm::scale(glm::mat4(1.0f), glm::vec3(CHESSBOARD_GRID_SIZE + CHESSBOARD_LINE_WIDTH, CHESSBOARD_GRID_SIZE + CHESSBOARD_LINE_WIDTH, 1));

    const glm::vec2 rc[] = {
        //线框[0]
        glm::vec2(8, 0), glm::vec2(rc[0].x - 1, rc[0].y + 1), glm::vec2(15, 8), glm::vec2(rc[2].x - 1, rc[2].y + 1), glm::vec2(rc[0].x, 14), glm::vec2(rc[1].x, rc[4].y + 1),
        //线框[1]
        glm::vec2(rc[1].x, rc[0].y), glm::vec2(rc[0].x, rc[1].y), glm::vec2(rc[3].x, rc[2].y), glm::vec2(rc[2].x, rc[3].y), glm::vec2(rc[1].x, rc[4].y), glm::vec2(rc[4].x, rc[5].y)
    };
    glm::mat4 pos[12];
    for (size_t i = 0; i < sizeof(rc) / sizeof(glm::vec2); ++i){
        pos[i] = glm::translate(glm::mat4(1), glm::vec3(COLUMN_TO_X(rc[i].x, CHESSBOARD_GRID_SIZE + CHESSBOARD_LINE_WIDTH, 0), ROW_TO_Y(rc[i].y, CHESSBOARD_GRID_SIZE + CHESSBOARD_LINE_WIDTH, 0), 0)) * gridScale;
    }
    mWireFrameUniform.UpdateData(device, sizeof(pos), &pos);

    mWireframePipeline.AllocateDescriptorSets(device, pool.descriptorPool, 1, &mSet);
    mPointPipeline.AllocateDescriptorSets(device, pool.descriptorPool, 1, &mPointSet);
    mBackground.pipeline.AllocateDescriptorSets(device, pool.descriptorPool, 1, &mBackground.mSet);
}
void Chessboard::Play(uint32_t row, uint32_t column){
    //通过row和column算出坐标。
    //然后将mSelected指定的棋子下到算出的坐标处

    //mSelected赋值为nullptr,为了不影响后面的操作
    mSelected = nullptr;
}
void Chessboard::Select(uint32_t row, uint32_t column){
    //通过row和column算出坐标。
    //然后给mSelected赋值指定的棋子
}
void Chessboard::UnSelect(uint32_t row, uint32_t column){
    
    //mSelected赋值为nullptr,为了不影响后面的操作
    mSelected = nullptr;
}