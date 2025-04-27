#include <iostream>
#include "VulkanImage.h"
VkResult VulkanImage::CreateImage(VkDevice device, const VkExtent3D&extent, VkImageUsageFlags usage, VkFormat format, VkImageType type){
    return CreateImage(device, extent, usage, format, 1, type);
}
VkResult VulkanImage::CreateImage(VkDevice device, const VkExtent3D&extent, VkImageUsageFlags usage, VkFormat format, uint32_t arrayLayer, VkImageType type){
    return CreateImage(device, extent, usage, format, VK_IMAGE_TILING_OPTIMAL, arrayLayer, type);
}
VkResult VulkanImage::CreateImage(VkDevice device, const VkExtent3D &extent, VkImageUsageFlags usage, VkFormat format, VkImageTiling tiling, uint32_t arrayLayer, VkImageType type){
    return CreateImage(device, extent, usage, format, type, tiling, arrayLayer);
}
VkResult VulkanImage::CreateImage(VkDevice device, const VkExtent3D &extent, VkImageUsageFlags usage, VkFormat format, VkImageType type, VkImageTiling tiling, uint32_t arrayLayer, VkSampleCountFlagBits samples, VkImageLayout initialLayout){
    this->size = extent;
    this->format = format;
    this->layout = initialLayout;
    VkImageCreateInfo imageInfo = {};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.mipLevels = mipLevels = 1;
    imageInfo.extent = size;
    if(arrayLayer == 6 && size.width == size.height){
        imageInfo.flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
    }
    imageInfo.usage = usage;
    imageInfo.format = format;
    imageInfo.tiling = tiling;
    imageInfo.imageType = type;
    imageInfo.samples = samples;
    imageInfo.arrayLayers = arrayLayer;
    imageInfo.initialLayout = initialLayout;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    return vkCreateImage(device, &imageInfo, nullptr, &image);
}
VkResult VulkanImage::CreateImageView(VkDevice device, VkImageViewType type, uint32_t arrayLayer){
    return CreateImageView(device, VK_IMAGE_ASPECT_COLOR_BIT, type, arrayLayer);
}
VkResult VulkanImage::CreateImageView(VkDevice device, VkImageAspectFlags aspectMask, VkImageViewType type, uint32_t arrayLayer) {
	VkImageViewCreateInfo imageViewInfo = {};
    imageViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	imageViewInfo.image = image;
    imageViewInfo.format = format;
    imageViewInfo.viewType = type;
    imageViewInfo.subresourceRange.levelCount = 1;
    imageViewInfo.subresourceRange.baseMipLevel = 0;
    imageViewInfo.subresourceRange.baseArrayLayer = 0;
    imageViewInfo.subresourceRange.layerCount = arrayLayer;
    imageViewInfo.subresourceRange.aspectMask = aspectMask;
    subresourceRange = imageViewInfo.subresourceRange;
	return vkCreateImageView(device, &imageViewInfo, nullptr, &view);
}
void VulkanImage::SetImageLayout(VkCommandBuffer cmd, VkImageLayout newLayout, VkPipelineStageFlags srcStage, VkPipelineStageFlags dstStage, VkAccessFlags srcAccessMask, VkAccessFlags dstAccessMask) {
    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = layout;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = image;
    barrier.subresourceRange = subresourceRange;
    barrier.srcAccessMask = srcAccessMask;
    barrier.dstAccessMask = dstAccessMask;

    vkCmdPipelineBarrier(cmd, srcStage, dstStage, 0,
        0, nullptr, 0, nullptr, 1, &barrier);

    layout = newLayout;
}
VkCommandBuffer VulkanImage::BeginSingleTimeCommands(VkDevice device, VulkanPool pool){
	VkCommandBuffer command;
    pool.AllocateCommandBuffers(device, 1, &command);
	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	if(vkBeginCommandBuffer(command, &beginInfo) != VK_SUCCESS){
        printf("vulkan error:in function %s line %d;vkBeginCommandBuffer error\n", __FUNCTION__, __LINE__);
        assert(0);
    }
	return command;
}
void VulkanImage::CopyImage(VkCommandBuffer command, const VulkanBuffer &dataBuffer){
    SetImageLayout(command, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, VK_ACCESS_TRANSFER_WRITE_BIT);
    VkBufferImageCopy bufferCopyRegions = {};
    // bufferCopyRegions.bufferOffset = bufferOffset;
    bufferCopyRegions.imageSubresource.layerCount = 1;
    bufferCopyRegions.imageExtent = { size.width, size.height, 1 };
    // bufferCopyRegions.imageSubresource.baseArrayLayer = baseArrayLayer;
    bufferCopyRegions.imageSubresource.aspectMask = subresourceRange.aspectMask;
	vkCmdCopyBufferToImage(command, dataBuffer.buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &bufferCopyRegions);
    SetImageLayout(command, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT);
}
void VulkanImage::CopyImage(VulkanDevice device, const void *const *datas, uint32_t imageCount, VulkanPool pool, VkQueue graphics){
    VulkanBuffer tempStorageBuffer;
	const VkDeviceSize imageSize = size.width * size.height * channels;
	tempStorageBuffer.CreateBuffer(device, imageSize * imageCount, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    for (size_t i = 0; i < imageCount; i++){
	    tempStorageBuffer.UpdateData(device.device, imageSize, datas[i], i * imageSize);
    }
	VkCommandBuffer command = BeginSingleTimeCommands(device.device, pool);
    CopyImage(command, tempStorageBuffer, imageCount);
	EndSingleTimeCommands(device.device, pool.command, graphics, command);
    tempStorageBuffer.Destroy(device.device);
}
void VulkanImage::CopyImage(VkCommandBuffer command, const VulkanBuffer &dataBuffer, uint32_t imageCount){
	const VkDeviceSize imageSize = size.width * size.height * channels;
    SetImageLayout(command, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, VK_ACCESS_TRANSFER_WRITE_BIT);
	std::vector<VkBufferImageCopy> bufferCopyRegions(imageCount);
	for (size_t i = 0; i < imageCount; ++i) {
        bufferCopyRegions[i].imageExtent.depth = 1;
        bufferCopyRegions[i].bufferOffset = imageSize * i;
        bufferCopyRegions[i].imageExtent.width = size.width;
        bufferCopyRegions[i].imageExtent.height = size.height;
        bufferCopyRegions[i].imageSubresource.layerCount = 1;
        bufferCopyRegions[i].imageSubresource.baseArrayLayer = i;
        bufferCopyRegions[i].imageSubresource.aspectMask = subresourceRange.aspectMask;
	}
	vkCmdCopyBufferToImage(command, dataBuffer.buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, bufferCopyRegions.size(), bufferCopyRegions.data());
    SetImageLayout(command, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT);
}
void VulkanImage::EndSingleTimeCommands(VkDevice device, VkCommandPool pool, VkQueue graphics, VkCommandBuffer command){
    vkEndCommandBuffer(command);
    
    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &command;

    vkQueueSubmit(graphics, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(graphics);

    vkFreeCommandBuffers(device, pool, 1, &command);
}
void VulkanImage::CopyImage(VulkanDevice device, const void *datas, VulkanPool pool, VkQueue graphics){
    VulkanBuffer tempStorageBuffer;
	VkDeviceSize imageSize = size.width * size.height * channels;
	tempStorageBuffer.CreateBuffer(device, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT|VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    tempStorageBuffer.UpdateData(device.device, imageSize, datas);
	VkCommandBuffer command = BeginSingleTimeCommands(device.device, pool);
    CopyImage(command, tempStorageBuffer);
	EndSingleTimeCommands(device.device, pool.command, graphics, command);
    tempStorageBuffer.Destroy(device.device);
}

// void VulkanImage::UpdateImage(VkDevice device, const void *datas, uint32_t imageCount){
//     void *data;
//     const uint32_t imageSize = size.width * size.height * channels;
//     const VkDeviceSize deviceSize = imageSize * imageCount;
//     vkMapMemory(device, memory, 0, deviceSize, 0, &data);
//     memcpy(data, datas, deviceSize);
//     vkUnmapMemory(device, memory);
// }

void VulkanImage::CopyImage(VkCommandBuffer command, VulkanImage &srcImage) {
    // 验证mip层级匹配
    assert(mipLevels == srcImage.mipLevels && "Mip levels mismatch");
    if(srcImage.layout == VK_IMAGE_LAYOUT_UNDEFINED){
        printf("in function %s:srcImage layout is undefined, need modify srcImage layout\n", __FUNCTION__);
    }

    // 源图像布局转换屏障 (UNDEFINED → TRANSFER_SRC_OPTIMAL)
    VkImageMemoryBarrier srcBarrier = {};
    srcBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    srcBarrier.srcAccessMask = 0;
    srcBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
    srcBarrier.oldLayout = srcImage.layout;
    srcBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    srcBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    srcBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    srcBarrier.image = srcImage.image;
    srcBarrier.subresourceRange = srcImage.subresourceRange;
    

    vkCmdPipelineBarrier(command, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &srcBarrier);

    // 目标图像布局转换屏障 (UNDEFINED → TRANSFER_DST_OPTIMAL)
    VkImageMemoryBarrier dstBarrier = {};
    dstBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    dstBarrier.srcAccessMask = 0;
    dstBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    dstBarrier.oldLayout = layout;
    dstBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    dstBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    dstBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    dstBarrier.image = image;
    dstBarrier.subresourceRange = subresourceRange;

    vkCmdPipelineBarrier(command, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &dstBarrier);

    // 创建拷贝区域（处理所有mip层级）
    std::vector<VkImageCopy> copyRegions(mipLevels);
    for (uint32_t i = 0; i < mipLevels; ++i) {
        copyRegions[i].srcOffset = {0, 0, 0};
        copyRegions[i].dstOffset = {0, 0, 0};
        copyRegions[i].srcSubresource.mipLevel = i;
        copyRegions[i].dstSubresource.mipLevel = i;
        copyRegions[i].extent.depth = srcImage.size.depth;
        copyRegions[i].extent.width = std::max(srcImage.size.width >> i, 1u);
        copyRegions[i].extent.height = std::max(srcImage.size.height >> i, 1u);
        copyRegions[i].dstSubresource.layerCount = subresourceRange.layerCount;
        copyRegions[i].dstSubresource.aspectMask = subresourceRange.aspectMask;
        copyRegions[i].dstSubresource.baseArrayLayer = subresourceRange.baseArrayLayer;
        copyRegions[i].srcSubresource.layerCount = srcImage.subresourceRange.layerCount;
        copyRegions[i].srcSubresource.aspectMask = srcImage.subresourceRange.aspectMask;
        copyRegions[i].srcSubresource.baseArrayLayer = srcImage.subresourceRange.baseArrayLayer;
    }

    // 执行图像拷贝命令
    vkCmdCopyImage(command, srcImage.image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, static_cast<uint32_t>(copyRegions.size()), copyRegions.data());

    // 目标图像后处理屏障 (TRANSFER_DST_OPTIMAL → SHADER_READ_ONLY_OPTIMAL)
    VkImageMemoryBarrier postBarrier = {};
    postBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
    postBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
    postBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT,
    postBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
    postBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
    postBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
    postBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
    postBarrier.image = image,
    postBarrier.subresourceRange = subresourceRange;

    vkCmdPipelineBarrier(command, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &postBarrier);

    // 更新目标对象的布局状态
    layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
}

VkResult VulkanImage::AllocateAndBindMemory(VulkanDevice device, VkMemoryPropertyFlags properties){
	VkMemoryRequirements memoryRequirements;
	vkGetImageMemoryRequirements(device.device, image, &memoryRequirements);
    VkResult result = device.AllocateMemory(memoryRequirements.size, memoryRequirements.memoryTypeBits, properties, memory);
	vkBindImageMemory(device.device, image, memory, 0);
    return result;
}

void VulkanImage::Destroy(VkDevice device){
    if(image != VK_NULL_HANDLE)vkDestroyImage(device, image, nullptr);
    if(view != VK_NULL_HANDLE)vkDestroyImageView(device, view, nullptr);
    if(memory != VK_NULL_HANDLE)vkFreeMemory(device, memory, nullptr);
    view = VK_NULL_HANDLE;
    image = VK_NULL_HANDLE;
    memory = VK_NULL_HANDLE;
}

void VulkanGrayImage::CreateImage(VulkanDevice device, uint32_t width, uint32_t height, uint32_t arrayLayers){
    channels = 1;
    size.depth = 1;
    size.width = width;
    size.height = height;
    VulkanImage::CreateImage(device.device, size, VK_IMAGE_USAGE_TRANSFER_DST_BIT|VK_IMAGE_USAGE_SAMPLED_BIT, VK_FORMAT_R8_UNORM, arrayLayers);
    AllocateAndBindMemory(device, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
}

void VulkanGrayImage::CreateImage(VulkanDevice device, const void *data, uint32_t width, uint32_t height, VkQueue graphics, VulkanPool pool){
    CreateImage(device, width, height);
    CreateImageView(device.device);
    CopyImage(device, data, pool, graphics);
}

void VulkanGrayImage::CreateImageArray(VulkanDevice device, const void *const *datas, uint32_t imageCount, uint32_t width, uint32_t height, VkQueue graphics, VulkanPool pool){
    CreateImage(device, width, height, imageCount);
    CreateImageView(device.device, VK_IMAGE_VIEW_TYPE_2D_ARRAY, imageCount);
    CopyImage(device, datas, imageCount, pool, graphics);
}
void VulkanFontImage::CreateImage(VulkanDevice device, uint32_t width, uint32_t height, uint32_t arrayLayers){
    size.depth = 1;
    size.width = width;
    size.height = height;
    VkFormat format = VK_FORMAT_R8_UNORM;
    if(channels == 4){
        format = VK_FORMAT_R8G8B8A8_UNORM;
    }
    VulkanImage::CreateImage(device.device, size, VK_IMAGE_USAGE_TRANSFER_DST_BIT|VK_IMAGE_USAGE_SAMPLED_BIT, format, arrayLayers);
    AllocateAndBindMemory(device, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
}
void VulkanFontImage::CreateImage(VulkanDevice device, const void *data, uint32_t width, uint32_t height, uint32_t channels, VkQueue graphics, VulkanPool pool){
    this->channels = channels;
    CreateImage(device, width, height);
    CreateImageView(device.device);
    CopyImage(device, data, pool, graphics);
}

void VulkanFontImage::CreateImageArray(VulkanDevice device, const void *const *datas, uint32_t imageCount, uint32_t width, uint32_t height, uint32_t channels, VkQueue graphics, VulkanPool pool){
    this->channels = channels;
    CreateImage(device, width, height, imageCount);
    CreateImageView(device.device, VK_IMAGE_VIEW_TYPE_2D_ARRAY, imageCount);
    CopyImage(device, datas, imageCount, pool, graphics);
}

void VulkanTextureImage::CreateImage(VulkanDevice device, uint32_t width, uint32_t height, uint32_t arrayLayers){
    channels = 4;
    size.depth = 1;
    size.width = width;
    size.height = height;
    VulkanImage::CreateImage(device.device, size, VK_IMAGE_USAGE_TRANSFER_DST_BIT|VK_IMAGE_USAGE_SAMPLED_BIT, VK_FORMAT_R8G8B8A8_UNORM, arrayLayers);
    AllocateAndBindMemory(device, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
}

void VulkanTextureImage::CreateImage(VulkanDevice device, const void *data, uint32_t width, uint32_t height, VkQueue graphics, VulkanPool pool){
    CreateImage(device, width, height);
    CreateImageView(device.device);
    CopyImage(device, data, pool, graphics);
}

void VulkanTextureImage::CreateImageArray(VulkanDevice device, const void *const *datas, uint32_t imageCount, uint32_t width, uint32_t height, VkQueue graphics, VulkanPool pool){
    CreateImage(device, width, height, imageCount);
    CreateImageView(device.device, VK_IMAGE_VIEW_TYPE_2D_ARRAY, imageCount);
    CopyImage(device, datas, imageCount, pool, graphics);
}

void VulkanCubeImage::CreateImage(VulkanDevice device, uint32_t width){
    channels = 4;
    size.depth = 1;
    size.width = width;
    size.height = width;
    VulkanImage::CreateImage(device.device, size, VK_IMAGE_USAGE_TRANSFER_DST_BIT|VK_IMAGE_USAGE_SAMPLED_BIT, VK_FORMAT_R8G8B8A8_UNORM, 6);
    AllocateAndBindMemory(device, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
}
void VulkanCubeImage::CreateImage(VulkanDevice device, const void *const *datas, uint32_t width, VkQueue graphics, VulkanPool pool){
    CreateImage(device, width);
    CreateImageView(device.device, VK_IMAGE_VIEW_TYPE_CUBE, 6);
    CopyImage(device, datas, 6, pool, graphics);
}
// void VulkanCubeImage::CreateImageArray(VulkanDevice device, const void *const *datas, uint32_t imageCount, uint32_t width, VkQueue graphics, VulkanPool pool){
//     channels = 4;
//     size.depth = 1;
//     size.depth = 1;
//     size.width = width;
//     size.height = width;
//     VulkanImage::CreateImage(device.device, size, VK_IMAGE_USAGE_TRANSFER_DST_BIT|VK_IMAGE_USAGE_SAMPLED_BIT, VK_FORMAT_R8G8B8A8_UNORM, imageCount);
//     AllocateAndBindMemory(device, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
//     CopyImage(device, datas, imageCount, pool, graphics);
//     CreateImageView(device.device, VK_IMAGE_VIEW_TYPE_CUBE_ARRAY, imageCount);
// }