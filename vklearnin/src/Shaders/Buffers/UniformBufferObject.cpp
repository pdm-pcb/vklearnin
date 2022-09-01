#include "vklearnin/common.hpp"
#include "vklearnin/Shaders/Buffers/UniformBufferObject.hpp"

#include "vklearnin/Instance.hpp"

// =============================================================================
void UniformBufferObject::update(const void *data, const uint32_t frame_index)
{
    void *destination = nullptr;
    ::vkMapMemory(
        _instance.logical_device(),
        _memory_handles[frame_index],
        0u,
        _data_size,
        0u,
        &destination
    );

    memcpy(destination, data, _data_size);

    ::vkUnmapMemory(_instance.logical_device(), _memory_handles[frame_index]);
}

// =============================================================================
void UniformBufferObject::init_buffers() {
    CONSOLE_INFO("");

    for(size_t frame = 0; (frame < _buffer_handles.size() &&
                           frame < _memory_handles.size()); ++frame)
    {
        BufferTools::create_buffer(
            _buffer_handles[frame],
            _data_size,
            vk::BufferUsageFlagBits::eUniformBuffer,
            _memory_handles[frame],
            vk::MemoryPropertyFlagBits::eHostVisible |
            vk::MemoryPropertyFlagBits::eHostCoherent,
            _instance
        );
    }
}

// =============================================================================
UniformBufferObject::UniformBufferObject(const size_t data_size,
                                         const size_t frames_in_flight,
                                         const Instance &instance) :
    _data_size { data_size },
    _instance  { instance  }
{
    CONSOLE_INFO("");

    _buffer_handles.resize(frames_in_flight);
    _memory_handles.resize(frames_in_flight);                                                                      
 }

UniformBufferObject::~UniformBufferObject() {
    CONSOLE_INFO("");

    for(auto &buffer : _buffer_handles) {
        ::vkDestroyBuffer(_instance.logical_device(), buffer, nullptr);
    }

    for(auto &memory : _memory_handles) {
        ::vkFreeMemory(_instance.logical_device(), memory, nullptr);
    }
}