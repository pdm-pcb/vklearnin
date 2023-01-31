#include "vklearnin/vklearnin.hpp"
#include "vklearnin/resources/images/ImageTools.hpp"

#include "vklearnin/system/devices/LogicalDevice.hpp"
#include "vklearnin/system/devices/PhysicalDevice.hpp"

namespace vkl::ImageTools {

void _allocate(ImageObject &image,
               const vk::MemoryPropertyFlags memory_properties);

static uint32_t _find_memory_type(const vk::MemoryPropertyFlags flags,
                                  const vk::MemoryRequirements &reqs);

// =============================================================================
void create_image(ImageObject &image,
                  const vk::ImageType type,
                  const vk::Extent3D extent,
                  const vk::SampleCountFlagBits samples,
                  const vk::ImageUsageFlags usage_flags,
                  const vk::MemoryPropertyFlags memory_properties)
{
    if(image.format == vk::Format::eUndefined) {
        CONSOLE_CRITICAL("Cannot create image with undefined format.");
    }

    const vk::ImageCreateInfo image_info {
        .imageType   = type,
        .format      = image.format,
        .extent      = extent,
        .mipLevels   = 1u,
        .arrayLayers = 1u,
        .samples     = samples,
        .tiling      = vk::ImageTiling::eOptimal,
        .usage       = usage_flags
    };

    auto [result, handle] = LogicalDevice::native().createImage(image_info);
    if(result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL(
            "Could not create image: '{}'",
            to_string(result)
        );
    }
    else {
        CONSOLE_TRACE("Created image {:#x}",
                      reinterpret_cast<uint64_t>(::VkImage(handle)));
    }
    
    image.handle = handle;

    _allocate(image, memory_properties);
}

// =============================================================================
void destroy_image(ImageObject &image) {
    CONSOLE_TRACE("Destroying image {:#x}",
                   reinterpret_cast<uint64_t>(::VkImage(image.handle)));

    if(image.view) {
        destroy_view(image);
    }

    LogicalDevice::native().destroyImage(image.handle);
    LogicalDevice::native().freeMemory(image.memory);

    image.handle = nullptr;
    image.memory = nullptr;
}

// =============================================================================
void create_view(ImageObject &image,
                 const vk::ImageViewType view_type,
                 const vk::ImageAspectFlags &aspect_flags) {
    if(!image.handle) {
        CONSOLE_CRITICAL("Cannot create view for non-existant image.");
    }
    if(image.format == vk::Format::eUndefined) {
        CONSOLE_CRITICAL("Cannot create view for image with undefined format.");
    }

    const vk::ImageViewCreateInfo view_info {
        .image    = image.handle,
        .viewType = view_type,
        .format   = image.format,
        .components = {                     
            .r = vk::ComponentSwizzle::eR,  // If color channel values are
            .g = vk::ComponentSwizzle::eG,  // swapped for some reason, these
            .b = vk::ComponentSwizzle::eB,  // paremeters allow us to specify
            .a = vk::ComponentSwizzle::eA,  // which should go where.
        },
        .subresourceRange {
            .aspectMask = aspect_flags, // Aspect flags describe suitable
                                        // interpretations for this image's
                                        // data
            .baseMipLevel   = 0u, // Starting mip level
            .levelCount     = 1u, // Total mip levels
            .baseArrayLayer = 0u, // Starting array layer
            .layerCount     = 1u  // Total array layers
        }
    };

    auto [result, view] = LogicalDevice::native().createImageView(view_info);
    if(result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL(
            "Could not create image view: '{}'",
            to_string(result)
        );
    }
    else {
        CONSOLE_TRACE("Created image view {:#x}",
                      reinterpret_cast<uint64_t>(::VkImageView(view)));
    }
    
    image.view = view;
}

// =============================================================================
void destroy_view(ImageObject &image) {
    CONSOLE_TRACE("Destroying image view {:#x}",
                   reinterpret_cast<uint64_t>(::VkImageView(image.view)));
    LogicalDevice::native().destroyImageView(image.view);

    image.view = nullptr;
}

// =============================================================================
void _allocate(ImageObject &image,
               const vk::MemoryPropertyFlags memory_properties)
{
    vk::MemoryRequirements mem_reqs { };
    LogicalDevice::native().getImageMemoryRequirements(
        image.handle,
        &mem_reqs
    );

    auto type_index = _find_memory_type(memory_properties, mem_reqs);

    const vk::MemoryAllocateInfo alloc_info {
        .allocationSize  = mem_reqs.size,
        .memoryTypeIndex = type_index,
    };

    auto alloc_result = LogicalDevice::native().allocateMemory(alloc_info);
    if(alloc_result.result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL(
            "Failed to allocate {} bytes for image {:#x}: '{}'",
            mem_reqs.size,
            reinterpret_cast<uint64_t>(VkImage(image.handle)),
            to_string(alloc_result.result)
        );
        return;
    }

    CONSOLE_TRACE(
        "\n\tAllocated {} bytes : {:#x}"
        "\n\tFor image {:#x}",
        mem_reqs.size,
        reinterpret_cast<uint64_t>(VkDeviceMemory(alloc_result.value)),
        reinterpret_cast<uint64_t>(VkImage(image.handle))
    );

    image.memory = alloc_result.value;

    auto bind_result = LogicalDevice::native().bindImageMemory(
        image.handle,
        image.memory,
        0u
    );

    if(bind_result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL(
            "Binding attempt failed with '{}' for:"
            "\n\tImage: {:#x}"
            "\n\tMemory: {:#x}",
            to_string(bind_result),
            reinterpret_cast<uint64_t>(VkImage(image.handle)),
            reinterpret_cast<uint64_t>(VkDeviceMemory(image.memory))
        );
    }
}

// =============================================================================
uint32_t _find_memory_type(const vk::MemoryPropertyFlags flags,
                           const vk::MemoryRequirements &reqs)
{
    const auto &memory_properties = PhysicalDevice::memory_props();
    const auto type_count = memory_properties.memoryTypeCount;

    for(uint32_t type_index = 0u; type_index < type_count; ++type_index) {
        if((reqs.memoryTypeBits & (1u << type_index)) != 0u) {
            const auto &props = memory_properties.memoryTypes[type_index];
            if(props.propertyFlags & flags) {
                return type_index;
            }
        }
    }

    CONSOLE_CRITICAL("Could not find memory to match buffer requirements.");
    return std::numeric_limits<uint32_t>::max();
}

} // namespace vkl::ImageTools