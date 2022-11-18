#ifndef VKLEARNIN_SHADERS_DESCRIPTORSET_HPP
#define VKLEARNIN_SHADERS_DESCRIPTORSET_HPP

#include "vklearnin/system/pch.hpp"
#include "vklearnin/shaders/BufferObject.hpp"

namespace vkl {

class DescriptorPool;
class DescriptorSetLayout;

class DescriptorSet {
public:
    void create(const DescriptorPool &descriptor_pool,
                const DescriptorSetLayout &layout,
                const size_t set_size_bytes);
    void destroy();

    inline const auto & native() const { return _descriptor_set; }
    inline const auto & buffer() const { return _buffer; }

    DescriptorSet() = default;
    ~DescriptorSet() = default;
    
    DescriptorSet(DescriptorSet &&other);
    DescriptorSet(const DescriptorSet &other) = delete;
    
    DescriptorSet & operator=(DescriptorSet &&other) = delete;
    DescriptorSet & operator=(const DescriptorSet &other) = delete;

private:
    vk::DescriptorSet _descriptor_set;
    BufferObject      _buffer;
};

} // namespace vkl

#endif // VKLEARNIN_SHADERS_DESCRIPTORSET_HPP