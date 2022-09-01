#include "vklearnin/common.hpp"
#include "vklearnin/Tools/VKDebugger.hpp"

#include "vklearnin/Instance.hpp"

vk::DebugUtilsMessengerEXT VKDebugger::_debug_messenger { };

// =============================================================================
VKAPI_ATTR vk::Bool32 VKAPI_CALL VKDebugger::callback(
        VkDebugUtilsMessageSeverityFlagBitsEXT severity,
        [[maybe_unused]] VkDebugUtilsMessageTypeFlagsEXT types,
        const VkDebugUtilsMessengerCallbackDataEXT *callback_data,
        [[maybe_unused]] void *user_data)
{
    if(strstr(callback_data->pMessage, "small-allocation") ||
       strstr(callback_data->pMessage, "small-dedicated-allocation"))
    {
        return false;
    }

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
            CONSOLE_TRACE("???: {:s}", callback_data->pMessage);
            break;
    }

    return false;
}

// =============================================================================
void VKDebugger::init(vk::Instance &instance) {

    vk::DebugUtilsMessengerCreateInfoEXT debug_info {
        .messageSeverity = (
#if defined(__linux__)
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
#endif
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo    |
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eError
        ),
        .messageType = (
            vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral    |
            vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
            vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance
        ),
        .pfnUserCallback = VKDebugger::callback
    };

    auto result = instance.createDebugUtilsMessengerEXT(
        &debug_info,
        nullptr,
        &_debug_messenger
    );
    
    if(result != vk::Result::eSuccess) {
        CONSOLE_WARN("Unable to create debug messenger.");
    }
}

// =============================================================================
void VKDebugger::shutdown(vk::Instance &instance) {    
    instance.destroy(_debug_messenger);
}