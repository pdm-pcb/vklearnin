#ifndef VKLEARNIN_ENGINE_PIPELINE_HPP
#define VKLEARNIN_ENGINE_PIPELINE_HPP

#include "vklearnin/system/pch.hpp"
#include "vklearnin/engine/Swapchain.hpp"
#include "vklearnin/engine/Framebuffer.hpp"

namespace vkl {

class Framebuffer;
class FrameData;
struct BufferObject;

class Pipeline final {
public:
    void vertex_from_binary(const char *filepath);
    void fragment_from_binary(const char *filepath);

    void create(const FrameData &frame_data);
    void destroy();

    void create_framebuffers();
    void destroy_framebuffers();

    // Update if the swapchain has changed size
    inline void update_dimensions() {
        const auto [width, height] = _swapchain.extent(); 
        const auto [x, y]          = _swapchain.offset();

        _viewport = vk::Viewport {
            .x = static_cast<float>(x),
            .y = static_cast<float>(height),
            .width  = static_cast<float>(width),
            .height = -static_cast<float>(height),
            .minDepth = 0.0f,
            .maxDepth = 1.0f,
        };

        _scissor = vk::Rect2D {
            .offset = { x, y },
            .extent = { width, height },
        };

        CONSOLE_TRACE(
            "Pipeline viewport updated: {:.0f} by {:.0f} at ({:.0f}, {:.0f}) ",
            _viewport.width,
            _viewport.height,
            _viewport.x,
            _viewport.y
        );
    }

    // For those concerned with pipeline attributes
    inline const auto & renderpass() const { return _renderpass; }
    inline const auto & native()     const { return _pipeline;   }
    inline const auto & viewport()   const { return _viewport;   }
    inline const auto & scissor()    const { return _scissor;    }
    inline const auto & layout()     const { return _layout;     }

    inline const auto & framebuffer(const uint32_t index) const {
        return _framebuffers[index];
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

    std::vector<vk::AttachmentDescription>             _attachments;
    std::vector<vk::PipelineColorBlendAttachmentState> _blend_attachments;

    std::vector<Framebuffer> _framebuffers;

    vk::PipelineLayout _layout;
    vk::RenderPass     _renderpass;

    std::vector<vk::DynamicState>            _dynamic_states;
    vk::PipelineDynamicStateCreateInfo       _dynamic_state_info;
    vk::PipelineViewportStateCreateInfo      _viewport_info;
    vk::PipelineVertexInputStateCreateInfo   _vert_input_info;
    vk::PipelineInputAssemblyStateCreateInfo _assembly_info;
    vk::PipelineColorBlendStateCreateInfo    _blend_info;
    vk::PipelineRasterizationStateCreateInfo _raster_info;

    vk::Pipeline _pipeline;

    const Swapchain  &_swapchain;

    void _init_layout(const FrameData &frame_data);
    void _init_render_passes();
    void _init_dynamic_states();
    void _init_viewport();
    void _init_vert_input();
    void _init_assembly();
    void _init_blend();
    void _init_raster();
};

} // namespace vkl

#endif // VKLEARNIN_ENGINE_PIPELINE_HPP