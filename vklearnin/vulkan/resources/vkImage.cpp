#include "vklearnin/vklearnin.hpp"
#include "vklearnin/vulkan/resources/vkImage.hpp"

#include "vklearnin/vulkan/devices/vkPhysicalDevice.hpp"
#include "vklearnin/vulkan/devices/vkDevice.hpp"
#include "vklearnin/vulkan/devices/vkCmdBuffer.hpp"
#include "vklearnin/vulkan/resources/vkBuffer.hpp"

#include <stb/stb_image.h>

namespace vkl {

// =============================================================================
vkImage::vkImage(vkImage &&other) :
    _handle          { other._handle },
    _memory_handle   { other._memory_handle },
    _physical_device { other._physical_device },
    _device          { other._device },
    _format          { other._format },
    _extent          { other._extent },
    _aspect_flags    { other._aspect_flags },
    _layout          { other._layout },
    _array_layers    { other._array_layers },
    _mip_levels      { other._mip_levels },
    _size_bytes      { other._size_bytes },
    _raw_data        { other._raw_data }
#ifdef VKL_DEBUG
    , _debug_name      { other._debug_name }
#endif // VKL_DEBUG
{
    other._handle          = nullptr;
    other._memory_handle   = nullptr;
    other._physical_device = nullptr;
    other._device          = nullptr;
    other._format          = vk::Format::eUndefined;
    other._extent          = vk::Extent3D { };
    other._aspect_flags    = { };
    other._layout          = vk::ImageLayout::eUndefined;
    other._array_layers    = 0u;
    other._mip_levels      = 0u;
    other._size_bytes      = 0u;
    other._raw_data        = nullptr;

#ifdef VKL_DEBUG
    other._debug_name.clear();
#endif // VKL_DEBUG
}

// =============================================================================
bool vkImage::create(vk::Image const &handle,
                     vk::Format const format,
                     vk::Extent3D const &extent,
                     [[maybe_unused]] vkDevice const &device,
                     [[maybe_unused]] std::string_view const debug_name)
{
    if(_handle) {
        Log::error("Swapchain image {}", _handle);
        return false;
    }

    _handle = handle;

#ifdef VKL_DEBUG
    _debug_name = debug_name;
    auto const result = device.native().setDebugUtilsObjectNameEXT(
        vk::DebugUtilsObjectNameInfoEXT {
            .pNext        = nullptr,
            .objectType   = _handle.objectType,
            .objectHandle = reinterpret_cast<uint64_t>(VkImage(_handle)),
            .pObjectName  = _debug_name.c_str(),
        }
    );

    if(result != vk::Result::eSuccess) {
        Log::error(
            "Unable to set image {} debug name: '{}'",
            _handle,
            vk::to_string(result)
        );
        return false;
    }
#endif // VKL_DEBUG

    _format = format;
    _extent = extent;
    _aspect_flags = vk::ImageAspectFlagBits::eColor;

    return true;
}

// =============================================================================
bool vkImage::create(std::string_view const file_name,
                     Details const &details,
                     vkPhysicalDevice const &physical_device,
                     vkDevice const &device,
                     [[maybe_unused]] std::string_view const debug_name)
{
    if(_handle) {
        Log::error("Image  {} already exists", _handle);
        return false;
    }

    if(!physical_device.native()) {
        Log::error("Cannot create image with invalid physical device.");
        return false;
    }

    if(!device.native()) {
        Log::error("Cannot create image with invalid device.");
        return false;
    }

    _physical_device = &physical_device;
    _device = &device;
    _aspect_flags = details.aspect_flags;
    _array_layers = details.array_layers;

    _raw_data = _load_from_file(file_name);

    if(details.generate_mips) {
        _calc_mip_levels();
    }

    Log::trace(
        "\nCreating image with"
        "\n    Size:         {} bytes"
        "\n    Extent:       {}x{}x{}"
        "\n    Array Layers: {}",
        _size_bytes,
        _extent.width, _extent.height, _extent.depth,
        _array_layers
    );

    vk::ImageCreateInfo const create_info {
        .pNext                 = nullptr,
        .flags                 = { },
        .imageType             = details.type,
        .format                = _format,
        .extent                = _extent,
        .mipLevels             = _mip_levels,
        .arrayLayers           = _array_layers,
        .samples               = details.samples,
        .tiling                = vk::ImageTiling::eOptimal,
        .usage                 = details.usage_flags,
        .sharingMode           = vk::SharingMode::eExclusive,
        .queueFamilyIndexCount = 0u,
        .pQueueFamilyIndices   = nullptr,
        .initialLayout         = vk::ImageLayout::eUndefined,
    };

    auto const [ result, value ] = _device->native().createImage(create_info);
    if(result != vk::Result::eSuccess) {
        Log::error(
            "Unable to create image from file '{:s}': '{}'",
            file_name.data(),
            vk::to_string(result)
        );
        return false;
    }

    _handle = value;
    Log::trace("Created image {} from file '{}'.", _handle, file_name.data());

#ifdef VKL_DEBUG
    _debug_name = debug_name;
    auto const debug_name_result = device.native().setDebugUtilsObjectNameEXT(
        vk::DebugUtilsObjectNameInfoEXT {
            .pNext        = nullptr,
            .objectType   = _handle.objectType,
            .objectHandle = reinterpret_cast<uint64_t>(VkImage(_handle)),
            .pObjectName  = _debug_name.c_str(),
        }
    );

    if(debug_name_result != vk::Result::eSuccess) {
        Log::error(
            "Unable to set image {} debug name: '{}'",
            _handle,
            vk::to_string(debug_name_result)
        );
        return false;
    }
#endif // VKL_DEBUG

    Log::trace("Image {}: {} mip levels.", _handle, _mip_levels);

    if(!_allocate(details.memory_flags)) {
        destroy();
        return false;
    }

    if(!_send_to_device()) {
        destroy();
        return false;
    }

    return true;
}

// =============================================================================
bool vkImage::create(vk::Extent2D const &extent,
                     vk::Format const format,
                     Details const &details,
                     vkPhysicalDevice const &physical_device,
                     vkDevice const &device,
                     [[maybe_unused]] std::string_view const debug_name)
{
    if(_handle) {
        Log::error("Image {} already exists", _handle);
        return false;
    }

    if(!device.native()) {
        Log::error("Cannot create image with invalid device.");
        return false;
    }

    if(!physical_device.native()) {
        Log::error("Cannot create image with invalid physical device.");
        return false;
    }

    _physical_device = &physical_device;
    _device = &device;
    _aspect_flags = details.aspect_flags;
    _array_layers = details.array_layers;

    _format = format;
    _extent = vk::Extent3D {
        .width = extent.width,
        .height = extent.height,
        .depth = 1u
    };

    vk::ImageCreateInfo const create_info {
        .pNext = nullptr,
        .flags = { },
        .imageType = details.type,
        .format = _format,
        .extent = _extent,
        .mipLevels = _mip_levels,
        .arrayLayers = _array_layers,
        .samples = details.samples,
        .tiling = vk::ImageTiling::eOptimal,
        .usage = details.usage_flags,
        .sharingMode = vk::SharingMode::eExclusive,
        .queueFamilyIndexCount = 0u,
        .pQueueFamilyIndices = nullptr,
        .initialLayout = vk::ImageLayout::eUndefined,
    };

    auto const [ result, value ] = _device->native().createImage(create_info);

    if(result != vk::Result::eSuccess) {
        Log::error("Unable to create image: '{}'", vk::to_string(result));

        _physical_device = nullptr;
        _device = nullptr;
        _aspect_flags = { };
        _array_layers = 0u;
        _format = { };
        _extent = vk::Extent3D { };

        return false;
    }

    _handle = value;

    Log::trace(
        "Created image {} with extent {}x{}, samples {}",
        _handle,
        extent.width, extent.height,
        vk::to_string(details.samples)
    );

#ifdef VKL_DEBUG
    _debug_name = debug_name;
    auto const debug_name_result = device.native().setDebugUtilsObjectNameEXT(
        vk::DebugUtilsObjectNameInfoEXT {
            .pNext        = nullptr,
            .objectType   = _handle.objectType,
            .objectHandle = reinterpret_cast<uint64_t>(VkImage(_handle)),
            .pObjectName  = _debug_name.c_str(),
        }
    );

    if(debug_name_result != vk::Result::eSuccess) {
        Log::error(
            "Unable to set image {} debug name: '{}'",
            _handle,
            vk::to_string(debug_name_result)
        );

        destroy();
        return false;
    }
#endif // VKL_DEBUG

    if(!_allocate(details.memory_flags)) {
        destroy();
        return false;
    }

    return true;
}

// =============================================================================
bool vkImage::destroy() {
    if(!_handle) {
        Log::error("Must create image before calling destroy.");
        return false;
    }

    Log::trace("Destroying image {}", _handle);
    _device->native().destroy(_handle);

    _handle = nullptr;

#ifdef VKL_DEBUG
    _debug_name.clear();
#endif // VKL_DEBUG

    if(_memory_handle) {
        Log::trace(
            "Freeing memory {} for image {}.",
            _memory_handle,
            _handle
        );

        _device->native().freeMemory(_memory_handle);
        _memory_handle = nullptr;
    }

    _physical_device = nullptr;
    _device = nullptr;

    _format = vk::Format::eUndefined;
    _extent = vk::Extent3D { };
    _aspect_flags = { };

    _layout = vk::ImageLayout::eUndefined;

    _array_layers = 1u;
    _mip_levels = 1u;

    _size_bytes = 0u;
    _raw_data = nullptr;

    return true;
}

// =============================================================================
void vkImage::transition_layout(vkCmdBuffer const &cmd_buffer,
                                TransitionDetails const &details)
{
    vk::ImageMemoryBarrier barrier {
        .pNext = nullptr,
        .srcAccessMask = details.src_access,
        .dstAccessMask = details.dst_access,
        .oldLayout = details.old_layout,
        .newLayout = details.new_layout,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .image = _handle,
        .subresourceRange {
            .aspectMask     = details.aspect_flags,
            .baseMipLevel   = details.base_mip_level,
            .levelCount     = details.mip_level_count,
            .baseArrayLayer = details.base_array_layer,
            .layerCount     = details.array_layer_count,
        }
    };

    // Log::trace(
    //     "Image {} - {:s}->{:s} aspect {:s}"
    //     "\n\tsrcStage  = {:s}"
    //     "\n\tdstStage  = {:s}"
    //     "\n\tsrcAccess = {:s}"
    //     "\n\tdstAccess = {:s}",
    //     _handle,
    //     vk::to_string(details.old_layout),
    //     vk::to_string(details.new_layout),
    //     vk::to_string(details.aspect_flags),
    //     vk::to_string(details.src_stage),
    //     vk::to_string(details.dst_stage),
    //     vk::to_string(barrier.srcAccessMask),
    //     vk::to_string(barrier.dstAccessMask)
    // );

    cmd_buffer.native().pipelineBarrier(
        details.src_stage, // Source stage
        details.dst_stage, // Destination stage
        { },               // Dependency flags
        nullptr,           // Memory barriers
        nullptr,           // Buffer memory barriers
        {{ barrier }}      // Image memory barriers
    );

    _layout = barrier.newLayout;
}

// =============================================================================
void * vkImage::_load_from_file(std::string_view const file_name) {
    auto const file_path = VKL_ASSET_PATH / file_name;

    int width = 0;
    int height = 0;
    int channels = 0;

    ::stbi_uc *data = ::stbi_load(
        file_path.string().c_str(),
        &width, &height, &channels,
        ::STBI_rgb_alpha
    );

    if(data == nullptr) {
        Log::error(
            "Failed to load image '{}'"
            "\n    {}x{} @ {}bpc"
            "\n    Error: '{}'",
             file_path.string(),
             width, height, channels,
             ::stbi_failure_reason()
        );
    }
    else {
        Log::trace("Loaded image {}", file_path.string());

        _extent.width  = static_cast<uint32_t>(width);
        _extent.height = static_cast<uint32_t>(height);
        _extent.depth  = 1u;

        channels = static_cast<uint32_t>(::STBI_rgb_alpha);
        _size_bytes = _extent.width * _extent.height *
                      static_cast<uint32_t>(channels);
        _format = vk::Format::eR8G8B8A8Unorm;
    }

    return data;
}

// =============================================================================
void vkImage::_calc_mip_levels() {
    auto const longest_side = std::max(static_cast<float>(_extent.width),
                                       static_cast<float>(_extent.height));

    _mip_levels =
        static_cast<uint32_t>(std::floor(std::log2(longest_side))) + 1u;
}

// =============================================================================
bool vkImage::_allocate(vk::MemoryPropertyFlags const memory_flags) {
    vk::MemoryRequirements mem_reqs { };

    _device->native().getImageMemoryRequirements(_handle, &mem_reqs);

    auto type_index = _memory_type_index(
        _physical_device->native().getMemoryProperties(),
        memory_flags,
        mem_reqs
    );

    vk::MemoryAllocateInfo const alloc_info {
        .allocationSize  = mem_reqs.size,
        .memoryTypeIndex = type_index,
    };

    auto [ result, value ] = _device->native().allocateMemory(alloc_info);
    if(result != vk::Result::eSuccess) {
        Log::error(
            "Unable to allocate device memory for image {} - '{}'",
            _handle,
            vk::to_string(result)
        );
        return false;
    }

    _memory_handle = value;
    Log::trace(
        "Allocated {} bytes as {} for image {}",
        mem_reqs.size,
        _memory_handle,
        _handle
    );

    result = _device->native().bindImageMemory(_handle, _memory_handle, 0u);

    if(result != vk::Result::eSuccess) {
        Log::error(
            "Unable to bind device memory {} for image {}",
            _memory_handle,
            _handle
        );
        return false;
    }

    Log::trace("Bound memory {} for image {}", _memory_handle, _handle);
    return true;
}

// =============================================================================
bool vkImage::_send_to_device() {
    vkBuffer staging_buffer;
    if(!staging_buffer.create(
        _size_bytes,
        vk::BufferUsageFlagBits::eTransferSrc,
        *_physical_device,
        *_device
    ))
    {
        Log::error("Failed to create staging buffer for image {}", _handle);
        return false;
    }

    if(!staging_buffer.allocate(vk::MemoryPropertyFlagBits::eHostVisible
                                | vk::MemoryPropertyFlagBits::eHostCoherent))
    {
        Log::error("Failed to allocate staging buffer for image {}", _handle);
        staging_buffer.destroy();
        return false;
    }

    if(!staging_buffer.fill_buffer(_raw_data)) {
        Log::error("Failed to fill staging buffer for image {}", _handle);
        staging_buffer.destroy();
        return false;
    }

    vk::BufferImageCopy const copy_region {
        .bufferOffset      = 0u,
        .bufferRowLength   = 0u,
        .bufferImageHeight = 0u,
        .imageSubresource {
            .aspectMask     = _aspect_flags,
            .mipLevel       = 0u,
            .baseArrayLayer = 0u,
            .layerCount     = _array_layers,
        },
        .imageOffset {
            .x = 0,
            .y = 0,
            .z = 0
        },
        .imageExtent = _extent
    };

    vkCmdPool cmd_pool;
    if(!cmd_pool.create(
        *_device,
        vk::CommandPoolCreateInfo {
            .flags = vk::CommandPoolCreateFlagBits::eTransient,
            .queueFamilyIndex = _device->graphics_queue().family_index(),
        })
    )
    {
        Log::error("Failed to create command pool for image {}", _handle);
        staging_buffer.destroy();
        return false;
    }

    vkCmdBuffer cmd_buffer;
    if(!cmd_buffer.allocate(*_device, cmd_pool, _device->graphics_queue())) {
        Log::error("Failed to create command buffer for image {}", _handle);
        cmd_pool.destroy();
        staging_buffer.destroy();
        return false;
    }

    if(!cmd_buffer.begin_one_time_submit()) {
        Log::error("Failed to begin one-time-submit for image {}", _handle);
        cmd_buffer.free();
        cmd_pool.destroy();
        staging_buffer.destroy();
        return false;
    }

        transition_layout(
            cmd_buffer,
            TransitionDetails {
                .old_layout = vk::ImageLayout::eUndefined,
                .new_layout = vk::ImageLayout::eTransferDstOptimal,
                .aspect_flags = vk::ImageAspectFlagBits::eColor,
                .src_stage = vk::PipelineStageFlagBits::eTopOfPipe,
                .dst_stage = vk::PipelineStageFlagBits::eTransfer,
                .src_access = vk::AccessFlagBits::eNone,
                .dst_access = vk::AccessFlagBits::eTransferWrite,
                .base_mip_level = 0u,
                .mip_level_count = _mip_levels,
                .base_array_layer = 0u,
                .array_layer_count = _array_layers,
            }
        );

        cmd_buffer.native().copyBufferToImage(
            staging_buffer.native(),
            _handle,
            _layout,
            copy_region
        );

        _generate_mipmaps(cmd_buffer, vk::Filter::eLinear);

    cmd_buffer.end_recording();

    auto const submit_success = _device->graphics_queue().submit(
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
    cmd_pool.destroy();
    staging_buffer.destroy();

    ::stbi_image_free(_raw_data);
    _raw_data = nullptr;

    return submit_success;
}

// =============================================================================
void vkImage::_generate_mipmaps(vkCmdBuffer const &cmd_buffer,
                                vk::Filter const filter)
{
    for(uint32_t layer = 0u; layer < _array_layers; ++layer) {
        Log::trace("Processing array layer {}", layer);

        int32_t mip_width  = static_cast<int32_t>(_extent.width);
        int32_t mip_height = static_cast<int32_t>(_extent.height);

        for(uint32_t mip = 1u; mip < _mip_levels; ++mip) {
            Log::trace("Generating mip level {}", mip);

            transition_layout(
                cmd_buffer,
                TransitionDetails {
                    .old_layout = vk::ImageLayout::eTransferDstOptimal,
                    .new_layout = vk::ImageLayout::eTransferSrcOptimal,
                    .aspect_flags = vk::ImageAspectFlagBits::eColor,
                    .src_stage = vk::PipelineStageFlagBits::eTransfer,
                    .dst_stage = vk::PipelineStageFlagBits::eTransfer,
                    .src_access = vk::AccessFlagBits::eTransferWrite,
                    .dst_access = vk::AccessFlagBits::eTransferRead,
                    .base_mip_level = mip - 1u,
                    .mip_level_count = 1u,
                    .base_array_layer = layer,
                    .array_layer_count = 1u
                }
            );

            vk::ImageBlit const blit {
                .srcSubresource {
                    .aspectMask     = _aspect_flags,
                    .mipLevel       = mip - 1,
                    .baseArrayLayer = layer,
                    .layerCount     = 1u,
                },
                .srcOffsets = std::array<vk::Offset3D, 2> {
                    vk::Offset3D { 0, 0, 0 },
                    vk::Offset3D { mip_width, mip_height, 1 }
                },
                .dstSubresource {
                    .aspectMask     = _aspect_flags,
                    .mipLevel       = mip,
                    .baseArrayLayer = layer,
                    .layerCount     = 1u,
                },
                .dstOffsets = std::array<vk::Offset3D, 2> {
                    vk::Offset3D { 0, 0, 0 },
                    vk::Offset3D {
                        (mip_width  > 1 ? mip_width  / 2 : 1),
                        (mip_height > 1 ? mip_height / 2 : 1),
                        1
                    }
                },
            };

            cmd_buffer.native().blitImage(
                _handle, vk::ImageLayout::eTransferSrcOptimal,
                _handle, vk::ImageLayout::eTransferDstOptimal,
                { blit },
                filter
            );

            transition_layout(
                cmd_buffer,
                TransitionDetails {
                    .old_layout = vk::ImageLayout::eTransferSrcOptimal,
                    .new_layout = vk::ImageLayout::eShaderReadOnlyOptimal,
                    .aspect_flags = vk::ImageAspectFlagBits::eColor,
                    .src_stage = vk::PipelineStageFlagBits::eTransfer,
                    .dst_stage = vk::PipelineStageFlagBits::eFragmentShader,
                    .src_access = vk::AccessFlagBits::eTransferRead,
                    .dst_access = vk::AccessFlagBits::eShaderRead,
                    .base_mip_level = mip - 1u,
                    .mip_level_count = 1u,
                    .base_array_layer = layer,
                    .array_layer_count = 1u
                }
            );

            if(mip_width  > 1) { mip_width  /= 2; }
            if(mip_height > 1) { mip_height /= 2; }
        }

        transition_layout(
            cmd_buffer,
            TransitionDetails {
                .old_layout = vk::ImageLayout::eTransferDstOptimal,
                .new_layout = vk::ImageLayout::eShaderReadOnlyOptimal,
                .aspect_flags = vk::ImageAspectFlagBits::eColor,
                .src_stage = vk::PipelineStageFlagBits::eTransfer,
                .dst_stage = vk::PipelineStageFlagBits::eFragmentShader,
                .src_access = vk::AccessFlagBits::eTransferWrite,
                .dst_access = vk::AccessFlagBits::eShaderRead,
                .base_mip_level = _mip_levels - 1u,
                .mip_level_count = 1u,
                .base_array_layer = layer,
                .array_layer_count = 1u
            }
        );
    }
}

// =============================================================================
uint32_t vkImage::_memory_type_index(
        vk::PhysicalDeviceMemoryProperties const &memory_props,
        vk::MemoryPropertyFlags const flags,
        vk::MemoryRequirements const reqs)
{
    // This bit-rithmetic bears some explanation. We're checking two bit fields
    // against our requirements for the memory itself.

    for(uint32_t i = 0u; i < memory_props.memoryTypeCount; ++i) {
        auto const type = memory_props.memoryTypes[i];

        // Each type index is actually a field in memoryTypeBits. If the index
        // we're currently on is enabled, that means we've found a matching
        // memory type.

        if((reqs.memoryTypeBits & (1u << i)) != 0u) {
            // The second check is against the memory properties. This can be
            // any combination of local to the CPU, local to the GPU, visible
            // to the CPU or not, and more.

            if(type.propertyFlags & flags) {
                return i;
            }
        }
    }

    Log::error("Could not find memory to match image requirements.");
    return std::numeric_limits<uint32_t>::max();
}

} // namespace vkl