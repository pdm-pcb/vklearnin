#include "common.hpp"
#include "Instance.hpp"

#include "CommandQueues.hpp"

#if defined(__linux__)
    #include "X11Window.hpp"
#elif defined(_WIN32)
    #include "Win32Window.hpp"
#endif

// =============================================================================
void Instance::init_instance() {
    CONSOLE_INFO("");
    
    // -------------------------------------------------------------------------
    // Application Information

    ::VkApplicationInfo app_info { };
    app_info.sType = ::VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pApplicationName = APPLICATION_NAME;
    app_info.pEngineName = ENGINE_NAME;
    app_info.apiVersion = VK_API_VERSION_1_3;
    
    // -------------------------------------------------------------------------
    // The instance extensions to enable

    const char *instance_extensions[] {
        // we'll want a surface to draw to
        VK_KHR_SURFACE_EXTENSION_NAME,

// platform-specific surfaces
#if defined(__linux__)
        VK_KHR_XCB_SURFACE_EXTENSION_NAME,
#elif defined(_WIN32)
        VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
#endif

#ifdef VK_VALIDATION_LAYER
        VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
#endif // VK_VALIDATION_LAYER
    };

    // -------------------------------------------------------------------------
    // Instance Creation Information (wants app info from above)

    ::VkInstanceCreateInfo create_info { };
    create_info.sType = ::VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    create_info.pApplicationInfo = &app_info;
    create_info.enabledExtensionCount =
        static_cast<uint32_t>(std::size(instance_extensions));
    create_info.ppEnabledExtensionNames = instance_extensions;

#ifdef VK_VALIDATION_LAYER
    const char *layers[] { "VK_LAYER_KHRONOS_validation" };
    create_info.enabledLayerCount = static_cast<uint32_t>(std::size(layers));
    create_info.ppEnabledLayerNames = layers;
#endif // VK_VALIDATION_LAYER

    ::VkResult result = vkCreateInstance(&create_info, nullptr, &_instance);

    // For the following error on MacOS, some fiddling must be done:
    // https://vulkan-tutorial.com/Drawing_a_triangle/Setup/Instance#page_Encountered-VK_ERROR_INCOMPATIBLE_DRIVER
    if(result == ::VK_ERROR_INCOMPATIBLE_DRIVER) {
        CONSOLE_CRITICAL("Incompatible Vulkan driver version.");
    }
    else if(result != ::VK_SUCCESS) {
        CONSOLE_CRITICAL("Failed to create Vulkan instance.");
    }

// grab the function addresses for the debug utility
#ifdef VK_VALIDATION_LAYER
    GET_INSTANCE_PROC_ADDR(_instance, CreateDebugUtilsMessengerEXT);
    GET_INSTANCE_PROC_ADDR(_instance, DestroyDebugUtilsMessengerEXT);
    VKDebugger::init(*this);
#endif // VK_VALIDATION_LAYER

    // -------------------------------------------------------------------------
    // Query and populate the list of instance extensions

    uint32_t extension_count = 0;
    ::vkEnumerateInstanceExtensionProperties(
        nullptr,
        &extension_count,
        nullptr
    );

    CONSOLE_TRACE("Found {} instance extensions.", extension_count);    
    std::vector<::VkExtensionProperties> extensions(extension_count);

    ::vkEnumerateInstanceExtensionProperties(
        nullptr,
        &extension_count,
        extensions.data()
    );

    for(const auto &extension : extensions) {
        CONSOLE_TRACE("\t{:s}", extension.extensionName);
    }
}

