#ifndef VKLEARNIN_STAGEDBUFFER_HPP
#define VKLEARNIN_STAGEDBUFFER_HPP

#include "vklearnin/pch.hpp"
#include "vklearnin/common.hpp"
#include "vklearnin/Instance.hpp"

template <typename Datum>
class StagingBuffer {
public:
// =============================================================================
    inline vk::Buffer handle() const { return _staging_buffer; }

// =============================================================================
    StagingBuffer(const std::vector<Datum> &data, const Instance &instance,
                  const char *alloc_name) :
        _data           { data },
        _buffer_size    { _data.size() * sizeof(Datum) },
        _staging_buffer { nullptr },
        _staging_memory { nullptr },
        _instance       { instance }
    {
        CONSOLE_INFO("");

        _create_staging_buffer(alloc_name);
        _populate_staging_buffer();
    }

    StagingBuffer(const Datum *data_begin, const Datum *data_end,
                  const Instance &instance, const char *alloc_name) :
        _data           { data_begin, data_end },
        _buffer_size    { _data.size() * sizeof(Datum) },
        _staging_buffer { nullptr },
        _staging_memory { nullptr },
        _instance       { instance }
    {
        CONSOLE_INFO("");

        _create_staging_buffer(alloc_name);
        _populate_staging_buffer();
    }

    ~StagingBuffer() {
        CONSOLE_INFO("");

        BufferTools::destroy_buffer(_staging_buffer, _staging_memory);
    }

    StagingBuffer() = delete;

    StagingBuffer(StagingBuffer &&other) = delete;
    StagingBuffer(const StagingBuffer &other) = delete;

    StagingBuffer & operator=(StagingBuffer &&other) = delete;
    StagingBuffer & operator=(const StagingBuffer &other) = delete;

private:
    std::vector<Datum> _data;
    size_t _buffer_size;

    vk::Buffer      _staging_buffer;
    ::VmaAllocation _staging_memory;

    const Instance  &_instance;

// =============================================================================
    void _create_staging_buffer(const char *alloc_name) {
        CONSOLE_INFO("");

        BufferTools::create_buffer(
            _staging_buffer, sizeof(Datum) * _data.size(),
            vk::BufferUsageFlagBits::eTransferSrc,
            _staging_memory,
            ::VMA_MEMORY_USAGE_CPU_ONLY,
            ::VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT,
            alloc_name
        );
    }

// =============================================================================
    void _populate_staging_buffer() {
        CONSOLE_INFO("");

        void *staging_data = nullptr;
        
        ::vmaMapMemory(
            Allocator::allocator(),
            _staging_memory,
            &staging_data
        );

        memcpy(staging_data, _data.data(), _buffer_size);

        ::vmaUnmapMemory(Allocator::allocator(), _staging_memory);
    }
};

#endif // VKLEARNIN_STAGEDBUFFER_HPP