#ifndef VKLEANRIN_RENDERING_CAMERA_PERSPECTIVECAM_HPP
#define VKLEANRIN_RENDERING_CAMERA_PERSPECTIVECAM_HPP

#include "vklearnin/pch.hpp"

#include "vklearnin/vulkan/descriptors/vkDescriptorSetLayout.hpp"
#include "vklearnin/vulkan/descriptors/vkDescriptorSet.hpp"
#include "vklearnin/platform/TargetWindow.hpp"

namespace vkl {

class vkDevice;
class vkBuffer;
class vkDescriptorPool;
class vkGraphicsPipeline;
class vkCmdBuffer;

class PerspectiveCam final {
public:
    PerspectiveCam() = default;
    ~PerspectiveCam() = default;

    PerspectiveCam(PerspectiveCam &&) = delete;
    PerspectiveCam(PerspectiveCam const &) = delete;

    PerspectiveCam  & operator=(PerspectiveCam &&) = delete;
    PerspectiveCam  & operator=(PerspectiveCam const &) = delete;

    void create_ubos(std::size_t const swapchain_image_count,
                     vkDevice const &device);

    void create_descriptors(std::size_t const swapchain_image_count,
                            vkDescriptorPool const &descriptor_pool,
                            vkDevice const &device);

    void destroy_descriptors();
    void destroy_ubos();

    void set_view_matrix(glm::mat4 const &in_mat) { _persp_mats.view = in_mat; }
    void set_proj_matrix(glm::mat4 const &in_mat) { _persp_mats.proj = in_mat; }

    auto const & descriptor_set_layout() const { return _descriptor_set_layout; }

    void update_ubos(TargetWindow::InputStates const &input_states,
                     float const frame_delta,
                     uint32_t const frame_index);

    void bind_descriptor_set(uint32_t const frame_index,
                             vkGraphicsPipeline const &pipeline,
                             uint32_t const descriptor_set_number,
                             vkCmdBuffer const &cmd_buffer);

public:
    glm::vec3 _position = { 0.0f, 0.0f, 1.5f };
    glm::quat _orientation { };

    float const _move_speed       { 1.0f };
    float const _run_multiplier   { 2.0f };
    float const _mouse_sensitivty { 0.0025f };
    float const _roll_speed       { 1.0f };

    struct PerspectiveMats final {
        glm::mat4 view { };
        glm::mat4 proj { };
    } _persp_mats;

    std::vector<vkBuffer> _ubos;
    vkDescriptorSetLayout _descriptor_set_layout;
    std::vector<vkDescriptorSet> _descriptor_sets;

    auto const _forward() const { return _orientation * glm::vec3(0.0f, 0.0f, -1.0f); }
    auto const _right()   const { return _orientation * glm::vec3(1.0f, 0.0f, 0.0f); }
    auto const _up()      const { return _orientation * glm::vec3(0.0f, 1.0f, 0.0f); }

    void _translate();
    void _rotate();
};

} // namespace vkl

#endif // VKLEANRIN_RENDERING_CAMERA_PERSPECTIVECAM_HPP