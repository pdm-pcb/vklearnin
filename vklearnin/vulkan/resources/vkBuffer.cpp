#include "vklearnin/vklearnin.hpp"
#include "vklearnin/vulkan/resources/vkBuffer.hpp"

#include "vklearnin/vulkan/devices/vkPhysicalDevice.hpp"
#include "vklearnin/vulkan/devices/vkDevice.hpp"
#include "vklearnin/vulkan/devices/vkCmdBuffer.hpp"

namespace vkl {

// =============================================================================
vkBuffer::vkBuffer(vkBuffer &&other) :
    _handle          { other._handle },
    _memory_handle   { other._memory_handle },
    _size_bytes      { other._size_bytes },
    _physical_device { other._physical_device },
    _device          { other._device }
{
    other._handle          = nullptr;
    other._memory_handle   = nullptr;
    other._size_bytes      = 0u;
    other._physical_device = nullptr;
    other._device          = nullptr;
}

// =============================================================================
bool vkBuffer::create(vk::DeviceSize size_bytes,
                      vk::BufferUsageFlags const usage_flags,
                      vkPhysicalDevice const &physical_device,
                      vkDevice const &device)
{
    if(_handle) {
        Log::error("Buffer {} already exists", _handle);
        return false;
    }

    if(!physical_device.native()) {
        Log::error("Cannot create buffer with invalid physical device.");
        return false;
    }

    if(!device.native()) {
        Log::error("Cannot create buffer with invalid device.");
        return false;
    }

    _device = &device;
    _physical_device = &physical_device;
    _size_bytes = size_bytes;

    vk::BufferCreateInfo const create_info {
        .size        = _size_bytes,
        .usage       = usage_flags,
        .sharingMode = vk::SharingMode::eExclusive,

        // Why should a generic block of memory care about device queue
        // families, you might ask? Provided this structure assures Vulkan we
        // do not intend to share this buffer between threads, the below are
        // simply ignored.
        .queueFamilyIndexCount = 0u,
        .pQueueFamilyIndices   = nullptr,
    };

    _handle = _device->native().createBuffer(create_info);
    Log::trace("Created buffer {}", _handle);

    return true;
}

// =============================================================================
bool vkBuffer::destroy() {
    if(!_handle) {
        Log::error("Must create buffer before calling destroy.");
        return false;
    }

    Log::trace("Destroying buffer {}", _handle);
    _device->native().destroyBuffer(_handle);
    _handle = nullptr;
    _size_bytes = 0u;

    if(_memory_handle) {
        this->free();
    }

    _device = nullptr;
    _physical_device = nullptr;

    return true;
}

// =============================================================================
bool vkBuffer::allocate(vk::MemoryPropertyFlags const flags) {
    if(_memory_handle) {
        Log::error("Device memory {} for buffer {} already allocated",
                   _memory_handle,
                   _handle);
        return false;
    }

    if(!_handle) {
        Log::error("Must create buffer before calling allocate.");
        return false;
    }

    // The first order of business is to query the logical device about what
    // available memory matches properties we've specified thus far. A zero-
    // initialized vk::MemoryRequirements structure indicates that the
    // allocation we're after is the whole size of the buffer we've already
    // described with no offset.
    vk::MemoryRequirements mem_reqs { };
    _device->native().getBufferMemoryRequirements(_handle, &mem_reqs);

    // This function call will check the joint requirements of ourselves and
    // the logical device against the types of memory offered by the physical
    // device.
    auto const type_index = _get_memory_type_index(
        _physical_device->native().getMemoryProperties(),
        flags,
        mem_reqs
    );

    // Once a suitable memory type (and its index) is located, we're ready to
    // actually allocate the buffer.
    const vk::MemoryAllocateInfo alloc_info {
        .allocationSize  = mem_reqs.size,
        .memoryTypeIndex = type_index,
    };

    _memory_handle = _device->native().allocateMemory(alloc_info);
    Log::trace("Allocated {} bytes for buffer {}: Device memory {}",
               _size_bytes,
               _handle,
               _memory_handle);

    _device->native().bindBufferMemory(_handle, _memory_handle, 0u);

    return true;
}

// =============================================================================
bool vkBuffer::free() {
    if(!_memory_handle) {
        Log::error("Must allocate buffer memory before calling free.");
        return false;
    }

    Log::trace("Freeing device memory {}", _memory_handle);
    _device->native().freeMemory(_memory_handle);
    _memory_handle = nullptr;

    return true;
}

// =============================================================================
bool vkBuffer::fill_buffer(void const *data) const {
    if(!_memory_handle) {
        Log::error("Buffer {} has no allocation; cannot map.", _handle);
        return false;
    }

    void *mapped_memory_handle;

    auto const result = _device->native().mapMemory(
        _memory_handle,
        0,
        VK_WHOLE_SIZE,
        { },
        &mapped_memory_handle
    );

    if(result != vk::Result::eSuccess) {
        Log::error(
            "Unable to map buffer {} memory {}: '{}'",
            _handle,
            _memory_handle,
            vk::to_string(result)
        );

        return false;
    }

    ::memcpy(mapped_memory_handle, data, _size_bytes);

    _device->native().unmapMemory(_memory_handle);

    return true;
}

// =============================================================================
bool vkBuffer::send_to_device(void const *data,
                              vkCmdPool const &cmd_pool,
                              vkQueue const &queue) const
{
    if(!_handle) {
        Log::error("Must allocate buffer before sending to device.");
        return false;
    }

    vkBuffer staging_buffer;
    if(!staging_buffer.create(
            _size_bytes,
            vk::BufferUsageFlagBits::eTransferSrc,
            *_physical_device,
            *_device
    ))
    {
        Log::error(
            "Failed to create staging buffer for buffer {}",
            _handle
        );
        return false;
    }

    if(!staging_buffer.allocate(vk::MemoryPropertyFlagBits::eHostVisible
                                | vk::MemoryPropertyFlagBits::eHostCoherent))
    {
        Log::error(
            "Failed to allocate staging buffer for buffer {}",
            _handle
        );
        staging_buffer.destroy();
        return false;
    }

    if(!staging_buffer.fill_buffer(data)) {
        Log::error(
            "Failed to fill staging buffer for buffer {}",
            _handle
        );
        staging_buffer.destroy();
        return false;
    }

    vk::BufferCopy const copy_region {
        .srcOffset = 0u,
        .dstOffset = 0u,
        .size = _size_bytes
    };

    vkCmdBuffer cmd_buffer;
    if(!cmd_buffer.allocate(*_device, cmd_pool, queue)) {
        Log::error(
            "Failed to allocate command buffer for buffer {}",
            _handle
        );
        staging_buffer.destroy();
        return false;
    }

    if(!cmd_buffer.begin_one_time_submit()) {
        Log::error(
            "Failed to begin one-time-submit for buffer {}",
            _handle
        );
        staging_buffer.destroy();
        cmd_buffer.free();
        return false;
    }

        cmd_buffer.native().copyBuffer(
            staging_buffer.native(),
            _handle,
            copy_region
        );

    cmd_buffer.end_recording();

    auto const submit_success = queue.submit(
        vk::SubmitInfo {
            .pNext                = nullptr,
            .waitSemaphoreCount   = 0u,
            .pWaitSemaphores      = nullptr,
            .pWaitDstStageMask    = nullptr,
            .commandBufferCount   = 1u,
            .pCommandBuffers      = &cmd_buffer.native(),
            .signalSemaphoreCount = 0u,
            .pSignalSemaphores    = nullptr,
        }
    );

    _device->wait_idle();

    cmd_buffer.free();
    staging_buffer.destroy();

    return submit_success;
}

// =============================================================================
uint32_t vkBuffer::_get_memory_type_index(
    vk::PhysicalDeviceMemoryProperties const &properties,
    vk::MemoryPropertyFlags const flags,
    vk::MemoryRequirements const &reqs)
{
    auto const type_count = properties.memoryTypeCount;

    // This bit-rithmetic bears some explanation. We're checking two bit fields
    // against our requirements for the memory itself.

    for(uint32_t type_index = 0u; type_index < type_count; ++type_index) {
        auto const type = properties.memoryTypes[type_index];

        // Each type index is actually a field in memoryTypeBits. If the index
        // we're currently on is enabled, that means we've found a matching
        // memory type.

        if((reqs.memoryTypeBits & (1u << type_index)) != 0u) {
            // The second check is against the memory properties. This can be
            // any combination of local to the CPU, local to the GPU, visible
            // to the CPU or not, and more.

            if(type.propertyFlags & flags) {
                return type_index;
            }
        }
    }

    Log::error("Could not find memory to match buffer requirements.");
    return std::numeric_limits<uint32_t>::max();
}

} // namespace vkl