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

    void update_view_matrix(TargetWindow::InputStates const &input_states);
    void update_ubos(uint32_t const frame_index);

    void bind_descriptor_set(uint32_t const frame_index,
                             vkGraphicsPipeline const &pipeline,
                             uint32_t const descriptor_set_number,
                             vkCmdBuffer const &cmd_buffer);

private:
    struct PerspectiveMats final {
        glm::mat4 view { };
        glm::mat4 proj { };
    } _persp_mats;

    std::vector<vkBuffer> _ubos;
    vkDescriptorSetLayout _descriptor_set_layout;
    std::vector<vkDescriptorSet> _descriptor_sets;
};

} // namespace vkl

#endif // VKLEANRIN_RENDERING_CAMERA_PERSPECTIVECAM_HPP