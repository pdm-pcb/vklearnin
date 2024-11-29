#ifndef VKLEARNIN_VULKAN_PIPELINES_VKRENDERPASS_HPP
#define VKLEARNIN_VULKAN_PIPELINES_VKRENDERPASS_HPP

#include "vklearnin/pch.hpp"

namespace vkl {

class vkDevice;

class vkRenderPass final {
public:
    vkRenderPass() = default;
    ~vkRenderPass() = default;

    vkRenderPass(vkRenderPass &&) = delete;
    vkRenderPass(vkRenderPass const &) = delete;

    vkRenderPass & operator=(vkRenderPass &&) = delete;
    vkRenderPass & operator=(vkRenderPass const &) = delete;

    using Attachments = std::span<vk::AttachmentDescription const>;
    using Subpasses = std::span<vk::SubpassDescription const>;
    using SubpassDeps = std::span<vk::SubpassDependency const>;

    bool create(Attachments const attachments,
                Subpasses const subpasses,
                SubpassDeps const subpass_deps,
                vkDevice const &device);
    bool destroy();

    inline auto const & native() const { return _handle; }

private:
    vk::RenderPass _handle { nullptr };
    vk::Device     _device { nullptr };
};

} // namespace vkl

#endif // VKLEARNIN_VULKAN_PIPELINES_VKRENDERPASS_HPP