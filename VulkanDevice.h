#ifndef VULKAN_DEVICE_H
#define VULKAN_DEVICE_H
#include <set>
#include <vector>
#include <vulkan/vulkan.h>
struct VulkanDevice{//以后如果想改成类, 可以考虑在类内保存device
    VkDevice device = VK_NULL_HANDLE;
    VkInstance instance = VK_NULL_HANDLE;
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkDebugUtilsMessengerEXT messenger = VK_NULL_HANDLE;
    uint32_t GetQueueFamiliesIndex(VkSurfaceKHR surface);
    uint32_t GetQueueFamiliesIndex(VkQueueFlagBits queue);
    bool GetPhysicalDevices(bool (*GetPhysicalDevices)(VkPhysicalDevice) = nullptr);
    VkResult CreateDevice(VkSurfaceKHR surface, const std::vector<const char*>& deviceExtensions = {});
    //找不到合适的队列族则返回-1...如果surface不等于nullptr并且返回值不等于-1, 则返回的队列族必定包含呈现能力
    VkResult CreateDebugUtilsMessenger(PFN_vkDebugUtilsMessengerCallbackEXT debugUtilsMessenger);
    VkResult CreateInstance(const std::vector<const char*>&instanceExtensions = {}, VkApplicationInfo *pApplicationInfo = nullptr);
    VkResult AllocateMemory(VkDeviceSize size, uint32_t typeFilter, VkMemoryPropertyFlags properties, VkDeviceMemory&memory);

    const VkPhysicalDeviceProperties&GetPhysicalDeviceProperties();

    void Cleanup();
private:
    VkPhysicalDeviceProperties physicalDeviceProperties = {};
    VkPhysicalDeviceMemoryProperties memoryProperties = {};
    std::vector<VkQueueFamilyProperties>queueFamiliesProperties;
    uint32_t findMemoryTypeIndex(uint32_t typeFilter, VkMemoryPropertyFlags properties);
};
#endif