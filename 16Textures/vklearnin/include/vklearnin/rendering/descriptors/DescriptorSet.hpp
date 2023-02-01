#ifndef VKLEARNIN_RENDERING_DESCRIPTORS_DESCRIPTORSET_HPP
#define VKLEARNIN_RENDERING_DESCRIPTORS_DESCRIPTORSET_HPP

#include "vklearnin/system/pch.hpp"
#include "vklearnin/resources/buffers/BufferTools.hpp"

namespace vkl {

class DescriptorPool;
class DescriptorSetLayout;

class DescriptorSet {
public:
    using UBOIter = std::vector<BufferObject>::iterator;
    UBOIter add_ubo(const size_t size);

    static void update_ubo(const UBOIter &buffer, const void *data);

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
    vk::DescriptorSet   _set;
};

} // namespace vkl

#endif // VKLEARNIN_RENDERING_DESCRIPTORS_DESCRIPTORSET_HPP