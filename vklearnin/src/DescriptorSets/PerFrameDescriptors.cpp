#include "vklearnin/common.hpp"
#include "vklearnin/DescriptorSets/PerFrameDescriptors.hpp"

#include "vklearnin/Shaders/Buffers/UniformBufferObject.hpp"

static constexpr uint32_t CAM_UBO_BINDING = 0u;

// =============================================================================
void PerFrameDescriptors::init_layout() {
    CONSOLE_INFO("");

    vk::DescriptorSetLayoutBinding bindings[] {{
        .binding            = CAM_UBO_BINDING,
        .descriptorType     = vk::DescriptorType::eUniformBuffer,
        .descriptorCount    = 1u,
        .stageFlags         = vk::ShaderStageFlagBits::eVertex,
        .pImmutableSamplers = nullptr
    }};

    vk::DescriptorSetLayoutCreateInfo desc_layout_info {
        .bindingCount = static_cast<uint32_t>(std::size(bindings)),
        .pBindings = bindings,
    };

    _layout = _device.createDescriptorSetLayout(desc_layout_info);
}

// =============================================================================
void PerFrameDescriptors::init_pool() {
    CONSOLE_INFO("");

    vk::DescriptorPoolSize pool_size[] {{
        .type = vk::DescriptorType::eUniformBuffer,
        .descriptorCount = FRAME_OVERLAP
    }};

    vk::DescriptorPoolCreateInfo pool_info {
        .maxSets = FRAME_OVERLAP,
        .poolSizeCount = static_cast<uint32_t>(std::size(pool_size)),
        .pPoolSizes = pool_size
    };

    _pool = _device.createDescriptorPool(pool_info);
}

// =============================================================================
void PerFrameDescriptors::init_sets(UniformBufferObject &ubo) {
    CONSOLE_INFO("");

    std::vector<vk::DescriptorSetLayout> set_layouts(FRAME_OVERLAP, _layout);

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

        vk::WriteDescriptorSet write_info[] {{
            .dstSet = _sets[frame],
            .dstBinding = CAM_UBO_BINDING,
            .dstArrayElement = 0u,
            .descriptorCount = 1u,
            .descriptorType = vk::DescriptorType::eUniformBuffer,
            .pImageInfo = nullptr,
            .pBufferInfo = &buffer_info,
            .pTexelBufferView = nullptr
        }};

        _device.updateDescriptorSets(write_info, { });
    }
}

// =============================================================================
PerFrameDescriptors::PerFrameDescriptors(const vk::Device &device) :
    _device { device  }
{
    CONSOLE_INFO("");

    _sets.resize(FRAME_OVERLAP);
}

PerFrameDescriptors::~PerFrameDescriptors() {
    _device.destroy(_layout);
    _device.destroy(_pool);
}