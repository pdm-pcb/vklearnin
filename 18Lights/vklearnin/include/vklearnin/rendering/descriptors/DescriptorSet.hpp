#ifndef VKLEARNIN_RENDERING_DESCRIPTORS_DESCRIPTORSET_HPP
#define VKLEARNIN_RENDERING_DESCRIPTORS_DESCRIPTORSET_HPP

#include "vklearnin/system/pch.hpp"
#include "vklearnin/resources/buffers/BufferObject.hpp"
#include "vklearnin/resources/images/ImageObject.hpp"

namespace vkl {

class DescriptorPool;
class DescriptorSetLayout;

class DescriptorSet {
public:
    void create(DescriptorPool const &descriptor_pool,
                DescriptorSetLayout const &set_layout);

    DescriptorSet & add_buffer(BufferObject const &buffer);
    DescriptorSet & add_image(ImageObject const &image);
    void write_set();

    inline auto const & native() const { return _set; }

    DescriptorSet();
    ~DescriptorSet() = default;

    DescriptorSet(DescriptorSet &&other) noexcept;
    DescriptorSet(const DescriptorSet &) = delete;

    DescriptorSet& operator=(DescriptorSet &&) = delete;
    DescriptorSet& operator=(const DescriptorSet &) = delete;

private:
    std::vector<BufferObject> _buffers;
    std::vector<ImageObject>  _images;
    vk::DescriptorSet _set;
};

} // namespace vkl

#endif // VKLEARNIN_RENDERING_DESCRIPTORS_DESCRIPTORSET_HPP