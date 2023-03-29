#ifndef VKLEARNIN_RESOURCES_IMAGES_IMAGETOOLS_HPP
#define VKLEARNIN_RESOURCES_IMAGES_IMAGETOOLS_HPP

#include "vklearnin/system/pch.hpp"
#include "vklearnin/resources/images/ImageObject.hpp"
#include "vklearnin/materials/Texture2D.hpp"

namespace vkl::ImageTools {

void create(ImageObject &image,
            vk::ImageType const type,
            vk::ImageAspectFlags const aspect_flags,
            vk::SampleCountFlagBits const samples,
            vk::ImageUsageFlags const usage_flags,
            vk::MemoryPropertyFlags const memory_properties,
            vk::ImageCreateFlags const flags = { });

void destroy(ImageObject &image);

void create_view(ImageObject &image,
                 vk::ImageViewType const view_type,
                 vk::ImageAspectFlags const &aspect_flags);

void destroy_view(ImageObject &image);

void * image_from_file(ImageObject &image, std::string_view filepath);
void * cubemap_from_files(ImageObject &image,
                          std::array<std::string_view, 6> const &filepaths);

void free_image_data(void *data);
void free_cubemap_data(void *data);

void host_to_device(ImageObject &dst, void const * const data);

void create_sampler(ImageObject &image,
                    vk::Filter const min_filter,
                    vk::Filter const mag_filter,
                    vk::SamplerMipmapMode const mip_filter,
                    vk::SamplerAddressMode const mode_u,
                    vk::SamplerAddressMode const mode_v,
                    vk::Bool32 const enable_compare,
                    vk::CompareOp const compare_op);

void destroy_sampler(ImageObject &image);

void generate_mipmap(ImageObject &image, vk::Filter const filter);

} // namespace vkl::ImageTools

#endif // VKLEARNIN_RESOURCES_IMAGES_IMAGETOOLS_HPP