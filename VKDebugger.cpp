#include "common.hpp"
#include "VKDebugger.hpp"

#include "Instance.hpp"

::VkDebugUtilsMessengerEXT VKDebugger::_debug_messenger = nullptr;

VKAPI_ATTR ::VkBool32 VKAPI_CALL VKDebugger::callback(
    ::VkDebugUtilsMessageSeverityFlagBitsEXT severity,
    ::VkDebugUtilsMessageTypeFlagsEXT type,
    const ::VkDebugUtilsMessengerCallbackDataEXT* callback_data,
    [[maybe_unused]] void *user_data)
{
    switch(severity) {
        case ::VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
            CONSOLE_TRACE("\n\t{:s}", callback_data->pMessage);
            break;
        case ::VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
            CONSOLE_INFO("\n\t{:s}", callback_data->pMessage);
            break;
        case ::VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            if((type & ::VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT) ==
               type)
            {
                CONSOLE_WARN("\n\tPerformance: {:s}", callback_data->pMessage);
            }
            else if((type & ::VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT) ==
               type)
            {
                CONSOLE_WARN("\n\tValidation: {:s}", callback_data->pMessage);
            }
            else {
                CONSOLE_WARN("\n\tUnknown: {:s}", callback_data->pMessage);
            }
            break;
        case ::VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
            CONSOLE_ERROR("\n\t{:s}", callback_data->pMessage);
            break;
        default:
            CONSOLE_TRACE("\n\tUnknwon: {:s}", callback_data->pMessage);
            break;
    }

    return VK_FALSE;
}

void VKDebugger::init(Instance &instance) {

    ::VkDebugUtilsMessengerCreateInfoEXT debug_info { };
    debug_info.sType =
        ::VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    debug_info.messageSeverity =
        ::VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
        ::VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    debug_info.messageType =
        ::VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
        ::VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    debug_info.pfnUserCallback = VKDebugger::callback;
    debug_info.pUserData = nullptr;

    ::VkResult result = instance._CreateDebugUtilsMessengerEXT(
        instance.vulkan_instance(),
        &debug_info,
        nullptr,
        &_debug_messenger
    );
    
    if(result != ::VK_SUCCESS) {
        CONSOLE_WARN("Unable to create debug messenger.");
    }
}

void VKDebugger::shutdown(Instance &instance) {    
    instance._DestroyDebugUtilsMessengerEXT(
        instance.vulkan_instance(),
        _debug_messenger,
        nullptr
    );
}