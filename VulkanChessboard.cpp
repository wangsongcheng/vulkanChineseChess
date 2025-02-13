#include <string.h>
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
    glm::mat4 model[CHESSBOARD_ROW * CHESSBOARD_COLUMN];
    glm::vec2 gridPos[CHESSBOARD_ROW][CHESSBOARD_COLUMN];
    const uint32_t gridBigPointOffset = CHESSBOARD_RECT_SIZE;
    const glm::mat4 scale = glm::scale(glm::mat4(1), glm::vec3(CHESSBOARD_RECT_SIZE, CHESSBOARD_RECT_SIZE, 1));
    for (size_t uiRow = 0; uiRow < CHESSBOARD_ROW; ++uiRow){
        for (size_t uiColumn = 0; uiColumn < CHESSBOARD_COLUMN; ++uiColumn){
            gridPos[uiRow][uiColumn] = glm::vec2(uiRow * CHESSBOARD_RECT_SIZE + (uiRow + 1) * CHESSBOARD_LINE_WIDTH + gridBigPointOffset, uiColumn * CHESSBOARD_RECT_SIZE + (uiColumn + 1) * CHESSBOARD_LINE_WIDTH + gridBigPointOffset);
            model[ROW_COLUMN_TO_INDEX(uiRow, uiColumn, CHESSBOARD_COLUMN)] = glm::translate(glm::mat4(1), glm::vec3(gridPos[uiRow][uiColumn], 0)) * scale;
        }
    }
    uniforms.grid.UpdateData(device, sizeof(model), model);
}
void VulkanChessboard::UpdateBigGridUniform(VkDevice device){
    glm::mat4 model[CHESSBOARD_BIG_GRID_COUNT];
    const glm::mat4 scale = glm::scale(glm::mat4(1), glm::vec3(CHESSBOARD_BIG_RECT_SIZE, CHESSBOARD_BIG_RECT_SIZE, 1));
    const glm::vec3 bigGridPos[] = {
        glm::vec3(CHESSBOARD_LINE_WIDTH + CHESSBOARD_RECT_SIZE, CHESSBOARD_LINE_WIDTH + CHESSBOARD_RECT_SIZE, 0),
        glm::vec3(9 * CHESSBOARD_RECT_SIZE + 10 * CHESSBOARD_LINE_WIDTH + CHESSBOARD_BIG_RECT_SIZE, bigGridPos[0].y, 0),
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
    const glm::vec2 wu_jiugongge[] = {
        glm::vec2(15, 7), glm::vec2(wu_jiugongge[0].x - 1, wu_jiugongge[0].y + 1),
        glm::vec2(wu_jiugongge[1].x, wu_jiugongge[0].y), glm::vec2(wu_jiugongge[0].x, wu_jiugongge[1].y)
    };
    const glm::vec2 wei_jiugongge[] = {
        glm::vec2(8, 0), glm::vec2(wei_jiugongge[0].x - 1, wei_jiugongge[0].y + 1),
        glm::vec2(wei_jiugongge[1].x, wei_jiugongge[0].y), glm::vec2(wei_jiugongge[0].x, wei_jiugongge[1].y)
    };
    const glm::vec2 shu_jiugongge[] = {
        glm::vec2(8, 14), glm::vec2(shu_jiugongge[0].x - 1, shu_jiugongge[0].y + 1),
        glm::vec2(shu_jiugongge[1].x, shu_jiugongge[0].y), glm::vec2(shu_jiugongge[0].x, shu_jiugongge[1].y)
    };
    const glm::vec2 han_jiugongge[] = {
        glm::vec2(1, 7), glm::vec2(han_jiugongge[0].x - 1, han_jiugongge[0].y + 1),
        glm::vec2(han_jiugongge[1].x, han_jiugongge[0].y), glm::vec2(han_jiugongge[0].x, han_jiugongge[1].y)
    };
    const glm::vec2 jiugonggePos[] = {
        //线框[0]
        wei_jiugongge[0], wei_jiugongge[1], shu_jiugongge[0], shu_jiugongge[1], wu_jiugongge[0], wu_jiugongge[1], han_jiugongge[0], han_jiugongge[1],
        //线框[1]
        wei_jiugongge[2], wei_jiugongge[3], shu_jiugongge[2], shu_jiugongge[3], wu_jiugongge[2], wu_jiugongge[3], han_jiugongge[2], han_jiugongge[3]
    };
    glm::mat4 model[CHESSBOARD_WIREFRAME_COUNT];
    for (size_t i = 0; i < CHESSBOARD_WIREFRAME_COUNT; ++i){
        model[i] = glm::translate(glm::mat4(1), glm::vec3(COLUMN_TO_X(jiugonggePos[i].x), ROW_TO_Y(jiugonggePos[i].y), 0)) * gridScale;
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
void VulkanChessboard::CreateRectResource(VulkanDevice device, VulkanPool pool, VkQueue graphics){
    float vertices[2 * 4 * 6];
    const uint16_t indices[] = {
        0, 1, 2, 0, 3, 1,//.../
        0, 2, 3, 2, 3, 1
    };
    GetRectVertices(glm::vec3(1, .8, .2), vertices);
    GetRectVertices(glm::vec3(0), vertices + 4 * 6);
    mRect.indexCount = 6;
    mRect.vertexCount = 4;
    mRect.index.CreateBuffer(device, sizeof(indices), VK_BUFFER_USAGE_INDEX_BUFFER_BIT|VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    //8是4种颜色+亮度较低的颜色(表示不是该玩家下子)
    mRect.vertex.CreateBuffer(device, sizeof(vertices), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT|VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    mRect.index.UpdateData(device, indices, graphics, pool);
    mRect.vertex.UpdateData(device, vertices, graphics, pool);
}
void VulkanChessboard::SetupDescriptorSet(VkDevice device, VkDescriptorSetLayout setLayout, VulkanPool pool){
    pool.AllocateDescriptorSets(device, &setLayout, 1, &descriptorSet.grid);
    pool.AllocateDescriptorSets(device, &setLayout, 1, &descriptorSet.bigGrid);
    pool.AllocateDescriptorSets(device, &setLayout, 1, &descriptorSet.background);
    pool.AllocateDescriptorSets(device, &setLayout, 1, &descriptorSet.wireframe.jiugongge);
    // pool.AllocateDescriptorSets(device, &setLayout, 1, &descriptorSet.wireframe.selectChess);
    
    VkDescriptorSetLayoutBinding descriptorSetLayoutBindings[1] = {};
    // descriptorSetLayoutBindings[0].binding = 0;
    descriptorSetLayoutBindings[0].descriptorCount = 1;
    descriptorSetLayoutBindings[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    descriptorSetLayoutBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
    vulkanFrame::UpdateDescriptorSets(device, descriptorSetLayoutBindings, 1, {uniforms.grid},{}, descriptorSet.grid);
    vulkanFrame::UpdateDescriptorSets(device, descriptorSetLayoutBindings, 1, {uniforms.bigGrid},{}, descriptorSet.bigGrid);
    vulkanFrame::UpdateDescriptorSets(device, descriptorSetLayoutBindings, 1, {uniforms.background},{}, descriptorSet.background);
    vulkanFrame::UpdateDescriptorSets(device, descriptorSetLayoutBindings, 1, {uniforms.wireframe.jiugongge},{}, descriptorSet.wireframe.jiugongge);
    // vulkanFrame::UpdateDescriptorSets(device, descriptorSetLayoutBindings, 1, {uniforms.wireframe.selectChess},{}, descriptorSet.wireframe.selectChess);
}
VulkanChessboard::VulkanChessboard(/* args */){
}
VulkanChessboard::~VulkanChessboard(){
}

void VulkanChessboard::Cleanup(VkDevice device){
    mRect.Destroy(device);
    uniforms.grid.Destroy(device);
    uniforms.bigGrid.Destroy(device);
    uniforms.background.Destroy(device);
    uniforms.wireframe.jiugongge.Destroy(device);
    // uniforms.wireframe.selectChess.Destroy(device);
}

void VulkanChessboard::Setup(VulkanDevice device, VkDescriptorSetLayout setLayout, VkQueue graphics, VulkanPool pool){
    VkPhysicalDeviceProperties physicalDeviceProperties;
    device.GetPhysicalDeviceProperties(physicalDeviceProperties);
    uint32_t minUniformBufferOffset = ALIGN(sizeof(glm::mat4), physicalDeviceProperties.limits.minUniformBufferOffsetAlignment);

    CreateRectResource(device, pool, graphics);

    //绘制所有方格, 即使是被大方格覆盖
    uniforms.grid.CreateBuffer(device, minUniformBufferOffset * CHESSBOARD_ROW * CHESSBOARD_COLUMN, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT|VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    uniforms.grid.size = minUniformBufferOffset;

    uniforms.bigGrid.CreateBuffer(device, minUniformBufferOffset * CHESSBOARD_BIG_GRID_COUNT, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT|VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    uniforms.bigGrid.size = minUniformBufferOffset;
    
    uniforms.wireframe.jiugongge.CreateBuffer(device, minUniformBufferOffset * CHESSBOARD_WIREFRAME_COUNT, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT|VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    uniforms.wireframe.jiugongge.size = minUniformBufferOffset;

    // uniforms.wireframe.selectChess.CreateBuffer(device, minUniformBufferOffset * (CHESSBOARD_ROW + CHESSBOARD_COLUMN), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT|VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    // uniforms.wireframe.selectChess.size = minUniformBufferOffset;

    uniforms.background.CreateBuffer(device, minUniformBufferOffset * 2, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT|VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    uniforms.background.size = minUniformBufferOffset;

    SetupDescriptorSet(device.device, setLayout, pool);
}
void VulkanChessboard::UpdateUniform(VkDevice device, uint32_t windowWidth){
    //因为棋盘里面的坐标都是固定的, 所以可以直接在类里全画出来
    //但棋子位置不固定...
    UpdateBigGridUniform(device);

    UpdateGridUniform(device);

    UpdateJiuGongGeWireframeUniform(device);

    UpdateBackgroundUniform(device, windowWidth);
}
// void VulkanChessboard::UpdateSelectChessWireframeUniform(VkDevice device, const glm::mat4 *pModel, uint32_t count){
//     uniforms.wireframe.selectChess.UpdateData(device, count * uniforms.grid.size, pModel);
// }
void VulkanChessboard::Draw(VkCommandBuffer command, const VkPipelineLayout &layout){
    uint32_t dynamicOffsets;
    mRect.Bind(command);
    for (size_t i = 0; i < 2; i++){
        dynamicOffsets = i * uniforms.grid.size;
        vkCmdBindDescriptorSets(command, VK_PIPELINE_BIND_POINT_GRAPHICS, layout, 0, 1, &descriptorSet.background, 1, &dynamicOffsets);
        mRect.Draw(command, i * mRect.vertexCount);
    }
    for (size_t i = 0; i < CHESSBOARD_ROW * CHESSBOARD_COLUMN; ++i){
        dynamicOffsets = i * uniforms.grid.size;
        vkCmdBindDescriptorSets(command, VK_PIPELINE_BIND_POINT_GRAPHICS, layout, 0, 1, &descriptorSet.grid, 1, &dynamicOffsets);
        mRect.Draw(command);
    }
    for (size_t i = 0; i < CHESSBOARD_BIG_GRID_COUNT; ++i){
        dynamicOffsets = i * uniforms.grid.size;
        vkCmdBindDescriptorSets(command, VK_PIPELINE_BIND_POINT_GRAPHICS, layout, 0, 1, &descriptorSet.bigGrid, 1, &dynamicOffsets);
        mRect.Draw(command);
    }
}
void VulkanChessboard::DrawWireframe(VkCommandBuffer command, const VkPipelineLayout&layout){
    uint32_t indexOffset = 0;
    uint32_t dynamicOffsets;
    mRect.Bind(command);
    for (size_t i = 0; i < CHESSBOARD_WIREFRAME_COUNT; ++i){
        dynamicOffsets = i * uniforms.grid.size;
        if(i < CHESSBOARD_WIREFRAME_COUNT * .5)
            indexOffset = 0;
        else
            indexOffset = mRect.indexCount;
        vkCmdBindDescriptorSets(command, VK_PIPELINE_BIND_POINT_GRAPHICS, layout, 0, 1, &descriptorSet.wireframe.jiugongge, 1, &dynamicOffsets);
        mRect.Draw(command, 4, indexOffset);
    }
    // for (size_t i = 0; i < CHESSBOARD_ROW * 2; ++i){
    //     dynamicOffsets = i * uniforms.grid.size;
    //     vkCmdBindDescriptorSets(command, VK_PIPELINE_BIND_POINT_GRAPHICS, layout, 0, 1, &descriptorSet.wireframe.selectChess, 1, &dynamicOffsets);
    //     mRect.Draw(command, mRect.vertexCount);
    // }
}
