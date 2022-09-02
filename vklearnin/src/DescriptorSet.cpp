#include "vklearnin/common.hpp"
#include "vklearnin/DescriptorSet.hpp"

#include "vklearnin/Shaders/Buffers/UniformBufferObject.hpp"
#include "vklearnin/Textures/Texture2D.hpp"

// =============================================================================
void DescriptorSet::init_layout() {
    CONSOLE_INFO("");
    vk::DescriptorSetLayoutBinding bindings[] {
        // VP matrices
        {
            .binding            = 0u,
            .descriptorType     = vk::DescriptorType::eUniformBuffer,
            .descriptorCount    = 1u,
            .stageFlags         = vk::ShaderStageFlagBits::eVertex,
            .pImmutableSamplers = nullptr
        },
        // Sampler2D
        {
            .binding            = 1u,
            .descriptorType     = vk::DescriptorType::eCombinedImageSampler,
            .descriptorCount    = 1u,
            .stageFlags         = vk::ShaderStageFlagBits::eFragment,
            .pImmutableSamplers = nullptr
        }
    };

    // now build the above descriptor set
    vk::DescriptorSetLayoutCreateInfo desc_layout_info {
        .bindingCount = static_cast<uint32_t>(std::size(bindings)),
        .pBindings = bindings,
    };

    _layout = _device.createDescriptorSetLayout(desc_layout_info);
}

// =============================================================================
void DescriptorSet::init_pool() {
    CONSOLE_INFO("");

    vk::DescriptorPoolSize pool_size[] {
        {
            .type = vk::DescriptorType::eUniformBuffer,
            .descriptorCount = FRAME_OVERLAP
        },
        {
            .type = vk::DescriptorType::eCombinedImageSampler,
            .descriptorCount = FRAME_OVERLAP
        },
    };

    vk::DescriptorPoolCreateInfo pool_info {
        .maxSets = FRAME_OVERLAP,
        .poolSizeCount = static_cast<uint32_t>(std::size(pool_size)),
        .pPoolSizes = pool_size
    };

    _pool = _device.createDescriptorPool(pool_info);
}

// =============================================================================
void DescriptorSet::init_sets(UniformBufferObject &ubo, Texture2D &texture) {
    CONSOLE_INFO("");

    std::vector<vk::DescriptorSetLayout>
        set_layouts(FRAME_OVERLAP, _layout);

    vk::DescriptorSetAllocateInfo alloc_info {
        .descriptorPool = _pool,
        .descriptorSetCount = FRAME_OVERLAP,
        .pSetLayouts = set_layouts.data()
    };

    _sets = _device.allocateDescriptorSets(alloc_info);

    for(uint32_t frame = 0; frame < FRAME_OVERLAP; ++frame) {
        vk::DescriptorBufferInfo buffer_info {
            .buffer = ubo.buffer_handles()[frame],
            .offset = 0u,
            .range  = static_cast<uint32_t>(sizeof(VPMatrices))
        };

        vk::DescriptorImageInfo image_info {
            .sampler = texture.sampler(),
            .imageView = texture.view(),
            .imageLayout = texture.layout(),
        };

        vk::WriteDescriptorSet write_info[] {
            {
                .dstSet = _sets[frame],
                .dstBinding = 0u,
                .dstArrayElement = 0u,
                .descriptorCount = 1u,
                .descriptorType = vk::DescriptorType::eUniformBuffer,
                .pImageInfo = nullptr,
                .pBufferInfo = &buffer_info,
                .pTexelBufferView = nullptr
            },
            {
                .dstSet = _sets[frame],
                .dstBinding = 1u,
                .dstArrayElement = 0u,
                .descriptorCount = 1u,
                .descriptorType = vk::DescriptorType::eCombinedImageSampler,
                .pImageInfo = &image_info,
                .pBufferInfo = nullptr,
                .pTexelBufferView = nullptr
            },
        };

        _device.updateDescriptorSets(write_info, { });
    }
}

// =============================================================================
DescriptorSet::DescriptorSet(const vk::Device &device) :
    _device { device  }
{
    CONSOLE_INFO("");

    _sets.resize(FRAME_OVERLAP);
}

DescriptorSet::~DescriptorSet() {
    _device.destroy(_layout);
    _device.destroy(_pool);
}