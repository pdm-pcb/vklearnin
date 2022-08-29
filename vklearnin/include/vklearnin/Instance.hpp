#ifndef VKLEARNIN_INSTANCE_HPP
#define VKLEARNIN_INSTANCE_HPP

#include <vulkan/vulkan.h>

// -----------------------------------------------------------------------------
// Thank goodness for macros - the below are responsible for locating and
// assigning the various function pointers from within the GPU driver.

#define GET_INSTANCE_PROC_ADDR(inst, entry) {                             \
    _##entry = (PFN_vk##entry)::vkGetInstanceProcAddr(inst, "vk" #entry); \
    if (_##entry == nullptr) {                                            \
        CONSOLE_WARN("vkGetInstanceProcAddr failed to find vk" #entry);   \
    }                                                                     \
}

#define GET_DEVICE_PROC_ADDR(dev, entry) {                             \
    _##entry = (PFN_vk##entry)::vkGetDeviceProcAddr(dev, "vk" #entry); \
    if (_##entry == nullptr) {                                         \
        CONSOLE_WARN("vkGetDeviceProcAddr failed to find vk" #entry);  \
    }                                                                  \
}

class CommandQueues;
class Pipeline;
class Swapchain;
class Framebuffers;

#if defined(__linux__)
    class X11Window;
#elif defined(_WIN32)
    class Win32Window;
#endif

// =============================================================================
class Instance {

#if defined(__linux__)
    using Window = X11Window;
#elif defined(_WIN32)
    using Window = Win32Window;
#endif

public:
    // -------------------------------------------------------------------------
    // Setup

    void init_instance();        // Initial setup
    void init_instance_procs();  // Find the instance-specific functions
    void init_physical_device(); // Get the physical devices

    void init_logical_device(const CommandQueues &command_queues);
    void init_logical_device_procs(); // Find the device-specific functions

    // -------------------------------------------------------------------------
    // For those concerned with instance atributes

    inline const ::VkInstance& vulkan_instance() const {
        return _instance;
    }
    inline const ::VkPhysicalDevice & physical_device() const {
        return _physical_device;
    }
    inline const ::VkDevice & logical_device() const {
        return _logical_device;
    }
    inline float max_anisotropy() const {
        return _max_anisotropy;
    }

    // -------------------------------------------------------------------------
    // Instance function pointers

    ::PFN_vkGetPhysicalDeviceSurfaceSupportKHR
        _GetPhysicalDeviceSurfaceSupportKHR;
    ::PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR
        _GetPhysicalDeviceSurfaceCapabilitiesKHR;
    ::PFN_vkGetPhysicalDeviceSurfaceFormatsKHR
        _GetPhysicalDeviceSurfaceFormatsKHR;
    ::PFN_vkGetPhysicalDeviceSurfacePresentModesKHR
        _GetPhysicalDeviceSurfacePresentModesKHR;
    ::PFN_vkGetPhysicalDeviceProperties2
        _GetPhysicalDeviceProperties2;

    // Debug layer function pointers (also instance functions)
    ::PFN_vkCreateDebugUtilsMessengerEXT  _CreateDebugUtilsMessengerEXT;
    ::PFN_vkDestroyDebugUtilsMessengerEXT _DestroyDebugUtilsMessengerEXT;

    // -------------------------------------------------------------------------
    // Logical device function pointers

    ::PFN_vkCreateSwapchainKHR    _CreateSwapchainKHR;
    ::PFN_vkDestroySwapchainKHR   _DestroySwapchainKHR;
    ::PFN_vkGetSwapchainImagesKHR _GetSwapchainImagesKHR;
    ::PFN_vkAcquireNextImageKHR   _AcquireNextImageKHR;
    ::PFN_vkQueuePresentKHR       _QueuePresentKHR;

    Instance();
    ~Instance();

private:
    // Handles
    ::VkInstance       _instance;
    ::VkPhysicalDevice _physical_device;
    ::VkDevice         _logical_device;
    float _max_anisotropy;
};

#endif // VKLEARNIN_INSTANCE_HPP