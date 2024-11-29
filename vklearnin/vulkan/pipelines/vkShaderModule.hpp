#ifndef VKLEARNIN_VULKAN_PIPELINES_VKSHADERMODULE_HPP
#define VKLEARNIN_VULKAN_PIPELINES_VKSHADERMODULE_HPP

#include "vklearnin/pch.hpp"

namespace vkl {

class vkDevice;

class vkShaderModule final {
public:
    vkShaderModule() = default;
    ~vkShaderModule() = default;

    vkShaderModule(vkShaderModule &&) = delete;
    vkShaderModule(vkShaderModule const &) = delete;

    vkShaderModule& operator=(vkShaderModule &&) = delete;
    vkShaderModule& operator=(vkShaderModule const &) = delete;

    struct SetLayoutData final {
        uint32_t set_number = std::numeric_limits<uint32_t>::max();
        std::vector<vk::DescriptorSetLayoutBinding> bindings;
        vk::DescriptorSetLayoutCreateInfo create_info { };
    };

    bool create(std::string_view const filepath, vkDevice const &device);
    bool destroy();

    inline auto const & native() const { return _handle; }
    inline auto stage() const { return _stage; }
    inline std::string_view const entry_point() const { return _entry_point; }
    inline auto const & vert_input_info() const { return _vert_input_info; }
    inline std::span<SetLayoutData const> const set_layout_data() const {
        return _set_layout_data;
    }

private:
    vk::ShaderModule _handle { nullptr };
    vk::Device       _device { nullptr };

    vk::ShaderStageFlagBits _stage { };
    std::string _entry_point;

    vk::VertexInputBindingDescription _vert_input { };
    std::vector<vk::VertexInputAttributeDescription> _vert_attribs;
    vk::PipelineVertexInputStateCreateInfo _vert_input_info { };

    std::vector<SetLayoutData> _set_layout_data;

    using StringData = std::vector<char>;
    static StringData _spirv_to_string(std::filesystem::path const &filepath);

    using BinaryData = std::vector<uint32_t>;
    static BinaryData _string_to_binary(StringData const &shader_string);

    void _reflect_shader(StringData const &shader_string);
    void _get_stage(::SpvReflectShaderModule const &module);
    void _get_entry_point(::SpvReflectShaderModule const &module);
    void _get_vertex_inputs(::SpvReflectShaderModule const &module);
    void _get_descriptors(::SpvReflectShaderModule const &module);

    static uint32_t _get_format_size(vk::Format const format);
};

} // namespace vkl

#endif // VKLEARNIN_VULKAN_PIPELINES_VKSHADERMODULE_HPP