#include "vklearnin/vklearnin.hpp"
#include "vklearnin/rendering/Renderer.hpp"

#include "vklearnin/rendering/GraphicsInstance.hpp"
#include "vklearnin/rendering/LogicalDevice.hpp"
#include "vklearnin/engine/Framebuffer.hpp"
#include "vklearnin/rendering/DeviceQueue.hpp"
#include "vklearnin/engine/Swapchain.hpp"
#include "vklearnin/engine/Pipeline.hpp"

////////////////////////////////////////////////////////////////////////////////
// TODO: Move up to Application/Engine
#include "vklearnin/buffers/BufferObject.hpp"
////////////////////////////////////////////////////////////////////////////////

namespace vkl {

// =============================================================================
void Renderer::draw(const vk::CommandBuffer &command_buffer,
                    const BufferObject &vertex_buffer,
                    const BufferObject &index_buffer,
                    const uint32_t index_count)
{
    // bind the VBO/IBO
    command_buffer.bindVertexBuffers(
        0u,
        { vertex_buffer.buffer },
        { 0u }
    );

    command_buffer.bindIndexBuffer(
        index_buffer.buffer,
        0u,
        vk::IndexType::eUint32
    );

    // draw!
    command_buffer.drawIndexed(
        index_count,
        1u, // instance count
        0u, // first index
        0u, // vertex offset
        0u  // first instance
    );
}

} // namespace vkl