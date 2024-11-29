#ifndef VKLEARNIN_VULKAN_DEVICES_VKCMDBUFFER_HPP
#define VKLEARNIN_VULKAN_DEVICES_VKCMDBUFFER_HPP

#include "vklearnin/pch.hpp"

namespace vkl {

class vkDevice;
class vkCmdPool;
class vkQueue;

class vkCmdBuffer final {
public:
    vkCmdBuffer() = default;
    ~vkCmdBuffer() = default;

    vkCmdBuffer(vkCmdBuffer &&other);
    vkCmdBuffer(vkCmdBuffer const &) = delete;

    vkCmdBuffer & operator=(vkCmdBuffer &&) = delete;
    vkCmdBuffer & operator=(vkCmdBuffer const &) = delete;

    bool allocate(vkDevice const &device, vkCmdPool const &pool,
                  vkQueue const &queue);
    bool free();

    bool begin_one_time_submit() const;

    void end_render_pass() const;
    void end_recording() const;

    struct GroupCounts final {
        uint32_t x { 0u };
        uint32_t y { 0u };
        uint32_t z { 0u };
    };
    void dispatch(GroupCounts const &group_counts) const;

    inline auto const & native() const { return _handle; }

private:
    vk::CommandBuffer _handle { nullptr };
    vk::Device        _device { nullptr };

    vk::CommandPool _pool  { nullptr };
    vk::Queue       _queue { nullptr };
};

} // namespace vkl

#endif // VKLEARNIN_VULKAN_DEVICES_VKCMDBUFFER_HPP