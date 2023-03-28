#include "vklearnin/vklearnin.hpp"
#include "vklearnin/resources/buffers/BufferTools.hpp"

#include "vklearnin/system/devices/LogicalDevice.hpp"
#include "vklearnin/system/devices/PhysicalDevice.hpp"
#include "vklearnin/system/devices/CmdBuffer.hpp"

namespace vkl::BufferTools {

void allocate(BufferObject &buffer, const vk::MemoryPropertyFlags flags);

uint32_t find_memory_type(const vk::MemoryPropertyFlags flags,
                          const vk::MemoryRequirements &reqs);

// =============================================================================
void create(BufferObject &buffer,
            const vk::BufferUsageFlags usage_flags,
            const vk::MemoryPropertyFlags memory_properties)
{
    if(buffer.handle) {
        CONSOLE_CRITICAL("Attempting to recreate a buffer object");
        return;
    }

    const vk::BufferCreateInfo buffer_info {
        .size        = buffer.size,
        .usage       = usage_flags,
        .sharingMode = vk::SharingMode::eExclusive,

        // Why should a generic block of memory care about device queue
        // families, you might ask? Provided this structure assures Vulkan we
        // do not intend to share this buffer between threads, the below are
        // simply ignored.
        .queueFamilyIndexCount = 0u,
        .pQueueFamilyIndices   = nullptr,
    };

    buffer.handle = LogicalDevice::native().createBuffer(buffer_info);

    if(!buffer.handle) {
        CONSOLE_CRITICAL("Failed to create {}-byte buffer'", buffer.size);
        return;
    }

    CONSOLE_TRACE(
        "Created buffer {:#x}",
        reinterpret_cast<uint64_t>(VkBuffer(buffer.handle))
    );

    allocate(buffer, memory_properties);
}

// =============================================================================
void destroy(BufferObject &buffer) {
    CONSOLE_TRACE(
        "\n\tDestroying buffer {:#x}"
        "\n\tFreeing device memory {:#x}",
        reinterpret_cast<uint64_t>(VkBuffer(buffer.handle)),
        reinterpret_cast<uint64_t>(VkDeviceMemory(buffer.memory))
    );

    LogicalDevice::native().destroyBuffer(buffer.handle);
    LogicalDevice::native().freeMemory(buffer.memory);

    buffer.handle = nullptr;
    buffer.memory = nullptr;
}

// =============================================================================
BufferObject stage_data(const size_t size, const void * const data) {
    BufferObject staging_buffer {
        .size = size,
    };

    BufferTools::create(
        staging_buffer,
        vk::BufferUsageFlagBits::eTransferSrc,
        (vk::MemoryPropertyFlagBits::eHostVisible |
         vk::MemoryPropertyFlagBits::eHostCoherent)
    );

    auto *mapped = LogicalDevice::native().mapMemory(
        staging_buffer.memory,
        0u,
        staging_buffer.size
    );

    if(mapped == nullptr) {
        CONSOLE_CRITICAL(
            "Unable to map device memory {:#x} for staging buffer",
            reinterpret_cast<uint64_t>(VkDeviceMemory(staging_buffer.memory))
        );
    }
    else {
        memcpy(mapped, data, staging_buffer.size);
        LogicalDevice::native().unmapMemory(staging_buffer.memory);

        CONSOLE_TRACE("Copied {} bytes to staging buffer", staging_buffer.size);
    }

    return staging_buffer;
}

// =============================================================================
void host_to_device(const BufferObject &dst, const void * const data) {
    const vk::BufferCopy copy_region {
        .srcOffset = 0u,
        .dstOffset = 0u,
        .size = dst.size
    };

    auto staging_buffer = stage_data(dst.size, data);

    CmdBuffer cmd_buffer;
    cmd_buffer.allocate(LogicalDevice::transient_pool().native());
    cmd_buffer.begin_one_time_submit();

        cmd_buffer.native().copyBuffer(
            staging_buffer.handle,
            dst.handle,
            copy_region
        );

    cmd_buffer.end_recording();
    cmd_buffer.submit_and_wait_on_device();
    cmd_buffer.free();

    destroy(staging_buffer);

    CONSOLE_TRACE("Copied {} bytes from staging buffer", dst.size);
}

// =============================================================================
void update_buffer(const BufferObject &buffer, const void * const data) {
    auto *mapped = LogicalDevice::native().mapMemory(
        buffer.memory,
        0u,
        buffer.size
    );

    if(mapped == nullptr) {
        CONSOLE_CRITICAL(
            "Unable to map buffer {:#x}",
            reinterpret_cast<uint64_t>(VkBuffer(buffer.handle))
        );
        return;
    }

    memcpy(mapped, data, buffer.size);
    LogicalDevice::native().unmapMemory(buffer.memory);
}

// =============================================================================
void allocate(BufferObject &buffer, const vk::MemoryPropertyFlags flags) {
    // The first order of business is to query the logical device about what
    // available memory matches properties we've specified thus far. A zero-
    // initialized vk::MemoryRequirements structure indicates that the
    // allocation we're after is the whole size of the buffer we've already
    // described with no offset.
    vk::MemoryRequirements mem_reqs { };
    LogicalDevice::native().getBufferMemoryRequirements(
        buffer.handle,
        &mem_reqs
    );

    // This function call will check the joint requirements of ourselves and
    // the logical device against the types of memory offered by the physical
    // device.
    auto type_index = find_memory_type(flags, mem_reqs);

    // Once a suitable memory type (and its index) is located, we're ready to
    // actually allocate the buffer.
    const vk::MemoryAllocateInfo alloc_info {
        .allocationSize  = mem_reqs.size,
        .memoryTypeIndex = type_index,
    };

    buffer.memory = LogicalDevice::native().allocateMemory(alloc_info);

    CONSOLE_TRACE(
        "\n\tAllocated {} bytes: Device memory {:#x}"
        "\n\tFor buffer {:#x}",
        buffer.size,
        reinterpret_cast<uint64_t>(VkDeviceMemory(buffer.memory)),
        reinterpret_cast<uint64_t>(VkBuffer(buffer.handle))
    );

    // Finally,
    LogicalDevice::native().bindBufferMemory(
        buffer.handle,
        buffer.memory,
        0u
    );
}

// =============================================================================
uint32_t find_memory_type(const vk::MemoryPropertyFlags flags,
                          const vk::MemoryRequirements &reqs)
{
    auto const& memory_properties = PhysicalDevice::memory_props();
    auto const type_count = memory_properties.memoryTypeCount;

    // This bit-rithmetic bears some explanation. We're checking two bit fields
    // against our requirements for the memory itself.
    for(uint32_t type_index = 0u; type_index < type_count; ++type_index) {

        // Each type index is actually a field in memoryTypeBits. If the index
        // we're currently on is enabled, that means we've found a matching
        // memory type.
        if((reqs.memoryTypeBits & (1u << type_index)) != 0u) {
            auto const& props = memory_properties.memoryTypes[type_index];

            // The second check is against the memory properties. This can be
            // any combination of local to the CPU, local to the GPU, visible
            // to the CPU or not, and more.
            if(props.propertyFlags & flags) {
                return type_index;
            }
        }
    }

    CONSOLE_CRITICAL("Could not find memory to match buffer requirements.");
    return std::numeric_limits<uint32_t>::max();
}

} // namespace vkl::BufferTools