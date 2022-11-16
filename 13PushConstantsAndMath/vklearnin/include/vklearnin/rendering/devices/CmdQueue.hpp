#ifndef VKLEARNIN_RENDERING_DEVICES_CMDQUEUE_HPP
#define VKLEARNIN_RENDERING_DEVICES_CMDQUEUE_HPP

#include "vklearnin/system/pch.hpp"

namespace vkl {

class CmdQueue {
public:
    void request_queue();

    inline const auto family() const { return _family_index; }
    inline const auto & native() const { return _queue; }

    CmdQueue(const uint32_t family_index, const float priority);
    ~CmdQueue() = default;

    CmdQueue() = delete;

    CmdQueue(CmdQueue &&) = delete;
    CmdQueue(const CmdQueue &) = delete;

    CmdQueue & operator=(CmdQueue &&) = delete;
    CmdQueue & operator=(const CmdQueue &) = delete;

private:
    // Creation-related information for the queue
    uint32_t _family_index;
    float    _priority;
    vk::DeviceQueueCreateInfo _create_info;

    vk::Queue _queue;
};

} // namespace vkl

#endif // VKLEARNIN_RENDERING_DEVICES_CMDQUEUE_HPP