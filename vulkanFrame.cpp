#include <iostream>
#include "vulkanFrame.h"
void VulkanSynchronize::Cleanup(VkDevice device){
    for (size_t i = 0; i < fences.size(); ++i){
        vkDestroyFence(device, fences[i], nullptr);
        vkDestroySemaphore(device, imageAcquired[i], nullptr);
        vkDestroySemaphore(device, renderComplete[i], nullptr);
    }
}
void VulkanSynchronize::CreateSynchronize(VkDevice device, uint32_t swapchainImageCount){
    VkFenceCreateInfo fenceInfo;
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    fenceInfo.pNext = nullptr;
	VkSemaphoreCreateInfo semaphoreInfo = {};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    // uint32_t swapchainImageCount;
	// vkGetSwapchainImagesKHR(device, swapchain, &swapchainImageCount, nullptr);
    fences.resize(swapchainImageCount);
	imageAcquired.resize(swapchainImageCount);
	renderComplete.resize(swapchainImageCount);
	for (size_t i = 0; i < swapchainImageCount; ++i) {
    	vkCreateFence(device, &fenceInfo, nullptr, &fences[i]);
		vkCreateSemaphore(device, &semaphoreInfo, nullptr, &renderComplete[i]);
		vkCreateSemaphore(device, &semaphoreInfo, nullptr, &imageAcquired[i]);
	}
}

const char *vulkanFrame::cvmx_chip_type_to_string(VkResult type){
	switch (type) {
	case VK_SUCCESS:return "VK_SUCCESS";
	case VK_NOT_READY:return "VK_NOT_READY";
	case VK_TIMEOUT:return "VK_TIMEOUT";
	case VK_EVENT_SET:return "VK_EVENT_SET";
	case VK_EVENT_RESET:return "VK_EVENT_RESET";
	case VK_INCOMPLETE:return "VK_INCOMPLETE";
	case VK_ERROR_OUT_OF_HOST_MEMORY:return "VK_ERROR_OUT_OF_HOST_MEMORY";
	case VK_ERROR_OUT_OF_DEVICE_MEMORY:return "VK_ERROR_OUT_OF_DEVICE_MEMORY";
	case VK_ERROR_INITIALIZATION_FAILED:return "VK_ERROR_INITIALIZATION_FAILED";
	case VK_ERROR_DEVICE_LOST:return "VK_ERROR_DEVICE_LOST";
	case VK_ERROR_MEMORY_MAP_FAILED:return "VK_ERROR_MEMORY_MAP_FAILED";
	case VK_ERROR_LAYER_NOT_PRESENT:return "VK_ERROR_LAYER_NOT_PRESENT";
	case VK_ERROR_EXTENSION_NOT_PRESENT:return "VK_ERROR_EXTENSION_NOT_PRESENT";
	case VK_ERROR_FEATURE_NOT_PRESENT:return "VK_ERROR_FEATURE_NOT_PRESENT";
	case VK_ERROR_INCOMPATIBLE_DRIVER:return "VK_ERROR_INCOMPATIBLE_DRIVER";
	case VK_ERROR_TOO_MANY_OBJECTS:return "VK_ERROR_TOO_MANY_OBJECTS";
	case VK_ERROR_FORMAT_NOT_SUPPORTED:return "VK_ERROR_FORMAT_NOT_SUPPORTED";
	case VK_ERROR_FRAGMENTED_POOL:return "VK_ERROR_FRAGMENTED_POOL";
	case VK_ERROR_OUT_OF_POOL_MEMORY:return "VK_ERROR_OUT_OF_POOL_MEMORY";
	case VK_ERROR_INVALID_EXTERNAL_HANDLE:return "VK_ERROR_INVALID_EXTERNAL_HANDLE";
	case VK_ERROR_FRAGMENTATION:return "VK_ERROR_FRAGMENTATION";
	case VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS:return "VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS";
	case VK_ERROR_SURFACE_LOST_KHR:return "VK_ERROR_SURFACE_LOST_KHR";
	case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:return "VK_ERROR_NATIVE_WINDOW_IN_USE_KHR";
	case VK_SUBOPTIMAL_KHR:return "VK_SUBOPTIMAL_KHR";
	case VK_ERROR_OUT_OF_DATE_KHR:return "VK_ERROR_OUT_OF_DATE_KHR";
	case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:return "VK_ERROR_INCOMPATIBLE_DISPLAY_KHR";
	case VK_ERROR_VALIDATION_FAILED_EXT:return "VK_ERROR_VALIDATION_FAILED_EXT";
	case VK_ERROR_INVALID_SHADER_NV:return "VK_ERROR_INVALID_SHADER_NV";
	case VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT:return "VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT";
	case VK_ERROR_NOT_PERMITTED_EXT:return "VK_ERROR_NOT_PERMITTED_EXT";
	case VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT:return "VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT";
	default:
		break;
	}
	return "VK_ERROR_UNKNOWN";
}

