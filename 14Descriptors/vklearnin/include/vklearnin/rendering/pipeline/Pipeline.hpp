#ifndef VKLEARNIN_RENDERINGPIPELINE_PIPELINE_HPP
#define VKLEARNIN_RENDERINGPIPELINE_PIPELINE_HPP

#include "vklearnin/system/pch.hpp"
#include "vklearnin/rendering/pipeline/Shader.hpp"
#include "vklearnin/meshes/Vertex.hpp"

namespace vkl {

class RenderPass;

class Pipeline final {
public:
    void vert_from_spirv(std::string_view filepath);
    void frag_from_spirv(std::string_view filepath);

    void describe_vertex_input(
        const Vertex::BindingDescriptions &bindings,
        const Vertex::AttribDescriptions &attributes
    );

    void add_descriptor_set(const vk::DescriptorSetLayout &set_layout);
    void add_push_constant(vk::ShaderStageFlags stage_flags, uint32_t size);

    void create(const RenderPass &render_pass);
    void destroy();
    void update_dimensions();

    inline const auto & native()   const { return _pipeline; }
    inline const auto & layout()   const { return _layout; }
    inline const auto & viewport() const { return _viewport; }
    inline const auto & scissor()  const { return _scissor; }

    Pipeline();
    ~Pipeline() = default;

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
    vk::PipelineColorBlendStateCreateInfo    _blend_info;
    std::vector<vk::DynamicState>            _dynamic_states;
    vk::PipelineDynamicStateCreateInfo       _dynamic_state_info;

    std::vector<vk::DescriptorSetLayout> _desc_set_layouts;
    std::vector<vk::PushConstantRange>   _push_constants;

    vk::PipelineLayout _layout;
    vk::Pipeline       _pipeline;

    void _init_assembly();
    void _init_viewport();
    void _init_raster();
    void _init_blend();
    void _init_dynamic_states();
    void _init_layout();
};

} // namespace vkl

#endif // VKLEARNIN_RENDERINGPIPELINE_PIPELINE_HPP