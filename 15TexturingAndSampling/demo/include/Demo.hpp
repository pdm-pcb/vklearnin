//
// The Demo class is a little playground which is used to prove out the
// rendering library that makes up the bulk of this project.
//
// It's required that at least one class in the user-side code inherits from
// vkl::Application.
//

#ifndef DEMO_HPP
#define DEMO_HPP

#include "vklearnin/vklearnin.hpp"

class Demo final : public vkl::Application {
public:
    Demo();
    ~Demo() = default;
};

#endif // DEMO_HPP