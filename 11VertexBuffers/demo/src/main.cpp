#include "Demo.hpp"

int main() {
    vkl::Application *app = new Demo;
    app->run();
    delete app;

    return 0;
}