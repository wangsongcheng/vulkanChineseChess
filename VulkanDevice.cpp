#include <iostream>
#include "VulkanDevice.h"
bool VulkanDevice::GetPhysicalDevices(bool (*GetPhysicalDevices)(VkPhysicalDevice)){
    uint32_t count;
	vkEnumeratePhysicalDevices(instance, &count, nullptr);
    if(count == 0){
		printf("No suitable graphics card!\n");
		return false;
    }
    std::vector<VkPhysicalDevice>physicalDevices(count);
    vkEnumeratePhysicalDevices(instance, &count, physicalDevices.data());
    if(GetPhysicalDevices){
        for (size_t i = 0; i < count; i++){
            if(GetPhysicalDevices(physicalDevices[i])){
                physicalDevice = physicalDevices[i];
                break;
            }
        }
    }
    if(physicalDevice == VK_NULL_HANDLE){
        physicalDevice = physicalDevices[0];
    }
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memoryProperties);
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);
	queueFamiliesProperties.resize(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamiliesProperties.data());
    return true;
}
uint32_t VulkanDevice::GetQueueFamiliesIndex(VkQueueFlagBits queue){
    int32_t queueFamilyIndex = -1;
	for (uint32_t i = 0; i < queueFamiliesProperties.size(); ++i) {
        if (queueFamiliesProperties[i].queueCount > 0 && queueFamiliesProperties[i].queueFlags & queue) {
            queueFamilyIndex = i;
            break;
        }
    }
    return queueFamilyIndex;
}
VkResult VulkanDevice::CreateInstance(const std::vector<const char *> &instanceExtensions, VkApplicationInfo *pApplicationInfo){
    std::vector<const char *>layers;
    std::vector<const char *>extensions = instanceExtensions;
    layers.push_back("VK_LAYER_KHRONOS_validation");
    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    VkInstanceCreateInfo instanceInfo = {};
    instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceInfo.enabledLayerCount = layers.size();
    instanceInfo.ppEnabledLayerNames = layers.data();
    instanceInfo.pApplicationInfo = pApplicationInfo;
    instanceInfo.enabledExtensionCount = extensions.size();
    instanceInfo.ppEnabledExtensionNames = extensions.data();
    return vkCreateInstance(&instanceInfo, nullptr, &instance);
}
void VulkanDevice::Cleanup(){
    PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessengerEXT = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    vkDestroyDebugUtilsMessengerEXT(instance, messenger, nullptr);
    vkDestroyDevice(device, nullptr);
    vkDestroyInstance(instance, nullptr);
}
VkResult VulkanDevice::CreateDevice(VkSurfaceKHR surface, const std::vector<const char *> &deviceExtensions){
	const float queuePriorities = 1.0f;
    std::set<uint32_t>uniqueQueueFamilies = {GetQueueFamiliesIndex(VK_QUEUE_GRAPHICS_BIT), GetQueueFamiliesIndex(surface)};
    std::vector<VkDeviceQueueCreateInfo>queueCreateInfos;
    VkDeviceQueueCreateInfo queueInfo = {};
    queueInfo.queueCount = 1;
    queueInfo.pQueuePriorities = &queuePriorities;
    queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    for(auto queueFamily:uniqueQueueFamilies){
        queueInfo.queueFamilyIndex = queueFamily;
        queueCreateInfos.push_back(queueInfo);
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
uint32_t VulkanDevice::GetQueueFamiliesIndex(VkSurfaceKHR surface){
    int32_t queueFamilyIndex = -1;
    VkBool32 presentSupport = false;
    if (surface != VK_NULL_HANDLE){
        //该队列族有呈现能力即可
        for (uint32_t i = 0; i < queueFamiliesProperties.size(); ++i) {
            vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &presentSupport);
            if (presentSupport) {
                queueFamilyIndex = i;
                break;
            }
        }
    }
    return queueFamilyIndex;
}
VkResult VulkanDevice::CreateDebugUtilsMessenger(PFN_vkDebugUtilsMessengerCallbackEXT debugUtilsMessenger){
    PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessenger = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    VkDebugUtilsMessengerCreateInfoEXT debugInfo = {};
    debugInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    debugInfo.pfnUserCallback = debugUtilsMessenger;
    debugInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    debugInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    return vkCreateDebugUtilsMessenger(instance, &debugInfo, nullptr, &messenger);
}
VkResult VulkanDevice::AllocateMemory(VkDeviceSize size, uint32_t typeFilter, VkMemoryPropertyFlags properties, VkDeviceMemory &memory){
    VkMemoryAllocateInfo allocateInfo;
    allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocateInfo.pNext = nullptr;
    allocateInfo.allocationSize = size;
    allocateInfo.memoryTypeIndex = findMemoryTypeIndex(typeFilter, properties);
    if (allocateInfo.memoryTypeIndex == -1u) {
        throw std::runtime_error("Failed to find suitable memory type!");
    }
	return vkAllocateMemory(device, &allocateInfo, nullptr, &memory);
}
const VkPhysicalDeviceProperties&VulkanDevice::GetPhysicalDeviceProperties(){
    return physicalDeviceProperties;
}
uint32_t VulkanDevice::findMemoryTypeIndex(uint32_t typeFilter, VkMemoryPropertyFlags properties){
    for (size_t i = 0; i < memoryProperties.memoryTypeCount; i++){
		if (typeFilter & (1 << i) && (memoryProperties.memoryTypes[i].propertyFlags & properties) == properties) {
			return i;
		}
	}
	return -1;
}