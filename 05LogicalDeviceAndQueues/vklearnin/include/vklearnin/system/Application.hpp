#ifndef VKLEARNIN_SYSTEM_APPLICATION_HPP
#define VKLEARNIN_SYSTEM_APPLICATION_HPP

#include "vklearnin/system/pch.hpp"

namespace vkl {

class GraphicsInstance;
class PhysicalDevice;
class LogicalDevice;

class Application {
public:
    void init();
    void run();

    Application();
    ~Application();

    Application(Application &&) = delete;
    Application(const Application &) = delete;

    Application & operator=(Application &&) = delete;
    Application & operator=(const Application &) = delete;

private:
    bool _running;

    GraphicsInstance *_graphics_instance;
    PhysicalDevice   *_physical_device;
    LogicalDevice    *_logical_device;
};

} // namespace vkl
#endif // VKLEARNIN_SYSTEM_APPLICATION_HPP