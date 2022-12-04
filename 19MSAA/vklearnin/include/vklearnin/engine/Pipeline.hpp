#ifndef VKLEARNIN_ENGINE_PIPELINE_HPP
#define VKLEARNIN_ENGINE_PIPELINE_HPP

#include "vklearnin/system/pch.hpp"
#include "vklearnin/rendering/RenderPass.hpp"
#include "vklearnin/engine/FrameData.hpp"

namespace vkl {

class Swapchain;
class DescriptorSetLayout;

class Pipeline final {
public:
    enum BindingFreq : uint8_t {
        PER_FRAME,
        PER_MATERIAL,
        PER_DRAW,
        MAX_BINDS
    };

    inline void reset_command_pool(const uint32_t frame_index) const {
        _frame_data[frame_index].command_pool().reset();
    }

    void vertex_from_binary(std::string_view filepath);
    void fragment_from_binary(std::string_view filepath);

    void set_push_constants(const PushConstantRanges &ranges);
    void set_per_frame_layout(const vk::DescriptorSetLayout &layout);
    void set_per_material_layout(const vk::DescriptorSetLayout &layout);
    void set_per_draw_layout(const vk::DescriptorSetLayout &layout);

    void create();
    void destroy();

    void create_framebuffers();
    void destroy_framebuffers();
    void update_dimensions();

    // For those concerned with pipeline attributes
    inline const auto & native()      const { return _pipeline;        }
    inline const auto & layout()      const { return _pipeline_layout; }
    inline const auto & render_pass() const { return _render_pass;     }
    inline const auto & viewport()    const { return _viewport;        }
    inline const auto & scissor()     const { return _scissor;         }

    inline const auto & command_buffer(const uint32_t frame_index) const {
        return _frame_data[frame_index].command_buffer().native();
    }

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

    vk::SampleCountFlagBits _sample_flags;
    std::vector<vk::PipelineColorBlendAttachmentState> _blend_states;

    vk::PipelineVertexInputStateCreateInfo   _vert_input_info;
    vk::PipelineInputAssemblyStateCreateInfo _assembly_info;
    vk::PipelineViewportStateCreateInfo      _viewport_info;
    vk::PipelineRasterizationStateCreateInfo _raster_info;
    vk::PipelineMultisampleStateCreateInfo   _msaa_info;
    vk::PipelineColorBlendStateCreateInfo    _blend_info;
    vk::PipelineDepthStencilStateCreateInfo  _depth_stencil_info;
    std::vector<vk::DynamicState>            _dynamic_states;
    vk::PipelineDynamicStateCreateInfo       _dynamic_state_info;

    PushConstantRanges   _push_constant_ranges;
    DescriptorSetLayouts _set_layouts;

    std::vector<FrameData> _frame_data;
    RenderPass  _render_pass;

    vk::PipelineLayout _pipeline_layout;
    vk::Pipeline       _pipeline;

    const Swapchain  &_swapchain;

    void _init_layout();
    void _init_vert_input();
    void _init_assembly();
    void _init_viewport();
    void _init_raster();
    void _init_msaa();
    void _init_depth();
    void _init_blend();
    void _init_dynamic_states();
};

} // namespace vkl

#endif // VKLEARNIN_ENGINE_PIPELINE_HPP