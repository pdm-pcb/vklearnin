#ifndef VKLEARNIN_RENDERING_DESCRIPTORS_DESCRIPTORSET_HPP
#define VKLEARNIN_RENDERING_DESCRIPTORS_DESCRIPTORSET_HPP

#include "vklearnin/system/pch.hpp"
#include "vklearnin/resources/buffers/BufferObject.hpp"
#include "vklearnin/resources/images/ImageObject.hpp"

namespace vkl {

class DescriptorPool;
class DescriptorSetLayout;
class Texture2D;

class DescriptorSet {
public:
    void add_ubo(const BufferObject &ubo);

    void create(const DescriptorPool &descriptor_pool,
                const DescriptorSetLayout &set_layout);
    void destroy();

    inline const auto& native() const { return _set; }

    DescriptorSet();
    ~DescriptorSet() = default;

    DescriptorSet(DescriptorSet &&other) noexcept;
    DescriptorSet(const DescriptorSet &) = delete;

    DescriptorSet& operator=(DescriptorSet &&) = delete;
    DescriptorSet& operator=(const DescriptorSet &) = delete;

private:
    std::vector<BufferObject> _ubos;
    std::vector<ImageObject>  _textures;
    vk::DescriptorSet _set;
};

} // namespace vkl

#endif // VKLEARNIN_RENDERING_DESCRIPTORS_DESCRIPTORSET_HPP