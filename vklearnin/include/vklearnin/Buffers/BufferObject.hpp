#ifndef VKLEARNIN_BUFFERS_BUFFEROBJECT_HPP
#define VKLEARNIN_BUFFERS_BUFFEROBJECT_HPP

#include "vklearnin/Instance.hpp"
#include "vklearnin/Shaders/Vertex.hpp"
#include "vklearnin/Shaders/Index.hpp"
#include "vklearnin/Buffers/StagingBuffer.hpp"
#include "vklearnin/SingleUseCommandBuffer.hpp"

#include <vulkan/vulkan.h>

#include <vector>
#include <cstdint>
#include <type_traits>

class Instance;

template <typename Datum>
class BufferObject {
public:
//==============================================================================
    void populate_buffer(const ::VkCommandPool &pool, const ::VkQueue &queue) {
        // I'm glad the tutorial mentioned making this into its own class. It's
        // quite good sense.
        SingleUseCommandBuffer command_buffer(pool, _instance);
        auto buffer_handle = command_buffer.init();

        command_buffer.begin();
            // copy the goodness
            ::VkBufferCopy copy_region { };
            copy_region.size = _buffer_size;
            ::vkCmdCopyBuffer(
                buffer_handle,
                _staging_buffer->handle(),
                _device_buffer,
                1u,
                &copy_region
            );
        command_buffer.end();
        command_buffer.submit(queue);

        // we've copied all the data, so the staging buffer can take its leave
        delete _staging_buffer;
    }

//==============================================================================
    inline ::VkBuffer handle() const { return _device_buffer; }
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
    ::VkBuffer            _device_buffer;
    ::VkDeviceMemory      _device_memory;

    const std::vector<Datum> &_data;
    const size_t _buffer_size;

    const Instance &_instance;

    //==========================================================================
    void _create_device_buffer() {
        ::VkBufferUsageFlagBits buffer_type;

        if constexpr(std::is_same_v<Datum, Vertex>) {
            buffer_type = ::VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        }
        if constexpr(std::is_same_v<Datum, Index>) {
            buffer_type = ::VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
        }

        BufferTools::create_buffer(
            _device_buffer, _buffer_size,
            ::VK_BUFFER_USAGE_TRANSFER_DST_BIT | buffer_type,
            _device_memory,
            ::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            _instance
        );
    }
};

#endif // VKLEARNIN_BUFFERS_BUFFEROBJECT_HPP