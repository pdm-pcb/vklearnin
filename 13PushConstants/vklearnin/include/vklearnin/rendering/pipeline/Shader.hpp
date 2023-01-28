#ifndef VKLEARNIN_RENDERINGPIPELINE_SHADER_HPP
#define VKLEARNIN_RENDERINGPIPELINE_SHADER_HPP

#include "vklearnin/system/pch.hpp"

namespace vkl {

class Shader final {
public:
    void create(std::string_view filepath);
    void destroy();

    inline auto native() const { return _shader; }

    Shader();
    ~Shader() = default;

    Shader(Shader &&) = delete;
    Shader(const Shader &) = delete;

    Shader& operator=(Shader &&) = delete;
    Shader& operator=(const Shader &) = delete;

private:
    vk::ShaderModule _shader;

    static std::vector<uint32_t> _spirv_to_binary(std::string_view filepath);
};

} // namespace vkl

#endif // VKLEARNIN_RENDERINGPIPELINE_SHADER_HPP