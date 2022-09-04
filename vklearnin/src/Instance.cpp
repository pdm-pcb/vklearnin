#include "vklearnin/common.hpp"
#include "vklearnin/Instance.hpp"

#include "vklearnin/CommandStructures/CommandQueues.hpp"

#if defined(__linux__)
    #include "vklearnin/Platform/X11/X11Window.hpp"
#elif defined(_WIN32)
    #include "vklearnin/Platform/Win32/Win32Window.hpp"
#endif

// This (and more; see the link) does away with the explicit loading of each
// function/extension
// https://github.com/KhronosGroup/Vulkan-Hpp#extensions--per-device-function-pointers
VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

// =============================================================================
void Instance::init_instance()
{
    CONSOLE_INFO("");
    
    // -------------------------------------------------------------------------
    // Application Information

    vk::ApplicationInfo app_info {
        .pApplicationName = APP_NAME,
        .applicationVersion = APP_VERSION,
        .pEngineName = ENGINE_NAME,
        .engineVersion = ENGINE_VERSION,
        .apiVersion = VK_API_VER
    };
    
    // -------------------------------------------------------------------------
    // The layers and extensions to enable

    std::vector<const char *> enabled_layers;

    if(_validate) {
        enabled_layers.push_back("VK_LAYER_KHRONOS_validation");
    }

    std::vector<vk::ValidationFeatureEnableEXT> enabled_features {
        vk::ValidationFeatureEnableEXT::eGpuAssisted,
        vk::ValidationFeatureEnableEXT::eGpuAssistedReserveBindingSlot,
        vk::ValidationFeatureEnableEXT::eBestPractices,
        // vk::ValidationFeatureEnableEXT::eDebugPrintf,
        vk::ValidationFeatureEnableEXT::eSynchronizationValidation
    };

    vk::ValidationFeaturesEXT validation_features {
        .enabledValidationFeatureCount =
            static_cast<uint32_t>(enabled_features.size()),
        .pEnabledValidationFeatures = enabled_features.data(),
        .disabledValidationFeatureCount = 0u,
        .pDisabledValidationFeatures = nullptr
    };

    std::vector<const char *> enabled_extensions {
        VK_KHR_SURFACE_EXTENSION_NAME
    };

#ifdef DEBUG
    enabled_extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

// platform-specific surfaces
#if defined(__linux__)
        enabled_extensions.push_back(VK_KHR_XCB_SURFACE_EXTENSION_NAME);
#elif defined(_WIN32)
        enabled_extensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#endif

    // -------------------------------------------------------------------------
    // Instance Creation Information (wants app info from above)

    vk::InstanceCreateInfo instance_info {
        .pNext = reinterpret_cast<void *>(&validation_features),
        .flags = { },
        .pApplicationInfo = &app_info,
        .enabledLayerCount = 
            static_cast<uint32_t>(enabled_layers.size()),
        .ppEnabledLayerNames = enabled_layers.data(),
        .enabledExtensionCount =
            static_cast<uint32_t>(enabled_extensions.size()),
        .ppEnabledExtensionNames = enabled_extensions.data()
    };

    auto result = vk::createInstance(
        &instance_info,
        nullptr,
        &_instance
    );

    if(result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL("Failed to create Vulkan instance.");
    }

    VULKAN_HPP_DEFAULT_DISPATCHER.init(_instance);

#ifdef DEBUG
    // grab the function addresses for the debug utility
    VKDebugger::init(_instance);
#endif

    // -------------------------------------------------------------------------
    // Query and populate the list of instance extensions 
    std::vector<vk::ExtensionProperties> extensions =
        vk::enumerateInstanceExtensionProperties();

    CONSOLE_TRACE("Found {} instance extensions.", extensions.size());   

    for(const char *required_extension : enabled_extensions) {
        bool supported = false;
        for(const auto &extension : extensions) {
            if(strcmp(required_extension, extension.extensionName) == 0) {
                supported = true;
                break;
            }
        }
        if(!supported) {
            CONSOLE_CRITICAL(
                "Instance extension {} unsupported",
                required_extension
            );
        }
    }

    for(const auto &extension : extensions) {
        CONSOLE_TRACE("\t{:s}", extension.extensionName);
    }
}

