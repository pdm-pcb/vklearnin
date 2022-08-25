#ifndef VKL_UNIFORMBUFFEROBJECT_HPP
#define VKL_UNIFORMBUFFEROBJECT_HPP

#include <glm/glm.hpp>
#include <vulkan/vulkan.hpp>

#include <vector>

struct MVPMatrices {
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
};

class Instance;

// =============================================================================
class UniformBufferObject {
public:
    void update(const MVPMatrices &data, const uint32_t frame_index);

    void init_descriptor_set();
    void init_buffers();

    inline ::VkDescriptorSetLayout descriptor_set_layout() const {
        return _desc_set_layout;
    }

    UniformBufferObject(const size_t frames_in_flight,
                        const Instance &instance);
    ~UniformBufferObject();

    UniformBufferObject() = delete;

private:
    ::VkDescriptorSetLayout _desc_set_layout;

    std::vector<::VkBuffer>       _buffer_handles;
    std::vector<::VkDeviceMemory> _memory_handles;
    
    const Instance &_instance;
};

#endif // VKL_UNIFORMBUFFEROBJECT_HPP