#include "VulkanWindow.h"
struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};
SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface) {
    SwapChainSupportDetails details;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &details.capabilities);

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, nullptr);

    if (formatCount != 0) {
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, details.formats.data());
    }

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, nullptr);

    if (presentModeCount != 0) {
        details.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, details.presentModes.data());
    }

    return details;
}
VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
    for (const auto& availableFormat : availableFormats) {
        if (availableFormat.format == SWAPCHAIN_FORMAT && availableFormat.colorSpace == SWAPCHAIN_COLOR_SPACE) {
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

VkResult VulkanSwapchain::CreateSwapchain(VulkanDevice device, VkSurfaceKHR surface){
    SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device.physicalDevice, surface);

    VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
    VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
    // VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
    if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }
    format = surfaceFormat.format;
    extent = swapChainSupport.capabilities.currentExtent;

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = surface;

    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = swapChainSupport.capabilities.currentExtent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    uint32_t queueFamilyIndices[2];
    device.GetGraphicAndPresentQueueFamiliesIndex(surface, queueFamilyIndices);

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

    return vkCreateSwapchainKHR(device.device, &createInfo, nullptr, &swapchain);
}

void VulkanSwapchain::Cleanup(VkDevice device){
    vkDestroySwapchainKHR(device, swapchain, nullptr);
    for (size_t i = 0; i < images.size(); ++i){
        images[i].image = VK_NULL_HANDLE;//因为image不是自己创建的所以不能销毁
        images[i].Destroy(device);
    }
}
void VulkanWindow::Cleanup(VulkanDevice device){
    CleanupSwapchain(device.device);
    swapchain.Cleanup(device.device);
    vkDestroySurfaceKHR(device.instance, surface, nullptr);
}
void VulkanWindow::CleanupSwapchain(VkDevice device){
    for (size_t i = 0; i < framebuffers.size(); ++i){
        vkDestroyFramebuffer(device, framebuffers[i], nullptr);
    }
    depthImage.Destroy(device);

    vkDestroyRenderPass(device, renderPass, nullptr);
}
void VulkanWindow::CreateFrameBuffer(VulkanDevice device, bool createDepthImage){
    uint32_t count;
    vkGetSwapchainImagesKHR(device.device, swapchain.swapchain, &count, nullptr);
    std::vector<VkImage>swapchainImages(count);
    vkGetSwapchainImagesKHR(device.device, swapchain.swapchain, &count, swapchainImages.data());
    swapchain.images.resize(swapchainImages.size());
    framebuffers.resize(swapchainImages.size());
    std::vector<VkImageView>frameBufferAttachments(1);
    if(createDepthImage){
        CreateDepthImage(device, swapchain.extent, depthImage);
        frameBufferAttachments.push_back(depthImage.view);
    }
    VkFramebufferCreateInfo frameBufferInfo = {};
    frameBufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    frameBufferInfo.layers = 1;
    frameBufferInfo.renderPass = renderPass;
    frameBufferInfo.width = swapchain.extent.width;
    frameBufferInfo.height = swapchain.extent.height;
    frameBufferInfo.pAttachments = frameBufferAttachments.data();
    frameBufferInfo.attachmentCount = frameBufferAttachments.size();
    for (size_t i = 0; i < framebuffers.size(); ++i){
        swapchain.images[i].image = swapchainImages[i];
        // swapchain.images[i].AllocateAndBindMemory(device, VK_MEMORY_HEAP_DEVICE_LOCAL_BIT);
        swapchain.images[i].CreateImageView(device.device, swapchain.format);
        frameBufferAttachments[0] = swapchain.images[i].view;
        vkCreateFramebuffer(device.device, &frameBufferInfo, nullptr, &framebuffers[i]);
    }
}
VkResult VulkanWindow::CreateRenderPass(VkDevice device, bool useDepthImage){
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
    attachmentDescription[0].format = swapchain.format;
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
    VkRenderPassCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    info.dependencyCount = 1;
    info.pDependencies = &dependency;
    info.subpassCount = 1;
    info.pSubpasses = &subPass;
    info.pAttachments = attachmentDescription.data();
    info.attachmentCount = attachmentDescription.size();
    return vkCreateRenderPass(device, &info, nullptr, &renderPass);
}
void VulkanWindow::CreateDepthImage(VulkanDevice device, const VkExtent2D &swapchainExtent, VulkanImage &image){
    image.CreateImage(device.device, swapchainExtent, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_FORMAT_D32_SFLOAT_S8_UINT);
    image.AllocateAndBindMemory(device, VK_MEMORY_HEAP_DEVICE_LOCAL_BIT);
    image.CreateImageView(device.device, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_IMAGE_ASPECT_DEPTH_BIT|VK_IMAGE_ASPECT_STENCIL_BIT);
}