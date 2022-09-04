#include "vklearnin/common.hpp"
#include "vklearnin/DescriptorSets/PerMaterialDescriptors.hpp"

#include "vklearnin/Textures/Texture2D.hpp"

// =============================================================================
void PerMaterialDescriptors::init_layout() {
    CONSOLE_INFO("");
    std::vector<vk::DescriptorSetLayoutBinding> bindings {{
        .binding            = SAMPLER_BINDING,
        .descriptorType     = vk::DescriptorType::eCombinedImageSampler,
        .descriptorCount    = 1u,
        .stageFlags         = vk::ShaderStageFlagBits::eFragment,
        .pImmutableSamplers = nullptr
    }};

    // now build the above descriptor set
    vk::DescriptorSetLayoutCreateInfo desc_layout_info {
        .bindingCount = static_cast<uint32_t>(bindings.size()),
        .pBindings = bindings.data(),
    };

    _layout = _device.createDescriptorSetLayout(desc_layout_info);
}

// =============================================================================
void PerMaterialDescriptors::init_pool() {
    CONSOLE_INFO("");

    vk::DescriptorPoolSize pool_size[] {{
        .type = vk::DescriptorType::eCombinedImageSampler,
        .descriptorCount = FRAME_OVERLAP * _material_count,
    }};

    vk::DescriptorPoolCreateInfo pool_info {
        .maxSets = FRAME_OVERLAP * _material_count,
        .poolSizeCount = static_cast<uint32_t>(std::size(pool_size)),
        .pPoolSizes = pool_size
    };

    _pool = _device.createDescriptorPool(pool_info);
}

// =============================================================================
void PerMaterialDescriptors::init_sets(
    const std::vector<Texture2D const *> &textures)
{
    CONSOLE_INFO("");

    for(size_t tex_index = 0; tex_index < _material_count; ++tex_index) {
        std::vector<vk::DescriptorSetLayout> set_layouts;
        set_layouts.resize(FRAME_OVERLAP);
        std::fill(set_layouts.begin(), set_layouts.end(), _layout);

        vk::DescriptorSetAllocateInfo alloc_info {
            .descriptorPool = _pool,
            .descriptorSetCount = FRAME_OVERLAP,
            .pSetLayouts = set_layouts.data()
        };

        _sets[tex_index] = _device.allocateDescriptorSets(alloc_info);

        for(uint32_t frame = 0; frame < FRAME_OVERLAP; ++frame) {
            vk::DescriptorImageInfo image_info {
                .sampler = textures[tex_index]->sampler(),
                .imageView = textures[tex_index]->view(),
                .imageLayout = textures[tex_index]->layout(),
            };

            vk::WriteDescriptorSet write_info[] {{
                .dstSet = _sets[tex_index][frame],
                .dstBinding = SAMPLER_BINDING,
                .dstArrayElement = 0u,
                .descriptorCount = 1u,
                .descriptorType = vk::DescriptorType::eCombinedImageSampler,
                .pImageInfo = &image_info,
                .pBufferInfo = nullptr,
                .pTexelBufferView = nullptr
            }};

            _device.updateDescriptorSets(write_info, { });
        }
    }
}

// =============================================================================
PerMaterialDescriptors::PerMaterialDescriptors(const uint32_t material_count,
                                               const vk::Device &device) :
    _material_count { material_count },
    _device { device }
{
    CONSOLE_INFO("");

    _sets.resize(_material_count);
    for(auto & set : _sets) {
        set.resize(FRAME_OVERLAP);
    }
}

PerMaterialDescriptors::~PerMaterialDescriptors() {
    _device.destroy(_layout);
    _device.destroy(_pool);
}