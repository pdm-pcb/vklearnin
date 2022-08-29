#include "vklearnin/common.hpp"
#include "vklearnin/DescriptorSet.hpp"

#include "vklearnin/Shaders/Buffers/UniformBufferObject.hpp"
#include "vklearnin/Textures/Texture2D.hpp"

// =============================================================================
void DescriptorSet::init_layout() {
    CONSOLE_INFO("");
    ::VkDescriptorSetLayoutBinding bindings[] {
        // MVP matrices UBO
        {
            .binding            = 0u,
            .descriptorType     = ::VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .descriptorCount    = 1u,
            .stageFlags         = ::VK_SHADER_STAGE_VERTEX_BIT,
            .pImmutableSamplers = nullptr
        },
        // Sampler2D
        {
            .binding            = 1u,
            .descriptorType     = ::VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            .descriptorCount    = 1u,
            .stageFlags         = ::VK_SHADER_STAGE_FRAGMENT_BIT,
            .pImmutableSamplers = nullptr
        }
    };

    // now build the above descriptor set
    ::VkDescriptorSetLayoutCreateInfo desc_layout_info {
        .sType = ::VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0u,
        .bindingCount = static_cast<uint32_t>(std::size(bindings)),
        .pBindings = bindings,
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

    ::VkDescriptorPoolSize pool_size[] {
        {
            .type = ::VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .descriptorCount = _max_images
        },
        {
            .type = ::VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            .descriptorCount = _max_images
        },
    };

    ::VkDescriptorPoolCreateInfo pool_info {
        .sType = ::VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0u,
        .maxSets = _max_images,
        .poolSizeCount = static_cast<uint32_t>(std::size(pool_size)),
        .pPoolSizes = pool_size
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
void DescriptorSet::init_sets(UniformBufferObject &ubo, Texture2D &texture) {
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

        ::VkDescriptorImageInfo image_info {
            .sampler = texture.sampler(),
            .imageView = texture.view(),
            .imageLayout = texture.layout(),
        };

        ::VkWriteDescriptorSet write_info[] {
            {
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
            },
            {
                .sType = ::VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                .pNext = nullptr,
                .dstSet = _sets[frame],
                .dstBinding = 1u,
                .dstArrayElement = 0u,
                .descriptorCount = 1u,
                .descriptorType = ::VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                .pImageInfo = &image_info,
                .pBufferInfo = nullptr,
                .pTexelBufferView = nullptr
            },
        };

        ::vkUpdateDescriptorSets(
            _device,
            static_cast<uint32_t>(std::size(write_info)), write_info,
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