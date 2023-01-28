#ifndef DEMO_HPP
#define DEMO_HPP

#include "vklearnin/vklearnin.hpp"

class Demo final : public vkl::Application {
public:
    void init() override;
    void shutdown() override;

    Demo();
    ~Demo() = default;
};

#endif // DEMO_HPP