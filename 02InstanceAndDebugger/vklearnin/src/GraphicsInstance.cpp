#include "vklearnin/vklearnin.hpp"
#include "vklearnin/GraphicsInstance.hpp"

// This (and more; see the link) does away with the explicit loading of each
// function/extension
// https://github.com/KhronosGroup/Vulkan-Hpp#extensions--per-device-function-pointers
VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

namespace vkl {

// =============================================================================
void GraphicsInstance::init() {
    // Provide hints about this application to the driver
    vk::ApplicationInfo app_info {
        .pApplicationName = APP_NAME,
        .applicationVersion = APP_VERSION,
        .pEngineName = ENGINE_NAME,
        .engineVersion = ENGINE_VERSION,
        .apiVersion = VK_API_VER
    };

    // Vulkan likes to know about things in lists. In plain-old-C, there's
    // frequently faffery due to a lack of love for VLAs. With Vulkan-Hpp,
    // we're thankfully permitted to just use vectors. Given that many of the
    // use cases for these is during initialization, there's no worry about
    // heap allocations slowing us down.
    std::vector<const char *> enabled_layers;
    std::vector<const char *> enabled_extensions {
        VK_KHR_SURFACE_EXTENSION_NAME
    };

#ifdef VKL_DEBUG
    // Here's the first step toward giving the drive a path to keep us abreast
    // of myriad details.
    enabled_extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

    // As with the last line, these features support our debugging efforts
    std::vector<vk::ValidationFeatureEnableEXT> enabled_features {
        vk::ValidationFeatureEnableEXT::eGpuAssisted,
        vk::ValidationFeatureEnableEXT::eGpuAssistedReserveBindingSlot,
        vk::ValidationFeatureEnableEXT::eBestPractices,
        // DebugPrintf and GpuAssisted are mutually exclusive. DebugPrintf is
        // very handy when used in conjucntion with RenderDoc, but I'm opting
        // for more self-contained guidance for now.
        // vk::ValidationFeatureEnableEXT::eDebugPrintf,
        vk::ValidationFeatureEnableEXT::eSynchronizationValidation
    };

    // The validation layer helps you know if you've strayed too far from the
    // expected path. It's also extremely opinionated, so each message should
    // be considered individually.
    enabled_layers.push_back("VK_LAYER_KHRONOS_validation");

    vk::ValidationFeaturesEXT validation_features {
        .enabledValidationFeatureCount =
            static_cast<uint32_t>(enabled_features.size()),
        .pEnabledValidationFeatures = enabled_features.data(),
        .disabledValidationFeatureCount = 0u,
        .pDisabledValidationFeatures = nullptr
    };
#endif // VKL_DEBUG

    // Surfaces describe the spaces to which you can draw in Vulkan. They're
    // also platform dependant.
#if defined(__linux__)
    enabled_extensions.push_back(VK_KHR_XCB_SURFACE_EXTENSION_NAME);
#elif defined(_WIN32)
    enabled_extensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#endif

    // Bringing it all together. If we want validation layer functionality, the
    // pNext member of vk::InstanceCreateInfo must point to the structure
    // assembled above.
    vk::InstanceCreateInfo instance_info {
#ifdef VKL_DEBUG
        .pNext = reinterpret_cast<void *>(&validation_features),
#else
        .pNext = nullptr,
#endif // VKL_DEBUG
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
        &_graphics_instance
    );

    // If this didn't work, we can go no further.
    if(result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL("Failed to create Vulkan instance.");
    }

    // Inform the dynamic dispatcher that we've got an instance.
    VULKAN_HPP_DEFAULT_DISPATCHER.init(_graphics_instance);

    // Disabling constructors in Vulkan-Hpp forces the programmer to check the
    // return status of any function which offers one. So we'll check that
    // before proceeding to utilize the other return value.
    auto [ext_result, extensions] = vk::enumerateInstanceExtensionProperties();
    if(ext_result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL("Failed to enumerate instance extensions.");
    }

    CONSOLE_TRACE("Found {} instance extensions.", extensions.size());   

    // At most, three instance extensions are required at this point. Run
    // through all extensions the driver offers and make sure we've got what
    // we need
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
                "Instance extension '{}' unsupported",
                required_extension
            );
        }
    }

    CONSOLE_INFO(
        "Created Vulkan v{}.{}.{} instance",
        VK_API_VERSION_MAJOR(VK_API_VER),
        VK_API_VERSION_MINOR(VK_API_VER),
        VK_API_VERSION_PATCH(VK_API_VER)
    );

#ifdef VKL_DEBUG
    VKDebugger::init(_graphics_instance);
#endif // VKL_DEBUG
}

// =============================================================================
GraphicsInstance::GraphicsInstance() :
    _graphics_instance { nullptr }
{
    // first step for using the dynamic loader
    using inst_proc = PFN_vkGetInstanceProcAddr;
    inst_proc vkGetInstanceProcAddr =
        _loader.getProcAddress<inst_proc>("vkGetInstanceProcAddr");
    VULKAN_HPP_DEFAULT_DISPATCHER.init(vkGetInstanceProcAddr);
}

// =============================================================================
GraphicsInstance::~GraphicsInstance() {
#ifdef VKL_DEBUG
    VKDebugger::shutdown(_graphics_instance);
#endif // VKL_DEBUG

    _graphics_instance.destroy();
}

} // namespace vkl