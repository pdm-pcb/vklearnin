#include "vklearnin/vklearnin.hpp"
#include "vklearnin/tools/VKDebugger.hpp"

#include "vklearnin/rendering/GraphicsInstance.hpp"

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
    vk::DebugUtilsMessengerCreateInfoEXT messenger_info {
        .messageSeverity = (
#if defined(__linux__)
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
            vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance
        ),
        .pfnUserCallback = VKDebugger::messenger
    };

    auto result = instance.createDebugUtilsMessengerEXT(
        &messenger_info,
        nullptr,
        &_debug_messenger
    );
    
    if(result != vk::Result::eSuccess) {
        CONSOLE_WARN("Unable to create debug messenger.");
    }
    else {
        CONSOLE_TRACE("Initialized VKDebugger messenger");
    }
}

// =============================================================================
void VKDebugger::shutdown(vk::Instance &instance) {    
    instance.destroy(_debug_messenger);
}

} // namespace vkl