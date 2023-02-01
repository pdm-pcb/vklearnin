#ifndef VKLEARNIN_RENDERING_RENDERER_HPP
#define VKLEARNIN_RENDERING_RENDERER_HPP

#include "vklearnin/system/pch.hpp"
#include "vklearnin/rendering/DrawSubmission.hpp"
#include "vklearnin/rendering/renderpass/RenderPass.hpp"
#include "vklearnin/rendering/renderpass/Framebuffer.hpp"
#include "vklearnin/rendering/pipeline/Pipeline.hpp"
#include "vklearnin/rendering/descriptors/DescriptorPool.hpp"
#include "vklearnin/rendering/descriptors/DescriptorSetLayout.hpp"
#include "vklearnin/rendering/descriptors/DescriptorSet.hpp"
#include "vklearnin/resources/buffers/BufferObject.hpp"
#include "vklearnin/resources/images/Texture2D.hpp"
#include "vklearnin/rendering/Camera.hpp"

namespace vkl {

class Renderer {
public:
    static void update_view_proj(const Camera::ViewProjMats &matrices);

    static void submit(const DrawSubmission &draw);
    static void render_pass(const vk::CommandBuffer &cmd_buffer);

    static void init();
    static void shutdown();

    Renderer() = delete;

private:
    static std::vector<Framebuffer> _framebuffers;

    static DescriptorPool             _desc_pool;
    static DescriptorSetLayout        _desc_layout;
    static std::vector<DescriptorSet> _desc_sets;

    static std::vector<BufferObject> _view_proj_ubos;

    static vkl::RenderPass _render_pass;
    static vkl::Pipeline   _pipeline;

    static std::vector<DrawSubmission> _draws;

    static void _init_framebuffers();
    static void _init_descriptors();
    static void _init_pipeline();

    /// @brief ///////////////////////////////////////////////////////////////
    static Texture2D _loltest;
    /// @brief ///////////////////////////////////////////////////////////////
};

} // namespace vkl

#endif // VKLEARNIN_RENDERING_RENDERER_HPP