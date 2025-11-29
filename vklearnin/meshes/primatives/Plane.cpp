#include "vklearnin/vklearnin.hpp"
#include "vklearnin/meshes/primatives/Plane.hpp"

#include "vklearnin/vulkan/devices/vkPhysicalDevice.hpp"
#include "vklearnin/vulkan/devices/vkDevice.hpp"
#include "vklearnin/vulkan/resources/vkBuffer.hpp"
#include "vklearnin/vulkan/devices/vkCmdBuffer.hpp"

namespace vkl {

std::vector<Vertex> const Plane::_vertices {
    {{ -0.5f,  0.5f, 0.0f, 1.0f }, { 0.0f, 0.0f }},
    {{  0.5f,  0.5f, 0.0f, 1.0f }, { 1.0f, 0.0f }},
    {{  0.5f, -0.5f, 0.0f, 1.0f }, { 1.0f, 1.0f }},
    {{ -0.5f, -0.5f, 0.0f, 1.0f }, { 0.0f, 1.0f }},
};

std::vector<Index::Type> const Plane::_indices {
    0u, 1u, 2u, 0u, 2u, 3u
};

// =============================================================================
bool Plane::create(vkPhysicalDevice const &physical_device,
                   vkDevice const &device)
{
    if(_vertex_buffer.native() || _index_buffer.native()) {
        Log::error(
            "Mesh with vertex buffer {} and index buffer {} alrady exists.",
            _vertex_buffer.native(),
            _index_buffer.native()
        );
        return false;
    }

    if(!physical_device.native()) {
        Log::error("Cannot create plane with invalid physical device.");
        return false;
    }

    if(!device.native()) {
        Log::error("Cannot create plane with invalid device.");
        return false;
    }

    // vertex buffer -----------------------------------------------------------
    if(!_vertex_buffer.create(
        sizeof(Vertex) * _vertices.size(),
        (vk::BufferUsageFlagBits::eVertexBuffer
         | vk::BufferUsageFlagBits::eTransferDst),
        physical_device,
        device
    ))
    {
        Log::error("Failed to create vertex buffer.");
        return false;
    }

    if(!_vertex_buffer.allocate(vk::MemoryPropertyFlagBits::eDeviceLocal)) {
        Log::error("Failed to allocate vertex buffer {}.",
                   _vertex_buffer.native());
        _vertex_buffer.destroy();
        return false;
    }

    vkCmdPool cmd_pool;
    if(!cmd_pool.create(
        device,
        vk::CommandPoolCreateInfo {
            .flags = vk::CommandPoolCreateFlagBits::eTransient,
            .queueFamilyIndex = device.graphics_queue().family_index(),
        })
    )
    {
        Log::error("Failed to create command pool for vertex buffer {} upload",
                   _vertex_buffer.native());
        _vertex_buffer.destroy();
        return false;
    }

    vkCmdBuffer cmd_buffer;
    if(!cmd_buffer.allocate(device, cmd_pool, device.graphics_queue())) {
        Log::error("Failed to create command buffer for vertex buffer {} upload",
                   _vertex_buffer.native());
        cmd_pool.destroy();
        _vertex_buffer.destroy();
        return false;
    }

    if(!_vertex_buffer.send_to_device(
        _vertices.data(),
        cmd_pool,
        device.graphics_queue()
    ))
    {
        Log::error("Failed to send vertices to device.");
        cmd_buffer.free();
        cmd_pool.destroy();
        _vertex_buffer.destroy();
        return false;
    }

    // index buffer -----------------------------------------------------------
    if(!_index_buffer.create(
        sizeof(Index::Type) * _indices.size(),
        (vk::BufferUsageFlagBits::eIndexBuffer
         | vk::BufferUsageFlagBits::eTransferDst),
        physical_device,
        device
    ))
    {
        Log::error("Failed to create index buffer.");
        cmd_buffer.free();
        cmd_pool.destroy();
        _vertex_buffer.destroy();
        return false;
    }

    if(!_index_buffer.allocate(vk::MemoryPropertyFlagBits::eDeviceLocal)) {
        Log::error("Failed to allocate index buffer.");
        cmd_buffer.free();
        cmd_pool.destroy();
        _vertex_buffer.destroy();
        _index_buffer.destroy();
        return false;
    }

    if(!_index_buffer.send_to_device(
        _indices.data(),
        cmd_pool,
        device.graphics_queue()
    ))
    {
        Log::error("Failed to send indices to device.");
        cmd_buffer.free();
        cmd_pool.destroy();
        _vertex_buffer.destroy();
        _index_buffer.destroy();
        return false;
    }

    cmd_buffer.free();
    cmd_pool.destroy();

    return true;
}

// =============================================================================
bool Plane::destroy() {
    if(!_vertex_buffer.native() || !_index_buffer.native()) {
        Log::error("Must create plane before calling destroy.");
        return false;
    }

    _index_buffer.destroy();
    _vertex_buffer.destroy();

    return true;
}

// =============================================================================
bool Plane::bind(vkCmdBuffer const &cmd_buffer) {
    cmd_buffer.native().bindVertexBuffers(
        0u,
        {{ _vertex_buffer.native() }},
        { 0u }
    );

    cmd_buffer.native().bindIndexBuffer(
        _index_buffer.native(),
        0u,
        Index::vulkan_type()
    );

    return true;
}

// =============================================================================
bool Plane::draw(vkCmdBuffer const &cmd_buffer) {
    if(!_vertex_buffer.native()) {
        Log::error("Must create vertex and index buffers before drawing plane.");
        return false;
    }

    if(!cmd_buffer.native()) {
        Log::error("Cannot draw plane with invalid command buffer.");
        return false;
    }

    cmd_buffer.native().drawIndexed(
        static_cast<uint32_t>(_indices.size()),
        1u, // instance count
        0u, // first index
        0u, // vertex offset
        0u  // first instance
    );

    return true;
}

} // namespace vkl