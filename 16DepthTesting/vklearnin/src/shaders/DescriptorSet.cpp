#include "vklearnin/vklearnin.hpp"
#include "vklearnin/shaders/DescriptorSet.hpp"

#include "vklearnin/shaders/DescriptorPool.hpp"
#include "vklearnin/shaders/DescriptorSetLayout.hpp"
#include "vklearnin/rendering/devices/LogicalDevice.hpp"

namespace vkl {

void DescriptorSet::add_texture2D(const char *filepath) {
    _textures.push_back(ImageTools::load_from_file(filepath));
}

void DescriptorSet::create(const DescriptorPool &descriptor_pool,
                           const DescriptorSetLayout &layout,
                           const size_t instance_buffer_size)
{
    vk::DescriptorSetAllocateInfo alloc_info {
        .descriptorPool = descriptor_pool.native(),
        .descriptorSetCount = 1u,
        .pSetLayouts = &layout.native()
    };
    
    auto result = LogicalDevice::native().allocateDescriptorSets(
        &alloc_info,
        &_descriptor_set
    );
    if(result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL("Could not allocate descriptor sets");
    }

    _instance_buffer = BufferTools::create_buffer(
        instance_buffer_size,
        vk::BufferUsageFlagBits::eUniformBuffer,
        vk::SharingMode::eExclusive,
        (vk::MemoryPropertyFlagBits::eHostVisible |
        vk::MemoryPropertyFlagBits::eHostCoherent),
        "instance desc"
    );

    vk::DescriptorBufferInfo buffer_info {
        .buffer = _instance_buffer.buffer,
        .offset = 0u,
        .range = instance_buffer_size
    };

    std::vector<vk::DescriptorImageInfo> image_info;
    image_info.reserve(_textures.size());
    
    for(const auto &texture : _textures) {
        image_info.emplace_back(vk::DescriptorImageInfo {
            .sampler     = texture.sampler,
            .imageView   = texture.view,
            .imageLayout = texture.layout
        });
    }

    vk::WriteDescriptorSet set_writes[] {
        {
            .dstSet = _descriptor_set,
            .dstBinding = 0u,
            .dstArrayElement = 0u,
            .descriptorCount = 1u,
            .descriptorType = vk::DescriptorType::eUniformBuffer,
            .pImageInfo = nullptr,
            .pBufferInfo = &buffer_info,
            .pTexelBufferView = nullptr
        },
        {
            .dstSet = _descriptor_set,
            .dstBinding = 1u,
            .dstArrayElement = 0u,
            .descriptorCount = static_cast<uint32_t>(image_info.size()),
            .descriptorType = vk::DescriptorType::eCombinedImageSampler,
            .pImageInfo = image_info.data(),
            .pBufferInfo = nullptr,
            .pTexelBufferView = nullptr
        },
    };

    LogicalDevice::native().updateDescriptorSets(set_writes, nullptr);
}

void DescriptorSet::destroy() {
    BufferTools::destroy_buffer(_instance_buffer);

    for(auto &texture : _textures) {
        ImageTools::destroy_image(texture);
    }
}

DescriptorSet::DescriptorSet(DescriptorSet &&other) :
    _descriptor_set  { std::move(other._descriptor_set)  },
    _instance_buffer { std::move(other._instance_buffer) }
{ }

} // namespace vkl