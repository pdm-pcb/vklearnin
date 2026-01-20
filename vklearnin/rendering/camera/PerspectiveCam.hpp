#ifndef VKLEANRIN_RENDERING_CAMERA_PERSPECTIVECAM_HPP
#define VKLEANRIN_RENDERING_CAMERA_PERSPECTIVECAM_HPP

#include "vklearnin/pch.hpp"

#include "vklearnin/vulkan/descriptors/vkDescriptorSetLayout.hpp"
#include "vklearnin/vulkan/descriptors/vkDescriptorSet.hpp"

namespace vkl {

class vkDevice;
class vkBuffer;

class PerspectiveCam final {
public:
    PerspectiveCam() = default;
    ~PerspectiveCam() = default;

    PerspectiveCam(PerspectiveCam &&) = delete;
    PerspectiveCam(PerspectiveCam const &) = delete;

    PerspectiveCam  & operator=(PerspectiveCam &&) = delete;
    PerspectiveCam  & operator=(PerspectiveCam const &) = delete;

    bool create();
    bool destroy();

private:
    glm::mat4 _view_matrix { };
    glm::mat4 _projection_matrix { };

    std::vector<vkBuffer> _ubos;
    vkDescriptorSetLayout camera_descriptor_set_layout;
    std::vector<vkDescriptorSet> camera_descriptor_sets;
};

} // namespace vkl

#endif // VKLEANRIN_RENDERING_CAMERA_PERSPECTIVECAM_HPP