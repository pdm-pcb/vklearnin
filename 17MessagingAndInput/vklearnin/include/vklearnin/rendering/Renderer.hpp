#ifndef VKLEARNIN_RENDERING_RENDERER_HPP
#define VKLEARNIN_RENDERING_RENDERER_HPP

#include "vklearnin/system/pch.hpp"

namespace vkl {

class Swapchain;
class Pipeline;

////////////////////////////////////////////////////////////////////////////////
// TODO: Move up to Application/Engine
struct BufferObject;
////////////////////////////////////////////////////////////////////////////////

class Renderer final {
public:
    static void draw(const vk::CommandBuffer &command_buffer,
                     const BufferObject &vertex_buffer,
                     const BufferObject &index_buffer,
                     const uint32_t index_count);

    Renderer() = delete;
    ~Renderer() = delete;

    Renderer(Renderer &&) = delete;
    Renderer(const Renderer &) = delete;

    Renderer & operator=(Renderer &&) = delete;
    Renderer & operator=(const Renderer &) = delete;
};

} // namespace vkl

#endif // VKLEARNIN_RENDERING_RENDERER_HPP