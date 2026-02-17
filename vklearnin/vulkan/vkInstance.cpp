#include "vklearnin/vklearnin.hpp"
#include "vklearnin/vulkan/vkInstance.hpp"

// See https://github.com/KhronosGroup/Vulkan-Hpp
VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

namespace vkl {

// =============================================================================
bool vkInstance::create(Config const &config,
                        std::string_view const app_name,
                        uint32_t app_version)
{
    if(_handle) {
        Log::error("Vulkan instance {} already exists.", _handle);
        return false;
    }

    _app_name = app_name;
    _app_version = app_version;

    _enabled_extensions = config.extensions;

    _init_dynamic_loader(); // Initial setup for the dynamic loader
    _init_app_info();       // Provide hints about this app to the driver

    // If validation is requested, add the layers, extensions, and features
    if(config.enable_validation) {
        _init_validation();
    }

    // Run through the layers the driver offers and make sure we've got what
    // we need
    if(!_check_layers()) {
        Log::error("Could not get support for all requested instance layers.");
        return false;
    }

    // Run through the extensions the driver offers and make sure we've got
    // what we need
    if(!_check_extensions()) {
        Log::error("Could not get support for all requested instance "
                   "extensions.");
        return false;
    }

    // Bringing it all together
    vk::StructureChain<vk::InstanceCreateInfo,
                       vk::ValidationFeaturesEXT> instance_info = {
        vk::InstanceCreateInfo {
            .flags = { },
            .pApplicationInfo = &_app_info,
            .enabledLayerCount =
                static_cast<uint32_t>(_enabled_layers.size()),
            .ppEnabledLayerNames = _enabled_layers.data(),
            .enabledExtensionCount =
                static_cast<uint32_t>(_enabled_extensions.size()),
            .ppEnabledExtensionNames = _enabled_extensions.data(),
        },
        _vvl_features
    };

    _handle = vk::createInstance(instance_info.get());

    Log::info(
        "Created Vulkan {}.{} instance {}",
        VKL_VK_TARGET_MAJOR,
        VKL_VK_TARGET_MINOR,
        _handle
    );

    // Inform the dynamic dispatcher that we've got an instance.
    VULKAN_HPP_DEFAULT_DISPATCHER.init(_handle);

    // Now that the instance exists, it's safe to create the debug messenger.
    if(config.enable_validation) {
        if(!_debug_messenger.create(*this)) {
            return false;
        }
    }

    return true;
}

// =============================================================================
bool vkInstance::destroy() {
    if(!_handle) {
        Log::error("Must create instance before calling destroy.");
        return false;
    }

    if(_debug_messenger.native()) {
        _debug_messenger.destroy();
    }

    Log::trace(
        "Destroying Vulkan {}.{} instance {}",
        VK_API_VERSION_MAJOR(_app_info.apiVersion),
        VK_API_VERSION_MINOR(_app_info.apiVersion),
        _handle
    );

    _handle.destroy();
    _handle = nullptr;

    return true;
}

// =============================================================================
void vkInstance::_init_dynamic_loader() {
    // The dynamic loader needs something to boostrap itself, so provide it a
    // pointer to find the instance
    auto vkGetInstanceProcAddr =
        _loader.getProcAddress<PFN_vkGetInstanceProcAddr>(
            "vkGetInstanceProcAddr"
        );

    // Now we're ready to let it run
    VULKAN_HPP_DEFAULT_DISPATCHER.init(vkGetInstanceProcAddr);
}

// =============================================================================
void vkInstance::_init_app_info() {
    _app_info = vk::ApplicationInfo {
        .pApplicationName   = _app_name.data(),
        .applicationVersion = _app_version,
        .pEngineName        = VKL_NAME,
        .engineVersion      = VKL_VERSION,
        .apiVersion         = VK_MAKE_API_VERSION(
            0,  // Variant
            VKL_VK_TARGET_MAJOR,
            VKL_VK_TARGET_MINOR,
            0   // Patch
        )
    };
}

// =============================================================================
void vkInstance::_init_validation() {
    // The validation layer helps you know if you've strayed too far from the
    // expected path. It's also extremely opinionated, so each message should
    // be considered individually.
    _enabled_layers.emplace_back("VK_LAYER_KHRONOS_validation");

    // We'll need the debug messenger to receive validation layer messages
    _enabled_extensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

    // Next we configure what we want the validation layers to report
    _vvl_enabled = {
        vk::ValidationFeatureEnableEXT::eBestPractices,
        vk::ValidationFeatureEnableEXT::eSynchronizationValidation,
        vk::ValidationFeatureEnableEXT::eDebugPrintf,
        //vk::ValidationFeatureEnableEXT::eGpuAssisted,
        //vk::ValidationFeatureEnableEXT::eGpuAssistedReserveBindingSlot,
    };

    // Not disabling any layers
    _vvl_disabled = { };

    // Build the final structure
    _vvl_features = {
        .enabledValidationFeatureCount =
            static_cast<uint32_t>(_vvl_enabled.size()),
        .pEnabledValidationFeatures = _vvl_enabled.data(),
        .disabledValidationFeatureCount =
            static_cast<uint32_t>(_vvl_disabled.size()),
        .pDisabledValidationFeatures = _vvl_disabled.data(),
    };
}

// =============================================================================
bool vkInstance::_check_layers() {
    auto const layers = vk::enumerateInstanceLayerProperties();

    if(layers.empty()) {
        Log::error("Failed to enumerate instance layer properties.");
        return false;
    }

    Log::trace("Found {} instance layers.", layers.size());

    bool all_layers_supported = true;
    for(auto const * const layer_name : _enabled_layers) {
        Log::trace("Requesting instance layer '{}'", layer_name);

        bool layer_found = false;
        for(auto const &layer : layers) {
            if(std::strcmp(layer_name, layer.layerName) == 0) {
                layer_found = true;
                break;
            }
        }

        if(!layer_found) {
            Log::warn("No support for instance layer '{}'", layer_name);
            all_layers_supported = false;
        }
    }

    return all_layers_supported;
}

// =============================================================================
bool vkInstance::_check_extensions() {
    auto const extensions = vk::enumerateInstanceExtensionProperties();

    if(extensions.empty()) {
        Log::error("Failed to enumerate instance extension properties.");
        return false;
    }

    Log::trace("Found {} instance extensions.", extensions.size());

    bool all_extensions_supported = true;
    for(auto const * const ext_name : _enabled_extensions) {
        Log::trace("Requesting instance extension '{}'", ext_name);

        bool extension_found = false;
        for(auto const &extension : extensions) {
            if(std::strcmp(ext_name, extension.extensionName) == 0) {
                extension_found = true;
                break;
            }
        }

        if(!extension_found) {
            Log::warn("No support for instance extension '{}'", ext_name);
            all_extensions_supported = false;
        }
    }

    return all_extensions_supported;
}

} // namespace vkl
