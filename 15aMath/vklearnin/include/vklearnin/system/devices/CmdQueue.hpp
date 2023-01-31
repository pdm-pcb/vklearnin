#ifndef VKLEARNIN_SYSTEM_DEVICES_CMDQUEUE_HPP
#define VKLEARNIN_SYSTEM_DEVICES_CMDQUEUE_HPP

#include "vklearnin/system/pch.hpp"

namespace vkl {

class CmdQueue final {
public:
    void fill_create_info(const uint32_t index, const float priority = 1.0f);
    void request_queue();

    inline const auto& native()      const { return _queue;       }
    inline const auto& index()       const { return _index;       }
    inline const auto& priority()    const { return _priority;    }
    inline const auto& create_info() const { return _create_info; }

    CmdQueue();
    ~CmdQueue() = default;

    CmdQueue(CmdQueue &&) = delete;
    CmdQueue(const CmdQueue &) = delete;

    CmdQueue& operator=(CmdQueue &&) = delete;
    CmdQueue& operator=(const CmdQueue &) = delete;

private:
    uint32_t _index;
    float    _priority;
    vk::DeviceQueueCreateInfo _create_info;

    vk::Queue _queue;
};

} // namespace vkl

#endif // VKLEARNIN_SYSTEM_DEVICES_CMDQUEUE_HPP