// =============================================================================
void Instance::init_physical_device() {
    CONSOLE_INFO("");

    // -------------------------------------------------------------------------
    // Query and populate the list of physical devices
    auto devices = _instance.enumeratePhysicalDevices();
    if(devices.size() == 0) {
        CONSOLE_CRITICAL("Found zero physical devices.");
    }
    CONSOLE_TRACE("Found {} devices", devices.size());

    // -------------------------------------------------------------------------
    // Iterate and detail each physical device

    std::vector<vk::PhysicalDeviceProperties> props_list;
    props_list.reserve(devices.size());

    for(const auto &device : devices) {
        props_list.emplace_back(device.getProperties());
        auto properties = props_list.back();

        // grabbing the VRAM amount in proper megabytes
        auto memory = device.getMemoryProperties();
        vk::DeviceSize vram = 0u;
        for(uint32_t index = 0; index < memory.memoryHeapCount; ++index) {
            auto flags = memory.memoryHeaps[index].flags;
            if((flags & vk::MemoryHeapFlagBits::eDeviceLocal) == flags) {
                vram = memory.memoryHeaps[index].size / 1000 / 1000;
                break;
            }
        }

        // query and populate list of physical device extensions
        auto extensions = device.enumerateDeviceExtensionProperties();
        
        if(extensions.size() == 0) {
            CONSOLE_CRITICAL("Found zero physical device extensions.");
        }
        CONSOLE_TRACE("Found {} physical device extensions", extensions.size());

        // This is some extra-ness just to acquire the correct GPU driver
        // version such that what's displayed matches what the vendors actually
        // list on their websites/etc
        vk::PhysicalDeviceDriverProperties driver_props { };

        vk::PhysicalDeviceProperties2KHR physical_props2 {
            .pNext = &driver_props
        };


        for(const auto &extension : extensions) {
            if(strcmp(extension.extensionName,
                    VK_KHR_DRIVER_PROPERTIES_EXTENSION_NAME) == 0)
            {
                device.getProperties2(&physical_props2);
                break;
            }
        }

        CONSOLE_TRACE(
            "\n"
            "\tDevice Name:    {}\n"
            "\tDevice Type:    {}\n"
            "\tDriver Version: {}\n"
            "\tVRAM:           {}MB\n"
            "\tMax Anisotropy: x{}\n"
            "\tMSAA samples  : {}\n"
            "\tUBO alignment : {} bytes\n"
            "\tVulkan Version: {}.{}.{}\n",
            properties.deviceName,
            to_string(properties.deviceType),
            driver_props.driverInfo,
            vram,
            properties.limits.maxSamplerAnisotropy,
            to_string(properties.limits.framebufferColorSampleCounts),
            properties.limits.minUniformBufferOffsetAlignment,
            VK_API_VERSION_MAJOR(properties.apiVersion),
            VK_API_VERSION_MINOR(properties.apiVersion),
            VK_API_VERSION_PATCH(properties.apiVersion)
        );
    }

    // TODO: this should actually be a choice, but whateva.
    _physical_device   = devices[0];
    _max_anisotropy    = props_list[0].limits.maxSamplerAnisotropy;
    _supported_msaa    = props_list[0].limits.framebufferColorSampleCounts;
    _min_ubo_alignment = props_list[0].limits.minUniformBufferOffsetAlignment;
}

// =============================================================================
void Instance::init_logical_device(const CommandQueues &command_queue) {
    CONSOLE_INFO("");

    // specify logical device extension(s) tahaaaaave
    std::vector<const char *> extensions {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    vk::PhysicalDeviceFeatures supported_features;
    _physical_device.getFeatures(&supported_features);

    if(supported_features.samplerAnisotropy == false) {
        CONSOLE_CRITICAL("Hardware device does not support "
                         "anisotropic filtering.");
    }

    if(supported_features.sampleRateShading == false) {
        CONSOLE_CRITICAL("Hardware device does not support "
                         "sample rate shading.");
    }

    vk::PhysicalDeviceFeatures enabled_features { };
    enabled_features.samplerAnisotropy = true;
    enabled_features.sampleRateShading = true;

    std::vector<const char *> layers;
    if(_validate) {
        layers.push_back("VK_LAYER_KHRONOS_validation");
    }

    // finally populate the logical device creation information
    vk::DeviceCreateInfo device_info {
        .queueCreateInfoCount = command_queue.queue_count(),
        .pQueueCreateInfos = command_queue.queues(),
        .enabledLayerCount = static_cast<uint32_t>(layers.size()),
        .ppEnabledLayerNames = layers.data(),
        .enabledExtensionCount = static_cast<uint32_t>(extensions.size()),
        .ppEnabledExtensionNames = extensions.data(),
        .pEnabledFeatures = &enabled_features,
    };

    auto result = _physical_device.createDevice(
        &device_info,
        nullptr,
        &_logical_device
    );

    if(result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL("Unable to create logical device.");
    }

    VULKAN_HPP_DEFAULT_DISPATCHER.init(_logical_device);

    Allocator::init(*this);
}

// =============================================================================
Instance::Instance(const bool validate) :
    _instance          { nullptr },
    _physical_device   { nullptr },
    _logical_device    { nullptr },
    _max_anisotropy    { 0.0f },
    _supported_msaa    { vk::SampleCountFlagBits::e1 },
    _min_ubo_alignment { 0u },
    _validate          { validate }
{    
    CONSOLE_INFO("");

    // first step for using the dynamic loader
    using inst_proc = PFN_vkGetInstanceProcAddr;
    inst_proc vkGetInstanceProcAddr =
        _loader.getProcAddress<inst_proc>("vkGetInstanceProcAddr");
    VULKAN_HPP_DEFAULT_DISPATCHER.init(vkGetInstanceProcAddr);
}

// =============================================================================
Instance::~Instance() {
    CONSOLE_INFO("");

#ifdef DEBUG
    VKDebugger::shutdown(_instance);
#endif

    Allocator::shutdown();

    _logical_device.destroy();
    _instance.destroy();
}