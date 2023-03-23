#include "vklearnin/vklearnin.hpp"
#include "vklearnin/system/GraphicsAPI.hpp"

#include "vklearnin/tools/VKDebugger.hpp"

// This (and more; see the link) does away with the explicit loading of each
// function/extension
// https://github.com/KhronosGroup/Vulkan-Hpp
VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

namespace vkl {

vk::DynamicLoader               GraphicsAPI::_loader   { };
vk::Instance                    GraphicsAPI::_instance { };
vk::ApplicationInfo             GraphicsAPI::_app_info { };
std::vector<char const *>       GraphicsAPI::_enabled_layers;
std::vector<char const *>       GraphicsAPI::_enabled_extensions;
GraphicsAPI::ValidationFeatures GraphicsAPI::_validation_features;
vk::ValidationFeaturesEXT       GraphicsAPI::_validation_extensions { };
vk::InstanceCreateInfo          GraphicsAPI::_instance_create_info  { };

// =============================================================================
void GraphicsAPI::init() {
    _init_dynamic_loader(); // The first step for using the dynamic loader
    _init_app_info();       // Provide hints about this program to the driver
    _init_layers();         // There are many layers. Validation is our favorite
    _init_extensions();     // Extensions are often implementation defined

    auto const extensions = vk::enumerateInstanceExtensionProperties();
    CONSOLE_TRACE("Found {} instance extensions.", extensions.size());

    // Run through the extensions the driver offers and make sure we've got
    // what we need
    if(!_check_extensions(extensions)) {
        return;
    }

    // Bringing it all together. If we want validation layer functionality, the
    // pNext member of vk::InstanceCreateInfo must point to the structure
    // assembled above.
    const vk::InstanceCreateInfo instance_info {
        .pNext = reinterpret_cast<void *>(&_validation_extensions),
        .flags = { },
        .pApplicationInfo = &_app_info,
        .enabledLayerCount =
            static_cast<uint32_t>(_enabled_layers.size()),
        .ppEnabledLayerNames = _enabled_layers.data(),
        .enabledExtensionCount =
            static_cast<uint32_t>(_enabled_extensions.size()),
        .ppEnabledExtensionNames = _enabled_extensions.data()
    };

    auto const result = vk::createInstance(
        &instance_info,
        nullptr,
        &_instance
    );

    // If this didn't work, we can go no further.
    if(result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL(
            "Failed to create Vulkan instance: '{}'",
            to_string(result)
        );
        return;
    }

    // Inform the dynamic dispatcher that we've got an instance.
    VULKAN_HPP_DEFAULT_DISPATCHER.init(_instance);

    CONSOLE_INFO(
        "Created Vulkan v{}.{}.{} instance",
        VK_API_VERSION_MAJOR(VK_API_VER),
        VK_API_VERSION_MINOR(VK_API_VER),
        VK_API_VERSION_PATCH(VK_API_VER)
    );

#ifdef VKL_DEBUG
    VKDebugger::init(_instance);
#endif // VKL_DEBUG
}

// =============================================================================
void GraphicsAPI::shutdown() {
#ifdef VKL_DEBUG
    VKDebugger::shutdown(_instance);
#endif // VKL_DEBUG

    _instance.destroy();
}

// =============================================================================
void GraphicsAPI::_init_dynamic_loader() {
    using inst_proc = PFN_vkGetInstanceProcAddr; // A little brevity
    auto vkGetInstanceProcAddr = _loader.getProcAddress<inst_proc>(
        "vkGetInstanceProcAddr"
    );
    // Bootstrap the auto-loader
    VULKAN_HPP_DEFAULT_DISPATCHER.init(vkGetInstanceProcAddr);
}

// =============================================================================
// Several static constexpr values loaded in from the central header
void GraphicsAPI::_init_app_info() {
    _app_info.pApplicationName   = APP_NAME;
    _app_info.applicationVersion = APP_VERSION;
    _app_info.pEngineName        = ENGINE_NAME;
    _app_info.engineVersion      = ENGINE_VERSION;
    _app_info.apiVersion         = VK_API_VER;
}

// =============================================================================
void GraphicsAPI::_init_layers() {
#ifdef VKL_DEBUG
    // The validation layer helps you know if you've strayed too far from the
    // expected path. It's also extremely opinionated, so each message should
    // be considered individually.
    _enabled_layers = { "VK_LAYER_KHRONOS_validation" };
#endif // VKL_DEBUG
}

// =============================================================================
void GraphicsAPI::_init_extensions() {
    _enabled_extensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);

    // Surfaces describe the spaces to which you can draw in Vulkan. They're
    // also platform dependant.
#if defined(VKL_LINUX)
    _enabled_extensions.push_back(VK_KHR_XLIB_SURFACE_EXTENSION_NAME);
#elif defined(VKL_WINDOWS)
    _enabled_extensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#endif

#ifdef VKL_DEBUG
    // The first steps toward giving the drive a path to keep us abreast of
    // myriad details.
    _enabled_extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    _enabled_extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);

    // As with the last line, these features support our debugging efforts
    _validation_features = {
        vk::ValidationFeatureEnableEXT::eGpuAssisted,
        vk::ValidationFeatureEnableEXT::eGpuAssistedReserveBindingSlot,
        vk::ValidationFeatureEnableEXT::eBestPractices,

        // DebugPrintf and GpuAssisted are mutually exclusive. DebugPrintf is
        // very handy when used in conjucntion with RenderDoc, but I'm opting
        // for more self-contained guidance for now.
        // vk::ValidationFeatureEnableEXT::eDebugPrintf,

        // Last time I checked, there were false positives/bugs around
        // synchronization validation on Linux
        #if defined(VKL_WINDOWS)
            vk::ValidationFeatureEnableEXT::eSynchronizationValidation
        #endif
    };
#endif // VKL_DEBUG

    _validation_extensions = {
        .enabledValidationFeatureCount =
            static_cast<uint32_t>(_validation_features.size()),
        .pEnabledValidationFeatures = _validation_features.data(),
        .disabledValidationFeatureCount = 0u,
        .pDisabledValidationFeatures = nullptr
    };

    for(auto const& extension : _enabled_extensions) {
        CONSOLE_TRACE("Requesting instance extension '{}'", extension);
    }
}

// =============================================================================
bool GraphicsAPI::_check_extensions(Extensions const &supported_extensions) {
    for(auto const * const ext_name : _enabled_extensions) {
        bool extension_found = false;

        for(auto const& extension : supported_extensions) {
            if(strcmp(ext_name, extension.extensionName) == 0) {
                extension_found = true;
                break;
            }
        }
        if(!extension_found) {
            CONSOLE_WARN("No support for instance extension '{}'", ext_name);
            return false;
        }
    }

    return true;
}

} // namespace vkl