#ifndef VKLEARNIN_PIPELINE_HPP
#define VKLEARNIN_PIPELINE_HPP

#include "vklearnin/pch.hpp"
#include "vklearnin/Swapchain.hpp"
#include "vklearnin/Buffers/DepthBuffer.hpp"

class Instance;
class DepthBuffer;

// =============================================================================
class Pipeline {
public:
    void vertex_from_binary(const char *filepath);
    void fragment_from_binary(const char *filepath);

    // create a suitable default layout
    void init_render_passes(const Swapchain &swapchain);
    void init_ubos(const vk::Device &device, const size_t count);
    void init_layout(const std::vector<vk::DescriptorSetLayout> &desc_layouts);
    void init_pipeline(const Swapchain &swapchain);

    // -------------------------------------------------------------------------
    // Update if the swapchain has changed size

    inline void update_dimensions(const Swapchain &swapchain) {
        auto [width, height] = swapchain.extent(); 
        auto [x, y]          = swapchain.offset();

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

        _init_color_buffer(swapchain);
        _init_depth_buffer(swapchain);
    }

    // -------------------------------------------------------------------------
    // For those concerned with pipeline atributes

    inline const vk::RenderPass     & renderpass() const { return _renderpass; }
    inline const vk::Pipeline       & pipeline()   const { return _pipeline;   }
    inline const vk::Viewport       & viewport()   const { return _viewport;   }
    inline const vk::Rect2D         & scissor()    const { return _scissor;    }
    inline const vk::PipelineLayout & layout()     const { return _layout;     }

    inline const vk::ImageView & depth_buffer() const {
        return _depth_buffer->image_view();
    }
    inline const vk::ImageView & color_buffer() const {
        return _color_buffer_view;
    }

    explicit Pipeline(const Instance &instance);
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

    vk::Image     _color_buffer_handle;
    VmaAllocation _color_buffer_alloc;
    vk::ImageView _color_buffer_view;

    DepthBuffer *_depth_buffer;

    vk::RenderPass     _renderpass;
    vk::PipelineLayout _layout;
    vk::Pipeline       _pipeline;

    const Instance &_instance;

    void _init_color_buffer(const Swapchain &swapchain);
    void _init_depth_buffer(const Swapchain &swapchain);
};

#endif // VKLEARNIN_PIPELINE_HPP