#include "vklearnin/common.hpp"
#include "vklearnin/Shaders/Buffers/UniformBufferObject.hpp"

#include "vklearnin/Instance.hpp"

// =============================================================================
void UniformBufferObject::update(const void *data, const uint32_t frame_index)
{
    void *destination = nullptr;
    ::vmaMapMemory(
        Allocator::allocator(),
        _memory_handles[frame_index],
        &destination
    );

    memcpy(destination, data, _data_size);

    ::vmaUnmapMemory(Allocator::allocator(), _memory_handles[frame_index]);
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
            ::VMA_MEMORY_USAGE_AUTO,
            ::VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT
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

    assert(_buffer_handles.size() == _memory_handles.size());
    for(size_t index = 0; index < _buffer_handles.size(); ++index) {
        CONSOLE_TRACE(
            "Destroying UBO buffer {}",
            fmt::ptr(&_buffer_handles[index])
        );
        ::vmaDestroyBuffer(
            Allocator::allocator(),
            _buffer_handles[index],
            _memory_handles[index]
        );
    }
}