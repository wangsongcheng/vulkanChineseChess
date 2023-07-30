#include <cstring>
#include <stdio.h>
#include "vulkanFrame.h"
VkResult VulkanImage::CreateImage(VkDevice device, VkImageUsageFlags usage, VkFormat format, VkImageType type){
    return CreateImage(device, usage, format, 1, type);
}
VkResult VulkanImage::CreateImage(VkDevice device, VkImageUsageFlags usage, VkFormat format, uint32_t arrayLayer, VkImageType type){
    return CreateImage(device, usage, format, VK_IMAGE_TILING_OPTIMAL, arrayLayer, type);
}
VkResult VulkanImage::CreateImage(VkDevice device, VkImageUsageFlags usage, VkFormat format, VkImageTiling tiling, uint32_t arrayLayer, VkImageType type){
    return CreateImage(device, usage, format, type, tiling, arrayLayer);
}
VkResult VulkanImage::CreateImage(VkDevice device, VkImageUsageFlags usage, VkFormat format, VkImageType type, VkImageTiling tiling, uint32_t arrayLayer, VkSampleCountFlagBits samples, VkImageLayout initialLayout){
    return CreateImage(device, size, usage, format, type, tiling, arrayLayer, samples, initialLayout);
}

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
VkResult VulkanImage::CreateImage(VkDevice device,const VkExtent3D&extent, VkImageUsageFlags usage, VkFormat format, VkImageTiling tiling, uint32_t arrayLayer, VkImageType type) {
    return CreateImage(device, extent, usage, format, type, tiling, arrayLayer);
}
VkResult VulkanImage::CreateImage(VkDevice device, const VkExtent3D&extent, VkImageUsageFlags usage, VkFormat format, VkImageType type, VkImageTiling tiling, uint32_t arrayLayer, VkSampleCountFlagBits samples, VkImageLayout initialLayout){
    size = extent;
    VkImageCreateInfo imageInfo = {};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.mipLevels = 1;
    imageInfo.extent = size;
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
    // imageViewInfo.subresourceRange.baseMipLevel = 0;
    // imageViewInfo.subresourceRange.baseArrayLayer = 0;
    imageViewInfo.subresourceRange.aspectMask = aspectMask;
    imageViewInfo.subresourceRange.layerCount = arrayLayer;
	return vkCreateImageView(device, &imageViewInfo, nullptr, &view);
}
void VulkanImage::AllocateAndBindMemory(VkDevice device, VkMemoryPropertyFlags properties){
	VkMemoryRequirements memoryRequirements;
	vkGetImageMemoryRequirements(device, image, &memoryRequirements);
    vkf::tool::AllocateMemory(device, memoryRequirements.size, memoryRequirements.memoryTypeBits, properties, memory);
	vkBindImageMemory(device, image, memory, 0);
}
void VulkanImage::Destroy(VkDevice device){
    if(image != VK_NULL_HANDLE)vkDestroyImage(device, image, nullptr);
    if(view != VK_NULL_HANDLE)vkDestroyImageView(device, view, nullptr);
    if(memory != VK_NULL_HANDLE)vkFreeMemory(device, memory, nullptr);
}
VkResult VulkanBuffer::CreateBuffer(VkDevice device, VkDeviceSize size, VkBufferUsageFlags usage){
    this->size = size;
    VkBufferCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    info.size = size;
    info.usage = usage;
    info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    return vkCreateBuffer(device, &info, nullptr, &buffer);
}
void VulkanBuffer::AllocateAndBindMemory(VkDevice device, VkMemoryPropertyFlags properties){
    //VK_MEMORY_PROPERTY_HOST_COHERENT_BIT可以保证数据写入后立即更新到buffer。但会导致性能下降(废话)
    //这种方式只使用于VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT应该用vkCmdCopyBuffer
	VkMemoryRequirements memoryRequirements;
	vkGetBufferMemoryRequirements(device, buffer, &memoryRequirements);
    vkf::tool::AllocateMemory(device, memoryRequirements.size, memoryRequirements.memoryTypeBits, properties, memory);
	vkBindBufferMemory(device, buffer, memory, 0);
}
void VulkanBuffer::UpdateData(VkDevice device, const void * pData, VkDeviceSize offset){
    UpdateData(device, size, pData, offset);
}
void VulkanBuffer::UpdateData(VkDevice device, VkDeviceSize size, const void * pData, VkDeviceSize offset){
	void* data;
	vkMapMemory(device, memory, offset, size, 0, &data);
	memcpy(data, pData, size);
	vkUnmapMemory(device, memory);
}
void VulkanBuffer::Destroy(VkDevice device){
    if(buffer != VK_NULL_HANDLE)vkDestroyBuffer(device, buffer, nullptr);
    if(memory != VK_NULL_HANDLE)vkFreeMemory(device, memory, nullptr);
}
VkPhysicalDevice vkf::GetPhysicalDevices(VkInstance instance, VkPhysicalDeviceType deviceType){
    uint32_t count;
    VkPhysicalDevice physicalDevice;
    VkPhysicalDeviceProperties physicalDeviceProperties;
	vkEnumeratePhysicalDevices(instance, &count, nullptr);
    if(count == 0){
		printf("No suitable graphics card!\n");
		return VK_NULL_HANDLE;
    }
    std::vector<VkPhysicalDevice>physicalDevices(count);
    vkEnumeratePhysicalDevices(instance, &count, physicalDevices.data());
    if(count == 1){
        physicalDevice = physicalDevices[0];
    	vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);
    }
    else{
		for (size_t i = 0; i < count; i++){
			vkGetPhysicalDeviceProperties(physicalDevices[i], &physicalDeviceProperties);
			if(physicalDeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU){
				physicalDevice = physicalDevices[i];
				break;
			}
		}
    }
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &gMemoryProperties);
    return physicalDevice;
}
void vkf::tool::GetGraphicAndPresentQueueFamiliesIndex(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, int queueFamilies[2]){
	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

	VkQueueFamilyProperties *queueFamiliesProperties = new VkQueueFamilyProperties[queueFamilyCount];
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamiliesProperties);
	for (int i = 0; i < queueFamilyCount; ++i) {
		if (queueFamiliesProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			queueFamilies[0] = i;
		}
		if (surface != VK_NULL_HANDLE) {
			VkBool32 presentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &presentSupport);
			if (presentSupport) {
				queueFamilies[1] = i;
			}
            if(queueFamilies[0] != -1 && queueFamilies[1] != -1)break;
		}
        else{
            if(queueFamilies[0] != -1)break;
        }
	}  
    delete[]queueFamiliesProperties;
}
VkResult vkf::CreateDevice(VkPhysicalDevice physicalDevice, const std::vector<const char*>& deviceExtensions, VkSurfaceKHR surface, VkDevice&device){
    uint32_t queueFamilyCount;
    int32_t queueFamily[] = { -1, -1 };
	const float queuePriorities = 1.0f;

    tool::GetGraphicAndPresentQueueFamiliesIndex(physicalDevice, surface, queueFamily);
    if(queueFamily[0] == queueFamily[1]){
        queueFamilyCount = 1;
    }
    else {
        queueFamily[0] != -1 && queueFamily[1] != -1?queueFamilyCount = 2:queueFamilyCount = 1;
    }
    std::vector<VkDeviceQueueCreateInfo>queueCreateInfos(queueFamilyCount);
    for (size_t i = 0; i < queueCreateInfos.size(); ++i){
        queueCreateInfos[i].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfos[i].queueCount = 1;
        queueCreateInfos[i].queueFamilyIndex = queueFamily[i];
        queueCreateInfos[i].pQueuePriorities = &queuePriorities;
    }
    VkPhysicalDeviceFeatures features;
	vkGetPhysicalDeviceFeatures(physicalDevice, &features);
	features.samplerAnisotropy = VK_TRUE;//保证该值一定为true
	std::vector<const char *>extensions = deviceExtensions;
    if(surface != VK_NULL_HANDLE)extensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
    VkDeviceCreateInfo deviceCreateInfo = {};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    //deviceCreateInfo.enabledLayerCount = 0;
    //deviceCreateInfo.ppEnabledLayerNames = nullptr;
    deviceCreateInfo.pEnabledFeatures = &features;
    deviceCreateInfo.enabledExtensionCount = extensions.size();
    deviceCreateInfo.ppEnabledExtensionNames = extensions.data();
    deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
    deviceCreateInfo.queueCreateInfoCount = queueCreateInfos.size();
    return vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &device);
}
VkResult vkf::CreateDebugUtilsMessenger(VkInstance instance, VkDebugUtilsMessengerEXT&messenger, PFN_vkDebugUtilsMessengerCallbackEXT debugUtilsMessenger){
    PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessenger = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    VkDebugUtilsMessengerCreateInfoEXT debugInfo = {};
    debugInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    debugInfo.pfnUserCallback = debugUtilsMessenger;
    debugInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    debugInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    return vkCreateDebugUtilsMessenger(instance, &debugInfo, nullptr, &messenger);   
}
void vkf::DestoryDebugUtilsMessenger(VkInstance instance, VkDebugUtilsMessengerEXT&messenger){
    PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessengerEXT = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    vkDestroyDebugUtilsMessengerEXT(instance, messenger, nullptr);
}
VkResult vkf::CreateInstance(const std::vector<const char*>&instanceExtensions, VkInstance&instance, bool enableValidation, VkApplicationInfo *pApplicationInfo){
    std::vector<const char *>layers;
	std::vector<const char *>extensions = instanceExtensions;
    if(enableValidation){
        layers.push_back("VK_LAYER_KHRONOS_validation");
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }
    VkInstanceCreateInfo instanceInfo = {};
    instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceInfo.enabledLayerCount = layers.size();
    instanceInfo.ppEnabledLayerNames = layers.data();
    instanceInfo.pApplicationInfo = pApplicationInfo;
    instanceInfo.enabledExtensionCount = extensions.size();
    instanceInfo.ppEnabledExtensionNames = extensions.data();
    return vkCreateInstance(&instanceInfo, nullptr, &instance);
}

