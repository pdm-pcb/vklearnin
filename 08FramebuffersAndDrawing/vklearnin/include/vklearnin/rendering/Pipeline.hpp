#ifndef BRASSTACKS_ENGINE_PIPELINE_HPP
#define BRASSTACKS_ENGINE_PIPELINE_HPP

#include "vklearnin/system/pch.hpp"
#include "vklearnin/rendering/Swapchain.hpp"

namespace vkl {

// =============================================================================
class Pipeline final {
public:
    void vertex_from_binary(const char *filepath);
    void fragment_from_binary(const char *filepath);

    // create a suitable default layout
    void init_layout();
    void init_render_passes();
    void create();

    // -------------------------------------------------------------------------
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

    // -------------------------------------------------------------------------
    // For those concerned with pipeline atributes
    inline const vk::RenderPass     & renderpass() const { return _renderpass; }
    inline const vk::Pipeline       & native()     const { return _pipeline;   }
    inline const vk::Viewport       & viewport()   const { return _viewport;   }
    inline const vk::Rect2D         & scissor()    const { return _scissor;    }
    inline const vk::PipelineLayout & layout()     const { return _layout;     }

    Pipeline(const vk::Device &logical_device, const Swapchain &swapchain);
    ~Pipeline();

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

    std::vector<vk::AttachmentDescription> _attachments;

    vk::RenderPass     _renderpass;
    vk::PipelineLayout _layout;
    vk::Pipeline       _pipeline;

    const vk::Device &_logical_device;
    const Swapchain  &_swapchain;
};

} // namespace vkl

#endif // BRASSTACKS_PIPELINE_HPP