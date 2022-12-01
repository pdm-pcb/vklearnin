#ifndef VKLEARNIN_SHADERS_DESCRIPTORSET_HPP
#define VKLEARNIN_SHADERS_DESCRIPTORSET_HPP

#include "vklearnin/system/pch.hpp"
#include "vklearnin/shaders/BufferObject.hpp"
#include "vklearnin/shaders/ImageObject.hpp"
#include "vklearnin/shaders/DescriptorSetLayout.hpp"

namespace vkl {

class DescriptorPool;
class DescriptorSetLayout;

class DescriptorSet {
public:
    void add_ubo(const size_t size, const vk::ShaderStageFlags stages);
    void add_texture2D(const char *filepath);

    void create(const DescriptorPool &descriptor_pool);
    void destroy();

    inline const auto & native() const { return _descriptor_set; }
    inline const auto & layout() const { return _layout; }

    inline auto & uniform_buffers() { return _uniform_buffers; }
    inline auto & textures()        { return _textures; }

    DescriptorSet() = default;
    ~DescriptorSet() = default;
    
    DescriptorSet(DescriptorSet &&other);
    DescriptorSet(const DescriptorSet &other) = delete;
    
    DescriptorSet & operator=(DescriptorSet &&other) = delete;
    DescriptorSet & operator=(const DescriptorSet &other) = delete;

private:
    std::vector<BufferObject> _uniform_buffers;
    std::vector<ImageObject>  _textures;
    DescriptorSetLayout       _layout;
    vk::DescriptorSet         _descriptor_set;
};

} // namespace vkl

#endif // VKLEARNIN_SHADERS_DESCRIPTORSET_HPP