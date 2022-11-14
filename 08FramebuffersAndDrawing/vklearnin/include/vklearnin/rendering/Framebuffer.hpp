#ifndef VKLEARNIN_RENDERING_FRAMEBUFFER_HPP
#define VKLEARNIN_RENDERING_FRAMEBUFFER_HPP

#include "vklearnin/system/pch.hpp"

namespace vkl {
    
class Swapchain;
class Pipeline;

class Framebuffer final {
public:
    void create(uint32_t buffer_idx);
    void destroy();

    inline const auto & native() const { return _framebuffer; }

    Framebuffer(const vk::Device &logical_device, const Swapchain &swapchain,
                 const Pipeline &pipeline);
    ~Framebuffer() = default;

    Framebuffer() = delete;

    Framebuffer(Framebuffer &&other);
    Framebuffer(const Framebuffer &other) = delete;

    Framebuffer & operator=(Framebuffer &&other) = delete;
    Framebuffer & operator=(const Framebuffer &other) = delete;

private:
    vk::Framebuffer            _framebuffer;
    std::vector<vk::ImageView> _attachments;

    const vk::Device &_logical_device;
    const Swapchain  &_swapchain;
    const Pipeline   &_pipeline;
};

} // namespace vkl

#endif // VKLEARNIN_RENDERING_FRAMEBUFFER_HPP