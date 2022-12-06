#include "vklearnin/vklearnin.hpp"
#include "vklearnin/tools/BufferTools.hpp"

#include "vklearnin/rendering/devices/PhysicalDevice.hpp"
#include "vklearnin/rendering/devices/LogicalDevice.hpp"
#include "vklearnin/rendering/devices/CmdQueue.hpp"
#include "vklearnin/rendering/devices/CmdPool.hpp"
#include "vklearnin/tools/VKAllocator.hpp"

namespace vkl {
namespace BufferTools {

// =============================================================================
BufferObject create_buffer(const size_t size_bytes,
                           const vk::BufferUsageFlags usage_flags,
                           const vk::SharingMode sharing_mode,
                           const vk::MemoryPropertyFlags memory_properties,
                           std::string_view buffer_name)
{
    const auto &family_indices = PhysicalDevice::family_indices();
    vk::BufferCreateInfo buffer_info {
        .size                  = size_bytes,
        .usage                 = usage_flags,
        .sharingMode           = sharing_mode,
        .queueFamilyIndexCount =
            static_cast<uint32_t>(family_indices.size()),
        .pQueueFamilyIndices   = family_indices.data()
    };

    auto create_result = LogicalDevice::native().createBuffer(buffer_info);
    if(create_result.result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL("Failed to create {}-byte buffer", size_bytes);
        return { };
    }

    assert(create_result.value);

    CONSOLE_TRACE(
        "Created buffer {:#x}, '{}'",
        reinterpret_cast<uint64_t>(VkBuffer(create_result.value)),
        buffer_name
    );

    return {
        .buffer     = create_result.value,
        .allocation = VKAllocator::allocate(create_result.value,
                                            memory_properties,
                                            buffer_name),
    };
}

// =============================================================================
vk::CommandBuffer begin_oneshot_cmd_buffer() {
    vk::CommandBufferAllocateInfo create_info {
        .commandPool = LogicalDevice::cmd_pool().native(),
        .level = vk::CommandBufferLevel::ePrimary,
        .commandBufferCount = 1u,
    };

    vk::CommandBuffer command_buffer;
    auto alloc_result = LogicalDevice::native().allocateCommandBuffers(
        &create_info,
        &command_buffer
    );

    if(alloc_result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL("Unable to allocate one shot command buffer");
    }

    // Fire it up
    vk::CommandBufferBeginInfo begin_info {
        .flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit
    };

    auto cmd_result = command_buffer.begin(begin_info);
    if(cmd_result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL("Failed to begin command buffer for copying");
    }

    return command_buffer;
}

// =============================================================================
void end_oneshot_cmd_buffer(const vk::CommandBuffer &command_buffer) {
    auto cmd_result = command_buffer.end();
    if(cmd_result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL("Failed to end command buffer for copying");
    }

    vk::SubmitInfo submit_info {
        .commandBufferCount = 1u,
        .pCommandBuffers = &command_buffer
    };

    // Submit and wait
    cmd_result = LogicalDevice::cmd_queue().native().submit(submit_info);
    if(cmd_result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL("Could not submit command buffer for copying");
    }

    cmd_result = LogicalDevice::cmd_queue().native().waitIdle();
    if(cmd_result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL("Unable to wait for device idle after buffer copy");
    }

    LogicalDevice::native().freeCommandBuffers(
        LogicalDevice::cmd_pool().native(),
        command_buffer
    );
}

// =============================================================================
BufferObject stage_data(const size_t size_bytes, const void *data) {
    auto staging_buffer = create_buffer(
        size_bytes,
        vk::BufferUsageFlagBits::eTransferSrc,
        vk::SharingMode::eExclusive,
        (vk::MemoryPropertyFlagBits::eHostVisible | 
         vk::MemoryPropertyFlagBits::eHostCoherent),
        "staging buf"
    );

    CONSOLE_WARN("staging buffer created: {:#x}",
        reinterpret_cast<uint64_t>(VkBuffer(staging_buffer.buffer))
    );

    void *destination = VKAllocator::map_buffer(staging_buffer.allocation);

    CONSOLE_WARN("staging buffer mapped: {:#x}",
        reinterpret_cast<uint64_t>(VkBuffer(staging_buffer.buffer))
    );
        memcpy(destination, data, size_bytes);

    CONSOLE_WARN("staging buffer memcpy'd: {:#x}",
        reinterpret_cast<uint64_t>(VkBuffer(staging_buffer.buffer))
    );
    VKAllocator::unmap_buffer(staging_buffer.allocation);

    CONSOLE_WARN("staging buffer unmapped: {:#x}",
        reinterpret_cast<uint64_t>(VkBuffer(staging_buffer.buffer))
    );

    return staging_buffer;
}

// =============================================================================
void move_to_device(const void *data, const BufferObject &dest_buffer) {
    const auto &allocation = dest_buffer.allocation;
    // create the staging buffer
    BufferObject staging_buffer = stage_data(allocation->size, data);
    auto command_buffer = begin_oneshot_cmd_buffer();

    // No offsets for either, full size of the buffer
    vk::BufferCopy copy_regions {
        .srcOffset = 0u,
        .dstOffset = 0u,
        .size = allocation->size
    };

    command_buffer.copyBuffer(
        staging_buffer.buffer,
        dest_buffer.buffer,
        copy_regions
    );

    end_oneshot_cmd_buffer(command_buffer);
    destroy_buffer(staging_buffer);
}

// =============================================================================
void destroy_buffer(BufferObject &buffer) {
    VKAllocator::free(buffer.allocation);
    LogicalDevice::native().destroy(buffer.buffer);

    buffer = { };
}

} // namespace BufferTools
} // namespace vkl