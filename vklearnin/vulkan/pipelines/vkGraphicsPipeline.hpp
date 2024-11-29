#ifndef VKLEARNIN_VULKAN_PIPELINES_VKGRAPHICSPIPELINE_HPP
#define VKLEARNIN_VULKAN_PIPELINES_VKGRAPHICSPIPELINE_HPP

#include "vklearnin/pch.hpp"

#include "vklearnin/vulkan/pipelines/vkShaderModule.hpp"

namespace vkl {

class vkRenderPass;
class vkDevice;
class vkCmdBuffer;

class vkGraphicsPipeline final {
public:
    vkGraphicsPipeline() = default;
    ~vkGraphicsPipeline() = default;

    vkGraphicsPipeline(vkGraphicsPipeline &&) = delete;
    vkGraphicsPipeline(vkGraphicsPipeline const &) = delete;

    vkGraphicsPipeline& operator=(vkGraphicsPipeline &&) = delete;
    vkGraphicsPipeline& operator=(vkGraphicsPipeline const &) = delete;

    struct Config final {
        // Viewport settings
        vk::Offset2D viewport_offset { };
        vk::Extent2D viewport_extent { };

        // Drawing options
        vk::PrimitiveTopology topology     { vk::PrimitiveTopology::eTriangleList };
        vk::PolygonMode       polygon_mode { vk::PolygonMode::eFill };
        float                 line_width   { 1.0f };
        vk::CullModeFlags     cull_mode    { vk::CullModeFlagBits::eBack };
        vk::FrontFace         front_face   { vk::FrontFace::eClockwise };

        // Multisample config
        vk::SampleCountFlagBits sample_flags { vk::SampleCountFlagBits::e1 };

        // Depth/shadow buffer options
        vk::Bool32    enable_depth_test   { VK_FALSE };
        vk::CompareOp depth_compare       { vk::CompareOp::eLess };
        vk::Bool32    enable_depth_bias   { VK_FALSE };
        float         depth_bias_constant { 0.0f };
        float         depth_bias_slope    { 0.0f };
    };

    using VertexBindings = std::span<vk::VertexInputBindingDescription const>;
    using VertexAttribs  = std::span<vk::VertexInputAttributeDescription const>;
    vkGraphicsPipeline & describe_vertex_input(VertexBindings const bindings,
                                               VertexAttribs const attributes);

    vkGraphicsPipeline & add_shader(vkShaderModule const &module);

    vkGraphicsPipeline & add_push_constant(vk::ShaderStageFlags const stage_flags,
                                   vk::DeviceSize const size);

    vkGraphicsPipeline &
    add_descriptor_set_layout(vk::DescriptorSetLayout const &layout);

    vkGraphicsPipeline & add_render_pass(vkRenderPass const &render_pass);

    bool create(Config const &config, vkDevice const &device);
    bool destroy();

    bool bind(vkCmdBuffer const &cmd_buffer) const;

    bool send_push_constants(vk::ShaderStageFlags stage_flags,
                             uint32_t offset,
                             uint32_t size_bytes,
                             void const * data,
                             vkCmdBuffer const &cmd_buffer) const;

    void update_dimensions(vk::Extent2D const &extent,
                           vk::Offset2D const &offset);

    inline auto const & native() const { return _handle; }
    inline auto const & layout() const { return _layout; }

private:
    vk::Pipeline _handle { nullptr };
    vk::Device   _device { nullptr };

    std::vector<vk::PipelineShaderStageCreateInfo> _shader_stages { };

    vk::Viewport _viewport {
        .x        = 0.0f,
        .y        = 0.0f,
        .width    = 0.0f,
        .height   = 0.0f,
        .minDepth = 0.0f,
        .maxDepth = 0.0f,
    };

    vk::Rect2D _scissor {
        .offset { .x = 0, .y = 0 },
        .extent { .width = 0u, .height = 0u },
    };

    vk::PipelineVertexInputStateCreateInfo   _vert_input_info    { };
    vk::PipelineInputAssemblyStateCreateInfo _assembly_info      { };
    vk::PipelineViewportStateCreateInfo      _viewport_info      { };
    vk::PipelineRasterizationStateCreateInfo _raster_info        { };
    vk::PipelineMultisampleStateCreateInfo   _multisample_info   { };
    vk::PipelineDepthStencilStateCreateInfo  _depth_stencil_info { };

    std::vector<vk::PipelineColorBlendAttachmentState> _blend_states { };
    vk::PipelineColorBlendStateCreateInfo              _blend_info   { };

    std::vector<vk::DynamicState>      _dynamic_states     { };
    vk::PipelineDynamicStateCreateInfo _dynamic_state_info { };

    std::vector<vk::PushConstantRange> _push_constants { };
    vk::DeviceSize _push_constant_offset { 0u };

    std::vector<vk::DescriptorSetLayout> _descriptor_set_layouts;

    vk::PipelineLayout _layout { nullptr };

    vk::RenderPass _render_pass { nullptr };

    vk::GraphicsPipelineCreateInfo _create_info { };

    void _init_input_assembly(Config const &config);
    void _init_viewport(Config const &config);
    void _init_raster(Config const &config);
    void _init_multisample(Config const &config);
    void _init_depth_stencil(Config const &config);
    void _init_blend_states();
    void _init_dynamic_states();
    bool _init_layout();
};

} // namespace vkl;

#endif // VKLEARNIN_VULKAN_PIPELINES_VKGRAPHICSPIPELINE_HPP