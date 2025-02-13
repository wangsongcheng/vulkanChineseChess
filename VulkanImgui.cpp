#include <array>
#include "VulkanImgui.h"
static uint32_t __glsl_shader_vert_spv[] ={
    0x07230203,0x00010000,0x00080001,0x0000002e,0x00000000,0x00020011,0x00000001,0x0006000b,
    0x00000001,0x4c534c47,0x6474732e,0x3035342e,0x00000000,0x0003000e,0x00000000,0x00000001,
    0x000a000f,0x00000000,0x00000004,0x6e69616d,0x00000000,0x0000000b,0x0000000f,0x00000015,
    0x0000001b,0x0000001c,0x00030003,0x00000002,0x000001c2,0x00040005,0x00000004,0x6e69616d,
    0x00000000,0x00030005,0x00000009,0x00000000,0x00050006,0x00000009,0x00000000,0x6f6c6f43,
    0x00000072,0x00040006,0x00000009,0x00000001,0x00005655,0x00030005,0x0000000b,0x0074754f,
    0x00040005,0x0000000f,0x6c6f4361,0x0000726f,0x00030005,0x00000015,0x00565561,0x00060005,
    0x00000019,0x505f6c67,0x65567265,0x78657472,0x00000000,0x00060006,0x00000019,0x00000000,
    0x505f6c67,0x7469736f,0x006e6f69,0x00030005,0x0000001b,0x00000000,0x00040005,0x0000001c,
    0x736f5061,0x00000000,0x00060005,0x0000001e,0x73755075,0x6e6f4368,0x6e617473,0x00000074,
    0x00050006,0x0000001e,0x00000000,0x61635375,0x0000656c,0x00060006,0x0000001e,0x00000001,
    0x61725475,0x616c736e,0x00006574,0x00030005,0x00000020,0x00006370,0x00040047,0x0000000b,
    0x0000001e,0x00000000,0x00040047,0x0000000f,0x0000001e,0x00000002,0x00040047,0x00000015,
    0x0000001e,0x00000001,0x00050048,0x00000019,0x00000000,0x0000000b,0x00000000,0x00030047,
    0x00000019,0x00000002,0x00040047,0x0000001c,0x0000001e,0x00000000,0x00050048,0x0000001e,
    0x00000000,0x00000023,0x00000000,0x00050048,0x0000001e,0x00000001,0x00000023,0x00000008,
    0x00030047,0x0000001e,0x00000002,0x00020013,0x00000002,0x00030021,0x00000003,0x00000002,
    0x00030016,0x00000006,0x00000020,0x00040017,0x00000007,0x00000006,0x00000004,0x00040017,
    0x00000008,0x00000006,0x00000002,0x0004001e,0x00000009,0x00000007,0x00000008,0x00040020,
    0x0000000a,0x00000003,0x00000009,0x0004003b,0x0000000a,0x0000000b,0x00000003,0x00040015,
    0x0000000c,0x00000020,0x00000001,0x0004002b,0x0000000c,0x0000000d,0x00000000,0x00040020,
    0x0000000e,0x00000001,0x00000007,0x0004003b,0x0000000e,0x0000000f,0x00000001,0x00040020,
    0x00000011,0x00000003,0x00000007,0x0004002b,0x0000000c,0x00000013,0x00000001,0x00040020,
    0x00000014,0x00000001,0x00000008,0x0004003b,0x00000014,0x00000015,0x00000001,0x00040020,
    0x00000017,0x00000003,0x00000008,0x0003001e,0x00000019,0x00000007,0x00040020,0x0000001a,
    0x00000003,0x00000019,0x0004003b,0x0000001a,0x0000001b,0x00000003,0x0004003b,0x00000014,
    0x0000001c,0x00000001,0x0004001e,0x0000001e,0x00000008,0x00000008,0x00040020,0x0000001f,
    0x00000009,0x0000001e,0x0004003b,0x0000001f,0x00000020,0x00000009,0x00040020,0x00000021,
    0x00000009,0x00000008,0x0004002b,0x00000006,0x00000028,0x00000000,0x0004002b,0x00000006,
    0x00000029,0x3f800000,0x00050036,0x00000002,0x00000004,0x00000000,0x00000003,0x000200f8,
    0x00000005,0x0004003d,0x00000007,0x00000010,0x0000000f,0x00050041,0x00000011,0x00000012,
    0x0000000b,0x0000000d,0x0003003e,0x00000012,0x00000010,0x0004003d,0x00000008,0x00000016,
    0x00000015,0x00050041,0x00000017,0x00000018,0x0000000b,0x00000013,0x0003003e,0x00000018,
    0x00000016,0x0004003d,0x00000008,0x0000001d,0x0000001c,0x00050041,0x00000021,0x00000022,
    0x00000020,0x0000000d,0x0004003d,0x00000008,0x00000023,0x00000022,0x00050085,0x00000008,
    0x00000024,0x0000001d,0x00000023,0x00050041,0x00000021,0x00000025,0x00000020,0x00000013,
    0x0004003d,0x00000008,0x00000026,0x00000025,0x00050081,0x00000008,0x00000027,0x00000024,
    0x00000026,0x00050051,0x00000006,0x0000002a,0x00000027,0x00000000,0x00050051,0x00000006,
    0x0000002b,0x00000027,0x00000001,0x00070050,0x00000007,0x0000002c,0x0000002a,0x0000002b,
    0x00000028,0x00000029,0x00050041,0x00000011,0x0000002d,0x0000001b,0x0000000d,0x0003003e,
    0x0000002d,0x0000002c,0x000100fd,0x00010038
};

