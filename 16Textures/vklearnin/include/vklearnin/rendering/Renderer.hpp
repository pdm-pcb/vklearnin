#ifndef VKLEARNIN_RENDERING_RENDERER_HPP
#define VKLEARNIN_RENDERING_RENDERER_HPP

#include "vklearnin/system/pch.hpp"
#include "vklearnin/rendering/DrawSubmission.hpp"
#include "vklearnin/rendering/renderpass/RenderPass.hpp"
#include "vklearnin/rendering/renderpass/Framebuffer.hpp"
#include "vklearnin/rendering/pipeline/Pipeline.hpp"
#include "vklearnin/rendering/descriptors/DescriptorPool.hpp"
#include "vklearnin/rendering/descriptors/DescriptorSet.hpp"

namespace vkl {

struct BufferObject;
struct ImageObject;

class Renderer {
public:
    const enum PipelineIndex {
        // COLOR,
        TEXTURE,

        MAX
    };

    static void submit(const PipelineIndex index, const DrawSubmission &draw);
    static void render_pass(const vk::CommandBuffer &cmd_buffer);

    static void init();
    static void shutdown();

    static void add_ubo(const PipelineIndex index,
                        const DescriptorSet::BufferObjects &buffers,
                        const vk::ShaderStageFlags stage_flags);
    static void add_texture2D(const PipelineIndex index,
                              const ImageObject &texture);
    static void create_pipelines();

    Renderer() = delete;

private:
    static DescriptorPool _desc_pool;

    using DrawSubmissions = std::vector<DrawSubmission>;
    struct FullPipeline {
        DescriptorSet   desc_set;
        Pipeline        pipeline;
        DrawSubmissions draws;
    };

    using Pipelines = std::array<FullPipeline, PipelineIndex::MAX>;
    static Pipelines _pipelines;

    using Framebuffers = std::vector<Framebuffer>;
    static Framebuffers _framebuffers;
    static RenderPass   _render_pass;

    static void _init_framebuffers();
    static void _init_descriptors();
};

} // namespace vkl

#endif // VKLEARNIN_RENDERING_RENDERER_HPP