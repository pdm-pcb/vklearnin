#ifndef VKLEARNIN_VULKAN_PIPELINES_VKCOMPUTEPIPELINE_HPP
#define VKLEARNIN_VULKAN_PIPELINES_VKCOMPUTEPIPELINE_HPP

#include "vklearnin/pch.hpp"

#include "vklearnin/vulkan/pipelines/vkShaderModule.hpp"

namespace vkl {

class vkRenderPass;
class vkDevice;
class vkCmdBuffer;

class vkComputePipeline final {
public:
    vkComputePipeline() = default;
    ~vkComputePipeline() = default;

    vkComputePipeline(vkComputePipeline &&) = delete;
    vkComputePipeline(vkComputePipeline const &) = delete;

    vkComputePipeline& operator=(vkComputePipeline &&) = delete;
    vkComputePipeline& operator=(vkComputePipeline const &) = delete;

    vkComputePipeline & add_shader(vkShaderModule const &module);

    vkComputePipeline &
    add_push_constant(vk::ShaderStageFlags const stage_flags,
                      vk::DeviceSize const size);

    vkComputePipeline &
    add_descriptor_set_layout(vk::DescriptorSetLayout const &layout);

    bool create(vkDevice const &device);
    bool destroy();

    bool bind(vkCmdBuffer const &cmd_buffer) const;

    bool send_push_constants(vk::ShaderStageFlags stage_flags,
                             uint32_t offset,
                             uint32_t size_bytes,
                             void const * data,
                             vkCmdBuffer const &cmd_buffer) const;

    inline auto const & native() const { return _handle; }
    inline auto const & layout() const { return _layout; }

private:
    vk::Pipeline _handle { nullptr };
    vk::Device   _device { nullptr };

    vk::PipelineShaderStageCreateInfo _shader_stage { };

    std::vector<vk::PushConstantRange> _push_constants { };
    vk::DeviceSize _push_constant_offset { 0u };

    std::vector<vk::DescriptorSetLayout> _descriptor_set_layouts;

    vk::PipelineLayout _layout { nullptr };
    vk::ComputePipelineCreateInfo _create_info { };

    void _init_layout();
};

} // namespace vkl;

#endif // VKLEARNIN_VULKAN_PIPELINES_VKCOMPUTEPIPELINE_HPP