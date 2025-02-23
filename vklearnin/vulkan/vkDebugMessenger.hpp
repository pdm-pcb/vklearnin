#ifndef VKLEARNIN_VULKAN_VKDEBUGMESSENGER_HPP
#define VKLEARNIN_VULKAN_VKDEBUGMESSENGER_HPP

#include "vklearnin/pch.hpp"

namespace vkl {

class vkInstance;

class vkDebugMessenger final {
public:
    vkDebugMessenger() = default;
    ~vkDebugMessenger() = default;

    vkDebugMessenger(vkDebugMessenger &&) = delete;
    vkDebugMessenger(vkDebugMessenger const &) = delete;

    vkDebugMessenger & operator=(vkDebugMessenger &&) = delete;
    vkDebugMessenger & operator=(vkDebugMessenger const &) = delete;

    bool create(vkInstance const &instance);
    bool destroy();

    inline auto const & native() const { return _handle; }

    static VKAPI_ATTR vk::Bool32 VKAPI_CALL messenger(
        vk::DebugUtilsMessageSeverityFlagBitsEXT      severity,
        vk::DebugUtilsMessageTypeFlagsEXT             types,
        vk::DebugUtilsMessengerCallbackDataEXT const *callback_data,
        void                                         *user_data
    );

private:
    vk::DebugUtilsMessengerEXT _handle { nullptr };
    vk::Instance _instance { nullptr };
};

} // namespace vkl

#endif // VKLEARNIN_VULKAN_VKDEBUGMESSENGER_HPP