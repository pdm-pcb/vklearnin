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

class UniformBufferObject {
public:
    void init_descriptor_set();
    void init_buffers();

    inline ::VkDescriptorSetLayout descriptor_set_layout() const {
        return _desc_set_layout;
    }

    UniformBufferObject(const ::VkDevice &device,
                        const size_t frames_in_flight);
    ~UniformBufferObject();

    UniformBufferObject() = delete;

private:
    ::VkDescriptorSetLayout _desc_set_layout;

    std::vector<::VkBuffer>       _buffer_handles;
    std::vector<::VkDeviceMemory> _memory_handles;
    
    const ::VkDevice &_device;
};

#endif // VKL_UNIFORMBUFFEROBJECT_HPP