#ifndef VKLEARNIN_COMMANDSTRUCTURES_COMMANDQUEUES_HPP
#define VKLEARNIN_COMMANDSTRUCTURES_COMMANDQUEUES_HPP

#include "vklearnin/common.hpp"

class Instance;

// =============================================================================
class CommandQueues {
public:
    inline void reset_command_buffer(const size_t index,
                                     ::VkCommandBufferResetFlagBits flags)
    {
        ::vkResetCommandBuffer(_command_buffers[index], flags);
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
    inline const ::VkQueue & graphics_queue() const {
        return _graphics_queue;
    }
    inline const ::VkQueue & present_queue() const {
        return _present_queue;
    }
    inline uint32_t queue_count() const {
        return static_cast<uint32_t>(_queue_info_structs.size());
    }
    inline const ::VkDeviceQueueCreateInfo * queues() const {
        return _queue_info_structs.data();
    }
    inline const ::VkCommandBuffer & command_buffer(const size_t index) const {
        return _command_buffers[index];
    }
    inline const ::VkCommandPool & command_pool() const {
        return _command_pool;
    }

    CommandQueues(const ::VkSurfaceKHR &surface, const Instance &instance);
    ~CommandQueues();

    CommandQueues() = delete;

private:
    std::optional<uint32_t> _graphics_family;
    std::optional<uint32_t> _present_family;

    std::vector<::VkDeviceQueueCreateInfo> _queue_info_structs;
    std::vector<float> _queue_priorities;

    ::VkQueue  _graphics_queue;
    ::VkQueue  _present_queue;

    ::VkCommandPool _command_pool;
    std::array<::VkCommandBuffer, MAX_IMAGES> _command_buffers;

    const ::VkSurfaceKHR &_surface;
    const Instance       &_instance;

};

#endif // VKLEARNIN_COMMANDSTRUCTURES_COMMANDQUEUES_HPP