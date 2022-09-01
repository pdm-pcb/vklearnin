#include "vklearnin/common.hpp"
#include "vklearnin/DescriptorSet.hpp"

#include "vklearnin/Shaders/Buffers/UniformBufferObject.hpp"
#include "vklearnin/Textures/Texture2D.hpp"

// =============================================================================
void DescriptorSet::init_layout() {
    CONSOLE_INFO("");
    vk::DescriptorSetLayoutBinding bindings[] {
        // MVP matrices UBO
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
            .descriptorCount = MAX_IMAGES
        },
        {
            .type = vk::DescriptorType::eCombinedImageSampler,
            .descriptorCount = MAX_IMAGES
        },
    };

    vk::DescriptorPoolCreateInfo pool_info {
        .maxSets = MAX_IMAGES,
        .poolSizeCount = static_cast<uint32_t>(std::size(pool_size)),
        .pPoolSizes = pool_size
    };

    _pool = _device.createDescriptorPool(pool_info);
}

// =============================================================================
void DescriptorSet::init_sets(UniformBufferObject &ubo, Texture2D &texture) {
    CONSOLE_INFO("");

    std::vector<vk::DescriptorSetLayout>
        set_layouts(MAX_IMAGES, _layout);

    vk::DescriptorSetAllocateInfo alloc_info {
        .descriptorPool = _pool,
        .descriptorSetCount = MAX_IMAGES,
        .pSetLayouts = set_layouts.data()
    };

    _sets = _device.allocateDescriptorSets(alloc_info);

    for(uint32_t frame = 0; frame < MAX_IMAGES; ++frame) {
        vk::DescriptorBufferInfo buffer_info {
            .buffer = ubo.buffer_handles()[frame],
            .offset = 0u,
            .range  = static_cast<uint32_t>(sizeof(MVPMatrices))
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

    _sets.resize(MAX_IMAGES);
}

DescriptorSet::~DescriptorSet() {
    ::vkDestroyDescriptorSetLayout(_device, _layout, nullptr);
    ::vkDestroyDescriptorPool(_device, _pool, nullptr);
}