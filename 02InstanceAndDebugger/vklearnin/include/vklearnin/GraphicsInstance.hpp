#ifndef VKLEARNIN_GRAPHICSINSTANCE_HPP
#define VKLEARNIN_GRAPHICSINSTANCE_HPP

#include "vklearnin/pch.hpp"

namespace vkl {

class GraphicsInstance final {
public:
    void init();

    GraphicsInstance();
    ~GraphicsInstance();

    GraphicsInstance(GraphicsInstance &&other) = delete;
    GraphicsInstance(const GraphicsInstance &other) = delete;

    GraphicsInstance & operator=(GraphicsInstance &&other) = delete;
    GraphicsInstance & operator=(const GraphicsInstance &other) = delete;

private:
    vk::DynamicLoader _loader; // thanks to vulkan-hpp, we can automate a ton
    vk::Instance      _graphics_instance;
};

} // namespace vkl
#endif // VKLEARNIN_GRAPHICSINSTANCE_HPP