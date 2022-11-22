#ifndef VKLEARNIN_SHADERS_DESCRIPTORSET_HPP
#define VKLEARNIN_SHADERS_DESCRIPTORSET_HPP

#include "vklearnin/system/pch.hpp"
#include "vklearnin/shaders/BufferObject.hpp"
#include "vklearnin/shaders/ImageObject.hpp"

namespace vkl {

class DescriptorPool;
class DescriptorSetLayout;

class DescriptorSet {
public:
    void create(const DescriptorPool &descriptor_pool,
                const DescriptorSetLayout &layout,
                const size_t instance_buffer_size);
    void destroy();

    inline const auto & native()          const { return _descriptor_set;  }
    inline const auto & instance_buffer() const { return _instance_buffer; }
    inline const auto & texture_image()   const { return _texture;         }

    DescriptorSet() = default;
    ~DescriptorSet() = default;
    
    DescriptorSet(DescriptorSet &&other);
    DescriptorSet(const DescriptorSet &other) = delete;
    
    DescriptorSet & operator=(DescriptorSet &&other) = delete;
    DescriptorSet & operator=(const DescriptorSet &other) = delete;

private:
    vk::DescriptorSet _descriptor_set;
    BufferObject      _instance_buffer;
    ImageObject       _texture;
};

} // namespace vkl

#endif // VKLEARNIN_SHADERS_DESCRIPTORSET_HPP