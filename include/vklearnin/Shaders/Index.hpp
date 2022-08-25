#ifndef VKL_INDEX_HPP
#define VKL_INDEX_HPP

#include <vulkan/vulkan.h>

#include  <cstdint>

using Index = uint16_t;
static constexpr ::VkIndexType IndexType = ::VK_INDEX_TYPE_UINT16;

#endif // VKL_INDEX_HPP