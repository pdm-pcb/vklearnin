#include "common.hpp"
#include "DescriptorSet.hpp"

#include "UniformBufferObject.hpp"

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

void DescriptorSet::init_sets(UniformBufferObject &ubo)
{
    CONSOLE_INFO("");

    std::vector<::VkDescriptorSetLayout>
        set_layouts(_max_images, ubo.descriptor_set_layout());

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
    ::vkDestroyDescriptorPool(_device, _pool, nullptr);
}