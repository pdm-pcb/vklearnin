#ifndef VKLEARNIN_ENGINE_FRAMEBUFFER_HPP
#define VKLEARNIN_ENGINE_FRAMEBUFFER_HPP

#include "vklearnin/system/pch.hpp"

namespace vkl {

class Swapchain;
class Pipeline;

class Framebuffer final {
public:
    void create(const Swapchain &swapchain, const Pipeline &pipeline,
                const uint32_t image_index);
    void destroy();

    const auto & native() const { return _framebuffer; }

    Framebuffer() = default;
    ~Framebuffer() = default;

    Framebuffer(Framebuffer &&other);
    Framebuffer(const Framebuffer &other) = delete;

    Framebuffer & operator=(Framebuffer &&other) = delete;
    Framebuffer & operator=(const Framebuffer &other) = delete;

private:
    vk::Framebuffer            _framebuffer;
    std::vector<vk::ImageView> _attachments;
};

} // namespace vkl

#endif // VKLEARNIN_ENGINE_FRAMEBUFFER_HPP