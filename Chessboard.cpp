#include "Chessboard.h"
void Chessboard::DrawChessboard(VkCommandBuffer cmd){
    VkDeviceSize offset = 0;
    uint32_t dynamicOffset = 0;
    const glm::mat4 projection = glm::ortho(0.0f, (float)mWindowWidth, 0.0f, (float)mWindowWidth, -1.0f, 1.0f);
    mGrid.pipeline.PushPushConstant(cmd, VK_SHADER_STAGE_VERTEX_BIT, sizeof(glm::mat4), &projection);
    mGrid.pipeline.BindPipeline(cmd);
    vkCmdBindVertexBuffers(cmd, 0, 1, &mGrid.vert.buffer, &offset);
    for (size_t i = 0; i < 4; ++i){
        dynamicOffset = i * mPointMinUniformBufferOffset;
        mGrid.pipeline.BindDescriptorSet(cmd, mGrid.set, 1, &dynamicOffset);
        vkCmdDraw(cmd, 1, 1, 1, 0);
        // vkCmdDrawIndexed(cmd, 1, 1, 0, 0, 0);
    }
    for (uint32_t uiRow = 0; uiRow < 4; ++uiRow){//32
        for (uint32_t uiColumn = 0; uiColumn < 8; ++uiColumn){
            dynamicOffset = (CHESSBOARD_ROW_COLUMN_INDEX(uiRow, uiColumn, 8) + 4) * mPointMinUniformBufferOffset;
            mGrid.pipeline.BindDescriptorSet(cmd, mGrid.set, 1, &dynamicOffset);
            vkCmdDraw(cmd, 1, 1, 0, 0);
            // vkCmdDrawIndexed(cmd, 1, 1, 0, 0, 0);
        }
    }
    for (uint32_t uiRow = 0; uiRow < 8; ++uiRow){//128
        for (uint32_t uiColumn = 0; uiColumn < 16; ++uiColumn){
            dynamicOffset = (CHESSBOARD_ROW_COLUMN_INDEX(uiRow, uiColumn, 16) + 36) * mPointMinUniformBufferOffset;
            mGrid.pipeline.BindDescriptorSet(cmd, mGrid.set, 1, &dynamicOffset);
            vkCmdDraw(cmd, 1, 1, 0, 0);
            // vkCmdDrawIndexed(cmd, 1, 1, 0, 0, 0);
        }
    }
    for (uint32_t uiRow = 0; uiRow < 4; ++uiRow){//32
        for (uint32_t uiColumn = 0; uiColumn < 8; ++uiColumn){
            dynamicOffset = (CHESSBOARD_ROW_COLUMN_INDEX(uiRow, uiColumn, 8) + 164) * mPointMinUniformBufferOffset;
            mGrid.pipeline.BindDescriptorSet(cmd, mGrid.set, 1, &dynamicOffset);
            vkCmdDraw(cmd, 1, 1, 0, 0);
            // vkCmdDrawIndexed(cmd, 1, 1, 0, 0, 0);
        }
    }
    //线框
    vkCmdBindVertexBuffers(cmd, 0, 1, &mWireframe.vert.buffer, &offset);
    mWireframe.pipeline.PushPushConstant(cmd, VK_SHADER_STAGE_VERTEX_BIT, sizeof(glm::mat4), &projection);
    mWireframe.pipeline.BindPipeline(cmd);

    for (uint32_t i = 0; i < 2; ++i){
        vkCmdBindIndexBuffer(cmd, mWireframe.index[i].buffer, 0, VK_INDEX_TYPE_UINT16);
        for (uint32_t j = 0; j < 6; ++j){
            dynamicOffset = (CHESSBOARD_ROW_COLUMN_INDEX(i, j, 6)) * mRectMinUniformBufferOffset;
            mWireframe.pipeline.BindDescriptorSet(cmd, mWireframe.set, 1, &dynamicOffset);
            vkCmdDrawIndexed(cmd, 6, 1, 0, 0, 0);
        }
    }
}
void Chessboard::DrawChess(VkCommandBuffer cmd, const glm::vec3&nowCountryColor){
    bool bBan = false;
    for (size_t uiCountry = 0; uiCountry < sizeof(mChess) / sizeof(ChessboardChess); ++uiCountry){
        if(mChess[uiCountry].chess[JIANG_CHESS]){
            bBan = nowCountryColor != mChess[uiCountry].chess[JIANG_CHESS]->GetCountryColor();
            if(uiCountry == HAN_CHESS_INDEX)bBan = false;
            for (uint32_t uiChess = 0; uiChess < sizeof(mChess[uiCountry].chess) / sizeof(Chess *); ++uiChess){
                if(mChess[uiCountry].chess[uiChess])mChess[uiCountry].chess[uiChess]->RecordCommand(cmd, mWindowWidth, bBan);
            }
            // mChess[i].jiang->RecordCommand(cmd, windowWidth);
            // for (size_t j = 0; j < sizeof(mChess->bing) / sizeof(Chess *); ++j){
            //     if(mChess[i].bing[j])mChess[i].bing[j]->RecordCommand(cmd, windowWidth);
            // }
            // for (size_t j = 0; j < sizeof(mChess->ma) / sizeof(Chess *); ++j){
            //     if(mChess[i].ma[j])mChess[i].ma[j]->RecordCommand(cmd, windowWidth);
            // }
            // for (size_t j = 0; j < sizeof(mChess->che) / sizeof(Chess *); ++j){
            //     if(mChess[i].che[j])mChess[i].che[j]->RecordCommand(cmd, windowWidth);
            // }
            // for (size_t j = 0; j < sizeof(mChess->pao) / sizeof(Chess *); ++j){
            //     if(mChess[i].pao[j])mChess[i].pao[j]->RecordCommand(cmd, windowWidth);
            // }
            // for (size_t j = 0; j < sizeof(mChess->shi) / sizeof(Chess *); ++j){
            //     if(mChess[i].shi[j])mChess[i].shi[j]->RecordCommand(cmd, windowWidth);
            // }
            // for (size_t j = 0; j < sizeof(mChess->xiang) / sizeof(Chess *); ++j){
            //     if(mChess[i].xiang[j])mChess[i].xiang[j]->RecordCommand(cmd, windowWidth);
            // }
        }
    }
}
void Chessboard::DrawSelectChessWireframe(VkCommandBuffer cmd){
    //显示“选中棋子后出现的线框"
    VkDeviceSize offset = 0;
    uint32_t dynamicOffset = 0;
    mWireframe.pipeline.BindPipeline(cmd);
    const glm::mat4 projection = glm::ortho(0.0f, (float)mWindowWidth, 0.0f, (float)mWindowWidth, -1.0f, 1.0f);
    mWireframe.pipeline.PushPushConstant(cmd, VK_SHADER_STAGE_VERTEX_BIT, sizeof(glm::mat4), &projection);

    vkCmdBindVertexBuffers(cmd, 0, 1, &mWireframe.vert.buffer, &offset);
    vkCmdBindIndexBuffer(cmd, mWireframe.index[0].buffer, 0, VK_INDEX_TYPE_UINT16);
    for (uint32_t i = 0; i < SELECT_CHESS_UNIFORM_MAX; ++i){
        dynamicOffset = i * mRectMinUniformBufferOffset;
        mWireframe.pipeline.BindDescriptorSet(cmd, mSelectChess.set, 1, &dynamicOffset);
        vkCmdDrawIndexed(cmd, 6, 1, 0, 0, 0);
    }
}
void Chessboard::DrawChessboardBackground(VkCommandBuffer cmd){
    VkDeviceSize offset = 0;
    uint32_t dynamicOffset = 0;
    const glm::mat4 projection = glm::ortho(0.0f, (float)mWindowWidth, 0.0f, (float)mWindowWidth, -1.0f, 1.0f);
    mBackground.pipeline.BindPipeline(cmd);
    mBackground.pipeline.PushPushConstant(cmd, VK_SHADER_STAGE_VERTEX_BIT, sizeof(glm::mat4), &projection);
    mBackground.pipeline.BindDescriptorSet(cmd, mBackground.set, 1, &dynamicOffset);
    vkCmdBindVertexBuffers(cmd, 0, 1, &mBackground.vert[0].buffer, &offset);
    vkCmdBindIndexBuffer(cmd, mWireframe.index[0].buffer, 0, VK_INDEX_TYPE_UINT16);
    vkCmdDrawIndexed(cmd, 6, 1, 0, 0, 0);
    dynamicOffset = mRectMinUniformBufferOffset;
    mBackground.pipeline.BindDescriptorSet(cmd, mBackground.set, 1, &dynamicOffset);
    vkCmdBindVertexBuffers(cmd, 0, 1, &mBackground.vert[1].buffer, &offset);
    vkCmdDrawIndexed(cmd, 6, 1, 0, 0, 0);
}
void Chessboard::GetRectVertices(const glm::vec3&color, ChessboardVertex vertices[]){
    const ChessboardRectVertex v[] = {
        ChessboardRectVertex(glm::vec3(.0f, 1.0f, .0f), color),//左下
        ChessboardRectVertex(glm::vec3(1.0f, .0f, .0f), color),//右上
        ChessboardRectVertex(glm::vec3(.0f, .0f, .0f), color), //左上

        ChessboardRectVertex(glm::vec3(1.0f, 1.0f, .0f), color)//右下
    };
    memcpy(vertices, v, sizeof(v));
}
void Chessboard::InitChess(VkDevice device, uint32_t country){
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
    mChess[country].chess[JIANG_CHESS]->UpdateUniform(device, rc_jiang[country].y, rc_jiang[country].x);
    if(country != HAN_CHESS_INDEX){
        //因为宏以及结构体chess是数组的原因,这里只能写死
        for (size_t j = 0; j < 5; ++j){
            mChess[country].chess[BING_CHESS_1 + j]->UpdateUniform(device, rc_bing[country][j].y, rc_bing[country][j].x);
        }
        for (size_t j = 0; j < 2; ++j){
            mChess[country].chess[MA_CHESS_1 + j]->UpdateUniform(device, rc_ma[country][j].y, rc_ma[country][j].x);

            mChess[country].chess[SHI_CHESS_1 + j]->UpdateUniform(device, rc_shi[country][j].y, rc_shi[country][j].x);

            mChess[country].chess[CHE_CHESS_1 + j]->UpdateUniform(device, rc_che[country][j].y, rc_che[country][j].x);

            mChess[country].chess[PAO_CHESS_1 + j]->UpdateUniform(device, rc_pao[country][j].y, rc_pao[country][j].x);

            mChess[country].chess[XIANG_CHESS_1 + j]->UpdateUniform(device, rc_xiang[country][j].y, rc_xiang[country][j].x);
        }
        //等到漢被消灭，这些棋子作为奖励出现//目前漢只有3个車1个炮，所以直接写死
        mChess[country].chess[PAO_CHESS_3]->UpdateUniform(device, 20, 20);

        for (size_t i = 0; i < 3; ++i){
            mChess[country].chess[CHE_CHESS_3 + i]->UpdateUniform(device, 20, 20);
        }
    }
    else{
        mChess[country].chess[PAO_CHESS_1]->UpdateUniform(device, rc_pao[country][0].y, rc_pao[country][0].x);
        for (size_t j = 0; j < 3; ++j){
            mChess[country].chess[CHE_CHESS_1 + j]->UpdateUniform(device, rc_che[country][j].y, rc_che[country][j].x);
        }
    }
}
void Chessboard::InitChessForVulkan(VkDevice device, uint32_t country, uint32_t windowWidth, VkRenderPass renderpass, VulkanPool pool, VkQueue graphics, VkSampler fontSampler){
    const glm::vec3 color[] = { WEI_CHESS_COUNTRY_COLOR, SHU_CHESS_COUNTRY_COLOR, WU_CHESS_COUNTRY_COLOR, HAN_CHESS_COUNTRY_COLOR };
    // const glm::vec3 color[] = { glm::vec3(0, 0, 1), glm::vec3(1, 0, 0), glm::vec3(0, 1, 0), glm::vec3(1, 1, 0) };
    if(country == WU_CHESS_INDEX){
        mChess[country].chess[JIANG_CHESS] = new Wu;
    }
    else if(country == WEI_CHESS_INDEX){
        mChess[country].chess[JIANG_CHESS] = new Wei;
    }
    else if(country == SHU_CHESS_INDEX){
        mChess[country].chess[JIANG_CHESS] = new Shu;
    }
    else if(country == HAN_CHESS_INDEX){
        mChess[country].chess[JIANG_CHESS] = new Han;
    }
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
    mChess[country].chess[JIANG_CHESS]->CreatePipeline(device, renderpass, windowWidth);
    mChess[country].chess[JIANG_CHESS]->CreateFontTexture(device, fontBuffer, pool.commandPool, graphics);
    mChess[country].chess[JIANG_CHESS]->CreateVulkanResource(device, color[country], pool, graphics);
    mChess[country].chess[JIANG_CHESS]->UpdateSet(device, fontSampler);
    if(country != HAN_CHESS_INDEX){
        //因为宏以及结构体chess是数组的原因,这里只能写死
        for (size_t j = 0; j < 5; ++j){
            mChess[country].chess[BING_CHESS_1 + j] = new Bing;
            mChess[country].chess[BING_CHESS_1 + j]->CreatePipeline(device, renderpass, windowWidth);
            mChess[country].chess[BING_CHESS_1 + j]->CreateFontTexture(device, fontBuffer, pool.commandPool, graphics);
            mChess[country].chess[BING_CHESS_1 + j]->CreateVulkanResource(device, color[country], pool, graphics);
            mChess[country].chess[BING_CHESS_1 + j]->UpdateSet(device, fontSampler);
        }
        for (size_t j = 0; j < 2; ++j){
            mChess[country].chess[MA_CHESS_1 + j] = new Ma;
            mChess[country].chess[MA_CHESS_1 + j]->CreatePipeline(device, renderpass, windowWidth);
            mChess[country].chess[MA_CHESS_1 + j]->CreateFontTexture(device, fontBuffer, pool.commandPool, graphics);
            mChess[country].chess[MA_CHESS_1 + j]->CreateVulkanResource(device, color[country], pool, graphics);
            mChess[country].chess[MA_CHESS_1 + j]->UpdateSet(device, fontSampler);

            mChess[country].chess[SHI_CHESS_1 + j] = new Shi;
            mChess[country].chess[SHI_CHESS_1 + j]->CreatePipeline(device, renderpass, windowWidth);
            mChess[country].chess[SHI_CHESS_1 + j]->CreateFontTexture(device, fontBuffer, pool.commandPool, graphics);
            mChess[country].chess[SHI_CHESS_1 + j]->CreateVulkanResource(device, color[country], pool, graphics);
            mChess[country].chess[SHI_CHESS_1 + j]->UpdateSet(device, fontSampler);

            mChess[country].chess[CHE_CHESS_1 + j] = new Che;
            mChess[country].chess[CHE_CHESS_1 + j]->CreatePipeline(device, renderpass, windowWidth);
            mChess[country].chess[CHE_CHESS_1 + j]->CreateFontTexture(device, fontBuffer, pool.commandPool, graphics);
            mChess[country].chess[CHE_CHESS_1 + j]->CreateVulkanResource(device, color[country], pool, graphics);
            mChess[country].chess[CHE_CHESS_1 + j]->UpdateSet(device, fontSampler);

            mChess[country].chess[PAO_CHESS_1 + j] = new Pao;
            mChess[country].chess[PAO_CHESS_1 + j]->CreatePipeline(device, renderpass, windowWidth);
            mChess[country].chess[PAO_CHESS_1 + j]->CreateFontTexture(device, fontBuffer, pool.commandPool, graphics);
            mChess[country].chess[PAO_CHESS_1 + j]->CreateVulkanResource(device, color[country], pool, graphics);
            mChess[country].chess[PAO_CHESS_1 + j]->UpdateSet(device, fontSampler);

            mChess[country].chess[XIANG_CHESS_1 + j] = new Xiang;
            mChess[country].chess[XIANG_CHESS_1 + j]->CreatePipeline(device, renderpass, windowWidth);
            mChess[country].chess[XIANG_CHESS_1 + j]->CreateFontTexture(device, fontBuffer, pool.commandPool, graphics);
            mChess[country].chess[XIANG_CHESS_1 + j]->CreateVulkanResource(device, color[country], pool, graphics);
            mChess[country].chess[XIANG_CHESS_1 + j]->UpdateSet(device, fontSampler);
        }
        //等到漢被消灭，这些棋子作为奖励出现//目前漢只有3个車1个炮，所以直接写死
        mChess[country].chess[PAO_CHESS_3] = new Pao;
        mChess[country].chess[PAO_CHESS_3]->CreatePipeline(device, renderpass, windowWidth);
        mChess[country].chess[PAO_CHESS_3]->CreateFontTexture(device, fontBuffer, pool.commandPool, graphics);
        mChess[country].chess[PAO_CHESS_3]->CreateVulkanResource(device, color[country], pool, graphics);
        mChess[country].chess[PAO_CHESS_3]->UpdateSet(device, fontSampler);

        for (size_t i = 0; i < 3; ++i){
            mChess[country].chess[CHE_CHESS_3 + i] = new Che;
            mChess[country].chess[CHE_CHESS_3 + i]->CreatePipeline(device, renderpass, windowWidth);
            mChess[country].chess[CHE_CHESS_3 + i]->CreateFontTexture(device, fontBuffer, pool.commandPool, graphics);
            mChess[country].chess[CHE_CHESS_3 + i]->CreateVulkanResource(device, color[country], pool, graphics);
            mChess[country].chess[CHE_CHESS_3 + i]->UpdateSet(device, fontSampler);
        }
    }
    else{
        mChess[country].chess[PAO_CHESS_1] = new Pao;
        mChess[country].chess[PAO_CHESS_1]->CreatePipeline(device, renderpass, windowWidth);
        mChess[country].chess[PAO_CHESS_1]->CreateFontTexture(device, fontBuffer, pool.commandPool, graphics);
        mChess[country].chess[PAO_CHESS_1]->CreateVulkanResource(device, color[country], pool, graphics);
        mChess[country].chess[PAO_CHESS_1]->UpdateSet(device, fontSampler);
        for (size_t j = 0; j < 3; ++j){
            mChess[country].chess[CHE_CHESS_1 + j] = new Che;
            mChess[country].chess[CHE_CHESS_1 + j]->CreatePipeline(device, renderpass, windowWidth);
            mChess[country].chess[CHE_CHESS_1 + j]->CreateFontTexture(device, fontBuffer, pool.commandPool, graphics);
            mChess[country].chess[CHE_CHESS_1 + j]->CreateVulkanResource(device, color[country], pool, graphics);
            mChess[country].chess[CHE_CHESS_1 + j]->UpdateSet(device, fontSampler);
        }
    }
    fclose(fontFile);
    free(fontBuffer);
    for (uint32_t uiChess = 0; uiChess < sizeof(mChess[country].chess) / sizeof(Chess*); ++uiChess) {
        if(mChess[country].chess[uiChess])mChess[country].chess[uiChess]->UpdateUniform(device, 20, 20);
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
    mGrid.vert.Destroy(device);
    // mPointIndex.Destroy(device);
    mGrid.uniform.Destroy(device);
    mWireframe.vert.Destroy(device);
    mBackground.pos.Destroy(device);
    mWireframe.uniform.Destroy(device);
    mSelectChess.uniform.Destroy(device);
    for (size_t i = 0; i < sizeof(mBackground.vert) / sizeof(VulkanBuffer); ++i){
        mBackground.vert[i].Destroy(device);
    }
    for (size_t i = 0; i < sizeof(mWireframe.index) / sizeof(VulkanBuffer); ++i){
        mWireframe.index[i].Destroy(device);
    }
    for (size_t i = 0; i < sizeof(mChess) / sizeof(ChessboardChess); ++i){
        DestroyChess(device, i);
    }
    DestroyGraphicsPipeline(device);
}
void Chessboard::UpdateSet(VkDevice device){
    mGrid.pipeline.UpdateDescriptorSets(device, { mGrid.uniform }, {}, mGrid.set);
    mBackground.pipeline.UpdateDescriptorSets(device, { mBackground.pos }, {}, mBackground.set);
    mWireframe.pipeline.UpdateDescriptorSets(device, { mWireframe.uniform }, {}, mWireframe.set);
    mWireframe.pipeline.UpdateDescriptorSets(device, { mSelectChess.uniform }, {}, mSelectChess.set);
}
void Chessboard::DestroyGraphicsPipeline(VkDevice device){
    std::vector<uint32_t>cacheData;
    mGrid.pipeline.DestroyCache(device, cacheData);
    vkf::tool::WriteFileContent("ChessboardPipelineCache", cacheData.data(), cacheData.size() * sizeof(uint32_t)); 
    mGrid.pipeline.DestroySetLayout(device);
    mGrid.pipeline.DestroyLayout(device);
    mGrid.pipeline.DestroyPipeline(device);

    mWireframe.pipeline.DestroyCache(device, cacheData);
    mWireframe.pipeline.DestroySetLayout(device);
    mWireframe.pipeline.DestroyLayout(device);
    mWireframe.pipeline.DestroyPipeline(device);

    mBackground.pipeline.DestroyCache(device, cacheData);
    mBackground.pipeline.DestroySetLayout(device);
    mBackground.pipeline.DestroyLayout(device);
    mBackground.pipeline.DestroyPipeline(device);
}
void Chessboard::DestroyChess(VkDevice device, uint32_t country){
    for (uint32_t i = 0; i < sizeof(mChess[country].chess) / sizeof(Chess *); ++i){
        if(mChess[country].chess[i]){
            mChess[country].chess[i]->Cleanup(device);
            mChess[country].chess[i] = nullptr;
        }
    }
}
void Chessboard::RecordCommand(VkCommandBuffer cmd, const glm::vec3&nowCountryColor){
    DrawChessboardBackground(cmd);
    DrawChessboard(cmd);
    DrawChess(cmd, nowCountryColor);

    DrawSelectChessWireframe(cmd);
}
void Chessboard::CreatePipeline(VkDevice device, VkRenderPass renderpass, uint32_t windowWidth){
    GraphicsPipelineStateInfo state;
    state.mRasterization.lineWidth = CHESSBOARD_LINE_WIDTH;
    state.mInputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
    mGrid.pipeline.SetVertexInputBindingDescription(sizeof(ChessboardVertex));
    mGrid.pipeline.PushPushConstant(sizeof(glm::mat4), VK_SHADER_STAGE_VERTEX_BIT);
    mGrid.pipeline.PushVertexInputAttributeDescription(0, 0, VK_FORMAT_R32G32B32_SFLOAT);
    mGrid.pipeline.PushVertexInputAttributeDescription(1, 3 * sizeof(float), VK_FORMAT_R32G32B32_SFLOAT);
    mGrid.pipeline.PushVertexInputAttributeDescription(2, 6 * sizeof(float), VK_FORMAT_R32_UINT);
    CreateGraphicsPipeline(device, windowWidth, renderpass, { "shaders/basePointVert.spv", "shaders/baseFrag.spv" }, mGrid.pipeline, state);
    state.mRasterization.polygonMode = VK_POLYGON_MODE_LINE;
    state.mInputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    mWireframe.pipeline.SetVertexInputBindingDescription(sizeof(ChessboardRectVertex));
    mWireframe.pipeline.PushPushConstant(sizeof(glm::mat4), VK_SHADER_STAGE_VERTEX_BIT);
    mWireframe.pipeline.PushVertexInputAttributeDescription(0, 0, VK_FORMAT_R32G32B32_SFLOAT);
    mWireframe.pipeline.PushVertexInputAttributeDescription(1, 3 * sizeof(float), VK_FORMAT_R32G32B32_SFLOAT);
    CreateGraphicsPipeline(device, windowWidth, renderpass, { "shaders/baseVert.spv", "shaders/baseFrag.spv" }, mWireframe.pipeline, state);
    // if(mPointSizeRange < CHESSBOARD_BLACK_BACKGROUND_SIZE(windowWidth)){
    state.mRasterization.polygonMode = VK_POLYGON_MODE_FILL;
    mBackground.pipeline.SetVertexInputBindingDescription(sizeof(ChessboardRectVertex));
    mBackground.pipeline.PushPushConstant(sizeof(glm::mat4), VK_SHADER_STAGE_VERTEX_BIT);
    mBackground.pipeline.PushVertexInputAttributeDescription(0, 0, VK_FORMAT_R32G32B32_SFLOAT);
    mBackground.pipeline.PushVertexInputAttributeDescription(1, 3 * sizeof(float), VK_FORMAT_R32G32B32_SFLOAT);
    CreateGraphicsPipeline(device, windowWidth, renderpass, { "shaders/baseVert.spv", "shaders/baseFrag.spv" }, mBackground.pipeline, state);
    // }

    // //没有更好的办法前先这么写
    // state.mRasterization.polygonMode = VK_POLYGON_MODE_FILL;
    // CreateGraphicsPipeline(device, windowWidth, renderpass, { "shaders/baseVert.spv", "shaders/baseFrag.spv" }, mCircularPipeline, state);
    // //字体管线需要用到图片
    // mFontPipeline.PushDescriptorSetLayoutBinding(1, VK_SHADER_STAGE_FRAGMENT_BIT, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
    // CreateGraphicsPipeline(device, windowWidth, renderpass, { "shaders/baseFontVert.spv", "shaders/baseFontFrag.spv" }, mFontPipeline, state);
}
void Chessboard::CreateVulkanResource(VkPhysicalDevice physicalDevice, VkDevice device, uint32_t windowWidth, VkQueue graphics, VulkanPool pool, VkRenderPass renderpass, VkSampler fontSampler){
    mWindowWidth = windowWidth;
    VkPhysicalDeviceProperties physicalDeviceProperties;
    vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);
    // mPointSizeRange = physicalDeviceProperties.limits.pointSizeRange[1];
    mRectMinUniformBufferOffset = ALIGN(sizeof(glm::mat4), physicalDeviceProperties.limits.minUniformBufferOffsetAlignment);
    mPointMinUniformBufferOffset = ALIGN(sizeof(glm::mat4), physicalDeviceProperties.limits.minUniformBufferOffsetAlignment);
    ChessboardVertex vertices[4];
    const ChessboardVertex pointVertices[] = {
        ChessboardVertex(glm::vec3(.0f), glm::vec3(1, .9, .5), CHESSBOARD_GRID_SIZE),
        // ChessboardVertex(glm::vec3(.0f), glm::vec3(50, 215, 90.0f)),
        ChessboardVertex(glm::vec3(.0f), glm::vec3(1, .9, .5), CHESSBOARD_GRID_SIZE_BIG)
    };
    // const uint16_t pointIndices[] = { 0 };
    const uint16_t indices[] = { 0, 1, 2, 0, 3, 1 };
    const uint16_t mirrorIndices[] = { 0, 2, 3, 2, 3, 1 };
    GetRectVertices(glm::vec3(0), vertices);
    // mPointIndex.CreateBuffer(device, sizeof(pointIndices), VK_BUFFER_USAGE_INDEX_BUFFER_BIT|VK_BUFFER_USAGE_TRANSFER_DST_BIT);
    mGrid.vert.CreateBuffer(device, sizeof(pointVertices), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT|VK_BUFFER_USAGE_TRANSFER_DST_BIT);
    mWireframe.vert.CreateBuffer(device, sizeof(vertices), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT|VK_BUFFER_USAGE_TRANSFER_DST_BIT);
    mWireframe.index[0].CreateBuffer(device, sizeof(indices), VK_BUFFER_USAGE_INDEX_BUFFER_BIT|VK_BUFFER_USAGE_TRANSFER_DST_BIT);
    mWireframe.index[1].CreateBuffer(device, sizeof(mirrorIndices), VK_BUFFER_USAGE_INDEX_BUFFER_BIT|VK_BUFFER_USAGE_TRANSFER_DST_BIT);
    mGrid.vert.AllocateAndBindMemory(device, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    // mPointIndex.AllocateAndBindMemory(device, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    mWireframe.vert.AllocateAndBindMemory(device, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    for (size_t i = 0; i < sizeof(mWireframe.index) / sizeof(VulkanBuffer); i++){
        mWireframe.index[i].AllocateAndBindMemory(device, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    }
    vkf::tool::CopyBuffer(device, sizeof(vertices), vertices, graphics, pool.commandPool, mWireframe.vert);
    vkf::tool::CopyBuffer(device, sizeof(indices), indices, graphics, pool.commandPool, mWireframe.index[0]);
    // vkf::tool::CopyBuffer(device, sizeof(pointIndices), pointIndices, graphics, pool.commandPool, mPointIndex);
    vkf::tool::CopyBuffer(device, sizeof(mirrorIndices), mirrorIndices, graphics, pool.commandPool, mWireframe.index[1]);
    vkf::tool::CopyBuffer(device, sizeof(pointVertices), pointVertices, graphics, pool.commandPool, mGrid.vert);
    // 中间那部分(8 * 16) + 汉和吴(8 * 8) + 4个大一些的矩形
    //8 * 16 + 8 * 8 + 4
    mBackground.pos.CreateBuffer(device, mRectMinUniformBufferOffset * 2, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
    mWireframe.uniform.CreateBuffer(device, mRectMinUniformBufferOffset * 12, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
    //为了显示所有指示能下位置的线框，先申请100个。这个数量是远大于需要的实际数量。以后可能会修改
    mSelectChess.uniform.CreateBuffer(device, mRectMinUniformBufferOffset * SELECT_CHESS_UNIFORM_MAX, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
    mGrid.uniform.CreateBuffer(device, mPointMinUniformBufferOffset * (8 * 16 + 68), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
    mGrid.uniform.AllocateAndBindMemory(device, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT|VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    mBackground.pos.AllocateAndBindMemory(device, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT|VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    mWireframe.uniform.AllocateAndBindMemory(device, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT|VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    mSelectChess.uniform.AllocateAndBindMemory(device, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT|VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    mGrid.uniform.size = mPointMinUniformBufferOffset;
    mSelectChess.uniform.size = mRectMinUniformBufferOffset;
    mBackground.pos.size = mWireframe.uniform.size = mRectMinUniformBufferOffset;

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
    glm::mat4 model[196] = { glm::scale(glm::mat4(1.0f), glm::vec3(windowWidth, windowWidth, 1)) };
    mBackground.pos.UpdateData(device, &model[0]);
    model[0] = glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(CHESSBOARD_GRID_SIZE, CHESSBOARD_GRID_SIZE, 0)), glm::vec3(CHESSBOARD_BLACK_BACKGROUND_SIZE(windowWidth), CHESSBOARD_BLACK_BACKGROUND_SIZE(windowWidth), 1));
    mBackground.pos.UpdateData(device, &model, mRectMinUniformBufferOffset);
    //4个大网格的大小
    const uint32_t gridBigPointOffset = CHESSBOARD_GRID_SIZE_BIG * .5 + CHESSBOARD_LINE_WIDTH;
    const glm::vec3 bigGridPos[] = {
        glm::vec3(gridBigPointOffset + CHESSBOARD_LINE_WIDTH + CHESSBOARD_GRID_SIZE, gridBigPointOffset + CHESSBOARD_LINE_WIDTH + CHESSBOARD_GRID_SIZE, 0),
        glm::vec3(gridBigPointOffset + 9 * CHESSBOARD_GRID_SIZE + 10 * CHESSBOARD_LINE_WIDTH + CHESSBOARD_GRID_SIZE_BIG, bigGridPos[0].y, 0),
        glm::vec3(bigGridPos[0].x, bigGridPos[1].x, 0),
        glm::vec3(bigGridPos[1].x, bigGridPos[2].y, 0)
    };
    for (size_t i = 0; i < 4; ++i){
        model[i] = glm::translate(glm::mat4(1.0f), bigGridPos[i]);
    }
    //魏
    const uint32_t gridPointOffset = CHESSBOARD_GRID_SIZE * .5;
    for (uint32_t uiRow = 0; uiRow < 4; ++uiRow){
        for (uint32_t uiColumn = 0; uiColumn < 8; ++uiColumn){
            model[CHESSBOARD_ROW_COLUMN_INDEX(uiRow, uiColumn, 8) + 4] = glm::translate(glm::mat4(1.0f), glm::vec3(gridPointOffset + COLUMN_TO_X(uiColumn + 4, CHESSBOARD_GRID_SIZE, CHESSBOARD_LINE_WIDTH), gridPointOffset + ROW_TO_Y(uiRow, CHESSBOARD_GRID_SIZE, CHESSBOARD_LINE_WIDTH), 0));
        }
    }
    //汉 吴
    for (uint32_t uiRow = 0; uiRow < 8; ++uiRow){
        for (uint32_t uiColumn = 0; uiColumn < 16; ++uiColumn){
            model[CHESSBOARD_ROW_COLUMN_INDEX(uiRow, uiColumn, 16) + 36] = glm::translate(glm::mat4(1.0f), glm::vec3(gridPointOffset + COLUMN_TO_X(uiColumn, CHESSBOARD_GRID_SIZE, CHESSBOARD_LINE_WIDTH), gridPointOffset + ROW_TO_Y(uiRow + 4, CHESSBOARD_GRID_SIZE, CHESSBOARD_LINE_WIDTH), 0));
        }
    }
    for (uint32_t uiRow = 0; uiRow < 4; ++uiRow){
        for (uint32_t uiColumn = 0; uiColumn < 8; ++uiColumn){
            model[CHESSBOARD_ROW_COLUMN_INDEX(uiRow, uiColumn, 8) + 164] = glm::translate(glm::mat4(1.0f), glm::vec3(gridPointOffset + COLUMN_TO_X(uiColumn + 4, CHESSBOARD_GRID_SIZE, CHESSBOARD_LINE_WIDTH), gridPointOffset + ROW_TO_Y(uiRow + 12, CHESSBOARD_GRID_SIZE, CHESSBOARD_LINE_WIDTH), 0));
        }
    }
    mGrid.uniform.UpdateData(device, sizeof(model), model);
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
    mWireframe.uniform.UpdateData(device, sizeof(pos), &pos);

    mGrid.pipeline.AllocateDescriptorSets(device, pool.descriptorPool, 1, &mGrid.set);
    mWireframe.pipeline.AllocateDescriptorSets(device, pool.descriptorPool, 1, &mWireframe.set);
    mWireframe.pipeline.AllocateDescriptorSets(device, pool.descriptorPool, 1, &mSelectChess.set);
    mBackground.pipeline.AllocateDescriptorSets(device, pool.descriptorPool, 1, &mBackground.set);

    for (size_t i = 0; i < sizeof(mChess) / sizeof(ChessboardChess); i++){
        InitChessForVulkan(device, i, windowWidth, renderpass, pool, graphics, fontSampler);
    }
}
void Chessboard::InitChessboard(VkDevice device){
    mPalacePos[WEI_CHESS_INDEX].row = 1;
    mPalacePos[WEI_CHESS_INDEX].column = 8;

    mPalacePos[SHU_CHESS_INDEX].row = 15;
    mPalacePos[SHU_CHESS_INDEX].column = mPalacePos[WEI_CHESS_INDEX].column;

    mPalacePos[WU_CHESS_INDEX].row = mPalacePos[WEI_CHESS_INDEX].column;
    mPalacePos[WU_CHESS_INDEX].column = mPalacePos[SHU_CHESS_INDEX].row;
    for (size_t i = 0; i < sizeof(mChess) / sizeof(ChessboardChess); i++){
        InitChess(device, i);
    }
}
bool Chessboard::Play(VkDevice device, const glm::vec2&pos, uint32_t&country, bool&next){
    // bool bUpdateScreen = false;
    bool bMate = false;
    next = false;
    for (size_t i = 0; i < mCanplays.size(); ++i){
        const uint32_t chessY = ROW_TO_Y(mCanplays[i].row, CHESSBOARD_GRID_SIZE, CHESSBOARD_LINE_WIDTH) - CHESSBOARD_GRID_SIZE * .5;
        const uint32_t chessX = COLUMN_TO_X(mCanplays[i].column, CHESSBOARD_GRID_SIZE, CHESSBOARD_LINE_WIDTH) - CHESSBOARD_GRID_SIZE * .5;
        if(pos.x >  chessX && pos.x < chessX + CHESSBOARD_GRID_SIZE && pos.y > chessY && pos.y < chessY + CHESSBOARD_GRID_SIZE){
            Chess *chess = GetChess(mCanplays[i].row, mCanplays[i].column);
            if(chess){
                for (uint32_t uiCountry = 0; uiCountry < sizeof(mChess) / sizeof(ChessboardChess); ++uiCountry){
                    for (uint32_t uiChess = 0; uiChess < sizeof(mChess[i].chess) / sizeof(Chess *); ++uiChess){
                        //确认是否是这棋子
                        if(mChess[uiCountry].chess[uiChess] && mChess[uiCountry].chess[uiChess]->isSelect(mCanplays[i].row, mCanplays[i].column)){
                            // bUpdateScreen = true;
                            country = uiCountry;
                            bMate = CaptureChess(device, mSelected, uiCountry, uiChess);
                            break;
                        }
                    }
                }
            }
            // if(!mSelected->isSelect(mCanplays[i].row, mCanplays[i].column)){
            //     bUpdateScreen = true;
            // }
            mSelected->UpdateUniform(device, mCanplays[i].row, mCanplays[i].column);
            next = true;
            break;
        }
    }
    //需要更新选中棋子线框坐标
    //mSelected赋值为nullptr,为了不影响后面的操作
    mCanplays.clear();
    mSelectChess.UpdateUniform(device, mCanplays);
    mSelected = nullptr;
    // return bUpdateScreen;
    return bMate;
}
bool Chessboard::CaptureChess(VkDevice device, const Chess *src, uint32_t dstCountry, uint32_t dstChess){
    bool bMate = false;
    if(mChess[dstCountry].chess[dstChess] == mChess[dstCountry].chess[JIANG_CHESS]){
        bMate = true;
        if(mChess[dstCountry].chess[JIANG_CHESS]->GetCountryColor() == HAN_CHESS_COUNTRY_COLOR){
            //棋子归属src所选国家
            uint32_t srcCountry = WEI_CHESS_INDEX;
            if(src->GetCountryColor() == SHU_CHESS_COUNTRY_COLOR){
                srcCountry = SHU_CHESS_INDEX;
            }
            else if(src->GetCountryColor() == WU_CHESS_COUNTRY_COLOR){
                srcCountry = WU_CHESS_INDEX;
            }
            uint32_t row, column;
            if(mChess[dstCountry].chess[PAO_CHESS_1]){
                mChess[dstCountry].chess[PAO_CHESS_1]->GetChessRanks(row, column);
                mChess[srcCountry].chess[PAO_CHESS_3]->UpdateUniform(device, row, column);
            }
            if(mChess[dstCountry].chess[CHE_CHESS_1]){
                mChess[dstCountry].chess[CHE_CHESS_1]->GetChessRanks(row, column);
                mChess[srcCountry].chess[CHE_CHESS_3]->UpdateUniform(device, row, column);
            }
            if(mChess[dstCountry].chess[CHE_CHESS_2]){
                mChess[dstCountry].chess[CHE_CHESS_2]->GetChessRanks(row, column);
                mChess[srcCountry].chess[CHE_CHESS_4]->UpdateUniform(device, row, column);
            }
            if(mChess[dstCountry].chess[CHE_CHESS_3]){
                mChess[dstCountry].chess[CHE_CHESS_3]->GetChessRanks(row, column);
                mChess[srcCountry].chess[CHE_CHESS_5]->UpdateUniform(device, row, column);
            }
        }
    }
    mChess[dstCountry].chess[dstChess]->UpdateUniform(device, 20, 20);
    // vkDeviceWaitIdle(device);
    // mChess[dstCountry].chess[dstChess]->Cleanup(device);
    // delete mChess[dstCountry].chess[dstChess];
    // mChess[dstCountry].chess[dstChess] = nullptr;
    return bMate;
}
bool Chessboard::Play(VkDevice device, uint32_t row, uint32_t column, uint32_t&country, bool&next){
    return Play(device, glm::vec2(COLUMN_TO_X(column, CHESSBOARD_GRID_SIZE, CHESSBOARD_LINE_WIDTH), ROW_TO_Y(row, CHESSBOARD_GRID_SIZE, CHESSBOARD_LINE_WIDTH)), country, next);
}
bool Chessboard::IsBoundary(uint32_t row, uint32_t column)const{
    if(row > 16 || column > 16){
        // printf("到达边界, row:%u, column:%u\n", row, column);
        return true;
    }
    else if ((row < 4 || row > 12) && (row < 0 || column < 4 || column > 12)){
        // printf("到达边界, row:%u, column:%u\n", row, column);
        return true;
    }
    return false;
}
Chess *Chessboard::GetChess(const glm::vec2&pos)const{
    Chess *chess = nullptr;    
    for (size_t i = 0; i < sizeof(mChess) / sizeof(ChessboardChess); ++i){
        for (uint32_t j = 0; j < sizeof(mChess[i].chess) / sizeof(Chess *); ++j){
            if(mChess[i].chess[j] && mChess[i].chess[j]->isSelect(pos)){
                chess = mChess[i].chess[j];
                break;
            }
        }
    }
    return chess;
}
Chess *Chessboard::GetChess(uint32_t row, uint32_t column) const{
    const uint32_t ypos = ROW_TO_Y(row, CHESSBOARD_GRID_SIZE, CHESSBOARD_LINE_WIDTH), xpos = COLUMN_TO_X(column, CHESSBOARD_GRID_SIZE, CHESSBOARD_LINE_WIDTH);
    return GetChess(glm::vec2(xpos, ypos));
}
void Chessboard::Select(VkDevice device, const glm::vec2 &pos){
    mCanplays.clear();
    for (size_t i = 0; i < sizeof(mChess) / sizeof(ChessboardChess); ++i){
        for (uint32_t j = 0; j < sizeof(mChess[i].chess) / sizeof(Chess *); ++j){
            if(mChess[i].chess[j] && mChess[i].chess[j]->isSelect(pos)){
                mSelected = mChess[i].chess[j];
                mChess[i].chess[j]->Selected(this, mCanplays);
                break;
            }
        }
    }
    mSelectChess.UpdateUniform(device, mCanplays);
}
void Chessboard::UnSelect(VkDevice device){
    mCanplays.clear();
    //不再显示“选中棋子后出现的线框”
    mSelectChess.UpdateUniform(device, mCanplays);
    //mSelected赋值为nullptr,为了不影响后面的操作
    mSelected = nullptr;
}
bool Chessboard::IsInPalace(uint32_t row, uint32_t column)const{
    bool bInIsPalace = false;
    if(IsPalaceCenter(row, column))return true;
    for (size_t i = 0; i < sizeof(mPalacePos) / sizeof(Ranks); ++i){
        if(abs((float)row - mPalacePos[i].row) < 2 && abs((float)column - mPalacePos[i].column) < 2){
            bInIsPalace = true;
            break;
        }
    }
    return bInIsPalace;
}
bool Chessboard::IsPalaceCenter(uint32_t row, uint32_t column)const{
    bool bIsPalaceCenter = false;
    for (size_t i = 0; i < sizeof(mPalacePos) / sizeof(Ranks); ++i){
        if(row == mPalacePos[i].row && column == mPalacePos[i].column){
            bIsPalaceCenter = true;
            break;
        }   
    }
    return bIsPalaceCenter;
}
glm::vec2 Chessboard::GetCountryBehind(const glm::vec3&countryColor)const{
    glm::vec2 behind = glm::vec2(0);
    if(countryColor == WU_CHESS_COUNTRY_COLOR){
        behind.x = 1;
    }
    else if(countryColor == WEI_CHESS_COUNTRY_COLOR){
        behind.y = -1;
    }
    else if(countryColor == SHU_CHESS_COUNTRY_COLOR){
        behind.y = 1;
    }
    else{
        printf("无效的国家颜色颜色:%f, %f, %f\n", countryColor.x, countryColor.y, countryColor.z);
    }
    return behind;
}
void Chessboard::SelectChessInPalace(const Chess *ptrChess, std::vector<Ranks>&canplays)const{
    //只需要管斜线，其他位置归棋子子类管
    Chess *chess = nullptr;
    for (size_t i = 0; i < sizeof(mPalacePos) / sizeof(Ranks); ++i){
        if(ptrChess->isSelect(mPalacePos[i].row, mPalacePos[i].column)){
            chess = GetChess(mPalacePos[i].row - 1, mPalacePos[i].column - 1);
            if(!chess || chess->GetCountryColor() != ptrChess->GetCountryColor())canplays.push_back(Ranks(mPalacePos[i].row - 1, mPalacePos[i].column - 1));
            chess = GetChess(mPalacePos[i].row + 1, mPalacePos[i].column - 1);
            if(!chess || chess->GetCountryColor() != ptrChess->GetCountryColor())canplays.push_back(Ranks(mPalacePos[i].row + 1, mPalacePos[i].column - 1));
            chess = GetChess(mPalacePos[i].row + 1, mPalacePos[i].column + 1);
            if(!chess || chess->GetCountryColor() != ptrChess->GetCountryColor())canplays.push_back(Ranks(mPalacePos[i].row + 1, mPalacePos[i].column + 1));
            chess = GetChess(mPalacePos[i].row - 1, mPalacePos[i].column + 1);
            if(!chess || chess->GetCountryColor() != ptrChess->GetCountryColor())canplays.push_back(Ranks(mPalacePos[i].row - 1, mPalacePos[i].column + 1));
            break;
        }
        else{
            chess = GetChess(mPalacePos[i].row, mPalacePos[i].column);
            if(!chess || chess->GetCountryColor() != ptrChess->GetCountryColor()){
                if(ptrChess->isSelect(mPalacePos[i].row + 1, mPalacePos[i].column + 1) || ptrChess->isSelect(mPalacePos[i].row + 1, mPalacePos[i].column - 1) || ptrChess->isSelect(mPalacePos[i].row - 1, mPalacePos[i].column + 1) || ptrChess->isSelect(mPalacePos[i].row - 1, mPalacePos[i].column - 1)){
                    canplays.push_back(Ranks(mPalacePos[i].row, mPalacePos[i].column));
                    break;
                }
            }
        }
    }
    // uint32_t row, column;
    // ptrChess->GetChessRanks(row, column);
    // if(IsPalaceCenter(row, column)){

    // }
    // else{
        
    // }
}