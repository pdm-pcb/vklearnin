#ifndef VKLEARNIN_BUFFERS_BUFFEROBJECT_HPP
#define VKLEARNIN_BUFFERS_BUFFEROBJECT_HPP

#include "vklearnin/Instance.hpp"
#include "vklearnin/Shaders/Vertex.hpp"
#include "vklearnin/Shaders/Index.hpp"
#include "vklearnin/Buffers/StagingBuffer.hpp"
#include "vklearnin/Tools/BufferTools.hpp"

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
        // ---------------------------------------------------------------------
        // allocate a command buffer for this one operation
        ::VkCommandBufferAllocateInfo alloc_info { };
        alloc_info.sType = ::VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        alloc_info.level = ::VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        alloc_info.commandPool = pool;
        alloc_info.commandBufferCount = 1u;

        ::VkCommandBuffer command_buffer;
        auto result = ::vkAllocateCommandBuffers(
            _instance.logical_device(),
            &alloc_info,
            &command_buffer
        );

        if(result != ::VK_SUCCESS) {
            CONSOLE_ERROR("Failed to allocate command buffer.");
            return;
        }

        // open the buffer
        ::VkCommandBufferBeginInfo buffer_info { };
        buffer_info.sType = ::VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        buffer_info.flags = ::VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        result = ::vkBeginCommandBuffer(command_buffer, &buffer_info);

        if(result != ::VK_SUCCESS) {
            CONSOLE_ERROR("Unable to begin command buffer recording.");
            return;
        }

            // copy the goodness
            ::VkBufferCopy copy_region { };
            copy_region.size = _buffer_size;
            ::vkCmdCopyBuffer(
                command_buffer,
                _staging_buffer->handle(),
                _device_buffer,
                1u,
                &copy_region
            );

        // close the buffer
        result = ::vkEndCommandBuffer(command_buffer);
        if(result != ::VK_SUCCESS) {
            CONSOLE_ERROR("Failed to record to command buffer.");
        }

        // finally, submit to the queue
        ::VkSubmitInfo submitInfo{};
        submitInfo.sType = ::VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1u;
        submitInfo.pCommandBuffers = &command_buffer;

        result = ::vkQueueSubmit(
            queue,
            1u,
            &submitInfo,
            nullptr
        );

        if(result != ::VK_SUCCESS) {
            CONSOLE_ERROR("Unable to submit buffer command queue.");
        }

        // give it some time...
        ::vkQueueWaitIdle(queue);

        // and we're done
        ::vkFreeCommandBuffers(
            _instance.logical_device(),
            pool,
            1u,
            &command_buffer
        );

        // done with this, too
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
    ::VkBuffer             _device_buffer;
    ::VkDeviceMemory       _device_memory;

    const std::vector<Datum> &_data;
    const size_t _buffer_size;

    const Instance &_instance;

//==============================================================================
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