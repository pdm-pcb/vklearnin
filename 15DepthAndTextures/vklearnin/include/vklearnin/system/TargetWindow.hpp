#ifndef VKLEARNIN_SYSTEM_TARGETWINDOW_HPP
#define VKLEARNIN_SYSTEM_TARGETWINDOW_HPP

#include "vklearnin/system/Win32TargetWindow.hpp"
#include "vklearnin/system/XCBTargetWindow.hpp"

namespace vkl {

#if defined(__linux__)
    using TargetWindow = XCBTargetWindow;
#elif defined(_WIN32)
    using TargetWindow = Win32TargetWindow;
#endif

} // namespace vkl
#endif // VKLEARNIN_SYSTEM_TARGETWINDOW_HPP