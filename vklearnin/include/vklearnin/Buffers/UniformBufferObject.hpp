#ifndef VKLEARNIN_UNIFORMBUFFEROBJECT_HPP
#define VKLEARNIN_UNIFORMBUFFEROBJECT_HPP

#include "vklearnin/Shaders/MVPMatrices.hpp"

#include <vulkan/vulkan.hpp>

#include <vector>

class Instance;

// =============================================================================
class UniformBufferObject {
public:
    void update(const void *data, const uint32_t frame_index);

    void init_buffers();

    inline std::vector<::VkBuffer> & buffer_handles() {
        return _buffer_handles;
    }

    UniformBufferObject(const size_t data_size,
                        const size_t frames_in_flight,
                        const Instance &instance);
    ~UniformBufferObject();

    UniformBufferObject() = delete;

private:
    std::vector<::VkBuffer>       _buffer_handles;
    std::vector<::VkDeviceMemory> _memory_handles;
    
    const size_t    _data_size;
    const Instance &_instance;
};

#endif // VKLEARNIN_UNIFORMBUFFEROBJECT_HPP