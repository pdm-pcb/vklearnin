#ifndef VKLEARNIN_COMMANDSTRUCTURES_COMMANDQUEUES_HPP
#define VKLEARNIN_COMMANDSTRUCTURES_COMMANDQUEUES_HPP

#include "vklearnin/common.hpp"

class Instance;

// =============================================================================
class CommandQueues final {
public:
    inline void reset_command_buffer(const size_t index,
                                     vk::CommandBufferResetFlagBits flags)
    {
        _command_buffers[index].reset(flags);
    }

    // -------------------------------------------------------------------------
    // Multi-stage setup

    void init_families();
    void init_queue_info();
    void init_pools();
    void init_queues();
    void init_buffers();

    // -------------------------------------------------------------------------
    // For those concerned with command queue atributes

    inline bool is_complete() const {
        return _graphics_family.has_value() && _present_family.has_value();
    }
    inline uint32_t graphics_index() const {
        return _graphics_family.value();
    }
    inline uint32_t present_index() const {
        return _present_family.value(); 
    }
    inline const vk::Queue & graphics_queue() const {
        return _graphics_queue;
    }
    inline const vk::Queue & present_queue() const {
        return _present_queue;
    }
    inline uint32_t queue_count() const {
        return static_cast<uint32_t>(_queue_info_structs.size());
    }
    inline const vk::DeviceQueueCreateInfo * queues() const {
        return _queue_info_structs.data();
    }
    inline const vk::CommandBuffer & command_buffer(const size_t index) const {
        return _command_buffers[index];
    }
    inline const vk::CommandPool & command_pool() const {
        return _command_pool;
    }

    CommandQueues(const vk::SurfaceKHR &surface, const Instance &instance);
    ~CommandQueues();

    CommandQueues() = delete;

    CommandQueues(CommandQueues &&other) = delete;
    CommandQueues(const CommandQueues &other) = delete;

    CommandQueues & operator=(CommandQueues &&other) = delete;
    CommandQueues & operator=(const CommandQueues &other) = delete;

private:
    std::optional<uint32_t> _graphics_family;
    std::optional<uint32_t> _present_family;

    std::vector<vk::DeviceQueueCreateInfo> _queue_info_structs;
    std::vector<float> _queue_priorities;

    vk::Queue  _graphics_queue;
    vk::Queue  _present_queue;

    vk::CommandPool _command_pool;
    std::array<vk::CommandBuffer, FRAME_OVERLAP> _command_buffers;

    const vk::SurfaceKHR &_surface;
    const Instance       &_instance;

};

#endif // VKLEARNIN_COMMANDSTRUCTURES_COMMANDQUEUES_HPP