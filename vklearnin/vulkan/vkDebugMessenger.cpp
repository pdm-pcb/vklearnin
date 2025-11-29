#include "vklearnin/vklearnin.hpp"
#include "vklearnin/vulkan/vkDebugMessenger.hpp"

#include "vklearnin/vulkan/vkInstance.hpp"

namespace vkl {

// =============================================================================
bool vkDebugMessenger::create(vkInstance const &instance) {
    if(_handle) {
        Log::error("Vulkan debug messenger {} already exists.", _handle);
        return false;
    }

    if(!instance.native()) {
        Log::error("Cannot create debug messenger with invalid instance.");
        return false;
    }

    _instance = instance.native();

    // Populate the create info struct with the severity levels we're
    // interested in, the types we're interested in, and offer a callback
    // pointer to the API
    const vk::DebugUtilsMessengerCreateInfoEXT messenger_info {
        .messageSeverity = (
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose
            | vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo
            | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning
            | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError
        ),
        .messageType = (
            vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral
            | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation
            | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance
        ),
        .pfnUserCallback = vkDebugMessenger::messenger
    };

    // Give it a shot
    auto const [ result, value ] =
        _instance.createDebugUtilsMessengerEXT(messenger_info);

    // React accordingly
    if(result != vk::Result::eSuccess) {
        Log::error("Unable to create Vulkan debug messenger: '{}'",
                  vk::to_string(result));
        return false;
    }

    _handle = value;
    Log::trace("Created Vulkan debug messenger {}", _handle);

    return true;
}

// =============================================================================
bool vkDebugMessenger::destroy() {
    if(!_handle) {
        Log::error("Must create Vulkan debug messenger before calling destroy.");
        return false;
    }

    Log::trace("Destroying Vulkan debug messenger {}", _handle);
    _instance.destroy(_handle);
    _handle = nullptr;
    _instance = nullptr;

    return true;
}

// =============================================================================
VKAPI_ATTR vk::Bool32 VKAPI_CALL vkDebugMessenger::messenger(
    vk::DebugUtilsMessageSeverityFlagBitsEXT severity,
    [[maybe_unused]] vk::DebugUtilsMessageTypeFlagsEXT types,
    vk::DebugUtilsMessengerCallbackDataEXT const *callback_data,
    [[maybe_unused]] void *user_data)
{
    if(severity == vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose) {
        Log::trace("{}", callback_data->pMessage);
    }
    else if(severity == vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo) {
        Log::info("{}", callback_data->pMessage);
    }
    else if(severity == vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning) {
        Log::warn("{}", callback_data->pMessage);
    }
    else if(severity == vk::DebugUtilsMessageSeverityFlagBitsEXT::eError) {
        Log::error("\n{}\n", callback_data->pMessage);
        // assert(false);
    }

    return vk::False;
}

} // namespace vkl
