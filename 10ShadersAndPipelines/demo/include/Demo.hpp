#ifndef DEMO_HPP
#define DEMO_HPP

#include "vklearnin/vklearnin.hpp"

class Demo final : public vkl::Application {
public:
    void init() override;
    void shutdown() override;

    Demo() = default;
    ~Demo() override = default;

    Demo(Demo &&) = delete;
    Demo(const Demo &) = delete;
    
    Demo& operator=(Demo &&) = delete;
    Demo& operator=(const Demo &) = delete;
};

#endif // DEMO_HPP