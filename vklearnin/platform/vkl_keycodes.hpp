#ifndef VKLEARNIN_PLATFORM_VKL_KEYCODES_HPP
#define VKLEARNIN_PLATFORM_VKL_KEYCODES_HPP

#include "vklearnin/pch.hpp"

namespace vkl {

using Keycode = uint32_t;

static constexpr Keycode VKL_KB_UNKNOWN       = ~0u;

static constexpr Keycode VKL_KB_A             = 0x0041;
static constexpr Keycode VKL_KB_B             = 0x0042;
static constexpr Keycode VKL_KB_C             = 0x0043;
static constexpr Keycode VKL_KB_D             = 0x0044;
static constexpr Keycode VKL_KB_E             = 0x0045;
static constexpr Keycode VKL_KB_F             = 0x0046;
static constexpr Keycode VKL_KB_G             = 0x0047;
static constexpr Keycode VKL_KB_H             = 0x0048;
static constexpr Keycode VKL_KB_I             = 0x0049;
static constexpr Keycode VKL_KB_J             = 0x004a;
static constexpr Keycode VKL_KB_K             = 0x004b;
static constexpr Keycode VKL_KB_L             = 0x004c;
static constexpr Keycode VKL_KB_M             = 0x004d;
static constexpr Keycode VKL_KB_N             = 0x004e;
static constexpr Keycode VKL_KB_O             = 0x004f;
static constexpr Keycode VKL_KB_P             = 0x0050;
static constexpr Keycode VKL_KB_Q             = 0x0051;
static constexpr Keycode VKL_KB_R             = 0x0052;
static constexpr Keycode VKL_KB_S             = 0x0053;
static constexpr Keycode VKL_KB_T             = 0x0054;
static constexpr Keycode VKL_KB_U             = 0x0055;
static constexpr Keycode VKL_KB_V             = 0x0056;
static constexpr Keycode VKL_KB_W             = 0x0057;
static constexpr Keycode VKL_KB_X             = 0x0058;
static constexpr Keycode VKL_KB_Y             = 0x0059;
static constexpr Keycode VKL_KB_Z             = 0x005a;

static constexpr Keycode VKL_KB_ESCAPE        = 0xff1b;
static constexpr Keycode VKL_KB_F1            = 0xffbe;
static constexpr Keycode VKL_KB_F2            = 0xffbf;
static constexpr Keycode VKL_KB_F3            = 0xffc0;
static constexpr Keycode VKL_KB_F4            = 0xffc1;
static constexpr Keycode VKL_KB_F5            = 0xffc2;
static constexpr Keycode VKL_KB_F6            = 0xffc3;
static constexpr Keycode VKL_KB_F7            = 0xffc4;
static constexpr Keycode VKL_KB_F8            = 0xffc5;
static constexpr Keycode VKL_KB_F9            = 0xffc6;
static constexpr Keycode VKL_KB_F10           = 0xffc7;
static constexpr Keycode VKL_KB_F11           = 0xffc8;
static constexpr Keycode VKL_KB_F12           = 0xffc9;

static constexpr Keycode VKL_KB_PRINT_SCREEN  = 0x011b;
static constexpr Keycode VKL_KB_SCROLL_LOCK   = 0x0119;
static constexpr Keycode VKL_KB_PAUSE         = 0x011c;

static constexpr Keycode VKL_KB_INSERT        = 0xff63;
static constexpr Keycode VKL_KB_DELETE        = 0xffff;
static constexpr Keycode VKL_KB_HOME          = 0xff50;
static constexpr Keycode VKL_KB_END           = 0xff57;
static constexpr Keycode VKL_KB_PAGE_UP       = 0xff55;
static constexpr Keycode VKL_KB_PAGE_DOWN     = 0xff56;

static constexpr Keycode VKL_KB_UP            = 0xff52;
static constexpr Keycode VKL_KB_DOWN          = 0xff54;
static constexpr Keycode VKL_KB_LEFT          = 0xff51;
static constexpr Keycode VKL_KB_RIGHT         = 0xff53;

static constexpr Keycode VKL_KB_GRAVE         = 0x0060;

static constexpr Keycode VKL_KB_0             = 0x0030;
static constexpr Keycode VKL_KB_1             = 0x0031;
static constexpr Keycode VKL_KB_2             = 0x0032;
static constexpr Keycode VKL_KB_3             = 0x0033;
static constexpr Keycode VKL_KB_4             = 0x0034;
static constexpr Keycode VKL_KB_5             = 0x0035;
static constexpr Keycode VKL_KB_6             = 0x0036;
static constexpr Keycode VKL_KB_7             = 0x0037;
static constexpr Keycode VKL_KB_8             = 0x0038;
static constexpr Keycode VKL_KB_9             = 0x0039;

static constexpr Keycode VKL_KB_MINUS         = 0x002d;
static constexpr Keycode VKL_KB_EQUAL         = 0x003d;
static constexpr Keycode VKL_KB_BACKSPACE     = 0xff08;

static constexpr Keycode VKL_KB_LEFT_BRACKET  = 0x005b;
static constexpr Keycode VKL_KB_RIGHT_BRACKET = 0x005d;
static constexpr Keycode VKL_KB_BACK_SLASH    = 0x005c;

static constexpr Keycode VKL_KB_SEMICOLON     = 0x003b;
static constexpr Keycode VKL_KB_APOSTROPHE    = 0x0027;
static constexpr Keycode VKL_KB_ENTER         = 0xff0d;

static constexpr Keycode VKL_KB_COMMA         = 0x002c;
static constexpr Keycode VKL_KB_PERIOD        = 0x002e;
static constexpr Keycode VKL_KB_FRONT_SLASH   = 0x002f;

static constexpr Keycode VKL_KB_SPACE         = 0x0020;
static constexpr Keycode VKL_KB_TAB           = 0xff09;
static constexpr Keycode VKL_KB_CAPS_LOCK     = 0x0118;
static constexpr Keycode VKL_KB_NUM_LOCK      = 0x011a;
static constexpr Keycode VKL_KB_LEFT_SHIFT    = 0xffe1;
static constexpr Keycode VKL_KB_RIGHT_SHIFT   = 0xffe2;
static constexpr Keycode VKL_KB_LEFT_CTRL     = 0xffe3;
static constexpr Keycode VKL_KB_RIGHT_CTRL    = 0xffe4;
static constexpr Keycode VKL_KB_LEFT_SUPER    = 0xffe5;
static constexpr Keycode VKL_KB_RIGHT_SUPER   = 0xffe6;
static constexpr Keycode VKL_KB_LEFT_ALT      = 0xffe9;
static constexpr Keycode VKL_KB_RIGHT_ALT     = 0xffea;
static constexpr Keycode VKL_KB_MENU          = 0xff67;

static constexpr Keycode VKL_KB_NP_0          = 0x0140;
static constexpr Keycode VKL_KB_NP_1          = 0x0141;
static constexpr Keycode VKL_KB_NP_2          = 0x0142;
static constexpr Keycode VKL_KB_NP_3          = 0x0143;
static constexpr Keycode VKL_KB_NP_4          = 0x0144;
static constexpr Keycode VKL_KB_NP_5          = 0x0145;
static constexpr Keycode VKL_KB_NP_6          = 0x0146;
static constexpr Keycode VKL_KB_NP_7          = 0x0147;
static constexpr Keycode VKL_KB_NP_8          = 0x0148;
static constexpr Keycode VKL_KB_NP_9          = 0x0149;
static constexpr Keycode VKL_KB_NP_DECIMAL    = 0x014a;
static constexpr Keycode VKL_KB_NP_DIVIDE     = 0x014b;
static constexpr Keycode VKL_KB_NP_MULTIPLY   = 0x014c;
static constexpr Keycode VKL_KB_NP_SUBTRACT   = 0x014d;
static constexpr Keycode VKL_KB_NP_ADD        = 0x014e;
static constexpr Keycode VKL_KB_NP_ENTER      = 0x014f;
static constexpr Keycode VKL_KB_NP_EQUAL      = 0x0150;

static constexpr Keycode VKL_MB_LEFT          = 0x0001;
static constexpr Keycode VKL_MB_MIDDLE        = 0x0002;
static constexpr Keycode VKL_MB_RIGHT         = 0x0003;
static constexpr Keycode VKL_MW_UP            = 0x0004;
static constexpr Keycode VKL_MW_DOWN          = 0x0005;
static constexpr Keycode VKL_MW_LEFT          = 0x0006;
static constexpr Keycode VKL_MW_RIGHT         = 0x0007;
static constexpr Keycode VKL_MB_FORWARD       = 0x0008;
static constexpr Keycode VKL_MB_BACK          = 0x0009;

} // namespace vkl

#endif // VKLEARNIN_PLATFORM_VKL_KEYCODES_HPP