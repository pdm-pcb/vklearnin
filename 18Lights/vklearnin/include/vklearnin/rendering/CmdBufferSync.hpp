#ifndef VKLEARNIN_RENDERING_CMDBUFFERSYNC_HPP
#define VKLEARNIN_RENDERING_CMDBUFFERSYNC_HPP

#include "vklearnin/system/pch.hpp"
#include "vklearnin/system/devices/CmdPool.hpp"
#include "vklearnin/system/devices/CmdBuffer.hpp"

namespace vkl {

class CmdBufferSync {
public:
    void wait_on_queue_fence() const;
    void submit_to_device() const;

    void init();
    void shutdown();

    inline auto const & cmd_pool()   const { return _cmd_pool; }
    inline auto const & cmd_buffer() const { return _cmd_buffer; }

    inline auto const & acquire_complete_sem() const {
        return _acquire_complete;
    }

    inline auto const & commands_complete_sem() const {
        return _commands_complete;
    }

    inline auto const & queue_complete_fence() const {
        return _queue_complete;
    }

    inline void set_image_index(uint32_t const index) { _image_index = index; }
    inline auto image_index() const { return _image_index; }

    CmdBufferSync();
    ~CmdBufferSync() = default;

    CmdBufferSync(CmdBufferSync &&other) noexcept;
    CmdBufferSync(CmdBufferSync const &) = delete;

    CmdBufferSync & operator=(CmdBufferSync &&) = delete;
    CmdBufferSync & operator=(CmdBufferSync const &) = delete;

private:
    CmdPool   _cmd_pool;
    CmdBuffer _cmd_buffer;

    vk::Semaphore _acquire_complete;
    vk::Semaphore _commands_complete;
    vk::Fence     _queue_complete;

    uint32_t _image_index;

    void _create_cmd_structures();
    void _destroy_cmd_structures();
    void _create_sync_primitives();
    void _destroy_sync_primitives();
};

} // namespace vkl

#endif // VKLEARNIN_RENDERING_CMDBUFFERSYNC_HPP