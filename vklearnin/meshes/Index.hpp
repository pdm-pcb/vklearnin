#ifndef VKLEARNIN_MESHES_INDEX_HPP
#define VKLEARNIN_MESHES_INDEX_HPP

#include "vklearnin/pch.hpp"

namespace vkl {

struct Index final {
    using Type = uint32_t;
    [[nodiscard]] static inline auto vulkan_type() { return vk::IndexType::eUint32; }
};

}

#endif // VKLEARNIN_MESHES_INDEX_HPP