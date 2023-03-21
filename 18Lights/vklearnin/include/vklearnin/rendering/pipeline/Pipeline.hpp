#ifndef VKLEARNIN_RENDERINGPIPELINE_PIPELINE_HPP
#define VKLEARNIN_RENDERINGPIPELINE_PIPELINE_HPP

#include "vklearnin/system/pch.hpp"
#include "vklearnin/rendering/pipeline/Shader.hpp"
#include "vklearnin/meshes/VertexTypes.hpp"

namespace vkl {

class RenderPass;

class Pipeline {
public:
    struct Config {
        vk::PolygonMode         polygon_mode;
        vk::CullModeFlags       cull_mode;
        vk::FrontFace           front_face;
        vk::SampleCountFlagBits max_msaa_samples;
        uint32_t                subpass_index;
    };

    Pipeline & vert_from_spirv(std::string_view filepath,
                         std::string_view entry_point = "main");
    Pipeline & frag_from_spirv(std::string_view filepath,
                         std::string_view entry_point = "main");

    Pipeline & describe_vertex_input(const VertexBindings &bindings,
                               const VertexAttribs &attributes);

    Pipeline & add_descriptor_set(const vk::DescriptorSetLayout &set_layout);
    Pipeline & add_push_constant(vk::ShaderStageFlags stage_flags, size_t size);

    void create(RenderPass const &render_pass, Config const &config);
    void destroy();
    void update_dimensions();

    void bind(vk::CommandBuffer const &cmd_buffer);

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

    std::vector<vk::DescriptorSetLayout> _desc_set_layouts;
    std::vector<vk::PushConstantRange>   _push_constants;
    size_t _push_constant_offset;

    vk::PipelineLayout _layout;
    vk::Pipeline       _pipeline;

    void _init_assembly();
    void _init_viewport();
    void _init_raster(Config const &config);
    void _init_multisample(Config const &config);
    void _init_depth_stencil();
    void _init_blend();
    void _init_dynamic_states();
    void _init_layout();
};

} // namespace vkl

#endif // VKLEARNIN_RENDERINGPIPELINE_PIPELINE_HPP