#include "vklearnin/vklearnin.hpp"
#include "vklearnin/rendering/descriptors/DescriptorSet.hpp"

#include "vklearnin/rendering/descriptors/DescriptorPool.hpp"
#include "vklearnin/rendering/descriptors/DescriptorSetLayout.hpp"
#include "vklearnin/system/devices/LogicalDevice.hpp"

namespace vkl {

// =============================================================================
void DescriptorSet::add_ubo(BufferObjects const& buffers,
                            vk::ShaderStageFlags const stage_flags)
{
    if(buffers.size() != RenderConfig::image_count) {
        CONSOLE_CRITICAL(
            "UBO vector size of {} does not match image count of {}",
            buffers.size(),
            RenderConfig::image_count
        );
    }
    _ubos.push_back(buffers);

    _layout.add_binding({
        .binding            = 0u,
        .descriptorType     = vk::DescriptorType::eUniformBuffer,
        .descriptorCount    = 1u,
        .stageFlags         = stage_flags,
        .pImmutableSamplers = nullptr
    });
}

// =============================================================================
void DescriptorSet::add_texture2D(const ImageObject &texture) {
    _textures.push_back(texture);

    _layout.add_binding({
        .binding            = 1u,
        .descriptorType     = vk::DescriptorType::eCombinedImageSampler,
        .descriptorCount    = 1u,
        .stageFlags         = vk::ShaderStageFlagBits::eFragment,
        .pImmutableSamplers = nullptr
    });
}

// =============================================================================
void DescriptorSet::create(const DescriptorPool &pool) {
    _layout.create();

    std::vector<vk::DescriptorSetLayout> layouts;
    layouts.resize(RenderConfig::image_count);
    std::fill(layouts.begin(), layouts.end(), _layout.native());

    const vk::DescriptorSetAllocateInfo alloc_info {
        .descriptorPool = pool.native(),
        .descriptorSetCount = static_cast<uint32_t>(layouts.size()),
        .pSetLayouts = layouts.data()
    };

    auto result = LogicalDevice::native().allocateDescriptorSets(
        &alloc_info,
        _sets.data()
    );
    if(result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL("Could not allocate descriptor set");
        return;
    }

    for(uint32_t frame = 0; frame < RenderConfig::image_count; ++frame) {
        std::vector<vk::DescriptorBufferInfo> buffer_info;
        buffer_info.reserve(_ubos.size());

        for(auto const& ubo : _ubos[frame]) {
            buffer_info.push_back({
                .buffer = ubo.handle,
                .offset = 0u,
                .range = VK_WHOLE_SIZE
            });
        }

        std::vector<vk::DescriptorImageInfo> image_info;
        image_info.reserve(_textures.size());

        for(auto const& texture : _textures) {
            image_info.push_back({
                .sampler     = texture.sampler,
                .imageView   = texture.view,
                .imageLayout = texture.layout
            });
        }

        std::vector<vk::WriteDescriptorSet> set_writes;

        if(!buffer_info.empty()) {
            set_writes.push_back({
                .dstSet           = _sets[frame],
                .dstBinding       = 0u,
                .dstArrayElement  = 0u,
                .descriptorCount  = static_cast<uint32_t>(buffer_info.size()),
                .descriptorType   = vk::DescriptorType::eUniformBuffer,
                .pImageInfo       = nullptr,
                .pBufferInfo      = buffer_info.data(),
                .pTexelBufferView = nullptr
            });
        }

        if(!image_info.empty()) {
            set_writes.push_back({
                .dstSet           = _sets[frame],
                .dstBinding       = 1u,
                .dstArrayElement  = 0u,
                .descriptorCount  = static_cast<uint32_t>(image_info.size()),
                .descriptorType   = vk::DescriptorType::eCombinedImageSampler,
                .pImageInfo       = image_info.data(),
                .pBufferInfo      = nullptr,
                .pTexelBufferView = nullptr
            });
        }

        LogicalDevice::native().updateDescriptorSets(set_writes, nullptr);
    }
}

// =============================================================================
void DescriptorSet::destroy() {
    for(size_t image = 0; image < RenderConfig::image_count; ++image) {
        for(auto &buffer : _ubos[image]) {
            BufferTools::destroy(buffer);
        }
    }

    _layout.destroy();
}

// =============================================================================
DescriptorSet::DescriptorSet() :
    _ubos     { },
    _textures { }
{
    _sets.resize(RenderConfig::image_count);
}

DescriptorSet::DescriptorSet(DescriptorSet &&other) noexcept :
    _ubos     { std::move(other._ubos) },
    _textures { std::move(other._textures) },
    _sets     { std::move(other._sets) }
{
    other._ubos.clear();
    other._textures.clear();
    other._sets.clear();
}

} // namespace vkl