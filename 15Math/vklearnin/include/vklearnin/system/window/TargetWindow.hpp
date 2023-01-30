#ifndef VKLEARNIN_SYSTEM_WINDOW_TARGETWINDOW_HPP
#define VKLEARNIN_SYSTEM_WINDOW_TARGETWINDOW_HPP

#include "vklearnin/system/window/Win32TargetWindow.hpp"
#include "vklearnin/system/window/XCBTargetWindow.hpp"

namespace vkl {

#if defined(VKL_LINUX)
    using TargetWindow = XCBTargetWindow;
#elif defined(VKL_WINDOWS)
    using TargetWindow = Win32TargetWindow;
#endif

} // namespace vkl

#endif // VKLEARNIN_SYSTEM_WINDOW_TARGETWINDOW_HPP