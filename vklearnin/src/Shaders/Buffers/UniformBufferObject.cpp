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
void UniformBufferObject::init_buffers(const char *alloc_name) {
    CONSOLE_INFO("");

    for(size_t frame = 0; (frame < _buffer_handles.size() &&
                           frame < _memory_handles.size()); ++frame)
    {
        BufferTools::create_buffer(
            _buffer_handles[frame],
            _data_size,
            vk::BufferUsageFlagBits::eUniformBuffer,
            _memory_handles[frame],
            ::VMA_MEMORY_USAGE_CPU_TO_GPU,
            ::VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT,
            fmt::format("{}.{}", alloc_name, frame).c_str()
        );
    }
}

// =============================================================================
UniformBufferObject::UniformBufferObject(const size_t data_size,
                                         const size_t offset,
                                         const Instance &instance) :
    _data_size { data_size },
    _offset    { offset    },
    _instance  { instance  }
{
    CONSOLE_INFO("");

    _buffer_handles.resize(FRAME_OVERLAP);
    _memory_handles.resize(FRAME_OVERLAP);                                                                      
 }

UniformBufferObject::~UniformBufferObject() {
    CONSOLE_INFO("");

    assert(_buffer_handles.size() == _memory_handles.size());
    for(size_t index = 0; index < _buffer_handles.size(); ++index) {
        BufferTools::destroy_buffer(
            _buffer_handles[index],
            _memory_handles[index]
        );
    }
}