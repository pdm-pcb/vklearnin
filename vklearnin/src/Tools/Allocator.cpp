#include "vklearnin/common.hpp"
#include "vklearnin/Tools/Allocator.hpp"

#include  "vklearnin/Instance.hpp"

#define VMA_IMPLEMENTATION
#include "vk_mem_alloc.h"

::VmaAllocator Allocator::_allocator = nullptr;

void Allocator::init(const Instance &instance) {
    ::VmaAllocatorCreateInfo alloc_info {
        .flags = 0u,
        .physicalDevice = instance.physical_device(),
        .device = instance.logical_device(),
        .preferredLargeHeapBlockSize = 0u,
        .pAllocationCallbacks = nullptr,
        .pDeviceMemoryCallbacks = nullptr,
        .pHeapSizeLimit = nullptr,
        .pVulkanFunctions = nullptr,
        .instance = instance.vulkan_instance(),
        .vulkanApiVersion = VK_API_VER,
#if VMA_EXTERNAL_MEMORY
        .pTypeExternalMemoryHandleTypes = nullptr
#endif
    };

    ::vmaCreateAllocator(&alloc_info, &_allocator);
}

::VmaAllocator & Allocator::allocator() {
    return _allocator;
}

void Allocator::snapshot() {
    static uint32_t count = 0u;

    char *stats;
    ::vmaBuildStatsString(_allocator, &stats, true);

    auto filename = fmt::format("../../vma_stats.{}.json", ++count);

    std::ofstream outfile;
    outfile.open(filename);
    if(!outfile.good()) {
        CONSOLE_CRITICAL("Could not open {}?", filename);
    }
    else {
        CONSOLE_TRACE("Allocator snapshot being written to {}", filename);
    }

    outfile << stats;
    outfile.close();

    ::vmaFreeStatsString(_allocator, stats);
}

void Allocator::shutdown() {
    ::vmaDestroyAllocator(_allocator);
}