// =============================================================================
void Instance::init_physical_device() {
    CONSOLE_INFO("");

    // -------------------------------------------------------------------------
    // Query and populate the list of physical devices

    uint32_t physical_count = 0u;
    ::VkResult result = ::vkEnumeratePhysicalDevices(
        _instance,
        &physical_count,
        nullptr
    );

    std::vector<::VkPhysicalDevice> devices(physical_count);
    ::vkEnumeratePhysicalDevices(_instance, &physical_count, devices.data());

    if(result != ::VK_SUCCESS || physical_count == 0u) {
        CONSOLE_CRITICAL("No suitable device found.");
    }

    CONSOLE_TRACE("Found {} devices", physical_count);

    // -------------------------------------------------------------------------
    // Iterate and detail each physical device

    for(uint32_t device_idx = 0; device_idx < physical_count; ++device_idx) {
        ::VkPhysicalDeviceProperties properties { };
        ::vkGetPhysicalDeviceProperties(devices[device_idx], &properties);

        const char *type_string;
        switch(properties.deviceType) {
            case ::VK_PHYSICAL_DEVICE_TYPE_OTHER:
                type_string = "Other";
                break;
            case ::VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
                type_string = "iGPU";
                break;
            case ::VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
                type_string = "dGPU";
                break;
            case ::VK_PHYSICAL_DEVICE_TYPE_CPU:
                type_string = "CPU";
                break;
            default:
                type_string = "Unknown";
                assert(false);
                break;
        }

        // I am sure this is supposed to be used somewhere, but...
        ::VkPhysicalDeviceFeatures features { };
        ::vkGetPhysicalDeviceFeatures(devices[device_idx], &features);

        // grabbing the VRAM amount in proper megabytes
        ::VkPhysicalDeviceMemoryProperties memory { };
        ::vkGetPhysicalDeviceMemoryProperties(devices[device_idx], &memory);

        ::VkDeviceSize vram = 0u;
        for(uint32_t index = 0; index < memory.memoryHeapCount; ++index) {
            auto flags = memory.memoryHeaps[index].flags;
            if((flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT) == flags) {
                vram = memory.memoryHeaps[index].size / 1000 / 1000;
                break;
            }
        }

        // query and populate list of physical device extensions
        uint32_t extension_count = 0u;
        ::vkEnumerateDeviceExtensionProperties(
            devices[device_idx],
            nullptr,
            &extension_count,
            nullptr
        );

        CONSOLE_TRACE("Found {} physical device extensions", extension_count);

        std::vector<::VkExtensionProperties> device_extensions(extension_count);
        ::vkEnumerateDeviceExtensionProperties(
            devices[device_idx],
            nullptr,
            &extension_count,
            device_extensions.data()
        );

        // This is some extra-ness just to acquire the correct GPU driver
        // version such that what's displayed matches what the vendors actually
        // list on their websites/etc
        ::VkPhysicalDeviceDriverProperties driver_props { };
        driver_props.sType =
            ::VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DRIVER_PROPERTIES;

        ::VkPhysicalDeviceProperties2KHR physical_props2 { };
        physical_props2.sType =
            ::VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2_KHR;
        physical_props2.pNext = &driver_props;

        for(const auto &extension : device_extensions) {
            if(strcmp(extension.extensionName,
                    VK_KHR_DRIVER_PROPERTIES_EXTENSION_NAME) == 0)
            {
                _GetPhysicalDeviceProperties2(
                    devices[device_idx],
                    &physical_props2
                );
                break;
            }
        }

        CONSOLE_TRACE(
            "\n"
            "\tDevice Name:    {}\n"
            "\tDevice Type:    {}\n"
            "\tDriver Version: {}\n"
            "\tVRAM:           {}MB\n"
            "\tVulkan Version: {}.{}.{}\n",
            properties.deviceName,
            type_string,
            driver_props.driverInfo,
            vram,
            VK_API_VERSION_MAJOR(properties.apiVersion),
            VK_API_VERSION_MINOR(properties.apiVersion),
            VK_API_VERSION_PATCH(properties.apiVersion)
        );
    }

    // TODO: this should actually be a choice, but whateva.
    _physical_device = devices[0];
}

