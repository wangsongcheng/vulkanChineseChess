#include <cstring>
#include <stdio.h>
#include "vulkanFrame.h"
static VkPhysicalDeviceMemoryProperties g_MemoryProperties;
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
    // VK_MEMORY_PROPERTY_HOST_COHERENT_BIT可以保证数据写入后立即更新到buffer。但会导致性能下降(废话)
    // 这种方式只使用于VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT应该用vkCmdCopyBuffer
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
    for (size_t i = 0; i < count; i++){
        vkGetPhysicalDeviceProperties(physicalDevices[i], &physicalDeviceProperties);
        if(physicalDeviceProperties.deviceType == deviceType){
            physicalDevice = physicalDevices[i];
            break;
        }
    }
    if(physicalDevice == VK_NULL_HANDLE){
        physicalDevice = physicalDevices[0];
    	vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);
    }
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &g_MemoryProperties);
    return physicalDevice;
}
const char *vkf::tool::cvmx_chip_type_to_string(VkResult type){
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
void vkf::tool::GetGraphicAndPresentQueueFamiliesIndex(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, uint32_t queueFamilies[2]){
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
    uint32_t queueFamily[2];
    uint32_t queueFamilyCount;
	const float queuePriorities = 1.0f;
    queueFamily[0] = -1;
    queueFamily[1] = -1;
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
    deviceCreateInfo.enabledLayerCount = 0;
    deviceCreateInfo.ppEnabledLayerNames = nullptr;
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
SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface) {
    SwapChainSupportDetails details;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

    if (formatCount != 0) {
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
    }

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

    if (presentModeCount != 0) {
        details.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
    }

    return details;
}
VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
    for (const auto& availableFormat : availableFormats) {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return availableFormat;
        }
    }

    return availableFormats[0];
}
// VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) {
//     if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
//         return capabilities.currentExtent;
//     } else {
//         int width, height;
//         glfwGetFramebufferSize(window, &width, &height);

//         VkExtent2D actualExtent = {
//             static_cast<uint32_t>(width),
//             static_cast<uint32_t>(height)
//         };

//         actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
//         actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

//         return actualExtent;
//     }
// }

VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
    for (const auto& availablePresentMode : availablePresentModes) {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
            return availablePresentMode;
        }
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}
VkResult vkf::CreateSwapchain(VkPhysicalDevice physicalDevice, VkDevice device, VulkanWindows&vulkanWindows){
    SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice, vulkanWindows.surface);

    VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
    VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
    // VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
    if(imageCount > 3)imageCount = 3;
    if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }
    vulkanWindows.swapchainInfo.format = surfaceFormat.format;
    vulkanWindows.swapchainInfo.extent = swapChainSupport.capabilities.currentExtent;

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = vulkanWindows.surface;

    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = swapChainSupport.capabilities.currentExtent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    uint32_t queueFamilyIndices[2];
    tool::GetGraphicAndPresentQueueFamiliesIndex(physicalDevice, vulkanWindows.surface, queueFamilyIndices);

    if (queueFamilyIndices[0] != queueFamilyIndices[1]) {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    } else {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }

    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;

    createInfo.oldSwapchain = VK_NULL_HANDLE;

    return vkCreateSwapchainKHR(device, &createInfo, nullptr, &vulkanWindows.swapchain);
}
void vkf::CreateSemaphoreAndFenceForSwapchain(VkDevice device, uint32_t swapchainImageCount, VulkanSynchronize&vulkanSynchronize){
	VkFenceCreateInfo fenceInfo;
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    fenceInfo.pNext = nullptr;
	VkSemaphoreCreateInfo semaphoreInfo = {};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    // uint32_t swapchainImageCount;
	// vkGetSwapchainImagesKHR(device, swapchain, &swapchainImageCount, nullptr);
    vulkanSynchronize.fences.resize(swapchainImageCount);
	vulkanSynchronize.imageAcquired.resize(swapchainImageCount);
	vulkanSynchronize.renderComplete.resize(swapchainImageCount);
	for (size_t i = 0; i < swapchainImageCount; ++i) {
    	vkCreateFence(device, &fenceInfo, nullptr, &vulkanSynchronize.fences[i]);
		vkCreateSemaphore(device, &semaphoreInfo, nullptr, &vulkanSynchronize.renderComplete[i]);
		vkCreateSemaphore(device, &semaphoreInfo, nullptr, &vulkanSynchronize.imageAcquired[i]);
	}
}
VkResult vkf::CreateRenderPassForSwapchain(VkDevice device, VkFormat swapchainFormat, VkRenderPass&renderPass, bool useDepthImage){
    VkSubpassDependency dependency = {};
    dependency.dstSubpass = 0;
    dependency.srcAccessMask = 0;
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
    attachmentDescription[0].format = swapchainFormat;
    attachmentDescription[0].samples = VK_SAMPLE_COUNT_1_BIT;
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
void vkf::CreateFrameBufferForSwapchain(VkDevice device, VulkanWindows&vulkanWindows, bool createDepthImage){
    uint32_t count;
	vkGetSwapchainImagesKHR(device, vulkanWindows.swapchain, &count, nullptr);
	std::vector<VkImage>swapchainImages(count);
	vkGetSwapchainImagesKHR(device, vulkanWindows.swapchain, &count, swapchainImages.data());
	vulkanWindows.swapchainImages.resize(swapchainImages.size());
	vulkanWindows.framebuffers.resize(swapchainImages.size());
	std::vector<VkImageView>frameBufferAttachments(1);
	if(createDepthImage){
        CreateDepthImage(device, vulkanWindows.swapchainInfo.extent, vulkanWindows.depthImage);
        frameBufferAttachments.push_back(vulkanWindows.depthImage.view);
    }
	for (size_t i = 0; i < vulkanWindows.framebuffers.size(); ++i){
        vulkanWindows.swapchainImages[i].image = swapchainImages[i];
        // vulkanWindows.swapchainImages[i].AllocateAndBindMemory(device, VK_MEMORY_HEAP_DEVICE_LOCAL_BIT);
        vulkanWindows.swapchainImages[i].CreateImageView(device, vulkanWindows.swapchainInfo.format);
		frameBufferAttachments[0] = vulkanWindows.swapchainImages[i].view;
        CreateFrameBuffer(device, vulkanWindows.renderpass, vulkanWindows.swapchainInfo.extent, frameBufferAttachments, vulkanWindows.framebuffers[i]);
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
    uint32_t graphicsFamily;
    tool::GetGraphicAndPresentQueueFamiliesIndex(physicalDevice, VK_NULL_HANDLE, &graphicsFamily);
	VkCommandPoolCreateInfo commandPoolInfo = {};
	commandPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	commandPoolInfo.flags = flags;
	commandPoolInfo.queueFamilyIndex = graphicsFamily;
	commandPoolInfo.flags = flags | VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
	commandPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
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

uint32_t vkf::PrepareFrame(VkDevice device, VkSwapchainKHR swapchain, const VkSemaphore&imageAcquired, void(*recreateSwapchain)(void* userData), void* userData){
    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(device, swapchain, UINT64_MAX, imageAcquired, VK_NULL_HANDLE, &imageIndex);
    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        vkDeviceWaitIdle(device);
        if(recreateSwapchain)recreateSwapchain(userData);
        return result;
    }
    else if(result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR){
        printf("failed to acquire swap chain image!\n");
        return result;
    }
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
    return vkQueuePresentKHR(present, &presentInfo);
}
VkResult vkf::DrawFrame(VkDevice device, uint32_t currentFrame, const VkCommandBuffer& commandbuffers, VkSwapchainKHR swapchain, const VulkanQueue&vulkanQueue, const VulkanSynchronize&vulkanSynchronize, void(*recreateSwapchain)(void* userData), void* userData){
    vkWaitForFences(device, 1, &vulkanSynchronize.fences[currentFrame], VK_TRUE, UINT64_MAX);
    vkResetFences(device, 1, &vulkanSynchronize.fences[currentFrame]);
    uint32_t imageIndex = PrepareFrame(device, swapchain, vulkanSynchronize.imageAcquired[currentFrame], recreateSwapchain, userData);
    
    RenderFrame(device, commandbuffers, vulkanQueue.graphics, vulkanSynchronize.imageAcquired[currentFrame], vulkanSynchronize.renderComplete[currentFrame], vulkanSynchronize.fences[currentFrame]);
    VkResult result = SubmitFrame(device, imageIndex, swapchain, vulkanQueue.present, vulkanSynchronize.renderComplete[currentFrame], recreateSwapchain, userData);
	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
        vkDeviceWaitIdle(device);
        //如果在结果等于VK_SUBOPTIMAL_KHR(一般发生在窗口最小化后)时重置栏杆, 会导致得不到回复栏杆对象的解锁
		if(result != VK_SUBOPTIMAL_KHR)vkResetFences(device, 1, &vulkanSynchronize.fences[currentFrame]);
        if(recreateSwapchain)recreateSwapchain(userData);
	}
	else if(VK_SUCCESS != result){
		printf("failed to acquire swap chain image!\n");
	}
    return result;
}

uint32_t vkf::tool::findMemoryTypeIndex(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
	for (size_t i = 0; i < g_MemoryProperties.memoryTypeCount; i++){
		if (typeFilter & (1 << i) && (g_MemoryProperties.memoryTypes[i].propertyFlags & properties) == properties) {
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
VkResult vkf::CreatePipelineCache(VkDevice device, const std::string&cacheFile, VkPipelineCache &cache){
    std::vector<uint32_t>cacheData;
    VkPipelineCacheCreateInfo cacheInfo = {};
    cacheInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
    if(vkf::tool::GetFileContent(cacheFile.c_str(), cacheData)){
        cacheInfo.initialDataSize = cacheData.size() * sizeof(uint32_t);
        cacheInfo.pInitialData = cacheData.data();
    }
    return vkCreatePipelineCache(device, &cacheInfo, nullptr, &cache);
}
void vkf::DestroyPipelineCache(VkDevice device, const std::string&cacheFile, VkPipelineCache &cache){
    if(cacheFile != ""){
        size_t cacheDataSize;
        std::vector<uint32_t>cacheData;
        vkGetPipelineCacheData(device, cache, &cacheDataSize, nullptr);
        cacheData.resize(cacheDataSize / sizeof(char));
        vkGetPipelineCacheData(device, cache, &cacheDataSize, cacheData.data());
        vkf::tool::WriteFileContent(cacheFile, cacheData.data(), cacheData.size() * sizeof(uint32_t));
    }
    if(cache != VK_NULL_HANDLE)vkDestroyPipelineCache(device, cache, nullptr);
}
void vkf::CreateFontImage(VkDevice device, const void *datas, uint32_t width, uint32_t height, VulkanImage &image, VkCommandPool pool, VkQueue graphics){
    VulkanBuffer tempStorageBuffer;
	VkDeviceSize imageSize = width * height * 4;//用于imgui的字体,原本没有乘4。
	tempStorageBuffer.CreateBuffer(device, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
    tempStorageBuffer.AllocateAndBindMemory(device, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT|VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    tempStorageBuffer.UpdateData(device, imageSize, datas);
	CreateFontImage(device, tempStorageBuffer, width, height, image, pool, graphics);
	tempStorageBuffer.Destroy(device);
}
void vkf::CreateTextureImage(VkDevice device, const void *datas, uint32_t width, uint32_t height, VulkanImage&image, VkCommandPool pool, VkQueue graphics){
	VulkanBuffer tempStorageBuffer;
	VkDeviceSize imageSize = width * height * 4;
	tempStorageBuffer.CreateBuffer(device, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
    tempStorageBuffer.AllocateAndBindMemory(device, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT|VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    tempStorageBuffer.UpdateData(device, imageSize, datas);
	CreateTextureImage(device, tempStorageBuffer, width, height, image, pool, graphics);
	tempStorageBuffer.Destroy(device);
}
void vkf::CreateFontImage(VkDevice device, const VulkanBuffer&dataBuffer, uint32_t width, uint32_t height, VulkanImage&image, VkCommandPool pool, VkQueue graphics){
    image.size.depth = 1;
    image.size.width = width;
    image.size.height = height;
    image.CreateImage(device, VK_IMAGE_USAGE_TRANSFER_DST_BIT|VK_IMAGE_USAGE_SAMPLED_BIT, VK_FORMAT_R8G8B8A8_UNORM);
    // image.CreateImage(device, VK_IMAGE_USAGE_TRANSFER_DST_BIT|VK_IMAGE_USAGE_SAMPLED_BIT, VK_FORMAT_R8_UNORM);
    image.AllocateAndBindMemory(device, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	VkCommandBuffer cmd;
	tool::BeginSingleTimeCommands(device, pool, cmd);
    tool::SetImageLayout(cmd, image.image, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT);
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
void vkf::CreateFontImageArray(VkDevice device, const void* datas, uint32_t imageCount, uint32_t width, uint32_t height, VulkanImage& image, VkCommandPool pool, VkQueue graphics){
    VulkanBuffer tempStorageBuffer;
    const VkDeviceSize imageSize = width * height;
    tempStorageBuffer.CreateBuffer(device, imageSize * imageCount, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
    tempStorageBuffer.AllocateAndBindMemory(device, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    tempStorageBuffer.UpdateData(device, imageSize * imageCount, datas);
    CreateFontImageArray(device, tempStorageBuffer, imageCount, width, height, image, pool, graphics);
    tempStorageBuffer.Destroy(device);
}
void vkf::CreateFontImageArray(VkDevice device, const VulkanBuffer& dataBuffer, uint32_t imageCount, uint32_t width, uint32_t height, VulkanImage& image, VkCommandPool pool, VkQueue graphics){
    const VkDeviceSize imageSize = width * height;
    image.size.depth = 1;
    image.size.width = width;
    image.size.height = height;
    image.CreateImage(device, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_FORMAT_R8_UNORM, imageCount);
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
    image.CreateImageView(device, VK_FORMAT_R8_UNORM, VK_IMAGE_VIEW_TYPE_2D_ARRAY, imageCount);
}
void vkf::CreateTextureImage(VkDevice device, const VulkanBuffer&dataBuffer, uint32_t width, uint32_t height, VulkanImage&image, VkCommandPool pool, VkQueue graphics){
    image.size.depth = 1;
    image.size.width = width;
    image.size.height = height;
    image.CreateImage(device, VK_IMAGE_USAGE_TRANSFER_DST_BIT|VK_IMAGE_USAGE_SAMPLED_BIT, VK_FORMAT_R8G8B8A8_UNORM);
    image.AllocateAndBindMemory(device, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	VkCommandBuffer cmd;
	tool::BeginSingleTimeCommands(device, pool, cmd);
    tool::SetImageLayout(cmd, image.image, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT);
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
void vkf::CreateCubeImage(VkDevice device, const void *datas, uint32_t width, VulkanImage &image, VkCommandPool pool, VkQueue graphics){
    CreateImageArray(device, datas, 6, width, width, image, pool, graphics, VK_IMAGE_VIEW_TYPE_CUBE);
}
void vkf::CreateCubeImage(VkDevice device, const void*const*datas, uint32_t width, VulkanImage&image, VkCommandPool pool, VkQueue graphics){
    CreateImageArray(device, datas, 6, width, width, image, pool, graphics, VK_IMAGE_VIEW_TYPE_CUBE);
}
void vkf::CreateCubeImage(VkDevice device, const VulkanBuffer &dataBuffer, uint32_t width, VulkanImage &image, VkCommandPool pool, VkQueue graphics){
    CreateImageArray(device, dataBuffer, 6, width, width, image, pool, graphics, VK_IMAGE_VIEW_TYPE_CUBE);
}
void vkf::CreateImageArray(VkDevice device, const void *datas, uint32_t imageCount, uint32_t width, uint32_t height, VulkanImage &image, VkCommandPool pool, VkQueue graphics, VkImageViewType type){
	VulkanBuffer tempStorageBuffer;
	const VkDeviceSize imageSize = width * height * 4;
	tempStorageBuffer.CreateBuffer(device, imageSize * imageCount, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
    tempStorageBuffer.AllocateAndBindMemory(device, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	tempStorageBuffer.UpdateData(device, imageSize * imageCount, datas);
	CreateImageArray(device, tempStorageBuffer, imageCount, width, height, image, pool, graphics, type);
	tempStorageBuffer.Destroy(device);
}
void vkf::CreateImageArray(VkDevice device, const void*const*datas, uint32_t imageCount, uint32_t width, uint32_t height, VulkanImage&image, VkCommandPool pool, VkQueue graphics, VkImageViewType type){
	VulkanBuffer tempStorageBuffer;
	const VkDeviceSize imageSize = width * height * 4;
	tempStorageBuffer.CreateBuffer(device, imageSize * imageCount, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
    tempStorageBuffer.AllocateAndBindMemory(device, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    for (size_t i = 0; i < imageCount; i++){
	    tempStorageBuffer.UpdateData(device, imageSize, datas[i], i * imageSize);
    }
	CreateImageArray(device, tempStorageBuffer, imageCount, width, height, image, pool, graphics, type);
	tempStorageBuffer.Destroy(device);
}
void vkf::CreateImageArray(VkDevice device, const VulkanBuffer&dataBuffer, uint32_t imageCount, uint32_t width, uint32_t height, VulkanImage&image, VkCommandPool pool, VkQueue graphics, VkImageViewType type){
	const VkDeviceSize imageSize = width * height * 4;
    image.size.depth = 1;
    image.size.width = width;
    image.size.height = height;
    image.CreateImage(device, VK_IMAGE_USAGE_TRANSFER_DST_BIT|VK_IMAGE_USAGE_SAMPLED_BIT, VK_FORMAT_R8G8B8A8_UNORM, imageCount);
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
	image.CreateImageView(device, VK_FORMAT_R8G8B8A8_UNORM, type, imageCount);
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
VkResult vkf::CreateDescriptorSetLayout(VkDevice device, uint32_t setlayoutBindingCount, const VkDescriptorSetLayoutBinding *setlayoutBindings, VkDescriptorSetLayout *setlayout){
    VkDescriptorSetLayoutCreateInfo descriptorSetLayout{};
    descriptorSetLayout.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptorSetLayout.pBindings = setlayoutBindings;
    descriptorSetLayout.bindingCount = setlayoutBindingCount;
    return vkCreateDescriptorSetLayout(device, &descriptorSetLayout, nullptr, setlayout);
}
void vkf::tool::AllocateMemory(VkDevice device, VkDeviceSize size, uint32_t typeFilter, VkMemoryPropertyFlags properties, VkDeviceMemory &memory){
    VkMemoryAllocateInfo allocateInfo;
    allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocateInfo.pNext = nullptr;
    allocateInfo.allocationSize = size;
    allocateInfo.memoryTypeIndex = tool::findMemoryTypeIndex(typeFilter, properties);
	vkAllocateMemory(device, &allocateInfo, nullptr, &memory);
}
VkResult vkf::tool::AllocateDescriptorSets(VkDevice device, VkDescriptorPool pool, const VkDescriptorSetLayout *setlayout, uint32_t count, VkDescriptorSet *pDescriptorSet){
    VkDescriptorSetAllocateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    info.descriptorPool = pool;
    info.pSetLayouts = setlayout;
    info.descriptorSetCount = count;
    return vkAllocateDescriptorSets(device, &info, pDescriptorSet);
}
VkResult vkf::tool::AllocateCommandBuffers(VkDevice device, VkCommandPool pool, uint32_t count, VkCommandBuffer *pCommandBuffers, VkCommandBufferLevel level){
    VkCommandBufferAllocateInfo allocateInfo = {};
    allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocateInfo.level = level;
    allocateInfo.commandPool = pool;
    allocateInfo.commandBufferCount = count;
    return vkAllocateCommandBuffers(device, &allocateInfo, pCommandBuffers);
}
void vkf::tool::UpdateDescriptorSets(VkDevice device, uint32_t setlayoutBindingCount, const VkDescriptorSetLayoutBinding *setlayoutBindings, const std::vector<VulkanBuffer> &descriptorBuffer, const std::vector<VulkanImage> &descriptorImage, VkDescriptorSet &destSet, const VkSampler &textureSampler){
    std::vector<uint32_t>index(2);//一个uniform一个图片采样器。如果需要其他则个数必须增加
    std::vector<VkWriteDescriptorSet>writeDescriptorSet(setlayoutBindingCount);
    std::vector<VkDescriptorImageInfo>descriptorImageInfo(descriptorImage.size());
    std::vector<VkDescriptorBufferInfo> descriptorBufferInfo(descriptorBuffer.size());
    for (size_t i = 0; i < writeDescriptorSet.size(); ++i){
        writeDescriptorSet[i].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writeDescriptorSet[i].dstSet = destSet;
        writeDescriptorSet[i].descriptorCount = 1;
        writeDescriptorSet[i].dstBinding = setlayoutBindings[i].binding;
        writeDescriptorSet[i].descriptorType = setlayoutBindings[i].descriptorType;
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
           descriptorBufferInfo[i].offset = 0;
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
void vkf::tool::CopyBuffer(VkDevice device, VkDeviceSize size, const void *pData, VkQueue graphics, VkCommandPool pool, VulkanBuffer &buffer){
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
VkResult vkf::tool::BeginCommands(VkCommandBuffer command, VkCommandBufferUsageFlags flags){
	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = flags;

	return vkBeginCommandBuffer(command, &beginInfo);
}
VkResult vkf::tool::BeginSingleTimeCommands(VkDevice device, VkCommandPool commandPool, VkCommandBuffer&command){
    AllocateCommandBuffers(device, commandPool, 1, &command);
    return BeginCommands(command, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
}
void vkf::tool::EndSingleTimeCommands(VkDevice device, VkCommandPool commandPool, VkQueue graphics, VkCommandBuffer command){
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

	vkFreeCommandBuffers(device, commandPool, 1, &command);
}
void vkf::tool::BeginRenderPassGeneral(VkCommandBuffer command, VkFramebuffer frame, VkRenderPass renderpass, uint32_t windowWidth, uint32_t windowHeight, bool enableDepth){
    std::vector<VkClearValue> clearValues(2);
    clearValues[0].color = { 0 , 0 , 0 , 1 };
    clearValues[0].color = { 0.1f , 0.1f , 0.1f , 1.0f };
    if(enableDepth)clearValues[1].depthStencil = { 1.0f , 0 };
    BeginRenderPass(command, frame, renderpass, windowWidth, windowHeight, enableDepth?2:clearValues.size(), clearValues.data());
}
void vkf::tool::BeginRenderPass(VkCommandBuffer command, VkFramebuffer frame, VkRenderPass renderpass, uint32_t windowWidth, uint32_t windowHeight, uint32_t clearValueCount, VkClearValue *pClearValues, int32_t renderAreaWidthOffset, int32_t renderAreaHeightOffset){
    VkRenderPassBeginInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    info.framebuffer = frame;
    info.renderPass = renderpass;
    info.pClearValues = pClearValues;
    info.clearValueCount = clearValueCount;
    info.renderArea = { renderAreaWidthOffset, renderAreaHeightOffset, windowWidth, windowHeight };
    vkCmdBeginRenderPass(command, &info, VK_SUBPASS_CONTENTS_INLINE);
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
void vkf::tool::InsertImageMemoryBarrier(VkCommandBuffer cmdbuffer, VkImage image, VkAccessFlags srcAccessMask, VkAccessFlags dstAccessMask, VkImageLayout oldImageLayout, VkImageLayout newImageLayout, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, VkImageSubresourceRange subresourceRange){
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