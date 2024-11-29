#ifndef VKLEARNIN_VULKAN_DEVICES_VKDEVICE_HPP
#define VKLEARNIN_VULKAN_DEVICES_VKDEVICE_HPP

#include "vklearnin/pch.hpp"

#include "vklearnin/vulkan/devices/vkQueue.hpp"
#include "vklearnin/vulkan/devices/vkCmdPool.hpp"

namespace vkl {

class vkPhysicalDevice;

class vkDevice final {
public:
    vkDevice() = default;
    ~vkDevice() = default;

    vkDevice(vkDevice &&) = delete;
    vkDevice(vkDevice const &) = delete;

    vkDevice & operator=(vkDevice &&) = delete;
    vkDevice & operator=(vkDevice const &) = delete;

    bool create(vkPhysicalDevice const &physical_device);
    bool destroy();

    bool wait_idle() const;

    inline auto const & native() const { return _handle; }
    inline auto const & cmd_queue() const { return _cmd_queue; }
    inline auto const & transient_pool() const { return _transient_pool; }

private:
    vk::Device _handle { nullptr };
    vkCmdPool  _transient_pool;
    vkQueue    _cmd_queue;
};

} // namespace vkl

#endif // VKLEARNIN_VULKAN_DEVICES_VKDEVICE_HPP