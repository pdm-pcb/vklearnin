#ifndef VKLEARNIN_RENDERING_DESCRIPTORS_DESCRIPTORSET_HPP
#define VKLEARNIN_RENDERING_DESCRIPTORS_DESCRIPTORSET_HPP

#include "vklearnin/system/pch.hpp"
#include "vklearnin/resources/buffers/BufferObject.hpp"
#include "vklearnin/resources/images/ImageObject.hpp"
#include "vklearnin/rendering/descriptors/DescriptorSetLayout.hpp"

namespace vkl {

class DescriptorPool;

class DescriptorSet {
public:
    DescriptorSet & add_ubo(BufferObject const &buffer);
    DescriptorSet & add_combined_sampler(ImageObject const &image);

    DescriptorSet & allocate(DescriptorPool const &descriptor_pool,
                             DescriptorSetLayout const &set_layout);

    void write_set();

    inline auto const & native() const { return _set; }
    inline auto const & layout() const { return _layout; }

    DescriptorSet();
    ~DescriptorSet() = default;

    DescriptorSet(DescriptorSet &&other) noexcept;
    DescriptorSet(const DescriptorSet &) = delete;

    DescriptorSet& operator=(DescriptorSet &&) = delete;
    DescriptorSet& operator=(const DescriptorSet &) = delete;

private:
    std::vector<BufferObject> _ubos;
    std::vector<BufferObject> _ssbos;
    std::vector<ImageObject>  _combined_samplers;

    vk::DescriptorSetLayout   _layout;
    vk::DescriptorSet         _set;

    using BufferInfoList = std::vector<vk::DescriptorBufferInfo>;
    BufferInfoList _get_ubo_info();

    using ImageInfoList = std::vector<vk::DescriptorImageInfo>;
    ImageInfoList _get_combined_sampler_info();

    using BufferInfoList = std::vector<vk::DescriptorBufferInfo>;
    BufferInfoList _get_ssbo_info();
};

} // namespace vkl

#endif // VKLEARNIN_RENDERING_DESCRIPTORS_DESCRIPTORSET_HPP