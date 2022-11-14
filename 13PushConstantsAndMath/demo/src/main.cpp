#include "vklearnin/vklearnin.hpp" // rendering library's catch-all header
#include "Demo.hpp"                // user-side code leveraging the library

int main() {
    vkl::Application *app = new Demo;
    app->run();
    delete app;

    return 0;
}