#ifndef VKL_PIPELINE_HPP
#define VKL_PIPELINE_HPP

#include "Swapchain.hpp"

#include <vulkan/vulkan.hpp>

#include <vector>

class Instance;

// =============================================================================
class Pipeline {
public:
    void vertex_from_binary(const char *filepath);
    void fragment_from_binary(const char *filepath);

    // create a suitable default layout
    void init_render_passes(const Swapchain &swapchain);
    void init_ubos(const ::VkDevice &device, const size_t count);
    void init_layout(const ::VkDescriptorSetLayout &desc_set_layout);
    void init_pipeline(const Swapchain &swapchain);

    // -------------------------------------------------------------------------
    // Update if the swapchain has changed size

    inline void update_dimensions(const Swapchain &swapchain) {
        auto [width, height] = swapchain.extent(); 
        auto [x, y]          = swapchain.offset();

        _viewport = {
            .x = static_cast<float>(x),
            .y = static_cast<float>(y),
            .width  = static_cast<float>(width),
            .height = static_cast<float>(height),
            .minDepth = 0.0f,
            .maxDepth = 1.0f,
        };

        _scissor = {
            .offset = { x, y },
            .extent = { width, height },
        };
    }

    // -------------------------------------------------------------------------
    // For those concerned with pipeline atributes

    inline const ::VkRenderPass & renderpass() const { return _renderpass; }
    inline const ::VkPipeline   & pipeline()   const { return _pipeline;   }
    inline const ::VkViewport   & viewport()   const { return _viewport;   }
    inline const ::VkRect2D     & scissor()    const { return _scissor;    }
    inline const ::VkPipelineLayout & layout() const { return _layout;     }

    Pipeline(const ::VkDevice &device);
    ~Pipeline();

private:
    const ::VkDevice &_device;

    ::VkShaderModule _vert;
    ::VkShaderModule _frag;
    std::vector<::VkPipelineShaderStageCreateInfo> _shader_stages;

    ::VkViewport _viewport;
    ::VkRect2D   _scissor;

    ::VkRenderPass     _renderpass;
    ::VkPipelineLayout _layout;
    ::VkPipeline       _pipeline;
};

#endif // VKL_PIPELINE_HPP