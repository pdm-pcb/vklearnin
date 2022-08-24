#include "common.hpp"
#include "UniformBufferObject.hpp"

void UniformBufferObject::init_descriptor_set() {
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
        _device,
        &desc_layout_info,
        nullptr,
        &_desc_set_layout
    );

    if(result != ::VK_SUCCESS) {
        CONSOLE_CRITICAL("Could not create descriptor set layout");
    }
}

void UniformBufferObject::init_buffers() {
    for(size_t frame = 0; frame < _buffer_handles.size(); ++frame) {
        
    }
}

UniformBufferObject::UniformBufferObject(const ::VkDevice &device,
                                         const size_t frames_in_flight) :
    _device { device }
{
    _buffer_handles.resize(frames_in_flight);
    _memory_handles.resize(frames_in_flight);
}

UniformBufferObject::~UniformBufferObject() {
    for(auto &buffer : _buffer_handles) {
        ::vkDestroyBuffer(_device, buffer, nullptr);
    }

    for(auto &memory : _memory_handles) {
        ::vkFreeMemory(_device, memory, nullptr);
    }

    ::vkDestroyDescriptorSetLayout(_device, _desc_set_layout, nullptr);
}