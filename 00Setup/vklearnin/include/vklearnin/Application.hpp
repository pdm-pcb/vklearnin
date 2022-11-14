#ifndef VKLEARNIN_APPLICATION_HPP
#define VKLEARNIN_APPLICATION_HPP

#include "vklearnin/pch.hpp"

namespace vkl {

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
};

} // namespace vkl
#endif // VKLEARNIN_APPLICATION_HPP