// glsl_shader.frag, compiled with:
// # glslangValidator -V -x -o glsl_shader.frag.u32 glsl_shader.frag
/*
#version 450 core
layout(location = 0) out vec4 fColor;
layout(set=0, binding=0) uniform sampler2D sTexture;
layout(location = 0) in struct { vec4 Color; vec2 UV; } In;
void main()
{
    fColor = In.Color * texture(sTexture, In.UV.st);
}
*/
static uint32_t __glsl_shader_frag_spv[] ={
    0x07230203,0x00010000,0x00080001,0x0000001e,0x00000000,0x00020011,0x00000001,0x0006000b,
    0x00000001,0x4c534c47,0x6474732e,0x3035342e,0x00000000,0x0003000e,0x00000000,0x00000001,
    0x0007000f,0x00000004,0x00000004,0x6e69616d,0x00000000,0x00000009,0x0000000d,0x00030010,
    0x00000004,0x00000007,0x00030003,0x00000002,0x000001c2,0x00040005,0x00000004,0x6e69616d,
    0x00000000,0x00040005,0x00000009,0x6c6f4366,0x0000726f,0x00030005,0x0000000b,0x00000000,
    0x00050006,0x0000000b,0x00000000,0x6f6c6f43,0x00000072,0x00040006,0x0000000b,0x00000001,
    0x00005655,0x00030005,0x0000000d,0x00006e49,0x00050005,0x00000016,0x78655473,0x65727574,
    0x00000000,0x00040047,0x00000009,0x0000001e,0x00000000,0x00040047,0x0000000d,0x0000001e,
    0x00000000,0x00040047,0x00000016,0x00000022,0x00000000,0x00040047,0x00000016,0x00000021,
    0x00000000,0x00020013,0x00000002,0x00030021,0x00000003,0x00000002,0x00030016,0x00000006,
    0x00000020,0x00040017,0x00000007,0x00000006,0x00000004,0x00040020,0x00000008,0x00000003,
    0x00000007,0x0004003b,0x00000008,0x00000009,0x00000003,0x00040017,0x0000000a,0x00000006,
    0x00000002,0x0004001e,0x0000000b,0x00000007,0x0000000a,0x00040020,0x0000000c,0x00000001,
    0x0000000b,0x0004003b,0x0000000c,0x0000000d,0x00000001,0x00040015,0x0000000e,0x00000020,
    0x00000001,0x0004002b,0x0000000e,0x0000000f,0x00000000,0x00040020,0x00000010,0x00000001,
    0x00000007,0x00090019,0x00000013,0x00000006,0x00000001,0x00000000,0x00000000,0x00000000,
    0x00000001,0x00000000,0x0003001b,0x00000014,0x00000013,0x00040020,0x00000015,0x00000000,
    0x00000014,0x0004003b,0x00000015,0x00000016,0x00000000,0x0004002b,0x0000000e,0x00000018,
    0x00000001,0x00040020,0x00000019,0x00000001,0x0000000a,0x00050036,0x00000002,0x00000004,
    0x00000000,0x00000003,0x000200f8,0x00000005,0x00050041,0x00000010,0x00000011,0x0000000d,
    0x0000000f,0x0004003d,0x00000007,0x00000012,0x00000011,0x0004003d,0x00000014,0x00000017,
    0x00000016,0x00050041,0x00000019,0x0000001a,0x0000000d,0x00000018,0x0004003d,0x0000000a,
    0x0000001b,0x0000001a,0x00050057,0x00000007,0x0000001c,0x00000017,0x0000001b,0x00050085,
    0x00000007,0x0000001d,0x00000012,0x0000001c,0x0003003e,0x00000009,0x0000001d,0x000100fd,
    0x00010038
};
void VulkanImgui::CreatePipelineLayout(VkDevice device){
    VkPipelineLayoutCreateInfo info = Pipeline::initializers::pipelineLayoutCreateInfo(1, 1);
    VkPushConstantRange pushConstantRange = Pipeline::initializers::pushConstantRange(VK_SHADER_STAGE_VERTEX_BIT, sizeof(float) * 4, 0);
    info.pSetLayouts = &mSetLayout;
    info.pPushConstantRanges = &pushConstantRange;
    VK_CHECK(vkCreatePipelineLayout(device, &info, nullptr, &pipelines.layout));
}
void VulkanImgui::SetupDescriptorSetLayout(VkDevice device){
    VkDescriptorSetLayoutBinding binding[1] = {};
    binding[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    binding[0].descriptorCount = 1;
    binding[0].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    vulkanFrame::CreateDescriptorSetLayout(device, binding, 1, &mSetLayout);
}
void VulkanImgui::CreateFontsTexture(VulkanDevice device, VkQueue graphics, VulkanPool pool){
    ImGuiIO& io = ImGui::GetIO();

    unsigned char* pixels;
    int width, height;
    io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);
    // size_t upload_size = width * height * 4 * sizeof(char);

    vulkanFrame::CreateFontImage(device, pixels, width, height, mFont, pool, graphics);

    // bd->FontDescriptorSet = (VkDescriptorSet)ImGui_ImplVulkan_AddTexture(bd->FontSampler, bd->Font.view, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    // Store our identifier
    io.Fonts->SetTexID((ImTextureID)mSet);

    IM_FREE(pixels);
}
// void VulkanImgui::CreateRect(VulkanDevice device, VkDeviceSize index_size, VkDeviceSize vertex_size){
//     if(mRect.vertex.buffer != VK_NULL_HANDLE){
//         mRect.Destroy(device.device);
//     }
//     VkMemoryRequirements req;
//     vkGetBufferMemoryRequirements(mDevice.device, mRect.index.buffer, &req);
//     mBufferMemoryAlignment = (mBufferMemoryAlignment > req.alignment) ? mBufferMemoryAlignment : req.alignment;
//     VkDeviceSize index_buffer_size_aligned = ((index_size - 1) / mBufferMemoryAlignment + 1) * mBufferMemoryAlignment;
//     mRect.index.CreateBuffer(device, index_size, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
//     vkGetBufferMemoryRequirements(mDevice.device, mRect.vertex.buffer, &req);
//     mBufferMemoryAlignment = (mBufferMemoryAlignment > req.alignment) ? mBufferMemoryAlignment : req.alignment;
//     VkDeviceSize vertex_buffer_size_aligned = ((vertex_size - 1) / mBufferMemoryAlignment + 1) * mBufferMemoryAlignment;
//     mRect.vertex.CreateBuffer(device, vertex_buffer_size_aligned, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
// }
void VulkanImgui::SetupRenderState(VkCommandBuffer command, ImDrawData *draw_data, int fb_width, int fb_height){
    vkCmdBindPipeline(command, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelines.pipeline);
    // Bind Vertex And Index Buffer:
    mRect.Bind(command, sizeof(ImDrawIdx) == 2 ? VK_INDEX_TYPE_UINT16 : VK_INDEX_TYPE_UINT32);

    // Setup viewport:
    VkViewport viewport = Pipeline::Viewport(fb_width, fb_height);
    vkCmdSetViewport(command, 0, 1, &viewport);

    // Setup scale and translation:
    // Our visible imgui space lies from draw_data->DisplayPps (top left) to draw_data->DisplayPos+data_data->DisplaySize (bottom right). DisplayPos is (0,0) for single viewport apps.
    float scale[2];
    scale[0] = 2.0f / draw_data->DisplaySize.x;
    scale[1] = 2.0f / draw_data->DisplaySize.y;
    float translate[2];
    translate[0] = -1.0f - draw_data->DisplayPos.x * scale[0];
    translate[1] = -1.0f - draw_data->DisplayPos.y * scale[1];
    vkCmdPushConstants(command, pipelines.layout, VK_SHADER_STAGE_VERTEX_BIT, sizeof(float) * 0, sizeof(float) * 2, scale);
    vkCmdPushConstants(command, pipelines.layout, VK_SHADER_STAGE_VERTEX_BIT, sizeof(float) * 2, sizeof(float) * 2, translate);
}
static uint32_t ImGui_ImplVulkan_MemoryType(VkPhysicalDevice physicalDevice, VkMemoryPropertyFlags properties, uint32_t type_bits){
    VkPhysicalDeviceMemoryProperties prop;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &prop);
    for (uint32_t i = 0; i < prop.memoryTypeCount; i++)
        if ((prop.memoryTypes[i].propertyFlags & properties) == properties && type_bits & (1 << i))
            return i;
    return 0xFFFFFFFF; // Unable to find memoryType
}
void VulkanImgui::CreateOrResizeBuffer(VkBuffer& buffer, VkDeviceMemory& buffer_memory, VkDeviceSize& p_buffer_size, size_t new_size, VkBufferUsageFlagBits usage){
    VkResult err;
    if (buffer != VK_NULL_HANDLE)
        vkDestroyBuffer(mDevice.device, buffer, VK_NULL_HANDLE);
    if (buffer_memory != VK_NULL_HANDLE)
        vkFreeMemory(mDevice.device, buffer_memory, VK_NULL_HANDLE);

    VkDeviceSize vertex_buffer_size_aligned = ((new_size - 1) / mBufferMemoryAlignment + 1) * mBufferMemoryAlignment;
    VkBufferCreateInfo buffer_info = {};
    buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buffer_info.size = vertex_buffer_size_aligned;
    buffer_info.usage = usage;
    buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    err = vkCreateBuffer(mDevice.device, &buffer_info, VK_NULL_HANDLE, &buffer);

    VkMemoryRequirements req;
    vkGetBufferMemoryRequirements(mDevice.device, buffer, &req);
    mBufferMemoryAlignment = (mBufferMemoryAlignment > req.alignment) ? mBufferMemoryAlignment : req.alignment;
    VkMemoryAllocateInfo alloc_info = {};
    alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    alloc_info.allocationSize = req.size;
    alloc_info.memoryTypeIndex = ImGui_ImplVulkan_MemoryType(mDevice.physicalDevice, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, req.memoryTypeBits);
    err = vkAllocateMemory(mDevice.device, &alloc_info, VK_NULL_HANDLE, &buffer_memory);

    err = vkBindBufferMemory(mDevice.device, buffer, buffer_memory, 0);
    p_buffer_size = req.size;
}
VulkanImgui::VulkanImgui(/* args */){
}

