#ifndef VKLEARNIN_PIPELINE_HPP
#define VKLEARNIN_PIPELINE_HPP

#include "vklearnin/Swapchain.hpp"
#include "vklearnin/Buffers/DepthBuffer.hpp"

#include <vulkan/vulkan.hpp>

#include <vector>

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
    void init_layout(const vk::DescriptorSetLayout &desc_set_layout);
    void init_pipeline(const Swapchain &swapchain);

    // -------------------------------------------------------------------------
    // Update if the swapchain has changed size

    inline void update_dimensions(const Swapchain &swapchain) {
        auto [width, height] = swapchain.extent(); 
        auto [x, y]          = swapchain.offset();

        _viewport = vk::Viewport {
            .x = static_cast<float>(x),
            .y = static_cast<float>(y),
            .width  = static_cast<float>(width),
            .height = static_cast<float>(height),
            .minDepth = 0.0f,
            .maxDepth = 1.0f,
        };

        _scissor = vk::Rect2D {
            .offset = { x, y },
            .extent = { width, height },
        };

        _init_depth_buffer(swapchain);
    }

    // -------------------------------------------------------------------------
    // For those concerned with pipeline atributes

    inline const vk::RenderPass & renderpass() const { return _renderpass;    }
    inline const vk::Pipeline   & pipeline()   const { return _pipeline;      }
    inline const vk::Viewport   & viewport()   const { return _viewport;      }
    inline const vk::Rect2D     & scissor()    const { return _scissor;       }
    inline const vk::PipelineLayout & layout() const { return _layout;        }
    inline const DepthBuffer & depth_buffer()  const { return *_depth_buffer; }

    Pipeline(const Instance &instance);
    ~Pipeline();

private:
    vk::ShaderModule _vert;
    vk::ShaderModule _frag;
    std::vector<vk::PipelineShaderStageCreateInfo> _shader_stages;

    vk::Viewport _viewport;
    vk::Rect2D   _scissor;

    DepthBuffer *_depth_buffer;

    vk::RenderPass     _renderpass;
    vk::PipelineLayout _layout;
    vk::Pipeline       _pipeline;

    const Instance &_instance;

    void _init_depth_buffer(const Swapchain &swapchain);
};

#endif // VKLEARNIN_PIPELINE_HPP