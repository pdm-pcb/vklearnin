#ifndef VKLEARNIN_INSTANCE_HPP
#define VKLEARNIN_INSTANCE_HPP

#include <vulkan/vulkan.hpp>

class CommandQueues;
class Pipeline;
class Swapchain;
class Framebuffers;

#if defined(__linux__)
    class X11Window;
#elif defined(_WIN32)
    class Win32Window;
#endif

// =============================================================================
class Instance final {

#if defined(__linux__)
    using Window = X11Window;
#elif defined(_WIN32)
    using Window = Win32Window;
#endif

public:
    // -------------------------------------------------------------------------
    // Setup

    void init_instance();
    void init_physical_device();
    void init_logical_device(const CommandQueues &command_queues);

    // -------------------------------------------------------------------------
    // For those concerned with instance atributes

    inline const vk::Instance & vulkan_instance() const {
        return _instance;
    }
    inline const vk::PhysicalDevice & physical_device() const {
        return _physical_device;
    }
    inline const vk::Device & logical_device() const {
        return _logical_device;
    }
    inline float max_anisotropy() const {
        return _max_anisotropy;
    }

    explicit Instance(const bool validate);
    ~Instance();

private:
    vk::DynamicLoader  _loader;
    vk::Instance       _instance;
    vk::PhysicalDevice _physical_device;
    vk::Device         _logical_device;

    float _max_anisotropy;

    bool _validate;
};

#endif // VKLEARNIN_INSTANCE_HPP