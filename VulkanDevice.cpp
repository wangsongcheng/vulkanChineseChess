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
    vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memoryProperties);
    return true;
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
void VulkanDevice::GetGraphicAndPresentQueueFamiliesIndex(VkSurfaceKHR surface, uint32_t queueFamilies[2]){
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
VkResult VulkanDevice::CreateDevice(VkSurfaceKHR surface, const std::vector<const char *> &deviceExtensions){
    uint32_t queueFamily[2];
    uint32_t queueFamilyCount;
	const float queuePriorities = 1.0f;
    queueFamily[0] = -1;
    queueFamily[1] = -1;
    GetGraphicAndPresentQueueFamiliesIndex(surface, queueFamily);
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
	return vkAllocateMemory(device, &allocateInfo, nullptr, &memory);
}
void VulkanDevice::GetPhysicalDeviceProperties(VkPhysicalDeviceProperties &properties){
    properties = physicalDeviceProperties;
}
uint32_t VulkanDevice::findMemoryTypeIndex(uint32_t typeFilter, VkMemoryPropertyFlags properties){
    for (size_t i = 0; i < memoryProperties.memoryTypeCount; i++){
		if (typeFilter & (1 << i) && (memoryProperties.memoryTypes[i].propertyFlags & properties) == properties) {
			return i;
		}
	}
	return -1;
}