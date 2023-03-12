#ifndef VKLEARNIN_SYSTEM_INPUT_KEYCODES_HPP
#define VKLEARNIN_SYSTEM_INPUT_KEYCODES_HPP

#include "vklearnin/system/pch.hpp"

namespace vkl {

using Keycode = uint32_t;

constexpr Keycode KB_UNKNOWN       = ~0u;

constexpr Keycode KB_A             = 0x0041;
constexpr Keycode KB_B             = 0x0042;
constexpr Keycode KB_C             = 0x0043;
constexpr Keycode KB_D             = 0x0044;
constexpr Keycode KB_E             = 0x0045;
constexpr Keycode KB_F             = 0x0046;
constexpr Keycode KB_G             = 0x0047;
constexpr Keycode KB_H             = 0x0048;
constexpr Keycode KB_I             = 0x0049;
constexpr Keycode KB_J             = 0x004a;
constexpr Keycode KB_K             = 0x004b;
constexpr Keycode KB_L             = 0x004c;
constexpr Keycode KB_M             = 0x004d;
constexpr Keycode KB_N             = 0x004e;
constexpr Keycode KB_O             = 0x004f;
constexpr Keycode KB_P             = 0x0050;
constexpr Keycode KB_Q             = 0x0051;
constexpr Keycode KB_R             = 0x0052;
constexpr Keycode KB_S             = 0x0053;
constexpr Keycode KB_T             = 0x0054;
constexpr Keycode KB_U             = 0x0055;
constexpr Keycode KB_V             = 0x0056;
constexpr Keycode KB_W             = 0x0057;
constexpr Keycode KB_X             = 0x0058;
constexpr Keycode KB_Y             = 0x0059;
constexpr Keycode KB_Z             = 0x005a;

constexpr Keycode KB_ESCAPE        = 0xff1b;
constexpr Keycode KB_F1            = 0xffbe;
constexpr Keycode KB_F2            = 0xffbf;
constexpr Keycode KB_F3            = 0xffc0;
constexpr Keycode KB_F4            = 0xffc1;
constexpr Keycode KB_F5            = 0xffc2;
constexpr Keycode KB_F6            = 0xffc3;
constexpr Keycode KB_F7            = 0xffc4;
constexpr Keycode KB_F8            = 0xffc5;
constexpr Keycode KB_F9            = 0xffc6;
constexpr Keycode KB_F10           = 0xffc7;
constexpr Keycode KB_F11           = 0xffc8;
constexpr Keycode KB_F12           = 0xffc9;

constexpr Keycode KB_INSERT        = 0xff63;
constexpr Keycode KB_DELETE        = 0xffff;
constexpr Keycode KB_HOME          = 0xff50;
constexpr Keycode KB_END           = 0xff57;
constexpr Keycode KB_PAGE_UP       = 0xff55;
constexpr Keycode KB_PAGE_DOWN     = 0xff56;

constexpr Keycode KB_UP            = 0xff52;
constexpr Keycode KB_DOWN          = 0xff54;
constexpr Keycode KB_LEFT          = 0xff51;
constexpr Keycode KB_RIGHT         = 0xff53;

constexpr Keycode KB_GRAVE         = 0x0060;

constexpr Keycode KB_0             = 0x0030;
constexpr Keycode KB_1             = 0x0031;
constexpr Keycode KB_2             = 0x0032;
constexpr Keycode KB_3             = 0x0033;
constexpr Keycode KB_4             = 0x0034;
constexpr Keycode KB_5             = 0x0035;
constexpr Keycode KB_6             = 0x0036;
constexpr Keycode KB_7             = 0x0037;
constexpr Keycode KB_8             = 0x0038;
constexpr Keycode KB_9             = 0x0039;

constexpr Keycode KB_MINUS         = 0x002d;
constexpr Keycode KB_EQUAL         = 0x003d;
constexpr Keycode KB_BACKSPACE     = 0xff08;

constexpr Keycode KB_LEFT_BRACKET  = 0x005b;
constexpr Keycode KB_RIGHT_BRACKET = 0x005d;
constexpr Keycode KB_BACK_SLASH    = 0x005c;

constexpr Keycode KB_SEMICOLON     = 0x003b;
constexpr Keycode KB_APOSTROPHE    = 0x0027;
constexpr Keycode KB_ENTER         = 0xff0d;

constexpr Keycode KB_COMMA         = 0x002c;
constexpr Keycode KB_PERIOD        = 0x002e;
constexpr Keycode KB_FRONT_SLASH   = 0x002f;

constexpr Keycode KB_SPACE         = 0x0020;
constexpr Keycode KB_TAB           = 0xff09;
constexpr Keycode KB_LEFT_SHIFT    = 0xffe1;
constexpr Keycode KB_RIGHT_SHIFT   = 0xffe2;
constexpr Keycode KB_LEFT_CTRL     = 0xffe3;
constexpr Keycode KB_RIGHT_CTRL    = 0xffe4;
constexpr Keycode KB_LEFT_ALT      = 0xffe9;
constexpr Keycode KB_RIGHT_ALT     = 0xffea;
constexpr Keycode KB_MENU          = 0xff67;

constexpr Keycode MOUSE_BUTTON_LEFT    = 0x0001;
constexpr Keycode MOUSE_BUTTON_MIDDLE  = 0x0002;
constexpr Keycode MOUSE_BUTTON_RIGHT   = 0x0003;
constexpr Keycode MOUSE_WHEEL_UP       = 0x0004;
constexpr Keycode MOUSE_WHEEL_DOWN     = 0x0005;
constexpr Keycode MOUSE_WHEEL_LEFT     = 0x0006;
constexpr Keycode MOUSE_WHEEL_RIGHT    = 0x0007;
constexpr Keycode MOUSE_BUTTON_FORWARD = 0x0008;
constexpr Keycode MOUSE_BUTTON_BACK    = 0x0009;

} // namespace vkl

#endif // VKLEARNIN_SYSTEM_INPUT_KEYCODES_HPP