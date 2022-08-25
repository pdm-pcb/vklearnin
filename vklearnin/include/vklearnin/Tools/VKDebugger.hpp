#ifndef VKLEARNIN_VKDEBUGGER_HPP
#define VKLEARNIN_VKDEBUGGER_HPP

#include <vulkan/vulkan.h>

class Instance;

class VKDebugger final {
public:
    static VKAPI_ATTR ::VkBool32 VKAPI_CALL callback(
        ::VkDebugUtilsMessageSeverityFlagBitsEXT severity,
        ::VkDebugUtilsMessageTypeFlagsEXT type,
        const ::VkDebugUtilsMessengerCallbackDataEXT* callback_data,
        void *user_data
    );

    static void init(Instance &instance);
    static void shutdown(Instance &instance);

    VKDebugger();

private:
    static ::VkDebugUtilsMessengerEXT _debug_messenger;
};

#endif // VKLEARNIN_VKDEBUGGER_HPP