#ifndef VKLEARNIN_PLATFORM_GLFWTOVKLKEYS_HPP
#define VKLEARNIN_PLATFORM_GLFWTOVKLKEYS_HPP

#include "vklearnin/pch.hpp"
#include "vklearnin/platform/vkl_keycodes.hpp"

namespace vkl {

class GLFWToVKLKeys final {
public:
    inline static auto translate(int const key) {
        auto const result = _map.find(key);
        if(result == _map.end()) {
            Log::warn("Unknown GLFW key: {}", key);
            return VKL_KB_UNKNOWN;
        }

        return result->second;
    }

    GLFWToVKLKeys() = delete;
    ~GLFWToVKLKeys() = delete;

    GLFWToVKLKeys(GLFWToVKLKeys &&) = delete;
    GLFWToVKLKeys(GLFWToVKLKeys const &) = delete;

    GLFWToVKLKeys & operator=(GLFWToVKLKeys &&) = delete;
    GLFWToVKLKeys & operator=(GLFWToVKLKeys const &) = delete;

private:
    static std::unordered_map<int, Keycode> const _map;
};

} // namespace vkl

#endif // VKLEARNIN_PLATFORM_GLFWTOVKLKEYS_HPP