#include "vklearnin/vklearnin.hpp"
#include "vklearnin/resources/images/ImageTools.hpp"

#include "vklearnin/system/devices/LogicalDevice.hpp"
#include "vklearnin/system/devices/PhysicalDevice.hpp"
#include "vklearnin/system/devices/CmdBuffer.hpp"
#include "vklearnin/rendering/swapchain/Swapchain.hpp"

#include <stb/stb_image.h>

namespace vkl::ImageTools {

void allocate(ImageObject &image, const vk::MemoryPropertyFlags flags);

uint32_t find_memory_type(const vk::MemoryPropertyFlags flags,
                          const vk::MemoryRequirements &reqs);

void transition_layout(ImageObject &image,
                       const vk::CommandBuffer &command_buffer,
                       const vk::ImageLayout old_layout,
                       const vk::ImageLayout new_layout);

// =============================================================================
void create(ImageObject &image,
            const vk::ImageType type,
            const vk::SampleCountFlagBits samples,
            const vk::ImageUsageFlags usage_flags,
            const vk::MemoryPropertyFlags memory_properties)
{
    if(image.format == vk::Format::eUndefined) {
        CONSOLE_CRITICAL("Cannot create image with undefined format.");
    }

    const vk::ImageCreateInfo image_info {
        .imageType     = type,
        .format        = image.format,
        .extent        = image.extent,
        .mipLevels     = 1u,
        .arrayLayers   = 1u,
        .samples       = samples,
        .tiling        = vk::ImageTiling::eOptimal,
        .usage         = usage_flags,
        .sharingMode   = vk::SharingMode::eExclusive,
        .initialLayout = vk::ImageLayout::eUndefined,
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

    allocate(image, memory_properties);
}

// =============================================================================
void destroy(ImageObject &image) {
    CONSOLE_TRACE("Destroying image {:#x}",
                   reinterpret_cast<uint64_t>(::VkImage(image.handle)));

    if(image.sampler) {
        destroy_sampler(image);
    }

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
void* load_from_file(ImageObject &image, std::string_view filepath) {
    auto const texture_path = ASSET_PATH / filepath.data();
    const std::string path = texture_path.string();

    int width    = 0;
    int height   = 0;
    int channels = 0;

    ::stbi_uc* data = ::stbi_load(
        path.c_str(),
        &width, &height, &channels,
        ::STBI_rgb_alpha
    );

    if(data == nullptr) {
        CONSOLE_CRITICAL(
            "Failed to load image '{}'\n\t"
            "{}x{} @ {}bpc\n\t"
            "Error: '{}'",
            path, width, height, channels,
            ::stbi_failure_reason()
        );
    }
    else {
        CONSOLE_TRACE("Loaded image {}", filepath);

        image.extent = vk::Extent3D {
            .width  = static_cast<uint32_t>(width),
            .height = static_cast<uint32_t>(height),
            .depth  = 1u
        };

        channels = static_cast<uint32_t>(::STBI_rgb_alpha);
        image.size = image.extent.width * image.extent.height * channels;
        image.format = Swapchain::image_format();
    }

    return data;
}

// =============================================================================
void free_file_data(void *data) {
    ::stbi_image_free(data);
}

// =============================================================================
void host_to_device(ImageObject &dst, const void * const data) {
    const vk::BufferImageCopy copy_region {
        .bufferOffset = 0u,
        .bufferRowLength = 0u,
        .bufferImageHeight = 0u,
        .imageSubresource {
            .aspectMask = vk::ImageAspectFlagBits::eColor,
            .mipLevel = 0u,
            .baseArrayLayer = 0u,
            .layerCount = 1u,
        },
        .imageOffset {
            .x = 0,
            .y = 0,
            .z = 0
        },
        .imageExtent = dst.extent
    };

    auto staging_buffer = BufferTools::stage_data(dst.size, data);

    const vk::CommandBufferBeginInfo begin_info {
        .flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit,
    };

    CmdBuffer cmd_buffer;
    cmd_buffer.allocate(LogicalDevice::transient_pool().native());
    auto result = cmd_buffer.native().begin(&begin_info);
    if(result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL(
            "Could not begin recording for iamge transfer: '{}'",
            to_string(result)
        );
        return;
    }

        transition_layout(
            dst,
            cmd_buffer.native(),
            vk::ImageLayout::eUndefined,
            vk::ImageLayout::eTransferDstOptimal
        );

        cmd_buffer.native().copyBufferToImage(
            staging_buffer.handle,
            dst.handle,
            dst.layout,
            copy_region
        );

        transition_layout(
            dst,
            cmd_buffer.native(),
            vk::ImageLayout::eTransferDstOptimal,
            vk::ImageLayout::eShaderReadOnlyOptimal
        );

    result = cmd_buffer.native().end();
    if(result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL(
            "Failed to end recording for image transfer: '{}'",
            to_string(result)
        );
        return;
    }

    const vk::SubmitInfo submit_info {
        .waitSemaphoreCount   = 0u,
        .pWaitSemaphores      = nullptr,
        .pWaitDstStageMask    = { },
        .commandBufferCount   = 1u,
        .pCommandBuffers      = &(cmd_buffer.native()),
        .signalSemaphoreCount = 0u,
        .pSignalSemaphores    = nullptr,
    };

    result = LogicalDevice::cmd_queue().native().submit(
        submit_info,
        nullptr
    );
    if(result != vk::Result::eSuccess) {
        CONSOLE_ERROR(
            "Could not submit command buffer copy commands: '{}'",
            to_string(result)
        );
        return;
    }

    CONSOLE_TRACE("Copied {} bytes from staging buffer", dst.size);

    result = LogicalDevice::native().waitIdle();
    if(result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL(
            "Failed to wait for device idle after copy from staging buffer: "
            "'{}'",
            to_string(result)
        );
        return;
    }

    cmd_buffer.free();
    BufferTools::destroy(staging_buffer);
}

// =============================================================================
void create_sampler(ImageObject &image,
                    const vk::Filter min_filter,
                    const vk::Filter mag_filter,
                    const vk::SamplerAddressMode mode_u,
                    const vk::SamplerAddressMode mode_v)
{
    const vk::SamplerCreateInfo sampler_info {
        .magFilter        = min_filter,
        .minFilter        = mag_filter,
        .addressModeU     = mode_u,
        .addressModeV     = mode_v
    };

    auto result = LogicalDevice::native().createSampler(sampler_info);
    if(result.result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL("Unable to create image sampler");
        return;
    }

    CONSOLE_TRACE(
        "Created image sampler {:#x}",
                reinterpret_cast<uint64_t>(::VkSampler(result.value))
    );

    image.sampler = result.value;
}

// =============================================================================
void destroy_sampler(ImageObject &image) {
    CONSOLE_TRACE("Destroying image sampler {:#x}",
                   reinterpret_cast<uint64_t>(::VkSampler(image.sampler)));
    LogicalDevice::native().destroySampler(image.sampler);
    image.sampler = nullptr;
}

// =============================================================================
void allocate(ImageObject &image, const vk::MemoryPropertyFlags flags) {
    vk::MemoryRequirements mem_reqs { };
    LogicalDevice::native().getImageMemoryRequirements(
        image.handle,
        &mem_reqs
    );

    auto type_index = find_memory_type(flags, mem_reqs);

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
uint32_t find_memory_type(const vk::MemoryPropertyFlags flags,
                          const vk::MemoryRequirements &reqs)
{
    auto const& memory_properties = PhysicalDevice::memory_props();
    auto const type_count = memory_properties.memoryTypeCount;

    for(uint32_t type_index = 0u; type_index < type_count; ++type_index) {
        if((reqs.memoryTypeBits & (1u << type_index)) != 0u) {
            auto const& props = memory_properties.memoryTypes[type_index];
            if(props.propertyFlags & flags) {
                return type_index;
            }
        }
    }

    CONSOLE_CRITICAL("Could not find memory to match buffer requirements.");
    return std::numeric_limits<uint32_t>::max();
}

// =============================================================================
void transition_layout(ImageObject &image,
                       const vk::CommandBuffer &command_buffer,
                       const vk::ImageLayout old_layout,
                       const vk::ImageLayout new_layout)
{
    vk::ImageMemoryBarrier barrier {
        .oldLayout = old_layout,
        .newLayout = new_layout,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .image = image.handle,
        .subresourceRange {
            .aspectMask     = vk::ImageAspectFlagBits::eColor,
            .baseMipLevel   = 0u,
            .levelCount     = 1u,
            .baseArrayLayer = 0u,
            .layerCount     = 1u,
        }
    };

    CONSOLE_TRACE(
        "Image {:#x}: '{}' -> '{}'",
        reinterpret_cast<uint64_t>(VkImage(image.handle)),
        to_string(barrier.oldLayout),
        to_string(barrier.newLayout)
    );

    vk::PipelineStageFlags src_stage = vk::PipelineStageFlagBits::eNone;
    vk::PipelineStageFlags dst_stage = vk::PipelineStageFlagBits::eNone;

    if(barrier.oldLayout == vk::ImageLayout::eUndefined) {
        if(new_layout == vk::ImageLayout::eTransferDstOptimal) {
            barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;

            src_stage = vk::PipelineStageFlagBits::eTopOfPipe;
            dst_stage = vk::PipelineStageFlagBits::eTransfer;
        }
        else {
            CONSOLE_CRITICAL("Unsupported image layout transition");
            return;
        }
    }
    else if(barrier.oldLayout == vk::ImageLayout::eTransferDstOptimal) {
        if(barrier.newLayout == vk::ImageLayout::eShaderReadOnlyOptimal) {    
            barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
            barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

            src_stage      = vk::PipelineStageFlagBits::eTransfer;
            dst_stage = vk::PipelineStageFlagBits::eFragmentShader;
        }
        else if(barrier.newLayout == vk::ImageLayout::eTransferSrcOptimal) {
            barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
            barrier.dstAccessMask = vk::AccessFlagBits::eTransferRead;

            src_stage      = vk::PipelineStageFlagBits::eTransfer;
            dst_stage = vk::PipelineStageFlagBits::eTransfer;
        }
        else {
            CONSOLE_CRITICAL("Unsupported image layout transition");
            return;
        }
    }
    else if(barrier.oldLayout == vk::ImageLayout::eTransferSrcOptimal) {
        if(barrier.newLayout == vk::ImageLayout::eShaderReadOnlyOptimal) {
            barrier.srcAccessMask = vk::AccessFlagBits::eTransferRead;
            barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

            src_stage      = vk::PipelineStageFlagBits::eTransfer;
            dst_stage = vk::PipelineStageFlagBits::eFragmentShader;
        }
        else {
            CONSOLE_CRITICAL("Unsupported image layout transition");
            return;
        }
    }
    else {
        CONSOLE_CRITICAL("Unsupported image layout transition");
        return;
    }

    command_buffer.pipelineBarrier(
        src_stage,
        dst_stage,
        { },
        nullptr,
        nullptr,
        { barrier }
    );

    image.layout = barrier.newLayout;
}

} // namespace vkl::ImageTools