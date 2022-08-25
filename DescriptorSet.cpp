#include "common.hpp"
#include "DescriptorSet.hpp"

#include "UniformBufferObject.hpp"

// =============================================================================
// ok, so very explicitly right now, this descriptor set  class can only be used
// in conjunction with uniform buffers. Not only that, but exclusively during
// the vertex shader stage. This will no doubt need to change, but let's see
// how far it takes us.
void DescriptorSet::init_layout() {
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
        _device,
        &desc_layout_info,
        nullptr,
        &_layout
    );

    if(result != ::VK_SUCCESS) {
        CONSOLE_ERROR("Could not create descriptor set layout");
    }
}

// =============================================================================
void DescriptorSet::init_pool() {
    CONSOLE_INFO("");

    ::VkDescriptorPoolSize pool_size {
        .type = ::VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .descriptorCount = _max_images
    };

    ::VkDescriptorPoolCreateInfo pool_info {
        .sType = ::VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0u,
        .maxSets = _max_images,
        .poolSizeCount = 1u,
        .pPoolSizes = &pool_size
    };

    auto result = ::vkCreateDescriptorPool(
        _device,
        &pool_info,
        nullptr,
        &_pool
    );

    if(result != ::VK_SUCCESS) {
        CONSOLE_ERROR("Failed to create descriptor pool");
    }
}

// =============================================================================
void DescriptorSet::init_sets(UniformBufferObject &ubo)
{
    CONSOLE_INFO("");

    std::vector<::VkDescriptorSetLayout>
        set_layouts(_max_images, _layout);

    ::VkDescriptorSetAllocateInfo alloc_info {
        .sType = ::VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .pNext = nullptr,
        .descriptorPool = _pool,
        .descriptorSetCount = _max_images,
        .pSetLayouts = set_layouts.data()
    };

    auto result = ::vkAllocateDescriptorSets(
        _device,
        &alloc_info,
        _sets.data()
    );

    if(result != ::VK_SUCCESS) {
        CONSOLE_ERROR("Unable to allocate descriptor sets");
    }

    for(uint32_t frame = 0; frame < _max_images; ++frame) {
        ::VkDescriptorBufferInfo buffer_info {
            .buffer = ubo.buffer_handles()[frame],
            .offset = 0u,
            .range  = static_cast<uint32_t>(sizeof(MVPMatrices))
        };

        ::VkWriteDescriptorSet write_info {
            .sType = ::VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .pNext = nullptr,
            .dstSet = _sets[frame],
            .dstBinding = 0u,
            .dstArrayElement = 0u,
            .descriptorCount = 1u,
            .descriptorType = ::VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .pImageInfo = nullptr,
            .pBufferInfo = &buffer_info,
            .pTexelBufferView = nullptr
        };

        ::vkUpdateDescriptorSets(
            _device,
            1u, &write_info,
            0u, nullptr
        );
    }
}

// =============================================================================
DescriptorSet::DescriptorSet(const uint32_t frames_in_flight,
                             const ::VkDevice &device) :
    _pool       { nullptr },
    _max_images { frames_in_flight },
    _device     { device  }
{
    CONSOLE_INFO("");

    _sets.resize(_max_images);
}

DescriptorSet::~DescriptorSet() {
    ::vkDestroyDescriptorSetLayout(_device, _layout, nullptr);
    ::vkDestroyDescriptorPool(_device, _pool, nullptr);
}