#include "vklearnin/vklearnin.hpp"
#include "vklearnin/tools/BufferTools.hpp"

#include "vklearnin/rendering/PhysicalDevice.hpp"
#include "vklearnin/rendering/LogicalDevice.hpp"

namespace vkl {
namespace BufferTools {

uint32_t find_memory_type(const vk::MemoryPropertyFlags &flags,
                          const vk::MemoryRequirements &mem_reqs);

BufferObject create_buffer(size_t size_bytes,
                           const vk::BufferUsageFlags usage_flags,
                           const vk::SharingMode sharing_mode,
                           const vk::MemoryPropertyFlags memory_flags,
                           const vk::Device &logical_device)
{
    const auto &family_indices = PhysicalDevice::current().family_indices();
    vk::BufferCreateInfo buffer_info {
        .size                  = size_bytes,
        .usage                 = usage_flags,
        .sharingMode           = sharing_mode,
        .queueFamilyIndexCount =
            static_cast<uint32_t>(family_indices.size()),
        .pQueueFamilyIndices   = family_indices.data()
    };

    auto create_result = logical_device.createBuffer(buffer_info);
    if(create_result.result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL("Failed to create {}-byte buffer", size_bytes);
        return { };
    }

    vk::Buffer buffer = create_result.value;
    
    vk::MemoryRequirements mem_reqs;
    logical_device.getBufferMemoryRequirements(buffer, &mem_reqs);
    uint32_t type_index = find_memory_type(memory_flags, mem_reqs);

    vk::MemoryAllocateInfo allocate_info {
        .allocationSize = mem_reqs.size,
        .memoryTypeIndex = type_index
    };

    auto alloc_result = logical_device.allocateMemory(allocate_info);

    if(alloc_result.result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL("Failed to allocate {}-byte buffer", size_bytes);
    }
    
    CONSOLE_TRACE("Allocated {}-byte buffer", size_bytes);

    vk::DeviceMemory device_memory = alloc_result.value;

    auto bind_result =
        logical_device.bindBufferMemory(buffer, device_memory, 0u);

    if(bind_result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL("Unable to bind {}-byte buffer", size_bytes);
    }

    CONSOLE_TRACE(
        "Created buffer {}, device memory {}",
        reinterpret_cast<uint64_t>(VkBuffer(buffer)),
        reinterpret_cast<uint64_t>(VkDeviceMemory(device_memory))
    );

    return {
        .buffer = buffer,
        .memory = device_memory,
        .size   = size_bytes
    };
}

void destroy_buffer(BufferObject &buffer, const LogicalDevice &logical_device) {
    CONSOLE_TRACE(
        "Destroying buffer {}, memory {}",
        reinterpret_cast<uint64_t>(VkBuffer(buffer.buffer)),
        reinterpret_cast<uint64_t>(VkDeviceMemory(buffer.memory))
    );
    logical_device.native().destroy(buffer.buffer);
    logical_device.native().freeMemory(buffer.memory);
}

void fill_buffer(const void *data, const BufferObject &dest_buffer,
                 const LogicalDevice &logical_device)
{
    // create the staging buffer
    BufferObject staging_buffer = create_buffer(
        dest_buffer.size,
        vk::BufferUsageFlagBits::eTransferSrc,
        vk::SharingMode::eExclusive,
        (vk::MemoryPropertyFlagBits::eHostVisible | 
         vk::MemoryPropertyFlagBits::eHostCoherent),
        logical_device.native()
    );

    // map the memory so it's available to write
    auto map_result = logical_device.native().mapMemory(
        staging_buffer.memory,
        0u,
        dest_buffer.size
    );

    if(map_result.result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL(
            "Could not map device memory {}",
            reinterpret_cast<uint64_t>(VkDeviceMemory(staging_buffer.memory))
        );
    }

    // perform the copy
    void *mapped_memory = map_result.value;
        memcpy(mapped_memory, data, staging_buffer.size);
    logical_device.native().unmapMemory(staging_buffer.memory);
    
    // Now that the data is copied into the staging buffer, it's time to copy it
    // once more, to the device local buffer. A command buffer is required for
    // this operation
    vk::CommandBufferAllocateInfo create_info {
        .commandPool = logical_device.default_queue().cmd_pool(),
        .level = vk::CommandBufferLevel::ePrimary,
        .commandBufferCount = 1u,
    };

    vk::CommandBuffer command_buffer;
    auto alloc_result = logical_device.native().allocateCommandBuffers(
        &create_info,
        &command_buffer
    );

    if(alloc_result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL("Unable to allocate command buffer for copying");
    }

    // Fire it up
    vk::CommandBufferBeginInfo begin_info {
        .flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit
    };

    auto cmd_result = command_buffer.begin(begin_info);
    if(cmd_result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL("Failed to begin command buffer for copying");
    }

    // No offsets for either, full size of the buffer
    vk::BufferCopy copy_regions {
        .srcOffset = 0u,
        .dstOffset = 0u,
        .size = dest_buffer.size
    };

    command_buffer.copyBuffer(
        staging_buffer.buffer,
        dest_buffer.buffer,
        copy_regions
    );

    cmd_result = command_buffer.end();
    if(cmd_result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL("Failed to end command buffer for copying");
    }

    vk::SubmitInfo submit_info {
        .commandBufferCount = 1u,
        .pCommandBuffers = &command_buffer
    };

    // Submit and wait
    cmd_result = logical_device.default_queue().native().submit(submit_info);
    if(cmd_result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL("Could not submit command buffer for copying");
    }

    cmd_result = logical_device.default_queue().native().waitIdle();
    if(cmd_result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL("Unable to wait for device idle after buffer copy");
    }

    logical_device.native().freeCommandBuffers(
        logical_device.default_queue().cmd_pool(),
        command_buffer
    );

    destroy_buffer(staging_buffer, logical_device);
}

uint32_t find_memory_type(const vk::MemoryPropertyFlags &flags,
                          const vk::MemoryRequirements &reqs)
{
    vk::PhysicalDeviceMemoryProperties mem_props;
    PhysicalDevice::current().native().getMemoryProperties(&mem_props);

    for(uint32_t type_index = 0;
        type_index < mem_props.memoryTypeCount;
        ++type_index)
    {
        if(reqs.memoryTypeBits & (1 << type_index)) {
            if(mem_props.memoryTypes[type_index].propertyFlags & flags) {
                return type_index;
            }
        }
    }

    return std::numeric_limits<uint32_t>::max();
}

} // namespace BufferTools
} // namespace vkl