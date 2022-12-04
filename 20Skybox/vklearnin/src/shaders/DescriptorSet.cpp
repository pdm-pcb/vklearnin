#include "vklearnin/vklearnin.hpp"
#include "vklearnin/shaders/DescriptorSet.hpp"

#include "vklearnin/shaders/DescriptorPool.hpp"
#include "vklearnin/shaders/DescriptorSetLayout.hpp"
#include "vklearnin/rendering/devices/LogicalDevice.hpp"

namespace vkl {

//==============================================================================
void DescriptorSet::update_ubo(const uint32_t buffer_index, const void *data) {
    auto &buffer = _ubos[buffer_index];

    void *mapped = VKAllocator::map_buffer(buffer.allocation);
        memcpy(mapped, data, buffer.allocation->size);
    VKAllocator::unmap_buffer(buffer.allocation);
}

//==============================================================================
void DescriptorSet::add_ubo(const size_t size,
                            const vk::ShaderStageFlags stages)
{
    _ubos.push_back(BufferTools::create_buffer(
        size,
        vk::BufferUsageFlagBits::eUniformBuffer,
        vk::SharingMode::eExclusive,
        (vk::MemoryPropertyFlagBits::eHostVisible |
         vk::MemoryPropertyFlagBits::eHostCoherent),
        std::format("ubo {}", _ubos.size()).c_str()
    ));

    _layout.add_binding({
        .binding            = static_cast<uint32_t>(_layout.bindings().size()),
        .descriptorType     = vk::DescriptorType::eUniformBuffer,
        .descriptorCount    = 1u,
        .stageFlags         = stages,
        .pImmutableSamplers = nullptr
    });
}

//==============================================================================
void DescriptorSet::add_texture2D(std::string_view filepath) {
    _textures.push_back(ImageTools::load_texture_from_file(filepath));

    _layout.add_binding({
        .binding            = static_cast<uint32_t>(_layout.bindings().size()),
        .descriptorType     = vk::DescriptorType::eCombinedImageSampler,
        .descriptorCount    = 1u,
        .stageFlags         = vk::ShaderStageFlagBits::eFragment,
        .pImmutableSamplers = nullptr
    });
}

//==============================================================================
void DescriptorSet::create(const DescriptorPool &descriptor_pool) {
    _layout.create();

    vk::DescriptorSetAllocateInfo alloc_info {
        .descriptorPool = descriptor_pool.native(),
        .descriptorSetCount = 1u,
        .pSetLayouts = &_layout.native()
    };

    auto result = LogicalDevice::native().allocateDescriptorSets(
        &alloc_info,
        &_descriptor_set
    );
    if(result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL("Could not allocate descriptor sets");
    }

    uint32_t binding_point = 0u;
    std::vector<vk::WriteDescriptorSet>   set_writes;
    std::vector<vk::DescriptorBufferInfo> buffer_info;
    std::vector<vk::DescriptorImageInfo>  image_info;

    if(_ubos.size() > 0) {
        CONSOLE_TRACE("Binding {} for UBOs", binding_point);
        buffer_info.reserve(_ubos.size());
        for(const auto &buffer : _ubos) {
            buffer_info.push_back({
                .buffer = buffer.buffer,
                .offset = 0u,
                .range = VK_WHOLE_SIZE
            });
        }

        set_writes.push_back({
            .dstSet = _descriptor_set,
            .dstBinding = binding_point,
            .dstArrayElement = 0u,
            .descriptorCount = static_cast<uint32_t>(buffer_info.size()),
            .descriptorType = vk::DescriptorType::eUniformBuffer,
            .pImageInfo = nullptr,
            .pBufferInfo = buffer_info.data(),
            .pTexelBufferView = nullptr
        });

        binding_point += static_cast<uint32_t>(image_info.size());
    }

    if(_textures.size() > 0) {
        CONSOLE_TRACE("Binding {} for images", binding_point);
        image_info.reserve(_textures.size());
        for(const auto &texture : _textures) {
            image_info.push_back({
                .sampler     = texture.sampler,
                .imageView   = texture.view,
                .imageLayout = texture.layout
            });
        }

        set_writes.push_back({
            .dstSet = _descriptor_set,
            .dstBinding = binding_point,
            .dstArrayElement = 0u,
            .descriptorCount = static_cast<uint32_t>(image_info.size()),
            .descriptorType = vk::DescriptorType::eCombinedImageSampler,
            .pImageInfo = image_info.data(),
            .pBufferInfo = nullptr,
            .pTexelBufferView = nullptr
        });

        binding_point += static_cast<uint32_t>(image_info.size());
    }

    LogicalDevice::native().updateDescriptorSets(set_writes, nullptr);
}

//==============================================================================
void DescriptorSet::destroy() {
    _layout.destroy();

    for(auto &buffer : _ubos) {
        BufferTools::destroy_buffer(buffer);
    }

    for(auto &texture : _textures) {
        ImageTools::destroy_image(texture);
    }
}

//==============================================================================
DescriptorSet::DescriptorSet(DescriptorSet &&other) :
    _ubos           { std::move(other._ubos)           },
    _textures       { std::move(other._textures)       },
    _layout         { std::move(other._layout)         },
    _descriptor_set { std::move(other._descriptor_set) }
{ }

} // namespace vkl