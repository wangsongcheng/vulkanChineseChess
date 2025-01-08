#include "VulkanImage.h"
#include "vulkanFrame.h"//CopyImage需要
VkResult VulkanImage::CreateImage(VkDevice device, const VkExtent2D&extent, VkImageUsageFlags usage, VkFormat format, VkImageType type){
    VkExtent3D e3d;
    e3d.width = extent.width;
    e3d.height = extent.height;
    e3d.depth = 1;
    return CreateImage(device, e3d, usage, format, type);
}
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
    size = extent;
    VkImageCreateInfo imageInfo = {};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.mipLevels = 1;
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
VkResult VulkanImage::CreateImageView(VkDevice device, VkFormat format, VkImageViewType type, uint32_t arrayLayer){
    return CreateImageView(device, format, VK_IMAGE_ASPECT_COLOR_BIT, type, arrayLayer);
}
VkResult VulkanImage::CreateImageView(VkDevice device, VkFormat format, VkImageAspectFlags aspectMask, VkImageViewType type, uint32_t arrayLayer) {
	VkImageViewCreateInfo imageViewInfo = {};
    imageViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	imageViewInfo.image = image;
    imageViewInfo.format = format;
    imageViewInfo.viewType = type;
    imageViewInfo.subresourceRange.levelCount = 1;
    imageViewInfo.subresourceRange.baseMipLevel = 0;
    imageViewInfo.subresourceRange.baseArrayLayer = 0;
    imageViewInfo.subresourceRange.aspectMask = aspectMask;
    imageViewInfo.subresourceRange.layerCount = arrayLayer;
	return vkCreateImageView(device, &imageViewInfo, nullptr, &view);
}
void VulkanImage::CopyImage(VkDevice device, const VulkanBuffer &dataBuffer, uint32_t width, uint32_t height, VulkanPool pool, VkQueue graphics){
	VkCommandBuffer cmd;
	vulkanFrame::BeginSingleTimeCommands(device, pool, cmd);
    vulkanFrame::SetImageLayout(cmd, image, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT);
    VkBufferImageCopy bufferCopyRegions = {};
    // bufferCopyRegions.bufferOffset = bufferOffset;
    bufferCopyRegions.imageSubresource.layerCount = 1;
    bufferCopyRegions.imageExtent = { width, height, 1 };
    // bufferCopyRegions.imageSubresource.baseArrayLayer = baseArrayLayer;
    bufferCopyRegions.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	vkCmdCopyBufferToImage(cmd, dataBuffer.buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &bufferCopyRegions);
    vulkanFrame::SetImageLayout(cmd, image, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);
	vulkanFrame::EndSingleTimeCommands(device, pool.command, graphics, cmd);
}
void VulkanImage::CopyImage(VulkanDevice device, const void *const *datas, uint32_t imageCount, uint32_t width, uint32_t height, VulkanPool pool, VkQueue graphics){
    VulkanBuffer tempStorageBuffer;
	const VkDeviceSize imageSize = width * height * 4;
	tempStorageBuffer.CreateBuffer(device, imageSize * imageCount, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    for (size_t i = 0; i < imageCount; i++){
	    tempStorageBuffer.UpdateData(device.device, imageSize, datas[i], i * imageSize);
    }
    CopyImage(device.device, tempStorageBuffer, imageCount, width, height, pool, graphics);
    tempStorageBuffer.Destroy(device.device);
}
void VulkanImage::CopyImage(VkDevice device, const VulkanBuffer &dataBuffer, uint32_t imageCount, uint32_t width, uint32_t height, VulkanPool pool, VkQueue graphics){
	VkCommandBuffer cmd;
	const VkDeviceSize imageSize = width * height * 4;
	vulkanFrame::BeginSingleTimeCommands(device, pool, cmd);
	vulkanFrame::SetImageLayout(cmd, image, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, imageCount);
	std::vector<VkBufferImageCopy> bufferCopyRegions(imageCount);
	for (size_t i = 0; i < imageCount; ++i) {
        bufferCopyRegions[i].imageExtent.depth = 1;
        bufferCopyRegions[i].imageExtent.width = width;
        bufferCopyRegions[i].imageExtent.height = height;
        bufferCopyRegions[i].bufferOffset = imageSize * i;
        bufferCopyRegions[i].imageSubresource.layerCount = 1;
        bufferCopyRegions[i].imageSubresource.baseArrayLayer = i;
        bufferCopyRegions[i].imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	}
	vkCmdCopyBufferToImage(cmd, dataBuffer.buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, bufferCopyRegions.size(), bufferCopyRegions.data());
	vulkanFrame::SetImageLayout(cmd, image, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, imageCount);
	vulkanFrame::EndSingleTimeCommands(device, pool.command, graphics, cmd);
}
void VulkanImage::CopyImage(VulkanDevice device, const void *datas, uint32_t width, uint32_t height, VulkanPool pool, VkQueue graphics){
    VulkanBuffer tempStorageBuffer;
	VkDeviceSize imageSize = width * height * 4;
	tempStorageBuffer.CreateBuffer(device, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT|VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    tempStorageBuffer.UpdateData(device.device, imageSize, datas);
    CopyImage(device.device, tempStorageBuffer, width, height, pool, graphics);
    tempStorageBuffer.Destroy(device.device);
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