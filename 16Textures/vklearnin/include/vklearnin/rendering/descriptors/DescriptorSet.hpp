#ifndef VKLEARNIN_RENDERING_DESCRIPTORS_DESCRIPTORSET_HPP
#define VKLEARNIN_RENDERING_DESCRIPTORS_DESCRIPTORSET_HPP

#include "vklearnin/system/pch.hpp"
#include "vklearnin/resources/buffers/BufferObject.hpp"
#include "vklearnin/resources/images/ImageObject.hpp"
#include "vklearnin/rendering/descriptors/DescriptorSetLayout.hpp"
#include "vklearnin/rendering/swapchain/Swapchain.hpp"

namespace vkl {

class DescriptorPool;
class DescriptorSetLayout;

class DescriptorSet {
public:
    using BufferObjects = std::vector<BufferObject>;

    void add_ubo(BufferObjects const& buffers,
                 vk::ShaderStageFlags const stage_flags);
    void add_texture2D(ImageObject const& texture);

    void create(DescriptorPool const& pool);
    void destroy();

    inline auto const& native() const { return _sets[Swapchain::image_index()]; }
    inline auto const& layout() const { return _layout; }

    DescriptorSet();
    ~DescriptorSet() = default;

    DescriptorSet(DescriptorSet &&other) noexcept;
    DescriptorSet(const DescriptorSet &) = delete;

    DescriptorSet& operator=(DescriptorSet &&) = delete;
    DescriptorSet& operator=(const DescriptorSet &) = delete;

private:
    std::vector<BufferObjects> _ubos;
    std::vector<ImageObject>   _textures;

    DescriptorSetLayout _layout;

    using DescriptorSets = std::vector<vk::DescriptorSet>;
    DescriptorSets _sets;
};

} // namespace vkl

#endif // VKLEARNIN_RENDERING_DESCRIPTORS_DESCRIPTORSET_HPP