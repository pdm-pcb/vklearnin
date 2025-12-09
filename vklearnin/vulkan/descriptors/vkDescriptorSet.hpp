#ifndef VKLEARNIN_VULKAN_VKDESCRIPTORSET_HPP
#define VKLEARNIN_VULKAN_VKDESCRIPTORSET_HPP

#include "vklearnin/pch.hpp"

namespace vkl {

class vkDescriptorSetLayout;
class vkDescriptorPool;
class vkDevice;
class vkGraphicsPipeline;
class vkComputePipeline;
class vkCmdBuffer;

class vkDescriptorSet final {
public:
    vkDescriptorSet() = default;
    ~vkDescriptorSet() = default;

    vkDescriptorSet(vkDescriptorSet &&other);
    vkDescriptorSet(vkDescriptorSet const &) = delete;

    vkDescriptorSet & operator=(vkDescriptorSet &&) = delete;
    vkDescriptorSet & operator=(vkDescriptorSet const &) = delete;

    bool allocate(vkDescriptorSetLayout const &layout,
                  vkDescriptorPool const &pool,
                  vkDevice const &device);

    vkDescriptorSet &  add_update(vk::DescriptorBufferInfo const &update,
                                  uint32_t const binding,
                                  vk::DescriptorType const descriptor_type);

    vkDescriptorSet &  add_update(vk::DescriptorImageInfo const &update,
                                  uint32_t const binding,
                                  vk::DescriptorType const descriptor_type);

    void update();

    void bind(vkGraphicsPipeline const &pipeline,
              uint32_t const set_number,
              vkCmdBuffer const &cmd_buffer) const;

    void bind(vkComputePipeline const &pipeline,
              uint32_t const set_number,
              vkCmdBuffer const &cmd_buffer) const;

    [[nodiscard]] inline auto const & native() const { return _handle; }

private:
    vk::DescriptorSet _handle { nullptr };
    vk::Device _device { nullptr };

    vk::DescriptorSetLayout _layout { nullptr };

    std::vector<vk::WriteDescriptorSet> _updates;
};

} // namespace vkl

#endif // VKLEARNIN_VULKAN_VKDESCRIPTORSET_HPP