VulkanImgui::~VulkanImgui(){
}

void VulkanImgui::Cleanup(VkDevice device){
    mRect.Destroy(device);
    mFont.Destroy(device);
    vkDestroySampler(device, mFontSampler, VK_NULL_HANDLE);
    vkDestroyPipeline(device, pipelines.pipeline, VK_NULL_HANDLE);
    vkDestroyPipelineLayout(device, pipelines.layout, VK_NULL_HANDLE);
    vkDestroyDescriptorSetLayout(device, mSetLayout, VK_NULL_HANDLE);
}

void VulkanImgui::Setup(VulkanDevice device, VkQueue graphics, VulkanPool pool){
    mDevice = device;
    mBufferMemoryAlignment = 256;
    vulkanFrame::CreateTextureSampler(device.device, mFontSampler);
    SetupDescriptorSetLayout(device.device);
    pool.AllocateDescriptorSets(device.device, &mSetLayout, 1, &mSet);

    ImGuiIO&io = ImGui::GetIO();
    io.Fonts->AddFontFromFileTTF("fonts/SourceHanSerifCN-Bold.otf", 20, NULL, io.Fonts->GetGlyphRangesChineseFull());
    CreateFontsTexture(device, graphics, pool);

    VkDescriptorSetLayoutBinding binding[1] = {};
    binding[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    binding[0].descriptorCount = 1;
    binding[0].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    vulkanFrame::UpdateDescriptorSets(device.device, binding, 1, {}, { mFont }, mSet, mFontSampler);
}

void VulkanImgui::CreatePipeline(VkDevice device, VkRenderPass renderPass, VkPipelineCache cache){
    CreatePipelineLayout(device);

    VkPipelineVertexInputStateCreateInfo inputState{};
    std::array<VkPipelineShaderStageCreateInfo,2> shaderStages;

    VkVertexInputBindingDescription binding_desc[1] = {};
    binding_desc[0].stride = sizeof(ImDrawVert);
    binding_desc[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    VkVertexInputAttributeDescription attribute_desc[3] = {};
    attribute_desc[0].location = 0;
    attribute_desc[0].binding = binding_desc[0].binding;
    attribute_desc[0].format = VK_FORMAT_R32G32_SFLOAT;
    attribute_desc[0].offset = IM_OFFSETOF(ImDrawVert, pos);
    attribute_desc[1].location = 1;
    attribute_desc[1].binding = binding_desc[0].binding;
    attribute_desc[1].format = VK_FORMAT_R32G32_SFLOAT;
    attribute_desc[1].offset = IM_OFFSETOF(ImDrawVert, uv);
    attribute_desc[2].location = 2;
    attribute_desc[2].binding = binding_desc[0].binding;
    attribute_desc[2].format = VK_FORMAT_R8G8B8A8_UNORM;
    attribute_desc[2].offset = IM_OFFSETOF(ImDrawVert, col);

    VkPipelineColorBlendAttachmentState color_attachment[1] = {};
    color_attachment[0].blendEnable = VK_TRUE;
    color_attachment[0].srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    color_attachment[0].dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    color_attachment[0].colorBlendOp = VK_BLEND_OP_ADD;
    color_attachment[0].srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    color_attachment[0].dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;//VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    color_attachment[0].alphaBlendOp = VK_BLEND_OP_ADD;
    color_attachment[0].colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

    VkPipelineViewportStateCreateInfo viewportState = Pipeline::initializers::pipelineViewportStateCreateInfo(1, 1);

    VkGraphicsPipelineCreateInfo pipelineCreateInfo = Pipeline::initializers::pipelineCreateInfo(pipelines.layout, renderPass);
    VkPipelineColorBlendStateCreateInfo colorBlendState = Pipeline::initializers::pipelineColorBlendStateCreateInfo(1, color_attachment);
    VkPipelineMultisampleStateCreateInfo multisampleState = Pipeline::initializers::pipelineMultisampleStateCreateInfo(VK_SAMPLE_COUNT_1_BIT);
    VkPipelineDepthStencilStateCreateInfo depthStencilState = Pipeline::initializers::pipelineDepthStencilStateCreateInfo(VK_FALSE, VK_FALSE, VK_COMPARE_OP_LESS_OR_EQUAL);
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyState = Pipeline::initializers::pipelineInputAssemblyStateCreateInfo(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, 0, VK_FALSE);
    VkPipelineRasterizationStateCreateInfo rasterizationState = Pipeline::initializers::pipelineRasterizationStateCreateInfo(VK_POLYGON_MODE_FILL, VK_CULL_MODE_NONE, VK_FRONT_FACE_COUNTER_CLOCKWISE);

    std::array<VkDynamicState, 2> dynamicStateEnables = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
    VkPipelineDynamicStateCreateInfo dynamicState = Pipeline::initializers::pipelineDynamicStateCreateInfo(dynamicStateEnables.data(), static_cast<uint32_t>(dynamicStateEnables.size()));

    inputState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    inputState.vertexBindingDescriptionCount = 1;
    inputState.vertexAttributeDescriptionCount = 3;
    inputState.pVertexBindingDescriptions = binding_desc;
    inputState.pVertexAttributeDescriptions = attribute_desc;

    shaderStages[0] = Pipeline::LoadShader(device, VK_SHADER_STAGE_VERTEX_BIT, __glsl_shader_vert_spv, sizeof(__glsl_shader_vert_spv));
    shaderStages[1] = Pipeline::LoadShader(device, VK_SHADER_STAGE_FRAGMENT_BIT, __glsl_shader_frag_spv, sizeof(__glsl_shader_frag_spv));

    pipelineCreateInfo.pStages = shaderStages.data();
    pipelineCreateInfo.stageCount = shaderStages.size();

    pipelineCreateInfo.pVertexInputState = &inputState;
    pipelineCreateInfo.pViewportState = &viewportState;
    pipelineCreateInfo.pDynamicState = &dynamicState;
    pipelineCreateInfo.pColorBlendState = &colorBlendState;
    pipelineCreateInfo.pMultisampleState = &multisampleState;
    pipelineCreateInfo.pDepthStencilState = &depthStencilState;
    pipelineCreateInfo.pInputAssemblyState = &inputAssemblyState;
    pipelineCreateInfo.pRasterizationState = &rasterizationState;
    VK_CHECK(vkCreateGraphicsPipelines(device, cache, 1, &pipelineCreateInfo, nullptr, &pipelines.pipeline));
    vkDestroyShaderModule(device, shaderStages[0].module, VK_NULL_HANDLE);
    vkDestroyShaderModule(device, shaderStages[1].module, VK_NULL_HANDLE);
}

void VulkanImgui::RenderDrawData(VkCommandBuffer command, ImDrawData *draw_data){
    // Avoid rendering when minimized, scale coordinates for retina displays (screen coordinates != framebuffer coordinates)
    int fb_width = (int)(draw_data->DisplaySize.x * draw_data->FramebufferScale.x);
    int fb_height = (int)(draw_data->DisplaySize.y * draw_data->FramebufferScale.y);
    if (fb_width <= 0 || fb_height <= 0)
        return;

    if (draw_data->TotalVtxCount > 0){
        // Create or resize the vertex/index buffers
        size_t vertex_size = draw_data->TotalVtxCount * sizeof(ImDrawVert);
        size_t index_size = draw_data->TotalIdxCount * sizeof(ImDrawIdx);
        if (mRect.vertex.buffer == VK_NULL_HANDLE || mRect.vertex.size < vertex_size)
            CreateOrResizeBuffer(mRect.vertex.buffer, mRect.vertex.memory, mRect.vertex.size, vertex_size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
        if (mRect.index.buffer == VK_NULL_HANDLE || mRect.index.size < index_size)
            CreateOrResizeBuffer(mRect.index.buffer, mRect.index.memory, mRect.index.size, vertex_size, VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
        // if (mRect.vertex.buffer == VK_NULL_HANDLE || mRect.vertex.size < vertex_size || mRect.index.buffer == VK_NULL_HANDLE || mRect.index.size < index_size){
        //     CreateRect(mDevice, index_size, vertex_size);
        //     mRect.indexCount = draw_data->TotalIdxCount;
        //     mRect.vertexCount = draw_data->TotalVtxCount;
        // }
        // Upload vertex/index data into a single contiguous GPU buffer
        ImDrawVert* vtx_dst = NULL;
        ImDrawIdx* idx_dst = NULL;
        VK_CHECK(vkMapMemory(mDevice.device, mRect.vertex.memory, 0, mRect.vertex.size, 0, (void**)(&vtx_dst)));
        VK_CHECK(vkMapMemory(mDevice.device, mRect.index.memory, 0, mRect.index.size, 0, (void**)(&idx_dst)));
        for (int n = 0; n < draw_data->CmdListsCount; n++){
            const ImDrawList* cmd_list = draw_data->CmdLists[n];
            memcpy(vtx_dst, cmd_list->VtxBuffer.Data, cmd_list->VtxBuffer.Size * sizeof(ImDrawVert));
            memcpy(idx_dst, cmd_list->IdxBuffer.Data, cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx));
            vtx_dst += cmd_list->VtxBuffer.Size;
            idx_dst += cmd_list->IdxBuffer.Size;
        }
        VkMappedMemoryRange range[2] = {};
        range[0].sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
        range[0].memory = mRect.vertex.memory;
        range[0].size = VK_WHOLE_SIZE;
        range[1].sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
        range[1].memory = mRect.index.memory;
        range[1].size = VK_WHOLE_SIZE;
        VK_CHECK(vkFlushMappedMemoryRanges(mDevice.device, 2, range));
        vkUnmapMemory(mDevice.device, mRect.vertex.memory);
        vkUnmapMemory(mDevice.device, mRect.index.memory);
    }

    // Setup desired Vulkan state
    SetupRenderState(command, draw_data, fb_width, fb_height);

    // Will project scissor/clipping rectangles into framebuffer space
    ImVec2 clip_off = draw_data->DisplayPos;         // (0,0) unless using multi-viewports
    ImVec2 clip_scale = draw_data->FramebufferScale; // (1,1) unless using retina display which are often (2,2)

    // Render command lists
    // (Because we merged all buffers into a single one, we maintain our own offset into them)
    int global_vtx_offset = 0;
    int global_idx_offset = 0;
    for (int n = 0; n < draw_data->CmdListsCount; n++)
    {
        const ImDrawList* cmd_list = draw_data->CmdLists[n];
        for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
        {
            const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
            if (pcmd->UserCallback != NULL)
            {
                // User callback, registered via ImDrawList::AddCallback()
                // (ImDrawCallback_ResetRenderState is a special callback value used by the user to request the renderer to reset render state.)
                if (pcmd->UserCallback == ImDrawCallback_ResetRenderState)
                    SetupRenderState(command, draw_data, fb_width, fb_height);
                else
                    pcmd->UserCallback(cmd_list, pcmd);
            }
            else
            {
                // Project scissor/clipping rectangles into framebuffer space
                ImVec2 clip_min((pcmd->ClipRect.x - clip_off.x) * clip_scale.x, (pcmd->ClipRect.y - clip_off.y) * clip_scale.y);
                ImVec2 clip_max((pcmd->ClipRect.z - clip_off.x) * clip_scale.x, (pcmd->ClipRect.w - clip_off.y) * clip_scale.y);

                // Clamp to viewport as vkCmdSetScissor() won't accept values that are off bounds
                if (clip_min.x < 0.0f) { clip_min.x = 0.0f; }
                if (clip_min.y < 0.0f) { clip_min.y = 0.0f; }
                if (clip_max.x > fb_width) { clip_max.x = (float)fb_width; }
                if (clip_max.y > fb_height) { clip_max.y = (float)fb_height; }
                if (clip_max.x <= clip_min.x || clip_max.y <= clip_min.y)
                    continue;

                // Apply scissor/clipping rectangle
                VkRect2D scissor;
                scissor.offset.x = (int32_t)(clip_min.x);
                scissor.offset.y = (int32_t)(clip_min.y);
                scissor.extent.width = (uint32_t)(clip_max.x - clip_min.x);
                scissor.extent.height = (uint32_t)(clip_max.y - clip_min.y);
                vkCmdSetScissor(command, 0, 1, &scissor);

                // Bind DescriptorSet with font or user texture
                VkDescriptorSet desc_set[1] = { (VkDescriptorSet)pcmd->TextureId };
                if (sizeof(ImTextureID) < sizeof(ImU64))
                {
                    // We don't support texture switches if ImTextureID hasn't been redefined to be 64-bit. Do a flaky check that other textures haven't been used.
                    IM_ASSERT(pcmd->TextureId == (ImTextureID)mSet);
                    desc_set[0] = mSet;
                }
                /*
                    使用推送描述符集则不需要每次都创建一个描述符集。
                    只需要图片的view
                */
                vkCmdBindDescriptorSets(command, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelines.layout, 0, 1, desc_set, 0, NULL);

                // Draw
                vkCmdDrawIndexed(command, pcmd->ElemCount, 1, pcmd->IdxOffset + global_idx_offset, pcmd->VtxOffset + global_vtx_offset, 0);
            }
        }
        global_idx_offset += cmd_list->IdxBuffer.Size;
        global_vtx_offset += cmd_list->VtxBuffer.Size;
    }

    // Note: at this point both vkCmdSetViewport() and vkCmdSetScissor() have been called.
    // Our last values will leak into user/application rendering IF:
    // - Your app uses a pipeline with VK_DYNAMIC_STATE_VIEWPORT or VK_DYNAMIC_STATE_SCISSOR dynamic state
    // - And you forgot to call vkCmdSetViewport() and vkCmdSetScissor() yourself to explicitely set that state.
    // If you use VK_DYNAMIC_STATE_VIEWPORT or VK_DYNAMIC_STATE_SCISSOR you are responsible for setting the values before rendering.
    // In theory we should aim to backup/restore those values but I am not sure this is possible.
    // We perform a call to vkCmdSetScissor() to set back a full viewport which is likely to fix things for 99% users but technically this is not perfect. (See github #4644)
    VkRect2D scissor = { { 0, 0 }, { (uint32_t)fb_width, (uint32_t)fb_height } };
    vkCmdSetScissor(command, 0, 1, &scissor);
}