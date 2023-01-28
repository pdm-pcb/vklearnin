#ifndef VKLEARNIN_MESHES_FACE_HPP
#define VKLEARNIN_MESHES_FACE_HPP

#include "vklearnin/system/pch.hpp"

namespace vkl {

static constexpr uint8_t INDICES_PER_FACE = 3u;
static constexpr vk::IndexType INDEX_TYPE = vk::IndexType::eUint32;

class Face final {
public:
    using Index = uint32_t;

    explicit Face(const std::array<Index, INDICES_PER_FACE> &indices);
    Face() = delete;

private:
    std::array<Index, INDICES_PER_FACE> _indices;
};

} // namespace vkl

#endif // VKLEARNIN_MESHES_FACE_HPP