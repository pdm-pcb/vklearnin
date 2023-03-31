#ifndef VKLEARNIN_RENDERINGPIPELINE_PIPELINE_HPP
#define VKLEARNIN_RENDERINGPIPELINE_PIPELINE_HPP

#include "vklearnin/system/pch.hpp"
#include "vklearnin/rendering/pipeline/Shader.hpp"

namespace vkl {

class DescriptorSetLayout;
class DescriptorSet;
class RenderPass;
class CmdBuffer;

class Pipeline {
public:
    struct Config {
        // Viewport settings
        vk::Extent2D viewport_extent { 0u, 0u };
        vk::Offset2D viewport_offset { 0, 0 };

        // Drawing options
        vk::PolygonMode   polygon_mode = vk::PolygonMode::eFill;
        vk::CullModeFlags cull_mode    = vk::CullModeFlagBits::eBack;
        vk::FrontFace     front_face   = vk::FrontFace::eClockwise;

        // Multisampling configuration
        vk::SampleCountFlagBits msaa_samples = vk::SampleCountFlagBits::e1;

        // Depth/shadow buffer options
        vk::Bool32    enable_depth_test   = VK_TRUE;
        vk::CompareOp depth_compare       = vk::CompareOp::eLess;
        vk::Bool32    enable_depth_bias   = VK_FALSE;
        float         depth_bias_constant = 0.0f;
        float         depth_bias_slope    = 0.0f;

        // Universal options
        uint32_t subpass_index = 0u;
    };

    void bind(CmdBuffer const &cmd_buffer);
    void bind_descriptor_set(CmdBuffer const &cmd_buffer,
                             DescriptorSet const &set);

    Pipeline & vert_from_spirv(std::string_view filepath,
                               std::string_view entry_point = "main");
    Pipeline & frag_from_spirv(std::string_view filepath,
                               std::string_view entry_point = "main");

    Pipeline & describe_vertex_input(Vertex::Bindings const &bindings,
                                     Vertex::Attribs const &attributes);

    Pipeline & add_descriptor_set(DescriptorSetLayout const &set_layout);
    Pipeline & add_push_constant(vk::ShaderStageFlags const stage_flags,
                                 size_t const size);

    void create(RenderPass const &render_pass, Config const &config);
    void destroy();
    void update_dimensions(vk::Extent2D const &extent,
                           vk::Offset2D const &offset);

    inline auto const& native()   const { return _pipeline; }
    inline auto const& layout()   const { return _layout; }
    inline auto const& viewport() const { return _viewport; }
    inline auto const& scissor()  const { return _scissor; }

    Pipeline();
    virtual ~Pipeline() = default;

    Pipeline(Pipeline &&) = delete;
    Pipeline(const Pipeline &) = delete;

    Pipeline& operator=(Pipeline &&) = delete;
    Pipeline& operator=(const Pipeline &) = delete;

private:
    Shader _vert;
    Shader _frag;

    std::vector<vk::PipelineShaderStageCreateInfo>     _shader_stages;
    std::vector<vk::PipelineColorBlendAttachmentState> _blend_states;

    vk::Viewport _viewport;
    vk::Rect2D   _scissor;

    vk::PipelineVertexInputStateCreateInfo   _vert_input_info;
    vk::PipelineInputAssemblyStateCreateInfo _assembly_info;
    vk::PipelineViewportStateCreateInfo      _viewport_info;
    vk::PipelineRasterizationStateCreateInfo _raster_info;
    vk::PipelineMultisampleStateCreateInfo   _multisample_info;
    vk::PipelineDepthStencilStateCreateInfo  _depth_stencil_info;
    vk::PipelineColorBlendStateCreateInfo    _blend_info;
    std::vector<vk::DynamicState>            _dynamic_states;
    vk::PipelineDynamicStateCreateInfo       _dynamic_state_info;

    std::vector<vk::DescriptorSetLayout>   _desc_set_layouts;
    std::unordered_map<uint64_t, uint32_t> _desc_set_bindings;
    uint32_t _next_set_binding;

    std::vector<vk::PushConstantRange> _push_constants;
    size_t _push_constant_offset;

    vk::PipelineLayout _layout;
    vk::Pipeline       _pipeline;

    void _init_assembly();
    void _init_viewport(Config const &config);
    void _init_raster(Config const &config);
    void _init_multisample(Config const &config);
    void _init_depth_stencil(Config const &config);
    void _init_blend();
    void _init_dynamic_states();
    void _init_layout();
};

} // namespace vkl

#endif // VKLEARNIN_RENDERINGPIPELINE_PIPELINE_HPP