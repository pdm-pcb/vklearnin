#include "vklearnin/vklearnin.hpp"
#include "vklearnin/rendering/camera/PerspectiveCam.hpp"

#include "vklearnin/vulkan/resources/vkBuffer.hpp"
#include "vklearnin/vulkan/descriptors/vkDescriptorSet.hpp"

namespace vkl {

// =============================================================================
void PerspectiveCam::create_ubos(std::size_t const swapchain_image_count,
                                 vkDevice const &device)
{
    _ubos.resize(swapchain_image_count);
    for(auto &ubo : _ubos) {
        ubo.create(sizeof(_persp_mats),
                   vk::BufferUsageFlagBits::eUniformBuffer,
                   device);

        ubo.allocate(vk::MemoryPropertyFlagBits::eHostVisible
                     | vk::MemoryPropertyFlagBits::eHostCoherent);
    }
}

// =============================================================================
void PerspectiveCam::create_descriptors(std::size_t const swapchain_image_count,
                                        vkDescriptorPool const &descriptor_pool,
                                        vkDevice const &device)
{
    _descriptor_set_layout
        .add_binding(0u,                                 // binding
                     vk::DescriptorType::eUniformBuffer, // type
                     1u,                                 // descriptor count
                     vk::ShaderStageFlagBits::eVertex)   // stage flags
        .create(device);

    _descriptor_sets.resize(swapchain_image_count);
    for(uint32_t i = 0u; i < _descriptor_sets.size(); ++i) {
        _descriptor_sets[i].allocate(_descriptor_set_layout,
                                     descriptor_pool,
                                     device);

        _descriptor_sets[i]
            .add_update(vk::DescriptorBufferInfo {
                            .buffer = _ubos[i].native(),
                            .offset = 0u,
                            .range = VK_WHOLE_SIZE,
                        },
                        0u,
                        vk::DescriptorType::eUniformBuffer)
            .update();
    }
}

// =============================================================================
void PerspectiveCam::destroy_descriptors() {
    _descriptor_set_layout.destroy();
}

// =============================================================================
void PerspectiveCam::destroy_ubos() {
    for(auto &ubo : _ubos) {
        ubo.destroy();
    }

    _ubos.clear();
}

// =============================================================================
void PerspectiveCam::update_ubos(TargetWindow::InputStates const &input_states,
                                 float const frame_delta,
                                 uint32_t const frame_index)
{
    glm::vec3 velocity { 0.0f, 0.0f, 0.0f };

    if(input_states.w_pressed)     { velocity += _forward(); }
    if(input_states.s_pressed)     { velocity -= _forward(); }
    if(input_states.d_pressed)     { velocity += _right(); }
    if(input_states.a_pressed)     { velocity -= _right(); }
    if(input_states.space_pressed) { velocity += _up(); }
    if(input_states.lctrl_pressed) { velocity -= _up(); }

    if(glm::length2(velocity) > 0.0f) {
        velocity = glm::normalize(velocity);

        auto offset_vector = velocity * frame_delta * _move_speed;
        if(input_states.lshift_pressed) { offset_vector *= _run_multiplier; }

        _position += offset_vector;
    }

    float yaw   = input_states.delta_x * _mouse_sensitivty;
    float pitch = input_states.delta_y * _mouse_sensitivty;
    float roll  = 0.0f;

    if(input_states.q_pressed) { roll -= _roll_speed * frame_delta; }
    if(input_states.e_pressed) { roll += _roll_speed * frame_delta; }

    if(yaw != 0.0f) {
        glm::quat q = glm::angleAxis(yaw, _up());
        _orientation = glm::normalize(q * _orientation);
    }

    if(pitch != 0.0f) {
        glm::quat q = glm::angleAxis(pitch, _right());
        _orientation = glm::normalize(q * _orientation);
    }

    if(roll != 0.0f) {
        glm::quat q = glm::angleAxis(roll, _forward());
        _orientation = glm::normalize(q * _orientation);
    }

    auto const rotation = glm::toMat4(glm::conjugate(_orientation));
    auto const translation = glm::translate(glm::mat4(1.0f), -_position);

    _persp_mats.view = rotation * translation;

    _ubos[frame_index].fill_buffer(&_persp_mats);
}

// =============================================================================
void PerspectiveCam::bind_descriptor_set(uint32_t const frame_index,
                                         vkGraphicsPipeline const &pipeline,
                                         uint32_t const descriptor_set_number,
                                         vkCmdBuffer const &cmd_buffer)
{
    _descriptor_sets[frame_index].bind(
        pipeline,
        descriptor_set_number,
        cmd_buffer
    );
}

} // namespace vkl