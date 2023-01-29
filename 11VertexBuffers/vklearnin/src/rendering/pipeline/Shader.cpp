#include "vklearnin/vklearnin.hpp"
#include "vklearnin/rendering/pipeline/Shader.hpp"

#include "vklearnin/system/devices/LogicalDevice.hpp"

namespace vkl {

using Binary = std::vector<uint32_t>;
using String = std::vector<char>;

// =============================================================================
void Shader::create(std::string_view filepath) {
    auto shader_binary = _spirv_to_binary(filepath);
    const vk::ShaderModuleCreateInfo module_info {
        .codeSize = shader_binary.size() * sizeof(uint32_t),
        .pCode = shader_binary.data(),
    };

    auto [result, shader_module] =
        LogicalDevice::native().createShaderModule(module_info);
    if(result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL("Unable to create shader module.");
        return;
    }    

    CONSOLE_TRACE(
        "Shader module {:#x} from '{}'",
        reinterpret_cast<uint64_t>(VkShaderModule(shader_module)),
        filepath
    );

    _shader = shader_module;
}

// =============================================================================
void Shader::destroy() {
    CONSOLE_TRACE(
        "Destroying shader module {:#x}",
        reinterpret_cast<uint64_t>(VkShaderModule(_shader))
    );
    LogicalDevice::native().destroyShaderModule(_shader);
    _shader = nullptr;
}

// =============================================================================
Binary Shader::_spirv_to_binary(std::string_view filepath) {
    // Here we're just accounting for the path and filename differences
    // between debug and release builds
    std::filesystem::path shader_path = ASSET_PATH / filepath.data();
    shader_path += SHADER_EXT;

    // Open the SPIR-V binary file and place the "cursor" at the end
    std::ifstream input_file(
        shader_path.native(),
        std::ios::binary | std::ios::ate
    );

    if(!input_file.good()) {
        CONSOLE_CRITICAL("Unable to open binary '{}'", filepath);
        return Binary { };
    }

    // Since we're already at the end, std::ifstream::tellg() will give us the
    // file's size. Capture that, then seek back to the beginning in order to
    // begin reading the file's content properly.
    auto filesize = static_cast<size_t>(input_file.tellg());
    input_file.seekg(0, std::ifstream::beg);

    // Start by reading in an array of characters
    String shader_string(filesize);
    input_file.read(
        shader_string.data(),
        static_cast<std::streamsize>(filesize)
    );
    input_file.close();

    // Then copy the character array into an integer array to provide Vulkan
    // with the binary data in the size and configuration it expects.
    Binary shader_binary(shader_string.size() / sizeof(uint32_t));
    memcpy(shader_binary.data(), shader_string.data(), shader_string.size());

    return shader_binary;
}

// =============================================================================
Shader::Shader() :
    _shader { }
{ }

} // namespace vkl