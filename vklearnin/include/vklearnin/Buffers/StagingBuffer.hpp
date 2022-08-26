#ifndef VKLEARNIN_STAGEDBUFFER_HPP
#define VKLEARNIN_STAGEDBUFFER_HPP

#include "vklearnin/common.hpp"
#include "vklearnin/Instance.hpp"

#include <vulkan/vulkan.h>

#include <vector>

template <typename Datum>
class StagingBuffer {
public:
// =============================================================================
    inline ::VkBuffer handle() const { return _staging_buffer; }

// =============================================================================
    StagingBuffer(const std::vector<Datum> &data, const Instance &instance) :
        _data           { data },
        _buffer_size    { _data.size() * sizeof(Datum) },
        _staging_buffer { nullptr },
        _staging_memory { nullptr },
        _instance       { instance }
    {
        CONSOLE_INFO("");

        _create_staging_buffer();
        _populate_staging_buffer();
    }

    StagingBuffer(const Datum *data_begin, const Datum *data_end,
                  const Instance &instance) :
        _data           { data_begin, data_end },
        _buffer_size    { _data.size() * sizeof(Datum) },
        _staging_buffer { nullptr },
        _staging_memory { nullptr },
        _instance       { instance }
    {
        CONSOLE_INFO("");

        _create_staging_buffer();
        _populate_staging_buffer();
    }

    ~StagingBuffer() {
        CONSOLE_INFO("");

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

    StagingBuffer() = delete;

private:
    std::vector<Datum> _data;
    size_t _buffer_size;

    ::VkBuffer       _staging_buffer;
    ::VkDeviceMemory _staging_memory;

    const Instance  &_instance;

// =============================================================================
    void _create_staging_buffer() {
        CONSOLE_INFO("");

        BufferTools::create_buffer(
            _staging_buffer, sizeof(Datum) * _data.size(),
            ::VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            _staging_memory,
            ::VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
            ::VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            _instance
        );
    }

// =============================================================================
    void _populate_staging_buffer() {
        CONSOLE_INFO("");

        void *_staging_data;
        
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
};

#endif // VKLEARNIN_STAGEDBUFFER_HPP