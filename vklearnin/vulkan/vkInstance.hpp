#ifndef VKLEARNIN_VULKAN_VKINSTANCE_HPP
#define VKLEARNIN_VULKAN_VKINSTANCE_HPP

#include "vklearnin/pch.hpp"

#include "vklearnin/vulkan/vkDebugMessenger.hpp"

namespace vkl {

class vkDebugMessenger;

class vkInstance final {
public:
    struct Config final {
        std::vector<char const *> extensions = { };
        bool enable_validation = false;
    };

    vkInstance() = default;
    ~vkInstance() = default;

    vkInstance(vkInstance &&other) = delete;
    vkInstance(vkInstance const &other) = delete;

    vkInstance& operator=(vkInstance &&other) = delete;
    vkInstance& operator=(vkInstance const &other) = delete;

    bool create(Config const &config,
                std::string_view const app_name,
                uint32_t app_version);
    bool destroy();

    inline vk::Instance const & native() const {
        if(!_handle) {
            Log::critical("Invalid Vulkan instance.");
        }

        return _handle;
    }

    inline auto const & loader() const { return _loader; }

private:
    vk::Instance _handle  { nullptr };

    std::string _app_name { };
    std::uint32_t _app_version { 0u };

    vk::detail::DynamicLoader _loader { };
    vk::ApplicationInfo       _app_info { };
    std::vector<char const *> _enabled_layers { };
    std::vector<char const *> _enabled_extensions { };

    std::vector<vk::ValidationFeatureEnableEXT>  _vvl_enabled { };
    std::vector<vk::ValidationFeatureDisableEXT> _vvl_disabled { };
    vk::ValidationFeaturesEXT                    _vvl_features { };

    vkDebugMessenger _debug_messenger { };

    void _init_dynamic_loader();
    void _init_app_info();
    void _init_validation();

    [[nodiscard]] bool _check_layers();
    [[nodiscard]] bool _check_extensions();
};

} // namespace vkl

#endif // VKLEARNIN_VULKAN_VKINSTANCE_HPP