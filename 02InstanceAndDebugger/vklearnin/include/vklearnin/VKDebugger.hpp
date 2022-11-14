#ifndef VKL_VKDEBUGGER_HPP
#define VKL_VKDEBUGGER_HPP

#include "vklearnin/pch.hpp"

namespace vkl {

class VKDebugger final {
public:
    static VKAPI_ATTR vk::Bool32 VKAPI_CALL callback(
        VkDebugUtilsMessageSeverityFlagBitsEXT      severity,
        VkDebugUtilsMessageTypeFlagsEXT             types,
        const VkDebugUtilsMessengerCallbackDataEXT *callback_data,
        void                                       *user_data
    );

    static void init(vk::Instance &instance);
    static void shutdown(vk::Instance &instance);

    VKDebugger() = delete;

private:
    static vk::DebugUtilsMessengerEXT _debug_messenger;
};

} // namespace vkl
#endif // VKL_VKDEBUGGER_HPP