#include "common.hpp"
#include "UniformBufferObject.hpp"

#include "Instance.hpp"

// =============================================================================
void UniformBufferObject::update(const MVPMatrices &data,
                                 const uint32_t frame_index)
{
    void *destination = nullptr;
    ::vkMapMemory(
        _instance.logical_device(),
        _memory_handles[frame_index],
        0u,
        sizeof(MVPMatrices),
        0u,
        &destination
    );

    memcpy(destination, &data, sizeof(MVPMatrices));

    ::vkUnmapMemory(_instance.logical_device(), _memory_handles[frame_index]);
}

// =============================================================================
void UniformBufferObject::init_descriptor_set() {
    CONSOLE_INFO("");

    ::VkDescriptorSetLayoutBinding ubo_layout_binding {
        .binding            = 0u,
        .descriptorType     = ::VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .descriptorCount    = 1u,
        .stageFlags         = ::VK_SHADER_STAGE_VERTEX_BIT,
        .pImmutableSamplers = nullptr
    };

    ::VkDescriptorSetLayoutCreateInfo desc_layout_info {
        .sType = ::VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0u,
        .bindingCount = 1u,
        .pBindings = &ubo_layout_binding,
    };

    auto result = ::vkCreateDescriptorSetLayout(
        _instance.logical_device(),
        &desc_layout_info,
        nullptr,
        &_desc_set_layout
    );

    if(result != ::VK_SUCCESS) {
        CONSOLE_ERROR("Could not create descriptor set layout");
    }
}

// =============================================================================
void UniformBufferObject::init_buffers() {
    CONSOLE_INFO("");

    for(size_t frame = 0; (frame < _buffer_handles.size() &&
                           frame < _memory_handles.size()); ++frame)
    {
        BufferTools::create_buffer(
            _buffer_handles[frame],
            sizeof(MVPMatrices),
            ::VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            _memory_handles[frame],
            ::VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
            ::VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            _instance
        );
    }
}

// =============================================================================
UniformBufferObject::UniformBufferObject(const size_t frames_in_flight,
                                         const Instance &instance) :
    _instance { instance }
{
    CONSOLE_INFO("");

    _buffer_handles.resize(frames_in_flight);
    _memory_handles.resize(frames_in_flight);                                                                      
 }

UniformBufferObject::~UniformBufferObject() {
    CONSOLE_INFO("");

    for(auto &buffer : _buffer_handles) {
        ::vkDestroyBuffer(_instance.logical_device(), buffer, nullptr);
    }

    for(auto &memory : _memory_handles) {
        ::vkFreeMemory(_instance.logical_device(), memory, nullptr);
    }

    ::vkDestroyDescriptorSetLayout(_instance.logical_device(),
                                   _desc_set_layout, nullptr);
}