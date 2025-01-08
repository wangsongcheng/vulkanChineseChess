#ifndef VULKAN_DEVICE_H
#define VULKAN_DEVICE_H
#include <vector>
#include <vulkan/vulkan.h>
struct VulkanQueue{
    VkQueue present = VK_NULL_HANDLE;
    VkQueue graphics = VK_NULL_HANDLE;
};
struct VulkanDevice{//以后如果想改成类, 可以考虑在类内保存device
    VkDevice device = VK_NULL_HANDLE;
    VkInstance instance = VK_NULL_HANDLE;
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkDebugUtilsMessengerEXT messenger = VK_NULL_HANDLE;
    VkPhysicalDeviceProperties physicalDeviceProperties = {};
    VkPhysicalDeviceMemoryProperties memoryProperties = {};
    bool GetPhysicalDevices(bool (*GetPhysicalDevices)(VkPhysicalDevice) = nullptr);
    void GetGraphicAndPresentQueueFamiliesIndex(VkSurfaceKHR surface, uint32_t queueFamilies[2]);
    VkResult CreateDevice(VkSurfaceKHR surface, const std::vector<const char*>& deviceExtensions = {});
    VkResult CreateDebugUtilsMessenger(PFN_vkDebugUtilsMessengerCallbackEXT debugUtilsMessenger);
    VkResult CreateInstance(const std::vector<const char*>&instanceExtensions = {}, VkApplicationInfo *pApplicationInfo = nullptr);
    VkResult AllocateMemory(VkDeviceSize size, uint32_t typeFilter, VkMemoryPropertyFlags properties, VkDeviceMemory&memory);

    void GetPhysicalDeviceProperties(VkPhysicalDeviceProperties&properties);

    void Cleanup();
private:
    uint32_t findMemoryTypeIndex(uint32_t typeFilter, VkMemoryPropertyFlags properties);
};
#endif