#ifndef VULKAN_DEVICE_H
#define VULKAN_DEVICE_H
#include <vector>
#include <vulkan/vulkan.h>
#define PRESENT_QUEUE_INDEX 1
#define GRAPHICS_QUEUE_INDEX 0
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
    VkResult CreateDevice(VkSurfaceKHR surface, const std::vector<const char*>& deviceExtensions = {});
    //找不到合适的队列族则返回-1...如果surface不等于nullptr并且返回值不等于-1, 则返回的队列族必定包含呈现能力
    uint32_t GetQueueFamiliesIndex(VkQueueFlagBits queue, VkSurfaceKHR surface = VK_NULL_HANDLE);
    VkResult CreateDebugUtilsMessenger(PFN_vkDebugUtilsMessengerCallbackEXT debugUtilsMessenger);
    VkResult CreateInstance(const std::vector<const char*>&instanceExtensions = {}, VkApplicationInfo *pApplicationInfo = nullptr);
    VkResult AllocateMemory(VkDeviceSize size, uint32_t typeFilter, VkMemoryPropertyFlags properties, VkDeviceMemory&memory);

    void GetPhysicalDeviceProperties(VkPhysicalDeviceProperties&properties);

    void Cleanup();
private:
    uint32_t findMemoryTypeIndex(uint32_t typeFilter, VkMemoryPropertyFlags properties);
    void GetGraphicAndPresentQueueFamiliesIndex(VkSurfaceKHR surface, uint32_t queueFamilies[2]);
};
#endif