// =============================================================================
void Instance::init_logical_device(const CommandQueues &queues) {
    CONSOLE_INFO("");

    // specify logical device extension(s) tahaaaaave
    const char *extensions[] {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    // the driver will tell us what kinda features we've got
    ::VkPhysicalDeviceFeatures features { };

    // finally populate the logical device creation information
    ::VkDeviceCreateInfo device_info { };
    device_info.sType = ::VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    device_info.pNext = nullptr;
    device_info.flags = 0u;
    device_info.queueCreateInfoCount = queues.queue_count();
    device_info.pQueueCreateInfos    = queues.queues();
    device_info.enabledExtensionCount = 
        static_cast<uint32_t>(std::size(extensions));
    device_info.ppEnabledExtensionNames = extensions;
    device_info.pEnabledFeatures = &features;

// enable the validation layer if we're in a debug build
// TODO: should probably change to permit multiple layers, but we'll cross
//       that bridge when we come to it
#ifdef VK_VALIDATION_LAYER
    const char *layers[] { "VK_LAYER_KHRONOS_validation" };
    device_info.enabledLayerCount = static_cast<uint32_t>(std::size(layers));
    device_info.ppEnabledLayerNames = layers;    
#endif // VK_VALIDATION_LAYER

    ::VkResult result = ::vkCreateDevice(
        _physical_device,
        &device_info,
        nullptr,
        &_logical_device
    );

    if(result != ::VK_SUCCESS) {
        CONSOLE_CRITICAL("Unable to create logical device.");
    }
}

// =============================================================================
void Instance::init_instance_procs() {
    CONSOLE_INFO("");
    GET_INSTANCE_PROC_ADDR(_instance, GetPhysicalDeviceSurfaceSupportKHR);
    assert(_GetPhysicalDeviceSurfaceSupportKHR != nullptr);
    GET_INSTANCE_PROC_ADDR(_instance, GetPhysicalDeviceSurfaceCapabilitiesKHR);
    assert(_GetPhysicalDeviceSurfaceCapabilitiesKHR != nullptr);
    GET_INSTANCE_PROC_ADDR(_instance, GetPhysicalDeviceSurfaceFormatsKHR);
    assert(_GetPhysicalDeviceSurfaceFormatsKHR != nullptr);
    GET_INSTANCE_PROC_ADDR(_instance, GetPhysicalDeviceSurfacePresentModesKHR);
    assert(_GetPhysicalDeviceSurfacePresentModesKHR != nullptr);
    GET_INSTANCE_PROC_ADDR(_instance, GetPhysicalDeviceProperties2);
    assert(_GetPhysicalDeviceProperties2 != nullptr);
}

// =============================================================================
void Instance::init_logical_device_procs() {
    GET_DEVICE_PROC_ADDR(_logical_device, CreateSwapchainKHR);
    assert(_CreateSwapchainKHR != nullptr);
    GET_DEVICE_PROC_ADDR(_logical_device, DestroySwapchainKHR);
    assert(_DestroySwapchainKHR != nullptr);
    GET_DEVICE_PROC_ADDR(_logical_device, GetSwapchainImagesKHR);
    assert(_GetSwapchainImagesKHR != nullptr);
    GET_DEVICE_PROC_ADDR(_logical_device, AcquireNextImageKHR);
    assert(_AcquireNextImageKHR != nullptr);
    GET_DEVICE_PROC_ADDR(_logical_device, QueuePresentKHR);
    assert(_QueuePresentKHR != nullptr);
}

// =============================================================================
Instance::Instance() :
    _GetPhysicalDeviceSurfaceSupportKHR      { nullptr },
    _GetPhysicalDeviceSurfaceCapabilitiesKHR { nullptr },
    _GetPhysicalDeviceSurfaceFormatsKHR      { nullptr },
    _GetPhysicalDeviceSurfacePresentModesKHR { nullptr },
    _CreateSwapchainKHR    { nullptr },
    _DestroySwapchainKHR   { nullptr },
    _GetSwapchainImagesKHR { nullptr },
    _AcquireNextImageKHR   { nullptr },
    _QueuePresentKHR       { nullptr },
    _instance        { nullptr },
    _physical_device { nullptr },
    _logical_device  { nullptr }
{    
    CONSOLE_INFO("");
}

// =============================================================================
Instance::~Instance() {
    CONSOLE_INFO("");

#ifdef VK_VALIDATION_LAYER
    VKDebugger::shutdown(*this);
#endif // VK_VALIDATION_LAYER

    ::vkDestroyDevice(_logical_device, nullptr);
    ::vkDestroyInstance(_instance, nullptr);
}