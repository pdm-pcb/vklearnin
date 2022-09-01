#ifndef VKLEARNIN_SHADERS_BUFFERS_BUFFEROBJECT_HPP
#define VKLEARNIN_SHADERS_BUFFERS_BUFFEROBJECT_HPP

#include "vklearnin/Instance.hpp"
#include "vklearnin/Shaders/Vertex.hpp"
#include "vklearnin/Shaders/Index.hpp"
#include "vklearnin/Shaders/Buffers/StagingBuffer.hpp"
#include "vklearnin/CommandStructures/SingleUseCommandBuffer.hpp"

#include <vulkan/vulkan.hpp>

#include <vector>
#include <cstdint>
#include <type_traits>

class Instance;

template <typename Datum>
class BufferObject {
public:
//==============================================================================
    void populate_buffer(const vk::CommandPool &pool, const vk::Queue &queue) {
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
                _device_buffer,
                copy_regions
            );
        command_buffer.end();
        command_buffer.submit(queue);

        // we've copied all the data, so the staging buffer can take its leave
        delete _staging_buffer;
    }

//==============================================================================
    inline vk::Buffer handle() const { return _device_buffer; }
    inline size_t count()      const { return _data.size();   }

// =============================================================================
    BufferObject(const std::vector<Datum> &data, const Instance &instance) :
        _staging_buffer { new StagingBuffer<Datum>(data, instance) },
        _device_buffer  { nullptr  }, 
        _device_memory  { nullptr  },
        _data           { data },
        _buffer_size    { sizeof(Datum) * _data.size() },
        _instance       { instance }
    {
        _create_device_buffer();
    }
 
    ~BufferObject() {
        ::vkDestroyBuffer(
            _instance.logical_device(),
            _device_buffer,
            nullptr
        );

        ::vkFreeMemory(
            _instance.logical_device(),
            _device_memory,
            nullptr
        );
    }

    BufferObject() = delete;

private:
    StagingBuffer<Datum> *_staging_buffer;
    vk::Buffer            _device_buffer;
    vk::DeviceMemory      _device_memory;

    const std::vector<Datum> &_data;
    const size_t _buffer_size;

    const Instance &_instance;

    //==========================================================================
    void _create_device_buffer() {
        vk::BufferUsageFlagBits buffer_type;

        if constexpr(std::is_same_v<Datum, Vertex>) {
            buffer_type = vk::BufferUsageFlagBits::eVertexBuffer;
        }
        if constexpr(std::is_same_v<Datum, Index>) {
            buffer_type = vk::BufferUsageFlagBits::eIndexBuffer;
        }

        BufferTools::create_buffer(
            _device_buffer, _buffer_size,
            vk::BufferUsageFlagBits::eTransferDst | buffer_type,
            _device_memory,
            vk::MemoryPropertyFlagBits::eDeviceLocal,
            _instance
        );
    }
};

#endif // VKLEARNIN_SHADERS_BUFFERS_BUFFEROBJECT_HPP