VkResult vulkanFrame::Submit(VkDevice device, const VkCommandBuffer&commandbuffers, VkQueue graphics, const VkSemaphore&imageAcquired, const VkSemaphore&renderComplete, const VkFence&fence){
	// imagesInFlight[imageIndex] = inFlightFences[currentFrame];
	VkSubmitInfo submitInfo = {};
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandbuffers;
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = &imageAcquired;
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = &renderComplete;
	submitInfo.pWaitDstStageMask = waitStages;
	return vkQueueSubmit(graphics, 1, &submitInfo, fence);
}
// VkResult vulkanFrame::Prepare(VkDevice device, uint32_t *imageIndex, VkSwapchainKHR swapchain, const VkSemaphore &imageAcquired){
//     return vkAcquireNextImageKHR(device, swapchain, UINT64_MAX, imageAcquired, VK_NULL_HANDLE, imageIndex);
// }

uint32_t vulkanFrame::Prepare(VkDevice device, VkSwapchainKHR swapchain, const VkSemaphore&imageAcquired, void(*recreateSwapchain)(void* userData), void* userData){
    uint32_t imageIndex;
    // VkResult result = Prepare(device, &imageIndex, swapchain, imageAcquired);
	VkResult result = vkAcquireNextImageKHR(device, swapchain, UINT64_MAX, imageAcquired, VK_NULL_HANDLE, &imageIndex);
    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        if(recreateSwapchain)recreateSwapchain(userData);
    }
    else if(result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR){
        printf("failed to acquire swap chain image!\n");
        return result;
    }
    return imageIndex;
}

VkResult vulkanFrame::Present(VkDevice device, uint32_t imageIndex, VkSwapchainKHR swapchain, const VkQueue present, const VkSemaphore&renderComplete, void(*recreateSwapchain)(void* userData), void* userData){
	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.pImageIndices = &imageIndex;
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = &swapchain;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = &renderComplete;
	VkResult result = vkQueuePresentKHR(present, &presentInfo);
	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
        if(recreateSwapchain)recreateSwapchain(userData);
		return VK_SUCCESS;
	}
	else if(VK_SUCCESS != result){
		printf("failed to acquire swap chain image!\n");
	}
    return result;
}
VkResult vulkanFrame::Render(VkDevice device, uint32_t currentFrame, const VkCommandBuffer& commandbuffers, VkSwapchainKHR swapchain, const VulkanQueue&vulkanQueue, const VulkanSynchronize&vulkanSynchronize, void(*recreateSwapchain)(void* userData), void* userData){
    vkWaitForFences(device, 1, &vulkanSynchronize.fences[currentFrame], VK_TRUE, UINT64_MAX);
    vkResetFences(device, 1, &vulkanSynchronize.fences[currentFrame]);
    uint32_t imageIndex = Prepare(device, swapchain, vulkanSynchronize.imageAcquired[currentFrame], recreateSwapchain, userData);
    
    VK_CHECK(Submit(device, commandbuffers, vulkanQueue.graphics, vulkanSynchronize.imageAcquired[currentFrame], vulkanSynchronize.renderComplete[currentFrame], vulkanSynchronize.fences[currentFrame]));
    return Present(device, imageIndex, swapchain, vulkanQueue.present, vulkanSynchronize.renderComplete[currentFrame], recreateSwapchain, userData);;
}

