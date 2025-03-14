#ifndef VKLEARNIN_RENDERING_PASSES_MSAAPASS_HPP
#define VKLEARNIN_RENDERING_PASSES_MSAAPASS_HPP

#include "vklearnin/pch.hpp"

#include "vklearnin/vulkan/pipelines/vkRenderPass.hpp"
#include "vklearnin/vulkan/resources/vkImage.hpp"
#include "vklearnin/vulkan/resources/vkImageView.hpp"

namespace vkl {

class vkSurface;
class vkPhysicalDevice;
class vkDevice;

class MSAAPass final {
public:
    MSAAPass() = default;
    ~MSAAPass() = default;

    MSAAPass(MSAAPass &&) = delete;
    MSAAPass(MSAAPass const &) = delete;

    MSAAPass & operator=(MSAAPass &&) = delete;
    MSAAPass & operator=(MSAAPass const &) = delete;

    bool create(vkSurface const &surface,
                vk::Format const depth_format,
                std::span<vk::ClearValue const> const clear_values,
                vk::SampleCountFlagBits const msaa_sample_count,
                vkPhysicalDevice const &physical_device,
                vkDevice const &device);
    bool destroy();

    void update_render_area(vkSurface const &surface);

    void destroy_swapchain_resources();
    void create_swapchain_resources(vkSurface const &surface,
                                    vk::Format const depth_format,
                                    vk::SampleCountFlagBits const msaa_sample_count,
                                    vkPhysicalDevice const &physical_device,
                                    vkDevice const &device);

    inline auto & begin_info() { return _begin_info; }

    inline auto const & render_pass()      const { return _render_pass; }
    inline auto const & multisample_view() const { return _multisample_view; }
    inline auto const & depth_view()       const { return _depth_view; }

private:
    vkRenderPass _render_pass;

    vk::RenderPassBeginInfo _begin_info { };

    std::vector<vk::AttachmentDescription> _attachment_descriptions;

    std::vector<vk::AttachmentReference> _multisample_refs;
    vk::AttachmentReference              _depth_ref   { };
    vk::AttachmentReference              _resolve_ref { };

    std::vector<vk::SubpassDescription> _subpass_descriptions;
    std::vector<vk::SubpassDependency>  _subpass_deps;

    vk::Format _multisample_format { vk::Format::eUndefined };
    vk::Format _depth_format       { vk::Format::eUndefined };

    vk::SampleCountFlagBits _msaa_sample_count { };

    vkImage     _multisample_buffer;
    vkImageView _multisample_view;

    vkImage     _depth_buffer;
    vkImageView _depth_view;

    void _init_attachments();
    void _init_subpasses();

    bool _create_multisample_buffer(vkSurface const &surface,
                                    vkPhysicalDevice const &physical_device,
                                    vkDevice const &device);
    bool _create_depth_buffer(vkSurface const &surface,
                              vkPhysicalDevice const &physical_device,
                              vkDevice const &device);

    void _destroy_multisample_buffer();
    void _destroy_depth_buffer();

    void _reset_object();
};

} // namespace vkl


#endif // VKLEARNIN_RENDERING_PASSES_MSAAPASS_HPP