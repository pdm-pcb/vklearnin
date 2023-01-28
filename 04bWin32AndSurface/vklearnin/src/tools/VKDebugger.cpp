#include "vklearnin/vklearnin.hpp"
#include "vklearnin/tools/VKDebugger.hpp"

#include "vklearnin/system/GraphicsAPI.hpp"

namespace vkl {

vk::DebugUtilsMessengerEXT VKDebugger::_debug_messenger { };

// =============================================================================
VKAPI_ATTR vk::Bool32 VKAPI_CALL VKDebugger::messenger(
        VkDebugUtilsMessageSeverityFlagBitsEXT severity,
        [[maybe_unused]] VkDebugUtilsMessageTypeFlagsEXT types,
        const VkDebugUtilsMessengerCallbackDataEXT *callback_data,
        [[maybe_unused]] void *user_data)
{
    switch(severity) {
        case ::VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
            CONSOLE_TRACE("{:s}", callback_data->pMessage);
            break;
        case ::VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
            CONSOLE_INFO("{:s}", callback_data->pMessage);
            break;
        case ::VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            CONSOLE_WARN("\n{:s}\n", callback_data->pMessage);
            break;
        case ::VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
            CONSOLE_ERROR("\n{:s}\n", callback_data->pMessage);
            assert(false);
            break;
        default:
            CONSOLE_CRITICAL("???: {:s}", callback_data->pMessage);
            assert(false);
            break;
    }

    return false;
}

// =============================================================================
void VKDebugger::init(vk::Instance &instance) {
    // Populate the create info struct with the severity levels we're
    // interested in, the types we're interested in, and offer a callback
    // pointer to the API
    vk::DebugUtilsMessengerCreateInfoEXT messenger_info {
        .messageSeverity = (
#if defined(VKL_LINUX)
			// I find this log level too verbose (ha) on Windows, but it can
            // be helpful on Linux
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
#endif
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo    |
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eError
        ),
        .messageType = (
            vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral     |
            vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation  |
            vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
            vk::DebugUtilsMessageTypeFlagBitsEXT::eDeviceAddressBinding
        ),
        .pfnUserCallback = VKDebugger::messenger
    };

    // Give it a shot
    auto result = instance.createDebugUtilsMessengerEXT(
        &messenger_info,
        nullptr,
        &_debug_messenger
    );
    
    // React accordingly
    if(result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL(
            "Unable to create debug messenger: '{}'",
            to_string(result)
        );
    }
    else {
        CONSOLE_TRACE(
            "Created VKDebugger messenger {:#x}",
            reinterpret_cast<uint64_t>(
                VkDebugUtilsMessengerEXT(_debug_messenger)
            )
        );
    }
}

// =============================================================================
void VKDebugger::shutdown(vk::Instance &instance) {
    CONSOLE_TRACE(
        "Destroying VKDebugger messenger {:#x}",
            reinterpret_cast<uint64_t>(
                VkDebugUtilsMessengerEXT(_debug_messenger)
            )
    );
    instance.destroy(_debug_messenger);
}

} // namespace vkl