void vulkanFrame::CreateFontImage(VulkanDevice device, const void *datas, uint32_t width, uint32_t height, VulkanImage &image, VulkanPool pool, VkQueue graphics){
    VulkanBuffer tempStorageBuffer;
	VkDeviceSize imageSize = width * height * 4;//用于imgui的字体,原本没有乘4。
	tempStorageBuffer.CreateBuffer(device, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT|VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    tempStorageBuffer.UpdateData(device.device, imageSize, datas);
	CreateFontImage(device, tempStorageBuffer, width, height, image, pool, graphics);
	tempStorageBuffer.Destroy(device.device);
}
void vulkanFrame::CreateTextureImage(VulkanDevice device, const void *datas, uint32_t width, uint32_t height, VulkanImage&image, VulkanPool pool, VkQueue graphics){
	VulkanBuffer tempStorageBuffer;
	VkDeviceSize imageSize = width * height * 4;
	tempStorageBuffer.CreateBuffer(device, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT|VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    tempStorageBuffer.UpdateData(device.device, imageSize, datas);
	CreateTextureImage(device, tempStorageBuffer, width, height, image, pool, graphics);
	tempStorageBuffer.Destroy(device.device);
}
void vulkanFrame::CreateFontImage(VulkanDevice device, const VulkanBuffer&dataBuffer, uint32_t width, uint32_t height, VulkanImage&image, VulkanPool pool, VkQueue graphics){
    image.size.depth = 1;
    image.size.width = width;
    image.size.height = height;
    image.CreateImage(device.device, image.size, VK_IMAGE_USAGE_TRANSFER_DST_BIT|VK_IMAGE_USAGE_SAMPLED_BIT, VK_FORMAT_R8G8B8A8_UNORM);
    // image.CreateImage(device, VK_IMAGE_USAGE_TRANSFER_DST_BIT|VK_IMAGE_USAGE_SAMPLED_BIT, VK_FORMAT_R8_UNORM);
    image.AllocateAndBindMemory(device, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    image.CopyImage(device.device, dataBuffer, width, height, pool, graphics);
	image.CreateImageView(device.device, VK_FORMAT_R8G8B8A8_UNORM);
}
void vulkanFrame::CreateFontImageArray(VulkanDevice device, const void* datas, uint32_t imageCount, uint32_t width, uint32_t height, VulkanImage& image, VulkanPool pool, VkQueue graphics){
    VulkanBuffer tempStorageBuffer;
    const VkDeviceSize imageSize = width * height;
    tempStorageBuffer.CreateBuffer(device, imageSize * imageCount, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    tempStorageBuffer.UpdateData(device.device, imageSize * imageCount, datas);
    CreateFontImageArray(device, tempStorageBuffer, imageCount, width, height, image, pool, graphics);
    tempStorageBuffer.Destroy(device.device);
}
void vulkanFrame::CreateFontImageArray(VulkanDevice device, const VulkanBuffer& dataBuffer, uint32_t imageCount, uint32_t width, uint32_t height, VulkanImage& image, VulkanPool pool, VkQueue graphics){
    const VkDeviceSize imageSize = width * height;
    image.size.depth = 1;
    image.size.width = width;
    image.size.height = height;
    image.CreateImage(device.device, image.size, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_FORMAT_R8_UNORM, imageCount);
    image.AllocateAndBindMemory(device, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	VkCommandBuffer cmd;
	BeginSingleTimeCommands(device.device, pool, cmd);
	SetImageLayout(cmd, image.image, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, imageCount);
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
	vkCmdCopyBufferToImage(cmd, dataBuffer.buffer, image.image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, bufferCopyRegions.size(), bufferCopyRegions.data());
	SetImageLayout(cmd, image.image, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, imageCount);
	EndSingleTimeCommands(device.device, pool.command, graphics, cmd);    
    image.CreateImageView(device.device, VK_FORMAT_R8_UNORM, VK_IMAGE_VIEW_TYPE_2D_ARRAY, imageCount);
}
void vulkanFrame::CreateTextureImage(VulkanDevice device, const VulkanBuffer&dataBuffer, uint32_t width, uint32_t height, VulkanImage&image, VulkanPool pool, VkQueue graphics){
    image.size.depth = 1;
    image.size.width = width;
    image.size.height = height;
    image.CreateImage(device.device, image.size, VK_IMAGE_USAGE_TRANSFER_DST_BIT|VK_IMAGE_USAGE_SAMPLED_BIT, VK_FORMAT_R8G8B8A8_UNORM);
    image.AllocateAndBindMemory(device, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    image.CopyImage(device.device, dataBuffer, width, height, pool, graphics);
	image.CreateImageView(device.device, VK_FORMAT_R8G8B8A8_UNORM);
}
void vulkanFrame::CreateCubeImage(VulkanDevice device, const void*const*datas, uint32_t width, VulkanImage&image, VulkanPool pool, VkQueue graphics){
    CreateImageArray(device, datas, 6, width, width, image, pool, graphics, VK_IMAGE_VIEW_TYPE_CUBE);
}
void vulkanFrame::CreateCubeImage(VulkanDevice device, const VulkanBuffer &dataBuffer, uint32_t width, VulkanImage &image, VulkanPool pool, VkQueue graphics){
    CreateImageArray(device, dataBuffer, 6, width, width, image, pool, graphics, VK_IMAGE_VIEW_TYPE_CUBE);
}

void vulkanFrame::CreateImageArray(VulkanDevice device, const void *const *datas, uint32_t imageCount, uint32_t width, uint32_t height, VulkanImage &image, VulkanPool pool, VkQueue graphics, VkImageViewType type){
    VulkanBuffer tempStorageBuffer;
	const VkDeviceSize imageSize = width * height * 4;
	tempStorageBuffer.CreateBuffer(device, imageSize * imageCount, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    for (size_t i = 0; i < imageCount; i++){
	    tempStorageBuffer.UpdateData(device.device, imageSize, datas[i], i * imageSize);
    }
	CreateImageArray(device, tempStorageBuffer, imageCount, width, height, image, pool, graphics, type);
	tempStorageBuffer.Destroy(device.device);
}
void vulkanFrame::CreateImageArray(VulkanDevice device, const VulkanBuffer&dataBuffer, uint32_t imageCount, uint32_t width, uint32_t height, VulkanImage&image, VulkanPool pool, VkQueue graphics, VkImageViewType type){
    image.size.depth = 1;
    image.size.width = width;
    image.size.height = height;
    image.CreateImage(device.device, image.size, VK_IMAGE_USAGE_TRANSFER_DST_BIT|VK_IMAGE_USAGE_SAMPLED_BIT, VK_FORMAT_R8G8B8A8_UNORM, imageCount);
    image.AllocateAndBindMemory(device, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    image.CopyImage(device.device, dataBuffer, imageCount, width, height, pool, graphics);
	image.CreateImageView(device.device, VK_FORMAT_R8G8B8A8_UNORM, type, imageCount);
}
void vulkanFrame::CopyImage(VkDevice device, VulkanImage&src, VulkanImage&dst, VulkanPool pool, VkQueue graphics){
    VkCommandBuffer command;
    VkImage srcImage = src.image, dstImage = dst.image;
    VkDeviceMemory srcImageMemory = src.memory, dstImageMemory = dst.memory;
    vulkanFrame::BeginSingleTimeCommands(device, pool, command);
    vulkanFrame::InsertImageMemoryBarrier(command, dstImage,
        0, VK_ACCESS_TRANSFER_WRITE_BIT,
        VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
        VkImageSubresourceRange{VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1});
    vulkanFrame::InsertImageMemoryBarrier(command, srcImage,
        VK_ACCESS_MEMORY_READ_BIT, VK_ACCESS_TRANSFER_READ_BIT,
        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
        VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
        VkImageSubresourceRange{VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1});
        // Otherwise use image copy (requires us to manually flip components)
        VkImageCopy imageCopyRegion{};
        imageCopyRegion.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imageCopyRegion.srcSubresource.layerCount = 1;
        imageCopyRegion.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imageCopyRegion.dstSubresource.layerCount = 1;
        imageCopyRegion.extent.width = src.size.width;
        imageCopyRegion.extent.height = src.size.height;
        imageCopyRegion.extent.depth = 1;

        // Issue the copy command
        vkCmdCopyImage(command,
            srcImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
            dstImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            1, &imageCopyRegion);
		// Transition destination image to general layout, which is the required layout for mapping the image memory later on
	 vulkanFrame::InsertImageMemoryBarrier(command, dstImage,
			VK_ACCESS_TRANSFER_WRITE_BIT,
			VK_ACCESS_MEMORY_READ_BIT,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			VK_IMAGE_LAYOUT_GENERAL,
			VK_PIPELINE_STAGE_TRANSFER_BIT,
			VK_PIPELINE_STAGE_TRANSFER_BIT,
			VkImageSubresourceRange{ VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 });
		// Transition back the swap chain image after the blit is done
	 vulkanFrame::InsertImageMemoryBarrier(command, srcImage,
			VK_ACCESS_TRANSFER_READ_BIT,
			VK_ACCESS_MEMORY_READ_BIT,
			VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
			VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
			VK_PIPELINE_STAGE_TRANSFER_BIT,
			VK_PIPELINE_STAGE_TRANSFER_BIT,
			VkImageSubresourceRange{ VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 });
    vulkanFrame::EndSingleTimeCommands(device, pool.command, graphics, command);
}
// void vulkanFrame::CopyImage(VulkanDevice device, const void *datas, uint32_t imageCount, uint32_t width, uint32_t height, VulkanImage&image, VkCommandPool pool, VkQueue graphics){
// 	VulkanBuffer tempStorageBuffer;
// 	VkDeviceSize imageSize = width * height * 4 * imageCount;
// 	tempStorageBuffer.CreateBuffer(device, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT|VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
//     tempStorageBuffer.UpdateData(device.device, imageSize, datas);
//     CopyImage(device.device, tempStorageBuffer, width, height, image, pool, graphics);
//     tempStorageBuffer.Destroy(device.device);
// }
VkResult vulkanFrame::CreateTextureSampler(VkDevice device, VkSampler &sampler){
    VkSamplerCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    createInfo.magFilter = VK_FILTER_LINEAR;
    createInfo.minFilter = VK_FILTER_LINEAR;
    createInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    createInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    createInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    createInfo.anisotropyEnable = VK_TRUE;
    createInfo.maxAnisotropy = 16;
    createInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    createInfo.unnormalizedCoordinates = VK_FALSE;
    createInfo.compareEnable = VK_FALSE;
    createInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    createInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    return vkCreateSampler(device, &createInfo, nullptr, &sampler);
}

void vulkanFrame::UpdateDescriptorSets(VkDevice device, const VkDescriptorSetLayoutBinding *pBindings, uint32_t count, const std::vector<VulkanBuffer> &descriptorBuffer, const std::vector<VulkanImage> &descriptorImage, VkDescriptorSet &destSet, const VkSampler &textureSampler){
    std::vector<uint32_t>index(2);//一个uniform一个图片采样器。如果需要其他则个数必须增加
    std::vector<VkWriteDescriptorSet>writeDescriptorSet(count);
    std::vector<VkDescriptorImageInfo>descriptorImageInfo(descriptorImage.size());
    std::vector<VkDescriptorBufferInfo> descriptorBufferInfo(descriptorBuffer.size());
    for (size_t i = 0; i < writeDescriptorSet.size(); ++i){
        writeDescriptorSet[i].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writeDescriptorSet[i].dstSet = destSet;
        writeDescriptorSet[i].descriptorCount = 1;
        writeDescriptorSet[i].dstBinding = pBindings[i].binding;
        writeDescriptorSet[i].descriptorType = pBindings[i].descriptorType;
        switch (writeDescriptorSet[i].descriptorType){
        case VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
            descriptorImageInfo[index[1]].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            descriptorImageInfo[index[1]].imageView = descriptorImage[index[1]].view;
            descriptorImageInfo[index[1]].sampler = textureSampler;
            writeDescriptorSet[i].pImageInfo = &descriptorImageInfo[index[1]++];
            break;
        case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER:
        case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
        case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC:
           descriptorBufferInfo[index[0]].offset = 0;
            descriptorBufferInfo[index[0]].range = descriptorBuffer[index[0]].size;
            descriptorBufferInfo[index[0]].buffer = descriptorBuffer[index[0]].buffer;
            writeDescriptorSet[i].pBufferInfo = &descriptorBufferInfo[index[0]++];
            break;
        default:
            break;
        }
    }
    vkUpdateDescriptorSets(device, writeDescriptorSet.size(), writeDescriptorSet.data(), 0, nullptr);
}
VkResult vulkanFrame::BeginCommands(VkCommandBuffer command, VkCommandBufferUsageFlags flags){
	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = flags;

	return vkBeginCommandBuffer(command, &beginInfo);
}
VkResult vulkanFrame::BeginSingleTimeCommands(VkDevice device, VulkanPool pool, VkCommandBuffer&command){
    pool.AllocateCommandBuffers(device, 1, &command);
    return BeginCommands(command, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
}
void vulkanFrame::EndSingleTimeCommands(VkDevice device, VkCommandPool pool, VkQueue graphics, VkCommandBuffer command){
    // Create fence to ensure that the command buffer has finished executing
    VkFence fence;
    VkFenceCreateInfo fenceInfo = {};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    vkCreateFence(device, &fenceInfo, nullptr, &fence);

	vkEndCommandBuffer(command);
    
	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &command;

	vkQueueSubmit(graphics, 1, &submitInfo, fence);
	vkQueueWaitIdle(graphics);
    
    // Wait for the fence to signal that command buffer has finished executing
    vkWaitForFences(device, 1, &fence, VK_TRUE, UINT64_MAX);
    vkDestroyFence(device, fence, nullptr);

	vkFreeCommandBuffers(device, pool, 1, &command);
}
VkResult vulkanFrame::CreateDescriptorSetLayout(VkDevice device, const VkDescriptorSetLayoutBinding *pBinding, uint32_t count, VkDescriptorSetLayout *pSetLayout){
    VkDescriptorSetLayoutCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    info.pBindings = pBinding;
    info.bindingCount = count;
    return vkCreateDescriptorSetLayout(device, &info, VK_NULL_HANDLE, pSetLayout);
}
void vulkanFrame::BeginRenderPassGeneral(VkCommandBuffer command, VkFramebuffer frame, VkRenderPass renderpass, uint32_t windowWidth, uint32_t windowHeight){
    std::vector<VkClearValue> clearValues(2);
    clearValues[0].color = { 0.1f , 0.1f , 0.1f , 1.0f };
    clearValues[1].depthStencil = { 1.0f , 0 };
    BeginRenderPass(command, frame, renderpass, windowWidth, windowHeight, clearValues.size(), clearValues.data());
}
void vulkanFrame::BeginRenderPass(VkCommandBuffer command, VkFramebuffer frame, VkRenderPass renderpass, uint32_t windowWidth, uint32_t windowHeight, uint32_t clearValueCount, VkClearValue *pClearValues, int32_t renderAreaWidthOffset, int32_t renderAreaHeightOffset){
    VkRenderPassBeginInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    info.framebuffer = frame;
    info.renderPass = renderpass;
    info.pClearValues = pClearValues;
    info.clearValueCount = clearValueCount;
    info.renderArea = { renderAreaWidthOffset, renderAreaHeightOffset, windowWidth, windowHeight };
    vkCmdBeginRenderPass(command, &info, VK_SUBPASS_CONTENTS_INLINE);
}
// uint32_t vkf::tool::GetFileSize(FILE *fp){
//     uint32_t size = 0;
//     if(fp){
//         fseek(fp, 0, SEEK_END);
//         size = ftell(fp);
//         fseek(fp, 0, SEEK_SET);
//     }
//     return size;
// }
// uint32_t vkf::tool::GetFileContent(const std::string&file, std::vector<uint32_t>&data){
//     FILE *fp = fopen(file.c_str(), "rb");
//     if(!fp){
//         perror("open file error");
//         printf("file is %s\n", file.c_str());
//         return 0;
//     }
//     uint32_t size = GetFileSize(fp);
//     data.resize(size / sizeof(uint32_t));
//     fread(data.data(), size, 1, fp);
//     fclose(fp);
//     return size;
// }
void vulkanFrame::SetImageLayout(VkCommandBuffer cmdbuffer, VkImage image, VkImageLayout oldImageLayout, VkImageLayout newImageLayout, VkImageSubresourceRange subresourceRange, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask){
	// Create an image barrier object
    VkAccessFlags srcAccessMask, dstAccessMask;
	// Source layouts (old)
	// Source access mask controls actions that have to be finished on the old layout
	// before it will be transitioned to the new layout
	switch (oldImageLayout){
	case VK_IMAGE_LAYOUT_UNDEFINED:
		// Image layout is undefined (or does not matter)
		// Only valid as initial layout
		// No flags required, listed only for completeness
		srcAccessMask = 0;
		break;

	case VK_IMAGE_LAYOUT_PREINITIALIZED:
		// Image is preinitialized
		// Only valid as initial layout for linear images, preserves memory contents
		// Make sure host writes have been finished
		srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
		break;

	case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
		// Image is a color attachment
		// Make sure any writes to the color buffer have been finished
		srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		break;

	case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
		// Image is a depth/stencil attachment
		// Make sure any writes to the depth/stencil buffer have been finished
		srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		break;

	case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
		// Image is a transfer source
		// Make sure any reads from the image have been finished
		srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
		break;

	case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
		// Image is a transfer destination
		// Make sure any writes to the image have been finished
		srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		break;

	case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
		// Image is read by a shader
		// Make sure any shader reads from the image have been finished
		srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
		break;
	case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:
		srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;//VK_ACCESS_TRANSFER_READ_BIT;
		break;
	default:
		// Other source layouts aren't handled (yet)
		break;
	}

	// Target layouts (new)
	// Destination access mask controls the dependency for the new image layout
	switch (newImageLayout){
	case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
		// Image will be used as a transfer destination
		// Make sure any writes to the image have been finished
		dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		break;

	case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
		// Image will be used as a transfer source
		// Make sure any reads from the image have been finished
		dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
		break;

	case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
		// Image will be used as a color attachment
		// Make sure any writes to the color buffer have been finished
		dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		break;

	case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
		// Image layout will be used as a depth/stencil attachment
		// Make sure any writes to depth/stencil buffer have been finished
		dstAccessMask = dstAccessMask | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		break;

	case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
		// Image will be read in a shader (sampler, input attachment)
		// Make sure any writes to the image have been finished
		if (srcAccessMask == 0){
			srcAccessMask = VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
		}
		dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		break;
	case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:
		dstAccessMask = VK_ACCESS_SHADER_WRITE_BIT;//VK_ACCESS_TRANSFER_WRITE_BIT;
		break;
	default:
		// Other source layouts aren't handled (yet)
		break;
	}
    InsertImageMemoryBarrier(cmdbuffer, image, srcAccessMask, dstAccessMask, oldImageLayout, newImageLayout, srcStageMask, dstStageMask, subresourceRange);
}

// Fixed sub resource on first mip level and layer
void vulkanFrame::SetImageLayout(VkCommandBuffer cmdbuffer, VkImage image, VkImageAspectFlags aspectMask, VkImageLayout oldImageLayout, VkImageLayout newImageLayout, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask){
    SetImageLayout(cmdbuffer, image, aspectMask, oldImageLayout, newImageLayout, srcStageMask, dstStageMask, 1);
}
void vulkanFrame::SetImageLayout(VkCommandBuffer cmdbuffer, VkImage image, VkImageAspectFlags aspectMask, VkImageLayout oldImageLayout, VkImageLayout newImageLayout, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, uint32_t imageCount){
	VkImageSubresourceRange subresourceRange = {};
	subresourceRange.aspectMask = aspectMask;
	subresourceRange.baseMipLevel = 0;
	subresourceRange.levelCount = 1;
	subresourceRange.layerCount = imageCount;
	SetImageLayout(cmdbuffer, image, oldImageLayout, newImageLayout, subresourceRange, srcStageMask, dstStageMask);
}
void vulkanFrame::InsertImageMemoryBarrier(VkCommandBuffer cmdbuffer, VkImage image, VkAccessFlags srcAccessMask, VkAccessFlags dstAccessMask, VkImageLayout oldImageLayout, VkImageLayout newImageLayout, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, VkImageSubresourceRange subresourceRange){
    VkImageMemoryBarrier imageMemoryBarrier = {};
    imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    imageMemoryBarrier.srcAccessMask = srcAccessMask;
    imageMemoryBarrier.dstAccessMask = dstAccessMask;
    imageMemoryBarrier.oldLayout = oldImageLayout;
    imageMemoryBarrier.newLayout = newImageLayout;
    imageMemoryBarrier.image = image;
    imageMemoryBarrier.subresourceRange = subresourceRange;

    vkCmdPipelineBarrier(
        cmdbuffer,
        srcStageMask,
        dstStageMask,
        0,
        0, nullptr,
        0, nullptr,
        1, &imageMemoryBarrier);
}