VkResult vkf::CreateSwapchain(VkPhysicalDevice physicalDevice, VkDevice device, VkSurfaceKHR surface, VkSwapchainKHR&swapchain){
	uint32_t count;
	VkSurfaceCapabilitiesKHR surfaceCapabilities;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &surfaceCapabilities);
    uint32_t minImageCount = surfaceCapabilities.minImageCount + 1;
	if(minImageCount > 3)minImageCount = 3;
	VkSwapchainCreateInfoKHR swapchainInfo = {};//vki::swapchainCreateInfoKHR(surface, surfaceCapabilities, presentModes);
    swapchainInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchainInfo.clipped = VK_TRUE;
    swapchainInfo.surface = surface;
    swapchainInfo.imageArrayLayers = 1;
    // swapchainInfo.presentMode = presentMode;
    // swapchainInfo.pQueueFamilyIndices = ;
    // swapchainInfo.queueFamilyIndexCount = ;
    swapchainInfo.minImageCount = minImageCount;
    // swapchainInfo.oldSwapchain = VK_NULL_HANDLE;
    swapchainInfo.imageFormat = VK_FORMAT_B8G8R8A8_UNORM;
    swapchainInfo.presentMode = VK_PRESENT_MODE_MAILBOX_KHR;
    swapchainInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    swapchainInfo.imageExtent = surfaceCapabilities.currentExtent;
    swapchainInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    swapchainInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchainInfo.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
    swapchainInfo.preTransform = surfaceCapabilities.currentTransform;
	return vkCreateSwapchainKHR(device, &swapchainInfo, nullptr, &swapchain);
}
void vkf::CreateSemaphoreAndFenceForSwapchain(VkDevice device, uint32_t swapchainImageCount, VulkanSynchronize&vulkanSynchronize){
	VkFenceCreateInfo fenceInfo;
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    fenceInfo.pNext = nullptr;
	VkSemaphoreCreateInfo semaphoreInfo = {};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	vulkanSynchronize.imageAcquired.resize(swapchainImageCount);
	vulkanSynchronize.renderComplete.resize(swapchainImageCount);
    vulkanSynchronize.fences.resize(swapchainImageCount);
	for (size_t i = 0; i < swapchainImageCount; ++i) {
    	vkCreateFence(device, &fenceInfo, nullptr, &vulkanSynchronize.fences[i]);
		vkCreateSemaphore(device, &semaphoreInfo, nullptr, &vulkanSynchronize.renderComplete[i]);
		vkCreateSemaphore(device, &semaphoreInfo, nullptr, &vulkanSynchronize.imageAcquired[i]);
	}
}
VkResult vkf::CreateRenderPassForSwapchain(VkDevice device, VkRenderPass&renderPass, bool useDepthImage){
    VkSubpassDependency dependency = {};
    // dependency.dstSubpass = 0;
    // dependency.srcAccessMask = 0;
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	VkAttachmentReference colorAttachmentRef;
    VkAttachmentReference depthAttachmentRef;
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	VkSubpassDescription subPass = {};
    subPass.colorAttachmentCount = 1;
    subPass.pColorAttachments = &colorAttachmentRef;
    subPass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    std::vector<VkAttachmentDescription>attachmentDescription(1);
    attachmentDescription[0].samples = VK_SAMPLE_COUNT_1_BIT;
    attachmentDescription[0].format = VK_FORMAT_B8G8R8A8_UNORM;
    attachmentDescription[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachmentDescription[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachmentDescription[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attachmentDescription[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    attachmentDescription[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachmentDescription[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    if(useDepthImage){
        depthAttachmentRef.attachment = 1;
        depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        subPass.pDepthStencilAttachment = &depthAttachmentRef;

        VkAttachmentDescription depthAttachmentDescription = {};
        depthAttachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
        depthAttachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depthAttachmentDescription.format = VK_FORMAT_D32_SFLOAT_S8_UINT;
        depthAttachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        depthAttachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depthAttachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachmentDescription.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        attachmentDescription.push_back(depthAttachmentDescription);
    }
    return CreateRenderPass(device, { subPass }, { dependency }, attachmentDescription, renderPass);
}
VkResult vkf::CreateRenderPass(VkDevice device, const std::vector<VkSubpassDescription>&subpassDescription, const std::vector<VkSubpassDependency>&subpassDependency, const std::vector<VkAttachmentDescription>&attachmentDescriptions, VkRenderPass&renderpass){
    VkRenderPassCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    info.attachmentCount = attachmentDescriptions.size();
    info.pAttachments = attachmentDescriptions.data();
    info.dependencyCount = subpassDependency.size();
    info.pDependencies = subpassDependency.data();
    info.subpassCount = subpassDescription.size();
    info.pSubpasses = subpassDescription.data();
    return vkCreateRenderPass(device, &info, nullptr, &renderpass);
}
void vkf::CreateFrameBufferForSwapchain(VkDevice device, const VkExtent2D&swapchainExtent, VulkanWindows&vulkanWindows, VkCommandPool pool, VkQueue graphics, bool createDepthImage){
    uint32_t count;
	vkGetSwapchainImagesKHR(device, vulkanWindows.swapchain, &count, nullptr);
	std::vector<VkImage>swapchainImages(count);
	vkGetSwapchainImagesKHR(device, vulkanWindows.swapchain, &count, swapchainImages.data());
	vulkanWindows.swapchainImageViews.resize(swapchainImages.size());
	vulkanWindows.framebuffers.resize(swapchainImages.size());
	std::vector<VkImageView>frameBufferAttachments(1);
	if(createDepthImage){
        CreateDepthImage(device, swapchainExtent, vulkanWindows.depthImage);
        VkCommandBuffer cmd;//不变换图片布局绘制后也看不出问题
        tool::BeginSingleTimeCommands(device, pool, cmd);
        tool::SetImageLayout(cmd, vulkanWindows.depthImage.image, VK_IMAGE_ASPECT_DEPTH_BIT|VK_IMAGE_ASPECT_STENCIL_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT);
        tool::EndSingleTimeCommands(device, pool, graphics, cmd);
        frameBufferAttachments.push_back(vulkanWindows.depthImage.view);
    }
    VkImageViewCreateInfo imageViewInfo = {};
	for (size_t i = 0; i < vulkanWindows.framebuffers.size(); ++i){
        imageViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        imageViewInfo.image = swapchainImages[i];
        imageViewInfo.subresourceRange.levelCount = 1;
        imageViewInfo.subresourceRange.layerCount = 1;
        imageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        imageViewInfo.format = VK_FORMAT_B8G8R8A8_UNORM;
        // imageViewInfo.subresourceRange.baseMipLevel = 0;
        // imageViewInfo.subresourceRange.baseArrayLayer = 0;
        imageViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        vkCreateImageView(device, &imageViewInfo, nullptr, &vulkanWindows.swapchainImageViews[i]);
		frameBufferAttachments[0] = vulkanWindows.swapchainImageViews[i];
        CreateFrameBuffer(device, vulkanWindows.renderpass, swapchainExtent, frameBufferAttachments, vulkanWindows.framebuffers[i]);
	}
}
VkResult vkf::CreateFrameBuffer(VkDevice device, VkRenderPass renderPass, const VkExtent2D&extent, const std::vector<VkImageView>&attachments, VkFramebuffer&frameBuffer, uint32_t layers){
    VkFramebufferCreateInfo frameBufferInfo = {};
    frameBufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    frameBufferInfo.layers = layers;
    frameBufferInfo.width = extent.width;
    frameBufferInfo.height = extent.height;
    frameBufferInfo.renderPass = renderPass;
    frameBufferInfo.pAttachments = attachments.data();
    frameBufferInfo.attachmentCount = attachments.size();
    return vkCreateFramebuffer(device, &frameBufferInfo, nullptr, &frameBuffer);
}

VkResult vkf::CreateCommandPool(VkPhysicalDevice physicalDevice, VkDevice device, VkCommandPool&pool, VkCommandPoolCreateFlags flags){
    int graphicsFamily;
    tool::GetGraphicAndPresentQueueFamiliesIndex(physicalDevice, VK_NULL_HANDLE, &graphicsFamily);
	VkCommandPoolCreateInfo commandPoolInfo = {};
	commandPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	commandPoolInfo.flags = flags;
	commandPoolInfo.queueFamilyIndex = graphicsFamily;
	//commandPoolInfo.flags = flags | VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
	//commandPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	return vkCreateCommandPool(device, &commandPoolInfo, nullptr, &pool);
}
VkResult vkf::CreateDescriptorPool(VkDevice device, uint32_t descriptorCount, VkDescriptorPool&pool){
	std::vector<VkDescriptorPoolSize> descriptorPoolSize = {
        { VK_DESCRIPTOR_TYPE_SAMPLER, descriptorCount },
        { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, descriptorCount },
        { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, descriptorCount },
        { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, descriptorCount },
        { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, descriptorCount },
        { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, descriptorCount },
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, descriptorCount },
        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, descriptorCount },
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, descriptorCount },
        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, descriptorCount },
        { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, descriptorCount}
	};
	VkDescriptorPoolCreateInfo descriptorPoolInfo;
	descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	descriptorPoolInfo.flags = 0;
	descriptorPoolInfo.pNext = nullptr;
	descriptorPoolInfo.maxSets = descriptorCount;
	descriptorPoolInfo.poolSizeCount = descriptorPoolSize.size();
	descriptorPoolInfo.pPoolSizes = descriptorPoolSize.data();
	return vkCreateDescriptorPool(device, &descriptorPoolInfo, nullptr, &pool);
}
VkResult vkf::RenderFrame(VkDevice device, const VkCommandBuffer&commandbuffers, VkQueue graphics, const VkSemaphore&imageAcquired, const VkSemaphore&renderComplete, const VkFence&fence){
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

uint32_t vkf::PrepareFrame(VkDevice device, VkSwapchainKHR swapchain, const VkSemaphore&imageAcquired, void(*recreateSwapchain)(void* userData), void* userData, const VkFence&fence){
    uint32_t imageIndex;
    if(fence != VK_NULL_HANDLE)
        vkWaitForFences(device, 1, &fence, VK_TRUE, UINT16_MAX);
    VkResult result = vkAcquireNextImageKHR(device, swapchain, UINT64_MAX, imageAcquired, VK_NULL_HANDLE, &imageIndex);
    if (recreateSwapchain && result == VK_ERROR_OUT_OF_DATE_KHR) {
        recreateSwapchain(userData);
        return result;
    }
    else if(result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR){
        printf("failed to acquire swap chain image!\n");
        return result;
    }
    if (fence != VK_NULL_HANDLE)
        vkResetFences(device, 1, &fence);
    return imageIndex;
}

VkResult vkf::SubmitFrame(VkDevice device, uint32_t imageIndex, VkSwapchainKHR swapchain, const VkQueue present, const VkSemaphore&renderComplete, void(*recreateSwapchain)(void* userData), void* userData){
	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.pImageIndices = &imageIndex;
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = &swapchain;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = &renderComplete;
	VkResult result = vkQueuePresentKHR(present, &presentInfo);
	if (recreateSwapchain && (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)) {
		recreateSwapchain(userData);
	}
	else if(VK_SUCCESS != result){
		printf("failed to acquire swap chain image!\n");
	}
    return result;
}
void vkf::DrawFrame(VkDevice device, uint32_t currentFrame, const VkCommandBuffer& commandbuffers, VkSwapchainKHR swapchain, const VulkanQueue&vulkanQueue, const VulkanSynchronize&vulkanSynchronize, void(*recreateSwapchain)(void* userData), void* userData){
    uint32_t imageIndex = PrepareFrame(device, swapchain, vulkanSynchronize.imageAcquired[currentFrame], recreateSwapchain, userData, vulkanSynchronize.fences[currentFrame]);
    RenderFrame(device, commandbuffers, vulkanQueue.graphics, vulkanSynchronize.imageAcquired[currentFrame], vulkanSynchronize.renderComplete[currentFrame], vulkanSynchronize.fences[currentFrame]);
    SubmitFrame(device, imageIndex, swapchain, vulkanQueue.present, vulkanSynchronize.renderComplete[currentFrame], recreateSwapchain, userData);
}

uint32_t vkf::tool::findMemoryTypeIndex(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
	for (size_t i = 0; i < gMemoryProperties.memoryTypeCount; i++){
		if (typeFilter & (1 << i) && (gMemoryProperties.memoryTypes[i].propertyFlags & properties) == properties) {
			return i;
		}
	}
	return -1;
}

void vkf::CreateDepthImage(VkDevice device, const VkExtent2D&swapchainExtent, VulkanImage&image){
    image.CreateImage(device, swapchainExtent, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_FORMAT_D32_SFLOAT_S8_UINT);
    image.AllocateAndBindMemory(device, VK_MEMORY_HEAP_DEVICE_LOCAL_BIT);
    image.CreateImageView(device, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_IMAGE_ASPECT_DEPTH_BIT|VK_IMAGE_ASPECT_STENCIL_BIT);
}
void vkf::CreateTextureImage(VkDevice device, const void *datas, uint32_t width, uint32_t height, VulkanImage&image, VkCommandPool pool, VkQueue graphics){
	VulkanBuffer tempStorageBuffer;
	VkDeviceSize imageSize = width * height * 4;
	tempStorageBuffer.CreateBuffer(device, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
    tempStorageBuffer.AllocateAndBindMemory(device, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT|VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    tempStorageBuffer.UpdateData(device, imageSize, datas);
	CreateTextureImage(device, tempStorageBuffer, width, height, image, pool, graphics);
	vkFreeMemory(device, tempStorageBuffer.memory, nullptr);
	vkDestroyBuffer(device, tempStorageBuffer.buffer, nullptr);
}
void vkf::CreateTextureImage(VkDevice device, const VulkanBuffer&dataBuffer, uint32_t width, uint32_t height, VulkanImage&image, VkCommandPool pool, VkQueue graphics){
    image.size.depth = 1;
    image.size.width = width;
    image.size.height = height;
    image.CreateImage(device, VK_IMAGE_USAGE_TRANSFER_SRC_BIT|VK_IMAGE_USAGE_TRANSFER_DST_BIT|VK_IMAGE_USAGE_SAMPLED_BIT, VK_FORMAT_R8G8B8A8_UNORM);
    image.AllocateAndBindMemory(device, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	VkCommandBuffer cmd;
	tool::BeginSingleTimeCommands(device, pool, cmd);
    tool::SetImageLayout(cmd, image.image, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT);
	// pipelineBarrier(cmd, VK_FORMAT_R8G8B8A8_UNORM, , image);
    VkBufferImageCopy bufferCopyRegions = {};
    // bufferCopyRegions.bufferOffset = bufferOffset;
    bufferCopyRegions.imageSubresource.layerCount = 1;
    bufferCopyRegions.imageExtent = { width, height, 1 };
    // bufferCopyRegions.imageSubresource.baseArrayLayer = baseArrayLayer;
    bufferCopyRegions.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	vkCmdCopyBufferToImage(cmd, dataBuffer.buffer, image.image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &bufferCopyRegions);
    tool::SetImageLayout(cmd, image.image, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);
	tool::EndSingleTimeCommands(device, pool, graphics, cmd);
	image.CreateImageView(device, VK_FORMAT_R8G8B8A8_UNORM);
}
void vkf::CreateImageArray(VkDevice device, const void * const * datas, uint32_t imageCount, uint32_t width, uint32_t height, VulkanImage&image, VkCommandPool pool, VkQueue graphics){
	// size_t imageCount = datas.size();
	VulkanBuffer tempStorageBuffer;
	VkDeviceSize imageSize = width * height * 4;
	tempStorageBuffer.CreateBuffer(device, imageSize * imageCount, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
    tempStorageBuffer.AllocateAndBindMemory(device, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	for(uint32_t i = 0; i < imageCount; ++i){
		tempStorageBuffer.UpdateData(device, imageSize, datas[i], i * imageSize);
	}
	CreateImageArray(device, tempStorageBuffer, imageCount, width, height, image, pool, graphics);
	vkFreeMemory(device, tempStorageBuffer.memory, nullptr);
	vkDestroyBuffer(device, tempStorageBuffer.buffer, nullptr);
}
void vkf::CreateImageArray(VkDevice device, const VulkanBuffer&dataBuffer, uint32_t imageCount, uint32_t width, uint32_t height, VulkanImage&image, VkCommandPool pool, VkQueue graphics){
	const VkDeviceSize imageSize = width * height * 4;
    image.size.depth = 1;
    image.size.width = width;
    image.size.height = height;
    image.CreateImage(device, VK_IMAGE_USAGE_TRANSFER_SRC_BIT|VK_IMAGE_USAGE_TRANSFER_DST_BIT|VK_IMAGE_USAGE_SAMPLED_BIT, VK_FORMAT_R8G8B8A8_UNORM, imageCount);
    image.AllocateAndBindMemory(device, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	VkCommandBuffer cmd;
	tool::BeginSingleTimeCommands(device, pool, cmd);

	tool::SetImageLayout(cmd, image.image, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, imageCount);
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
	tool::SetImageLayout(cmd, image.image, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, imageCount);
	tool::EndSingleTimeCommands(device, pool, graphics, cmd);
	image.CreateImageView(device, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_VIEW_TYPE_2D_ARRAY, imageCount);
}
VkResult vkf::CreateTextureSampler(VkDevice device, VkSampler&sampler){
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
void vkf::tool::AllocateMemory(VkDevice device, VkDeviceSize size, uint32_t typeFilter, VkMemoryPropertyFlags properties, VkDeviceMemory&memory){
	VkMemoryAllocateInfo allocateInfo;
    allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocateInfo.pNext = nullptr;
    allocateInfo.allocationSize = size;
    allocateInfo.memoryTypeIndex = tool::findMemoryTypeIndex(typeFilter, properties);
	// VkMemoryAllocateInfo allocateInfo = vki::memoryAllocateInfo(memoryRequirements.size);
	// allocateInfo.memoryTypeIndex = findMemoryTypeIndex(memoryRequirements.memoryTypeBits, properties);
	vkAllocateMemory(device, &allocateInfo, nullptr, &memory);
}
VkResult vkf::tool::AllocateCommandBuffers(VkDevice device, VkCommandPool pool, uint32_t count, VkCommandBuffer *pCommandBuffers, VkCommandBufferLevel level){
    VkCommandBufferAllocateInfo allocateInfo = {};
    allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocateInfo.level = level;
    allocateInfo.commandPool = pool;
    allocateInfo.commandBufferCount = count;
    return vkAllocateCommandBuffers(device, &allocateInfo, pCommandBuffers);
}
void vkf::tool::CopyBuffer(VkDevice device, VkDeviceSize size, const void *pData, VkQueue graphics, VkCommandPool pool, VulkanBuffer&buffer){
    VulkanBuffer temporary;
    temporary.CreateBuffer(device, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
    temporary.AllocateAndBindMemory(device, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    temporary.UpdateData(device, size, pData);
    VkCommandBuffer cmd;
    VkBufferCopy regions = {};
    regions.size = size;
    BeginSingleTimeCommands(device, pool, cmd);
    vkCmdCopyBuffer(cmd, temporary.buffer, buffer.buffer, 1, &regions);
    EndSingleTimeCommands(device, pool, graphics, cmd);
    temporary.Destroy(device);
}

VkResult vkf::tool::BeginSingleTimeCommands(VkDevice device, VkCommandPool commandPool, VkCommandBuffer&commandBuffer){
    AllocateCommandBuffers(device, commandPool, 1, &commandBuffer);
	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	return vkBeginCommandBuffer(commandBuffer, &beginInfo);
}
void vkf::tool::EndSingleTimeCommands(VkDevice device, VkCommandPool commandPool, VkQueue graphics, VkCommandBuffer commandBuffer){
	vkEndCommandBuffer(commandBuffer);

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	vkQueueSubmit(graphics, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(graphics);

	vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
}

uint32_t vkf::tool::GetFileSize(FILE *fp){
    uint32_t size = 0;
    if(fp){
        fseek(fp, 0, SEEK_END);
        size = ftell(fp);
        fseek(fp, 0, SEEK_SET);
    }
    return size;
}
uint32_t vkf::tool::GetFileContent(const std::string&file, std::vector<uint32_t>&data){
    FILE *fp = fopen(file.c_str(), "rb");
    if(!fp){
        perror("open file error");
        printf("file is %s\n", file.c_str());
        return 0;
    }
    uint32_t size = GetFileSize(fp);
    data.resize(size / sizeof(uint32_t));
    fread(data.data(), size, 1, fp);
    fclose(fp);
    return size;
}
bool vkf::tool::WriteFileContent(const std::string&file, const void *data, uint32_t size){
    if(data == nullptr){
        printf("write file content error:data is nullptr\n");
        return false;
    }
    FILE *fp = fopen(file.c_str(), "wb");
    if(!fp){
        perror("open file error");
        printf("file name is %s\n", file.c_str());
        return false;
    }
    fwrite(data, size, 1, fp);
    fclose(fp);
    return true; 
}
void vkf::tool::SetImageLayout(VkCommandBuffer cmdbuffer, VkImage image, VkImageLayout oldImageLayout, VkImageLayout newImageLayout, VkImageSubresourceRange subresourceRange, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask){
	// Create an image barrier object
	VkImageMemoryBarrier imageMemoryBarrier {};
	imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	imageMemoryBarrier.oldLayout = oldImageLayout;
	imageMemoryBarrier.newLayout = newImageLayout;
	imageMemoryBarrier.image = image;
	imageMemoryBarrier.subresourceRange = subresourceRange;

	// Source layouts (old)
	// Source access mask controls actions that have to be finished on the old layout
	// before it will be transitioned to the new layout
	switch (oldImageLayout){
	case VK_IMAGE_LAYOUT_UNDEFINED:
		// Image layout is undefined (or does not matter)
		// Only valid as initial layout
		// No flags required, listed only for completeness
		imageMemoryBarrier.srcAccessMask = 0;
		break;

	case VK_IMAGE_LAYOUT_PREINITIALIZED:
		// Image is preinitialized
		// Only valid as initial layout for linear images, preserves memory contents
		// Make sure host writes have been finished
		imageMemoryBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
		break;

	case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
		// Image is a color attachment
		// Make sure any writes to the color buffer have been finished
		imageMemoryBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		break;

	case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
		// Image is a depth/stencil attachment
		// Make sure any writes to the depth/stencil buffer have been finished
		imageMemoryBarrier.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		break;

	case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
		// Image is a transfer source
		// Make sure any reads from the image have been finished
		imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
		break;

	case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
		// Image is a transfer destination
		// Make sure any writes to the image have been finished
		imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		break;

	case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
		// Image is read by a shader
		// Make sure any shader reads from the image have been finished
		imageMemoryBarrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
		break;
	case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:
		imageMemoryBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;//VK_ACCESS_TRANSFER_READ_BIT;
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
		imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		break;

	case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
		// Image will be used as a transfer source
		// Make sure any reads from the image have been finished
		imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
		break;

	case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
		// Image will be used as a color attachment
		// Make sure any writes to the color buffer have been finished
		imageMemoryBarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		break;

	case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
		// Image layout will be used as a depth/stencil attachment
		// Make sure any writes to depth/stencil buffer have been finished
		imageMemoryBarrier.dstAccessMask = imageMemoryBarrier.dstAccessMask | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		break;

	case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
		// Image will be read in a shader (sampler, input attachment)
		// Make sure any writes to the image have been finished
		if (imageMemoryBarrier.srcAccessMask == 0)
		{
			imageMemoryBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
		}
		imageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		break;
	case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:
		imageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_WRITE_BIT;//VK_ACCESS_TRANSFER_WRITE_BIT;
		break;
	default:
		// Other source layouts aren't handled (yet)
		break;
	}

	// Put barrier inside setup command buffer
	vkCmdPipelineBarrier(
		cmdbuffer,
		srcStageMask,
		dstStageMask,
		0,
		0, nullptr,
		0, nullptr,
		1, &imageMemoryBarrier);
}

// Fixed sub resource on first mip level and layer
void vkf::tool::SetImageLayout(VkCommandBuffer cmdbuffer, VkImage image, VkImageAspectFlags aspectMask, VkImageLayout oldImageLayout, VkImageLayout newImageLayout, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask){
    SetImageLayout(cmdbuffer, image, aspectMask, oldImageLayout, newImageLayout, srcStageMask, dstStageMask, 1);
}
void vkf::tool::SetImageLayout(VkCommandBuffer cmdbuffer, VkImage image, VkImageAspectFlags aspectMask, VkImageLayout oldImageLayout, VkImageLayout newImageLayout, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, uint32_t imageCount){
	VkImageSubresourceRange subresourceRange = {};
	subresourceRange.aspectMask = aspectMask;
	subresourceRange.baseMipLevel = 0;
	subresourceRange.levelCount = 1;
	subresourceRange.layerCount = imageCount;
	tool::SetImageLayout(cmdbuffer, image, oldImageLayout, newImageLayout, subresourceRange, srcStageMask, dstStageMask);
}
// vulkanFrame::vulkanFrame(/* args */){
    
// }
// vulkanFrame::~vulkanFrame(){
// }
// void vulkanFrame::CreateVulkan(const std::vector<const char*>&instanceExtensions, const std::vector<const char*>&deviceExtensions, void(*createSurfaceFun)(VkInstance, VkSurfaceKHR&, void*), void *userData){
//     CreateInstance(instanceExtensions);
//     EnumeratePhysicalDevices();
//     if(createSurfaceFun)createSurfaceFun(mInstance, mSurface, userData);
//     CreateDevice(deviceExtensions, mSurface);
//     VkDevice&device = mDevices[0];
//     int32_t queueFamily[] = { -1, -1 };
//     GetGraphicAndPresentQueueFamiliesIndex(mSurface, queueFamily);
// 	vkGetDeviceQueue(device, queueFamily[0], 0, &mGraphics);
// 	vkGetDeviceQueue(device, queueFamily[1], 0, &mPresent);

//     //显示设备信息
//     const char *deviceType;
//     switch (mPhysicalDeviceProperties.deviceType){
//     case VK_PHYSICAL_DEVICE_TYPE_CPU:
//         deviceType = "CPU";
//         break;
//     case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
//         deviceType = "DISCRETE GPU";
//         break;
//     case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
//         deviceType = "INTEGRATED GPU";
//         break;
//     case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
//         deviceType = "VIRTUAL GPU";
//         break;
//     default:
//         deviceType = "OTHER";
//         break;
//     }
// 	printf("gpu name:%s, gpu type:%s\n", mPhysicalDeviceProperties.deviceName, deviceType);
// }