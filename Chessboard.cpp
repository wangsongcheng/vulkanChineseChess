#include "Chessboard.h"
void Chessboard::DestroyChess(VkDevice device, uint32_t country){
    mChess[country].jiang->Cleanup(device);
    delete mChess[country].jiang;
    for (size_t j = 0; j < sizeof(mChess->bing) / sizeof(Chess *); ++j){
        if(mChess[country].bing[j]){
            mChess[country].bing[j]->Cleanup(device);
            delete mChess[country].bing[j];
        }
    }
    for (size_t j = 0; j < sizeof(mChess->ma) / sizeof(Chess *); ++j){
        if(mChess[country].ma[j]){
            mChess[country].ma[j]->Cleanup(device);
            delete mChess[country].ma[j];
        }
    }
    for (size_t j = 0; j < sizeof(mChess->che) / sizeof(Chess *); ++j){
        if(mChess[country].che[j]){
            mChess[country].che[j]->Cleanup(device);
            delete mChess[country].che[j];
        }
    }
    for (size_t j = 0; j < sizeof(mChess->pao) / sizeof(Chess *); ++j){
        if(mChess[country].pao[j]){
            mChess[country].pao[j]->Cleanup(device);
            delete mChess[country].pao[j];
        }
    }
    for (size_t j = 0; j < sizeof(mChess->shi) / sizeof(Chess *); ++j){
        if(mChess[country].shi[j]){
            mChess[country].shi[j]->Cleanup(device);
            delete mChess[country].shi[j];
        }
    }
    for (size_t j = 0; j < sizeof(mChess->xiang) / sizeof(Chess *); ++j){
        if(mChess[country].xiang[j]){
            mChess[country].xiang[j]->Cleanup(device);
            delete mChess[country].xiang[j];
        }
    }
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
void Chessboard::InitChess(VkDevice device, uint32_t country, uint32_t windowWidth, VkRenderPass renderpass, VulkanPool pool, VkQueue graphics, VkSampler fontSampler){
    const glm::vec3 color[] = { glm::vec3(0, 0, 1), glm::vec3(1, 0, 0), glm::vec3(0, 1, 0), glm::vec3(1, 1, 0) };
    if(country == WU_CHESS_INDEX){
        mChess[country].jiang = new Wu;
    }
    else if(country == WEI_CHESS_INDEX){
        mChess[country].jiang = new Wei;
    }
    else if(country == SHU_CHESS_INDEX){
        mChess[country].jiang = new Shu;
    }
    else if(country == HAN_CHESS_INDEX){
        mChess[country].jiang = new Han;
    }
    //魏蜀吴汉
    const glm::vec2 rc_jiang[] = { glm::vec2(8, 0), glm::vec2(rc_jiang[0].x, 16), glm::vec2(rc_jiang[1].y, 8), glm::vec2(0, 8) };
    const glm::vec2 rc_bing[][5] = {
        glm::vec2(4, 3), glm::vec2(rc_bing[0][0].x + 2, rc_bing[0][0].y), glm::vec2(rc_bing[0][1].x + 2, rc_bing[0][0].y), glm::vec2(rc_bing[0][2].x + 2, rc_bing[0][0].y), glm::vec2(rc_bing[0][3].x + 2, rc_bing[0][0].y),//魏
        glm::vec2(rc_bing[0][0].x, 13), glm::vec2(rc_bing[0][1].x, rc_bing[1][0].y), glm::vec2(rc_bing[0][2].x, rc_bing[1][0].y), glm::vec2(rc_bing[0][3].x, rc_bing[1][0].y), glm::vec2(rc_bing[0][4].x, rc_bing[1][0].y),//蜀
        glm::vec2(rc_bing[1][0].y, 12), glm::vec2(rc_bing[2][0].x, rc_bing[2][0].y - 2), glm::vec2(rc_bing[2][0].x, rc_bing[2][1].y - 2), glm::vec2(rc_bing[2][0].x, rc_bing[2][2].y - 2), glm::vec2(rc_bing[2][0].x, rc_bing[2][3].y - 2)//吴
    };
    const glm::vec2 rc_ma[][2] = {
        glm::vec2(5, 0), glm::vec2(rc_ma[0][0].x + 6, rc_ma[0][0].y),
        glm::vec2(rc_ma[0][0].x, 16), glm::vec2(rc_ma[0][1].x, rc_ma[1][0].y),
        glm::vec2(rc_ma[1][0].y, 11), glm::vec2(rc_ma[2][0].x, rc_ma[2][0].y - 6)
    };
    const glm::vec2 rc_che[][3] = {
        glm::vec2(4, 0), glm::vec2(rc_che[0][0].x + 8, rc_che[0][0].y), glm::vec2(0),
        glm::vec2(rc_che[0][0].x, rc_ma[1][0].y), glm::vec2(rc_che[0][1].x, rc_che[1][0].y), glm::vec2(0),
        glm::vec2(rc_ma[2][0].x, rc_ma[2][0].y + 1), glm::vec2(rc_che[2][0].x, rc_che[2][0].y - 8), glm::vec2(0),
        glm::vec2(3, rc_bing[2][1].y), glm::vec2(rc_che[3][0].x, rc_bing[2][2].y), glm::vec2(rc_che[3][0].x, rc_bing[2][3].y)
    };
    const glm::vec2 rc_pao[][2] = {
        glm::vec2(rc_ma[0][0].x, 2), glm::vec2(rc_ma[0][1].x, rc_pao[0][0].y),
        glm::vec2(rc_pao[0][0].x, 14), glm::vec2(rc_pao[0][1].x, rc_pao[1][0].y),
        glm::vec2(rc_pao[1][0].y, 11), glm::vec2(rc_pao[2][0].x, rc_pao[2][0].y - 6),
        glm::vec2(2, rc_bing[2][2].y), glm::vec2(0)
    };
    const glm::vec2 rc_shi[][2] = {
        glm::vec2(7, 0), glm::vec2(rc_shi[0][0].x + 2, rc_shi[0][0].y),
        glm::vec2(rc_shi[0][0].x, rc_che[1][0].y), glm::vec2(rc_shi[0][1].x, rc_shi[1][0].y),
        glm::vec2(rc_che[2][0].x, 9), glm::vec2(rc_shi[2][0].x, rc_shi[2][0].y - 2)
    };
    const glm::vec2 rc_xiang[][2] = {
        glm::vec2(6, 0), glm::vec2(rc_xiang[0][0].x + 4, rc_xiang[0][0].y),
        glm::vec2(rc_xiang[0][0].x, rc_che[1][0].y), glm::vec2(rc_xiang[0][1].x, rc_xiang[1][0].y),
        glm::vec2(rc_che[2][0].x, 10), glm::vec2(rc_xiang[2][0].x, rc_xiang[2][0].y - 4)
    };
    mChess[country].jiang->CreatePipeline(device, renderpass, windowWidth);
    mChess[country].jiang->CreateFontTexture(device, pool.commandPool, graphics);
    mChess[country].jiang->CreateVulkanResource(device, color[country], pool, graphics);
    mChess[country].jiang->UpdateSet(device, fontSampler);
    mChess[country].jiang->UpdateUniform(device, rc_jiang[country].y, rc_jiang[country].x);
    if(country != HAN_CHESS_INDEX){
        for (size_t j = 0; j < sizeof(mChess->bing) / sizeof(Chess *); ++j){
            mChess[country].bing[j] = new Bing;
            mChess[country].bing[j]->CreatePipeline(device, renderpass, windowWidth);
            mChess[country].bing[j]->CreateFontTexture(device, pool.commandPool, graphics);
            mChess[country].bing[j]->CreateVulkanResource(device, color[country], pool, graphics);
            mChess[country].bing[j]->UpdateSet(device, fontSampler);
            mChess[country].bing[j]->UpdateUniform(device, rc_bing[country][j].y, rc_bing[country][j].x);
        }
        for (size_t j = 0; j < sizeof(mChess->ma) / sizeof(Chess *); ++j){
            mChess[country].ma[j] = new Ma;
            mChess[country].ma[j]->CreatePipeline(device, renderpass, windowWidth);
            mChess[country].ma[j]->CreateFontTexture(device, pool.commandPool, graphics);
            mChess[country].ma[j]->CreateVulkanResource(device, color[country], pool, graphics);
            mChess[country].ma[j]->UpdateSet(device, fontSampler);
            mChess[country].ma[j]->UpdateUniform(device, rc_ma[country][j].y, rc_ma[country][j].x);
        }
        for (size_t j = 0; j < sizeof(mChess->shi) / sizeof(Chess *); ++j){
            mChess[country].shi[j] = new Shi;
            mChess[country].shi[j]->CreatePipeline(device, renderpass, windowWidth);
            mChess[country].shi[j]->CreateFontTexture(device, pool.commandPool, graphics);
            mChess[country].shi[j]->CreateVulkanResource(device, color[country], pool, graphics);
            mChess[country].shi[j]->UpdateSet(device, fontSampler);
            mChess[country].shi[j]->UpdateUniform(device, rc_shi[country][j].y, rc_shi[country][j].x);
        }
        for (size_t j = 0; j < sizeof(mChess->xiang) / sizeof(Chess *); ++j){
            mChess[country].xiang[j] = new Xiang;
            mChess[country].xiang[j]->CreatePipeline(device, renderpass, windowWidth);
            mChess[country].xiang[j]->CreateFontTexture(device, pool.commandPool, graphics);
            mChess[country].xiang[j]->CreateVulkanResource(device, color[country], pool, graphics);
            mChess[country].xiang[j]->UpdateSet(device, fontSampler);
            mChess[country].xiang[j]->UpdateUniform(device, rc_xiang[country][j].y, rc_xiang[country][j].x);
        }
        for (size_t j = 0; j < sizeof(mChess->che) / sizeof(Chess *); ++j){
            mChess[country].che[j] = new Che;
            mChess[country].che[j]->CreatePipeline(device, renderpass, windowWidth);
            mChess[country].che[j]->CreateFontTexture(device, pool.commandPool, graphics);
            mChess[country].che[j]->CreateVulkanResource(device, color[country], pool, graphics);
            mChess[country].che[j]->UpdateSet(device, fontSampler);
            mChess[country].che[j]->UpdateUniform(device, rc_che[country][j].y, rc_che[country][j].x);
        }
        for (size_t j = 0; j < sizeof(mChess->pao) / sizeof(Chess *); ++j){
            mChess[country].pao[j] = new Pao;
            mChess[country].pao[j]->CreatePipeline(device, renderpass, windowWidth);
            mChess[country].pao[j]->CreateFontTexture(device, pool.commandPool, graphics);
            mChess[country].pao[j]->CreateVulkanResource(device, color[country], pool, graphics);
            mChess[country].pao[j]->UpdateSet(device, fontSampler);
            mChess[country].pao[j]->UpdateUniform(device, rc_pao[country][j].y, rc_pao[country][j].x);
        }
    }
    else{
        for (size_t j = 0; j < 3; ++j){
            mChess[country].che[j] = new Che;
            mChess[country].che[j]->CreatePipeline(device, renderpass, windowWidth);
            mChess[country].che[j]->CreateFontTexture(device, pool.commandPool, graphics);
            mChess[country].che[j]->CreateVulkanResource(device, color[country], pool, graphics);
            mChess[country].che[j]->UpdateSet(device, fontSampler);
            mChess[country].che[j]->UpdateUniform(device, rc_che[country][j].y, rc_che[country][j].x);
        }
        for (size_t j = 0; j < 1; ++j){
            mChess[country].pao[j] = new Pao;
            mChess[country].pao[j]->CreatePipeline(device, renderpass, windowWidth);
            mChess[country].pao[j]->CreateFontTexture(device, pool.commandPool, graphics);
            mChess[country].pao[j]->CreateVulkanResource(device, color[country], pool, graphics);
            mChess[country].pao[j]->UpdateSet(device, fontSampler);
            mChess[country].pao[j]->UpdateUniform(device, rc_pao[country][j].y, rc_pao[country][j].x);
        }
    }
}
void Chessboard::CreateGraphicsPipeline(VkDevice device, uint32_t windowWidth, VkRenderPass renderpass, const std::vector<std::string>&shader, GraphicsPipeline&pipeline, const GraphicsPipelineStateInfo&pipelineState){
    std::vector<VkShaderStageFlagBits> stage = { VK_SHADER_STAGE_VERTEX_BIT, VK_SHADER_STAGE_FRAGMENT_BIT };
    pipeline.PushDescriptorSetLayoutBinding(0, VK_SHADER_STAGE_VERTEX_BIT, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC);
    std::vector<uint32_t>cacheData;
    vkf::tool::GetFileContent("ChessboardPipelineCache", cacheData);
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
    mBackground.pos.Destroy(device);
    mWireFrameUniform.Destroy(device);
    for (size_t i = 0; i < sizeof(mBackground.vert) / sizeof(VulkanBuffer); ++i){
        mBackground.vert[i].Destroy(device);
    }
    for (size_t i = 0; i < sizeof(mWireframeIndex) / sizeof(VulkanBuffer); ++i){
        mWireframeIndex[i].Destroy(device);
    }
    for (size_t i = 0; i < sizeof(mChess) / sizeof(ChessboardChess); ++i){
        DestroyChess(device, i);
    }
    DestroyGraphicsPipeline(device);
}
void Chessboard::UpdateSet(VkDevice device){
    mPointPipeline.UpdateDescriptorSets(device, { mPointUniform }, {}, mPointSet);
    mWireframePipeline.UpdateDescriptorSets(device, { mWireFrameUniform }, {}, mSet);
    mBackground.pipeline.UpdateDescriptorSets(device, { mBackground.pos }, {}, mBackground.mSet);
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
    for (size_t i = 0; i < sizeof(mChess) / sizeof(ChessboardChess); ++i){
        mChess[i].jiang->RecodeCommand(cmd, windowWidth);
        for (size_t j = 0; j < sizeof(mChess->bing) / sizeof(Chess *); ++j){
            if(mChess[i].bing[j])mChess[i].bing[j]->RecodeCommand(cmd, windowWidth);
        }
        for (size_t j = 0; j < sizeof(mChess->ma) / sizeof(Chess *); ++j){
            if(mChess[i].ma[j])mChess[i].ma[j]->RecodeCommand(cmd, windowWidth);
        }
        for (size_t j = 0; j < sizeof(mChess->che) / sizeof(Chess *); ++j){
            if(mChess[i].che[j])mChess[i].che[j]->RecodeCommand(cmd, windowWidth);
        }
        for (size_t j = 0; j < sizeof(mChess->pao) / sizeof(Chess *); ++j){
            if(mChess[i].pao[j])mChess[i].pao[j]->RecodeCommand(cmd, windowWidth);
        }
        for (size_t j = 0; j < sizeof(mChess->shi) / sizeof(Chess *); ++j){
            if(mChess[i].shi[j])mChess[i].shi[j]->RecodeCommand(cmd, windowWidth);
        }
        for (size_t j = 0; j < sizeof(mChess->xiang) / sizeof(Chess *); ++j){
            if(mChess[i].xiang[j])mChess[i].xiang[j]->RecodeCommand(cmd, windowWidth);
        }
    }
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
    // CreateGraphicsPipeline(device, windowWidth, renderpass, { "shaders/baseVert.spv", "shaders/baseFrag.spv" }, mCircularPipeline, state);
    // //字体管线需要用到图片
    // mFontPipeline.PushDescriptorSetLayoutBinding(1, VK_SHADER_STAGE_FRAGMENT_BIT, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
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
        glm::vec2(8, 0), glm::vec2(rc[0].x - 1, rc[0].y + 1), glm::vec2(15, 7), glm::vec2(rc[2].x - 1, rc[2].y + 1), glm::vec2(rc[0].x, 14), glm::vec2(rc[1].x, rc[4].y + 1),
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
void Chessboard::InitChessboard(VkDevice device, uint32_t windowWidth, VkRenderPass renderpass, VulkanPool pool, VkQueue graphics, VkSampler fontSampler){
    mPalacePos[WEI_CHESS_INDEX].row = 1;
    mPalacePos[WEI_CHESS_INDEX].column = 8;

    mPalacePos[SHU_CHESS_INDEX].row = 16;
    mPalacePos[SHU_CHESS_INDEX].column = mPalacePos[WEI_CHESS_INDEX].column;

    mPalacePos[WU_CHESS_INDEX].row = mPalacePos[WEI_CHESS_INDEX].column;
    mPalacePos[WU_CHESS_INDEX].column = mPalacePos[SHU_CHESS_INDEX].row;
    for (size_t i = 0; i < sizeof(mChess) / sizeof(ChessboardChess); i++){
        InitChess(device, i, windowWidth, renderpass, pool, graphics, fontSampler);
    }   
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
    //判断是否在魏蜀吴的某一边, 然后将该势力的九格宫坐标传给棋子的Select函数
}
void Chessboard::UnSelect(uint32_t row, uint32_t column){
    
    //mSelected赋值为nullptr,为了不影响后面的操作
    mSelected = nullptr;
}