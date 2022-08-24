#ifndef VKL_STAGEDBUFFER_HPP
#define VKL_STAGEDBUFFER_HPP

#include <vulkan/vulkan.h>

#include <vector>
#include <cstdint>
#include <type_traits>

#include "Instance.hpp"
#include "Vertex.hpp"
#include "Index.hpp"
#include "BufferTools.hpp"

template <typename Datum>
class StagedBuffer {
public:
    void populate_buffer(const ::VkCommandPool &pool, const ::VkQueue &queue) {
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
            CONSOLE_CRITICAL("Failed to allocate command buffer.");
            return;
        }

        ::VkCommandBufferBeginInfo buffer_info { };
        buffer_info.sType = ::VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        buffer_info.flags = ::VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        result = ::vkBeginCommandBuffer(command_buffer, &buffer_info);

        if(result != ::VK_SUCCESS) {
            CONSOLE_CRITICAL("Unable to begin command buffer recording.");
            return;
        }

            ::VkBufferCopy copy_region { };
            copy_region.size = _buffer_size;
            ::vkCmdCopyBuffer(
                command_buffer,
                _staging_buffer,
                _device_buffer,
                1u,
                &copy_region
            );

        result = ::vkEndCommandBuffer(command_buffer);
        if(result != ::VK_SUCCESS) {
            CONSOLE_CRITICAL("Failed to record to command buffer.");
        }

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
            CONSOLE_CRITICAL("Unable to submit buffer command queue.");
        }

        ::vkQueueWaitIdle(queue);

        ::vkFreeCommandBuffers(
            _instance.logical_device(),
            pool,
            1u,
            &command_buffer
        );

        _destroy_staging_buffer();
    }

    inline ::VkBuffer handle() const { return _device_buffer; }
    inline size_t count()      const { return _data.size();   }

    StagedBuffer(const std::vector<Datum> &data, const Instance &instance) :
        _data           { data },
        _buffer_size    { _data.size() * sizeof(Datum) },
        _staging_buffer { nullptr },
        _staging_memory { nullptr },
        _staging_data   { nullptr },
        _device_buffer  { nullptr },
        _device_memory  { nullptr },
        _instance       { instance }
    {
        _create_staging_buffer();
        _populate_staging_buffer();
        _create_device_buffer();
    }

    ~StagedBuffer() {
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

    StagedBuffer() = delete;

private:
    std::vector<Datum> _data;
    size_t _buffer_size;

    ::VkBuffer       _staging_buffer;
    ::VkDeviceMemory _staging_memory;
    void            *_staging_data;
    ::VkBuffer       _device_buffer;
    ::VkDeviceMemory _device_memory;

    const Instance  &_instance;

    void _create_staging_buffer() {
        BufferTools::create_buffer(
            _staging_buffer, sizeof(Datum) * _data.size(),
            ::VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            _staging_memory,
            ::VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
            ::VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            _instance
        );
    }

    void _populate_staging_buffer() {
        ::vkMapMemory(
            _instance.logical_device(),
            _staging_memory,
            0u,
            _buffer_size,
            0u,
            &_staging_data
        );

        memcpy(_staging_data, _data.data(), _buffer_size);

        ::vkUnmapMemory(_instance.logical_device(), _staging_memory);
    }
    
    void _create_device_buffer() {
        ::VkFlags buffer_type_flag;

        if constexpr(std::is_same_v<Datum, Vertex>) {
            buffer_type_flag = ::VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        }
        if constexpr(std::is_same_v<Datum, Index>) {
            buffer_type_flag = ::VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
        }

        BufferTools::create_buffer(
            _device_buffer, sizeof(Datum) * _data.size(),
            ::VK_BUFFER_USAGE_TRANSFER_DST_BIT | buffer_type_flag,
             _device_memory, ::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            _instance
        );
    }
    
    void _destroy_staging_buffer() {
        ::vkDestroyBuffer(
            _instance.logical_device(),
            _staging_buffer,
            nullptr
        );

        ::vkFreeMemory(
            _instance.logical_device(),
            _staging_memory,
            nullptr
        );
    }
};

#endif // VKL_STAGEDBUFFER_HPP