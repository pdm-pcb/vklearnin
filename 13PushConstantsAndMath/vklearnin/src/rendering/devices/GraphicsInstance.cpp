#include "vklearnin/vklearnin.hpp"
#include "vklearnin/rendering/GraphicsInstance.hpp"

#include "vklearnin/system/TargetWindow.hpp"
#include "vklearnin/rendering/devices/PhysicalDevice.hpp"
#include "vklearnin/rendering/devices/LogicalDevice.hpp"

// This (and more; see the link) does away with the explicit loading of each
// function/extension
// https://github.com/KhronosGroup/Vulkan-Hpp#extensions--per-device-function-pointers
VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

namespace vkl {

vk::DynamicLoader         GraphicsInstance::_loader { };
vk::Instance              GraphicsInstance::_graphics_instance { };
vk::ApplicationInfo       GraphicsInstance::_app_info { };
std::vector<const char *> GraphicsInstance::_enabled_layers;
std::vector<const char *> GraphicsInstance::_enabled_extensions;
GraphicsInstance::ValidationFeatures GraphicsInstance::_validation_features;
vk::ValidationFeaturesEXT GraphicsInstance::_validation_extensions { };
vk::InstanceCreateInfo    GraphicsInstance::_instance_create_info { };

// =============================================================================
void GraphicsInstance::init() {    
    _init_dynamic_loader(); // The first step for using the dynamic loader
    _init_app_info();       // Provide hints about this program to the driver
    _init_layers();         // Of the many layers, we'll stick with validation
    _init_extensions();     // Extensions are often implementation defined

    // Bringing it all together. If we want validation layer functionality, the
    // pNext member of vk::InstanceCreateInfo must point to the structure
    // assembled above.
    vk::InstanceCreateInfo instance_info {
#ifdef VKL_DEBUG
        .pNext = reinterpret_cast<void *>(&_validation_extensions),
#else
        .pNext = nullptr,
#endif // VKL_DEBUG
        .flags = { },
        .pApplicationInfo = &_app_info,
        .enabledLayerCount = 
            static_cast<uint32_t>(_enabled_layers.size()),
        .ppEnabledLayerNames = _enabled_layers.data(),
        .enabledExtensionCount =
            static_cast<uint32_t>(_enabled_extensions.size()),
        .ppEnabledExtensionNames = _enabled_extensions.data()
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
    for(const char *required_extension : _enabled_extensions) {
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
void GraphicsInstance::init_devices() {
    PhysicalDevice::query_devices();
    PhysicalDevice::select_device();
    LogicalDevice::create();
}

// =============================================================================
void GraphicsInstance::wait_idle() {
    CONSOLE_TRACE(
        "Waiting for idle of {:#x}",
        reinterpret_cast<uint64_t>(VkDevice(LogicalDevice::native()))
    );

    auto result = LogicalDevice::native().waitIdle();

    if(result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL(
            "Failed to wait for idle of device {:#x}",
            reinterpret_cast<uint64_t>(VkDevice(LogicalDevice::native()))
        );
    }
}

// =============================================================================
void GraphicsInstance::shutdown() {
    CONSOLE_TRACE("");

#ifdef VKL_DEBUG
    VKDebugger::shutdown(_graphics_instance);
#endif // VKL_DEBUG

    LogicalDevice::destroy();
    _graphics_instance.destroy();
}
// =============================================================================
void GraphicsInstance::_init_dynamic_loader() {
    using inst_proc = PFN_vkGetInstanceProcAddr;
    inst_proc vkGetInstanceProcAddr =
        _loader.getProcAddress<inst_proc>("vkGetInstanceProcAddr");
    VULKAN_HPP_DEFAULT_DISPATCHER.init(vkGetInstanceProcAddr);
}

// =============================================================================
void GraphicsInstance::_init_app_info() {
    _app_info.pApplicationName = APP_NAME;
    _app_info.applicationVersion = APP_VERSION;
    _app_info.pEngineName = ENGINE_NAME;
    _app_info.engineVersion = ENGINE_VERSION;
    _app_info.apiVersion = VK_API_VER;
}


// =============================================================================
void GraphicsInstance::_init_layers() {
#ifdef VKL_DEBUG
    // The validation layer helps you know if you've strayed too far from the
    // expected path. It's also extremely opinionated, so each message should
    // be considered individually.
    _enabled_layers.push_back("VK_LAYER_KHRONOS_validation");
#endif // VKL_DEBUG
}

// =============================================================================
void GraphicsInstance::_init_extensions() {
    _enabled_extensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);

    // Surfaces describe the spaces to which you can draw in Vulkan. They're
    // also platform dependant.
#if defined(__linux__)
    _enabled_extensions.push_back(VK_KHR_XCB_SURFACE_EXTENSION_NAME);
#elif defined(_WIN32)
    _enabled_extensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#endif

#ifdef VKL_DEBUG
    // Here's the first step toward giving the drive a path to keep us abreast
    // of myriad details.
    _enabled_extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

    // As with the last line, these features support our debugging efforts
    _validation_features = {
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
    _enabled_layers.push_back("VK_LAYER_KHRONOS_validation");

    _validation_extensions = {
        .enabledValidationFeatureCount =
            static_cast<uint32_t>(_validation_features.size()),
        .pEnabledValidationFeatures = _validation_features.data(),
        .disabledValidationFeatureCount = 0u,
        .pDisabledValidationFeatures = nullptr
    };
#endif // VKL_DEBUG
}

} // namespace vkl