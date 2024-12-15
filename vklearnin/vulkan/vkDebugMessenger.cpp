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
        Log::error("Cannot create swapchain with invalid instance.");
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
#ifdef VK_EXT_DEVICE_ADDRESS_BINDING_REPORT_EXTENSION_NAME
            | vk::DebugUtilsMessageTypeFlagBitsEXT::eDeviceAddressBinding
#endif // VK_EXT_DEVICE_ADDRESS_BINDING_REPORT_EXTENSION_NAME
        ),
        .pfnUserCallback = vkDebugMessenger::messenger
    };

    // Give it a shot
    auto const result = _instance.createDebugUtilsMessengerEXT(
        &messenger_info,
        nullptr,
        &_handle
    );

    // React accordingly
    if(result != vk::Result::eSuccess) {
        Log::error("Unable to create Vulkan debug messenger: '{}'",
                  vk::to_string(result));
        return false;
    }

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
        ::VkDebugUtilsMessageSeverityFlagBitsEXT severity,
        [[maybe_unused]] ::VkDebugUtilsMessageTypeFlagsEXT types,
        const ::VkDebugUtilsMessengerCallbackDataEXT *callback_data,
        [[maybe_unused]] void *user_data)
{
    if((severity & ::VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT) != 0) {
        Log::trace("{}", callback_data->pMessage);
    }
    if((severity & ::VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT) != 0) {
        Log::info("{}", callback_data->pMessage);
    }
    if((severity & ::VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) != 0) {
        Log::warn("{}", callback_data->pMessage);
    }
    if((severity & ::VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) != 0) {
        Log::error("\n{}\n", callback_data->pMessage);
        assert(false);
    }

    return vk::False;
}

} // namespace vkl
