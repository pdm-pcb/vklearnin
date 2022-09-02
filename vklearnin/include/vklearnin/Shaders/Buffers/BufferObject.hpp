#ifndef VKLEARNIN_SHADERS_BUFFERS_BUFFEROBJECT_HPP
#define VKLEARNIN_SHADERS_BUFFERS_BUFFEROBJECT_HPP

#include "vklearnin/common.hpp"

#include "vklearnin/Instance.hpp"
#include "vklearnin/Shaders/Buffers/StagingBuffer.hpp"
#include "vklearnin/CommandStructures/SingleUseCommandBuffer.hpp"

class Instance;

template <typename Datum>
class BufferObject {
public:
//==============================================================================
    void populate_buffer(const vk::CommandPool &pool, const vk::Queue &queue) {
        CONSOLE_INFO("");

        // I'm glad the tutorial mentioned making this into its own class. It's
        // quite good sense.
        SingleUseCommandBuffer command_buffer(pool, _instance);
        auto buffer_handle = command_buffer.init();

        command_buffer.begin();
            // copy the goodness
            vk::BufferCopy copy_regions[] {{
                .srcOffset = 0u,
                .dstOffset = 0u,
                .size = _buffer_size,
            }};
            
            buffer_handle.copyBuffer(
                _staging_buffer->handle(),
                _buffer,
                copy_regions
            );
        command_buffer.end();
        command_buffer.submit(queue);

        // we've copied all the data, so the staging buffer can take its leave
        delete _staging_buffer;
    }

//==============================================================================
    inline vk::Buffer handle() const { return _buffer; }
    inline size_t count()      const { return _data.size();   }

// =============================================================================
    BufferObject(const std::vector<Datum> &data, const Instance &instance) :
        _staging_buffer { new StagingBuffer<Datum>(data, instance) },
        _buffer         { nullptr },
        _memory         { nullptr },
        _data           { data },
        _buffer_size    { sizeof(Datum) * _data.size() },
        _instance       { instance }
    {
        CONSOLE_INFO("");

        _create_device_buffer();
    }
 
    ~BufferObject() {
        CONSOLE_INFO("");

        CONSOLE_TRACE(
            "Destroying buffer object {}",
            fmt::ptr(&_buffer)
        );

        ::vmaDestroyBuffer(Allocator::allocator(), _buffer, _memory);
    }

    BufferObject() = delete;

    BufferObject(BufferObject &&other) = delete;
    BufferObject(const BufferObject &other) = delete;

    BufferObject & operator=(BufferObject &&other) = delete;
    BufferObject & operator=(const BufferObject &other) = delete;

private:
    StagingBuffer<Datum> *_staging_buffer;
    vk::Buffer    _buffer;
    VmaAllocation _memory;

    const std::vector<Datum> &_data;
    const size_t _buffer_size;

    const Instance &_instance;

    //==========================================================================
    void _create_device_buffer() {
        CONSOLE_INFO("");

        vk::BufferUsageFlagBits buffer_type;

        if constexpr(std::is_same_v<Datum, Vertex>) {
            buffer_type = vk::BufferUsageFlagBits::eVertexBuffer;
        }
        if constexpr(std::is_same_v<Datum, Index>) {
            buffer_type = vk::BufferUsageFlagBits::eIndexBuffer;
        }

        BufferTools::create_buffer(
            _buffer, _buffer_size,
            vk::BufferUsageFlagBits::eTransferDst | buffer_type,
            _memory, ::VMA_MEMORY_USAGE_CPU_TO_GPU
        );
    }
};

#endif // VKLEARNIN_SHADERS_BUFFERS_BUFFEROBJECT_HPP