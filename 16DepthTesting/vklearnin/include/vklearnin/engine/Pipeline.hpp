#ifndef VKLEARNIN_ENGINE_PIPELINE_HPP
#define VKLEARNIN_ENGINE_PIPELINE_HPP

#include "vklearnin/system/pch.hpp"
#include "vklearnin/engine/Swapchain.hpp"
#include "vklearnin/rendering/RenderPass.hpp"

namespace vkl {

class Pipeline final {
public:
    using PushConstantRanges = std::vector<vk::PushConstantRange>;
    using DescriptorSetLayouts = std::vector<vk::DescriptorSetLayout>;
    using RenderPasses = std::vector<vk::RenderPass>;

    void vertex_from_binary(const char *filepath);
    void fragment_from_binary(const char *filepath);

    void set_push_constants(const PushConstantRanges &ranges);
    void set_layout(const DescriptorSetLayouts &descriptor_layouts);
    void set_render_pass(const RenderPass &render_pass);

    void create();
    void destroy();

    void create_framebuffers();
    void destroy_framebuffers();
    void update_dimensions();

    // For those concerned with pipeline attributes
    inline const auto & native()      const { return _pipeline;    }
    inline const auto & render_pass() const { return _render_pass; }
    inline const auto & viewport()    const { return _viewport;    }
    inline const auto & scissor()     const { return _scissor;     }
    inline const auto & layout()      const { return _layout;      }

    explicit Pipeline(const Swapchain &swapchain);
    ~Pipeline() = default;

    Pipeline() = delete;

    Pipeline(Pipeline &&other) = delete;
    Pipeline(const Pipeline &other) = delete;

    Pipeline & operator=(Pipeline &&other) = delete;
    Pipeline & operator=(const Pipeline &other) = delete;

private:
    vk::ShaderModule _vert;
    vk::ShaderModule _frag;
    std::vector<vk::PipelineShaderStageCreateInfo> _shader_stages;

    vk::Viewport _viewport;
    vk::Rect2D   _scissor;

    std::vector<vk::PipelineColorBlendAttachmentState> _blend_states;

    vk::PipelineVertexInputStateCreateInfo   _vert_input_info;
    vk::PipelineInputAssemblyStateCreateInfo _assembly_info;
    vk::PipelineViewportStateCreateInfo      _viewport_info;
    vk::PipelineRasterizationStateCreateInfo _raster_info;
    vk::PipelineColorBlendStateCreateInfo    _blend_info;
    vk::PipelineDepthStencilStateCreateInfo  _depth_stencil_info;
    std::vector<vk::DynamicState>            _dynamic_states;
    vk::PipelineDynamicStateCreateInfo       _dynamic_state_info;

    RenderPass  _render_pass;

    PushConstantRanges _push_constant_ranges;
    vk::PipelineLayout _layout;
    vk::Pipeline       _pipeline;

    const Swapchain  &_swapchain;

    void _init_vert_input();
    void _init_assembly();
    void _init_viewport();
    void _init_raster();
    void _init_depth();
    void _init_blend();
    void _init_dynamic_states();
};

} // namespace vkl

#endif // VKLEARNIN_ENGINE_PIPELINE_HPP