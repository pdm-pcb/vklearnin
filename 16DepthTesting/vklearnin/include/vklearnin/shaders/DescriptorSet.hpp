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
    void add_ubo(const size_t size);
    void add_texture2D(const char *filepath);

    void create(const DescriptorPool &descriptor_pool,
                const DescriptorSetLayout &layout);
    void destroy();

    inline const auto & native()       const { return _descriptor_set; }
    inline const auto & camera_ubo()   const { return _uniform_buffers[0]; }
    inline const auto & instance_ubo() const { return _uniform_buffers[1]; }

    DescriptorSet() = default;
    ~DescriptorSet() = default;
    
    DescriptorSet(DescriptorSet &&other);
    DescriptorSet(const DescriptorSet &other) = delete;
    
    DescriptorSet & operator=(DescriptorSet &&other) = delete;
    DescriptorSet & operator=(const DescriptorSet &other) = delete;

private:
    vk::DescriptorSet         _descriptor_set;
    std::vector<BufferObject> _uniform_buffers;
    std::vector<ImageObject>  _textures;
};

} // namespace vkl

#endif // VKLEARNIN_SHADERS_DESCRIPTORSET_HPP