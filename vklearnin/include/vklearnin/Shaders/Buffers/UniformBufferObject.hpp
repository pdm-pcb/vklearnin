#ifndef VKLEARNIN_SHADERS_UNIFORMBUFFEROBJECT_HPP
#define VKLEARNIN_SHADERS_UNIFORMBUFFEROBJECT_HPP

#include "vklearnin/pch.hpp"
#include "vklearnin/Shaders/MVPMatrices.hpp"
#include "vklearnin/Tools/Allocator.hpp"

class Instance;

// =============================================================================
class UniformBufferObject {
public:
    void update(const void *data, const uint32_t frame_index);

    void init_buffers();

    inline std::vector<vk::Buffer> & buffer_handles() {
        return _buffer_handles;
    }

    UniformBufferObject(const size_t data_size,
                        const size_t frames_in_flight,
                        const Instance &instance);
    ~UniformBufferObject();

    UniformBufferObject() = delete;

    UniformBufferObject(UniformBufferObject &&other) = delete;
    UniformBufferObject(const UniformBufferObject &other) = delete;

    UniformBufferObject & operator=(UniformBufferObject &&other) = delete;
    UniformBufferObject & operator=(const UniformBufferObject &other) = delete;

private:
    std::vector<vk::Buffer>    _buffer_handles;
    std::vector<VmaAllocation> _memory_handles;
    
    const size_t    _data_size;
    const Instance &_instance;
};

#endif // VKLEARNIN_SHADERS_UNIFORMBUFFEROBJECT_HPP