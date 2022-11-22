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
                           const vk::MemoryPropertyFlags memory_properties)
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

    return {
        .buffer     = create_result.value,
        .allocation = VKAllocator::allocate(create_result.value,
                                            memory_properties),
    };
}

// =============================================================================
void destroy_buffer(BufferObject &buffer) {
    CONSOLE_TRACE(
        "Destroying buffer {:#x}, memory {:#x}",
        reinterpret_cast<uint64_t>(VkBuffer(buffer.buffer)),
        reinterpret_cast<uint64_t>(VkDeviceMemory(buffer.allocation.memory))
    );

    VKAllocator::free(buffer.allocation);
    LogicalDevice::native().destroy(buffer.buffer);

    buffer.buffer = nullptr;
}

// =============================================================================
void move_to_device(const void *data, const BufferObject &dest_buffer) {
    // create the staging buffer
    BufferObject staging_buffer = stage_data(dest_buffer.allocation.size, data);
    auto command_buffer = begin_oneshot_cmd_buffer();

    // No offsets for either, full size of the buffer
    vk::BufferCopy copy_regions {
        .srcOffset = 0u,
        .dstOffset = 0u,
        .size = dest_buffer.allocation.size
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
BufferObject stage_data(const size_t size_bytes, const void *data) {
    auto staging_buffer = create_buffer(
        size_bytes,
        vk::BufferUsageFlagBits::eTransferSrc,
        vk::SharingMode::eExclusive,
        (vk::MemoryPropertyFlagBits::eHostVisible | 
         vk::MemoryPropertyFlagBits::eHostCoherent)
    );

    auto result = LogicalDevice::native().mapMemory(
        staging_buffer.allocation.memory,
        0u,
        size_bytes
    );

    if(result.result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL(
            "Could not map device memory {:#x}",
            reinterpret_cast<uint64_t>(
                VkDeviceMemory(staging_buffer.allocation.memory)
            )
        );
    }

    void *destination = result.value;
        memcpy(destination, data, size_bytes);
    LogicalDevice::native().unmapMemory(staging_buffer.allocation.memory);

    return staging_buffer;
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

} // namespace BufferTools
} // namespace vkl