#include "vklearnin/vklearnin.hpp"
#include "vklearnin/shaders/DescriptorSet.hpp"

#include "vklearnin/shaders/DescriptorPool.hpp"
#include "vklearnin/shaders/DescriptorSetLayout.hpp"
#include "vklearnin/rendering/devices/LogicalDevice.hpp"

namespace vkl {

//==============================================================================
void DescriptorSet::add_ubo(const size_t size) {
    CONSOLE_TRACE("Add UBO");
    _uniform_buffers.push_back(BufferTools::create_buffer(
        size,
        vk::BufferUsageFlagBits::eUniformBuffer,
        vk::SharingMode::eExclusive,
        (vk::MemoryPropertyFlagBits::eHostVisible |
        vk::MemoryPropertyFlagBits::eHostCoherent),
        std::format("ubo {}", _uniform_buffers.size()).c_str()
    ));
}

//==============================================================================
void DescriptorSet::add_texture2D(const char *filepath) {
    CONSOLE_TRACE("Add Texture2D");
    _textures.push_back(ImageTools::load_from_file(filepath));
}

//==============================================================================
void DescriptorSet::create(const DescriptorPool &descriptor_pool,
                           const DescriptorSetLayout &layout,
                           const bool unbounded)
{
    vk::DescriptorSetVariableDescriptorCountAllocateInfo variable_count { };
    std::vector<uint32_t> descriptor_count = { 2u };
    if(unbounded) {
        variable_count.descriptorSetCount = 1u;
        variable_count.pDescriptorCounts  = descriptor_count.data(); 
    }

    vk::DescriptorSetAllocateInfo alloc_info {
        // .pNext = &variable_count,
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

    std::vector<vk::DescriptorBufferInfo> buffer_info;
    buffer_info.reserve(_uniform_buffers.size());
    for(const auto &buffer : _uniform_buffers) {
        buffer_info.push_back({
            .buffer = buffer.buffer,
            .offset = 0u,
            .range = VK_WHOLE_SIZE
        });
    }

    std::vector<vk::DescriptorImageInfo> image_info;
    image_info.reserve(_textures.size());
    for(const auto &texture : _textures) {
        image_info.push_back({
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
            .descriptorCount = static_cast<uint32_t>(buffer_info.size()),
            .descriptorType = vk::DescriptorType::eUniformBuffer,
            .pImageInfo = nullptr,
            .pBufferInfo = buffer_info.data(),
            .pTexelBufferView = nullptr
        },
        {
            .dstSet = _descriptor_set,
            .dstBinding = static_cast<uint32_t>(buffer_info.size()),
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

//==============================================================================
void DescriptorSet::destroy() {
    for(auto &buffer : _uniform_buffers) {
        BufferTools::destroy_buffer(buffer);
    }

    for(auto &texture : _textures) {
        ImageTools::destroy_image(texture);
    }
}

//==============================================================================
DescriptorSet::DescriptorSet(DescriptorSet &&other) :
    _descriptor_set  { std::move(other._descriptor_set) },
    _uniform_buffers { std::move(other._uniform_buffers) },
    _textures        { std::move(other._textures) }
{ }

} // namespace vkl