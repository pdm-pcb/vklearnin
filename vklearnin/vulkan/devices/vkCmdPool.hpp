#ifndef VKLEARNIN_VULKAN_DEVICES_VKCMDPOOL_HPP
#define VKLEARNIN_VULKAN_DEVICES_VKCMDPOOL_HPP

#include "vklearnin/pch.hpp"

namespace vkl {

class vkDevice;

class vkCmdPool final {
public:

    vkCmdPool() = default;
    ~vkCmdPool() = default;

    vkCmdPool(vkCmdPool &&other);
    vkCmdPool(vkCmdPool const &) = delete;

    vkCmdPool & operator=(vkCmdPool &&) = delete;
    vkCmdPool & operator=(vkCmdPool const &) = delete;

    bool create(vkDevice const &device,
                vk::CommandPoolCreateInfo const &create_info);
    bool destroy();

    bool reset(vk::CommandPoolResetFlags const flags = { }) const;

    [[nodiscard]] inline auto const & native() const { return _handle; }

private:
    vk::CommandPool _handle { nullptr };
    vk::Device      _device { nullptr };
};

} // namespace vkl

#endif // VKLEARNIN_VULKAN_DEVICES_VKCMDPOOL_HPP