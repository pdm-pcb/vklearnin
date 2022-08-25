#ifndef VKL_UNIFORMBUFFEROBJECT_HPP
#define VKL_UNIFORMBUFFEROBJECT_HPP

#include "MVPMatrices.hpp"

#include <vulkan/vulkan.hpp>

#include <vector>

class Instance;

// =============================================================================
class UniformBufferObject {
public:
    void update(const MVPMatrices &data, const uint32_t frame_index);

    void init_buffers();

    inline std::vector<::VkBuffer> & buffer_handles() {
        return _buffer_handles;
    }

    UniformBufferObject(const size_t frames_in_flight,
                        const Instance &instance);
    ~UniformBufferObject();

    UniformBufferObject() = delete;

private:
    std::vector<::VkBuffer>       _buffer_handles;
    std::vector<::VkDeviceMemory> _memory_handles;
    
    const Instance &_instance;
};

#endif // VKL_UNIFORMBUFFEROBJECT_HPP