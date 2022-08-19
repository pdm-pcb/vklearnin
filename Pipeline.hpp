#ifndef VKL_PIPELINE_HPP
#define VKL_PIPELINE_HPP

#include <vulkan/vulkan.hpp>

#include <vector>

class Instance;
class Swapchain;

// =============================================================================
class Pipeline {
public:
    // populate the module member variables
    // void vertex_from_source(const char *filepath,
    //                         const bool optimize = false);
    // void fragment_from_source(const char *filepath,
    //                           const bool optimize = false);
    void vertex_from_binary(const char *filepath);
    void fragment_from_binary(const char *filepath);

    // create a suitable default layout
    void init_render_passes(const Swapchain &swapchain);
    void init_layout();
    void init_pipeline(const Swapchain &swapchain);

    // -------------------------------------------------------------------------
    // For those concerned with pipeline atributes

    inline const ::VkRenderPass & renderpass() const { return _renderpass; }
    inline const ::VkPipeline   & pipeline()   const { return _pipeline;   }
    inline const ::VkViewport * viewports() const { return _viewports.data(); }
    inline const ::VkRect2D   * scissors()  const { return _scissors.data();  }

    Pipeline(const ::VkDevice &device);
    ~Pipeline();

private:
    const ::VkDevice &_device;

    ::VkShaderModule _vert;
    ::VkShaderModule _frag;
    std::vector<::VkPipelineShaderStageCreateInfo> _shader_stages;

    std::vector<::VkViewport> _viewports;
    std::vector<::VkRect2D>   _scissors;

    ::VkRenderPass     _renderpass;
    ::VkPipelineLayout _layout;
    ::VkPipeline       _pipeline;
};

#endif // VKL_PIPELINE_HPP