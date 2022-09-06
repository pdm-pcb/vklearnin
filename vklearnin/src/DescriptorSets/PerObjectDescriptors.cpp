#include "vklearnin/common.hpp"
#include "vklearnin/DescriptorSets/PerObjectDescriptors.hpp"

#include "vklearnin/Shaders/Buffers/UniformBufferObject.hpp"

// =============================================================================
void PerObjectDescriptors::init_layout() {
    CONSOLE_INFO("");

    std::vector<vk::DescriptorSetLayoutBinding> bindings {{
        .binding            = MODEL_MATRIX_BINDING,
        .descriptorType     = vk::DescriptorType::eUniformBufferDynamic,
        .descriptorCount    = 1u,
        .stageFlags         = vk::ShaderStageFlagBits::eVertex,
        .pImmutableSamplers = nullptr
    }};

    vk::DescriptorSetLayoutCreateInfo desc_layout_info {
        .bindingCount = static_cast<uint32_t>(bindings.size()),
        .pBindings = bindings.data(),
    };

    _layout = _device.createDescriptorSetLayout(desc_layout_info);
}

// =============================================================================
void PerObjectDescriptors::init_pool() {
    CONSOLE_INFO("");

    vk::DescriptorPoolSize pool_sizes[] {{
        .type = vk::DescriptorType::eUniformBufferDynamic,
        .descriptorCount = MAX_DESC_COUNT,
    }};

    vk::DescriptorPoolCreateInfo pool_info {
        .maxSets = MAX_DESC_SETS,
        .poolSizeCount = static_cast<uint32_t>(std::size(pool_sizes)),
        .pPoolSizes = pool_sizes
    };

    _pool = _device.createDescriptorPool(pool_info);
}

// =============================================================================
void PerObjectDescriptors::init_sets(UniformBufferObject &ubo)
{
    CONSOLE_INFO("");

    for(size_t object_idx = 0; object_idx < _object_count; ++object_idx) {
        std::vector<vk::DescriptorSetLayout> set_layouts;
        set_layouts.resize(FRAME_OVERLAP);
        std::fill(set_layouts.begin(), set_layouts.end(), _layout);

        vk::DescriptorSetAllocateInfo alloc_info {
            .descriptorPool = _pool,
            .descriptorSetCount = FRAME_OVERLAP,
            .pSetLayouts = set_layouts.data()
        };

        _sets[object_idx] = _device.allocateDescriptorSets(alloc_info);

        for(uint32_t frame = 0; frame < FRAME_OVERLAP; ++frame) {
            vk::DescriptorBufferInfo buffer_info {
                .buffer = ubo.buffer_handles()[frame],
                .offset = 0u,
                .range  = static_cast<uint32_t>(sizeof(glm::mat4))
            };

            vk::WriteDescriptorSet write_info[] {{
                .dstSet = _sets[object_idx][frame],
                .dstBinding = MODEL_MATRIX_BINDING,
                .dstArrayElement = 0u,
                .descriptorCount = 1u,
                .descriptorType = vk::DescriptorType::eUniformBufferDynamic,
                .pImageInfo = nullptr,
                .pBufferInfo = &buffer_info,
                .pTexelBufferView = nullptr
            }};

            _device.updateDescriptorSets(write_info, { });
        }
    }
}

// =============================================================================
PerObjectDescriptors::PerObjectDescriptors(const uint32_t material_count,
                                           const vk::Device &device) :
    _object_count { material_count },
    _device { device }
{
    CONSOLE_INFO("");

    _sets.resize(_object_count);
    for(auto & set : _sets) {
        set.resize(FRAME_OVERLAP);
    }
}

PerObjectDescriptors::~PerObjectDescriptors() {
    _device.destroy(_layout);
    _device.destroy(_pool);
}