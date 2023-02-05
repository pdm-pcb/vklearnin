#ifndef VKLEARNIN_SYSTEM_INPUT_KEYCODES_HPP
#define VKLEARNIN_SYSTEM_INPUT_KEYCODES_HPP

#include "vklearnin/system/pch.hpp"

namespace vkl {

using Keycode = uint32_t;

// lifted directly from glfw3
constexpr Keycode KB_UNKNOWN          = 0;
constexpr Keycode KB_SPACE            = 32;
constexpr Keycode KB_APOSTROPHE       = 39;
constexpr Keycode KB_COMMA            = 44;
constexpr Keycode KB_MINUS            = 45;
constexpr Keycode KB_PERIOD           = 46;
constexpr Keycode KB_SLASH            = 47;
constexpr Keycode KB_ZERO             = 48;
constexpr Keycode KB_ONE              = 49;
constexpr Keycode KB_TWO              = 50;
constexpr Keycode KB_THREE            = 51;
constexpr Keycode KB_FOUR             = 52;
constexpr Keycode KB_FIVE             = 53;
constexpr Keycode KB_SIX              = 54;
constexpr Keycode KB_SEVEN            = 55;
constexpr Keycode KB_EIGHT            = 56;
constexpr Keycode KB_NINE             = 57;
constexpr Keycode KB_SEMICOLON        = 59;
constexpr Keycode KB_EQUAL            = 61;

constexpr Keycode KB_A                = 65;
constexpr Keycode KB_B                = 66;
constexpr Keycode KB_C                = 67;
constexpr Keycode KB_D                = 68;
constexpr Keycode KB_E                = 69;
constexpr Keycode KB_F                = 70;
constexpr Keycode KB_G                = 71;
constexpr Keycode KB_H                = 72;
constexpr Keycode KB_I                = 73;
constexpr Keycode KB_J                = 74;
constexpr Keycode KB_K                = 75;
constexpr Keycode KB_L                = 76;
constexpr Keycode KB_M                = 77;
constexpr Keycode KB_N                = 78;
constexpr Keycode KB_O                = 79;
constexpr Keycode KB_P                = 80;
constexpr Keycode KB_Q                = 81;
constexpr Keycode KB_R                = 82;
constexpr Keycode KB_S                = 83;
constexpr Keycode KB_T                = 84;
constexpr Keycode KB_U                = 85;
constexpr Keycode KB_V                = 86;
constexpr Keycode KB_W                = 87;
constexpr Keycode KB_X                = 88;
constexpr Keycode KB_Y                = 89;
constexpr Keycode KB_Z                = 90;

constexpr Keycode KB_LEFT_BRACKET     = 91;
constexpr Keycode KB_BACKSLASH        = 92;
constexpr Keycode KB_RIGHT_BRACKET    = 93;
constexpr Keycode KB_GRAVE_ACCENT     = 96;
constexpr Keycode KB_WORLD_1          = 161;
constexpr Keycode KB_WORLD_2          = 162;
constexpr Keycode KB_ESCAPE           = 256;
constexpr Keycode KB_ENTER            = 257;
constexpr Keycode KB_TAB              = 258;
constexpr Keycode KB_BACKSPACE        = 259;
constexpr Keycode KB_INSERT           = 260;
constexpr Keycode KB_DEL              = 261;
constexpr Keycode KB_RIGHT            = 262;
constexpr Keycode KB_LEFT             = 263;
constexpr Keycode KB_DOWN             = 264;
constexpr Keycode KB_UP               = 265;
constexpr Keycode KB_PAGE_UP          = 266;
constexpr Keycode KB_PAGE_DOWN        = 267;
constexpr Keycode KB_HOME             = 268;
constexpr Keycode KB_END              = 269;
constexpr Keycode KB_CAPS_LOCK        = 280;
constexpr Keycode KB_SCROLL_LOCK      = 281;
constexpr Keycode KB_NUM_LOCK         = 282;
constexpr Keycode KB_PRINT_SCREEN     = 283;
constexpr Keycode KB_PAUSE            = 284;

constexpr Keycode KB_F1               = 290;
constexpr Keycode KB_F2               = 291;
constexpr Keycode KB_F3               = 292;
constexpr Keycode KB_F4               = 293;
constexpr Keycode KB_F5               = 294;
constexpr Keycode KB_F6               = 295;
constexpr Keycode KB_F7               = 296;
constexpr Keycode KB_F8               = 297;
constexpr Keycode KB_F9               = 298;
constexpr Keycode KB_F10              = 299;
constexpr Keycode KB_F11              = 300;
constexpr Keycode KB_F12              = 301;
constexpr Keycode KB_F13              = 302;
constexpr Keycode KB_F14              = 303;
constexpr Keycode KB_F15              = 304;
constexpr Keycode KB_F16              = 305;
constexpr Keycode KB_F17              = 306;
constexpr Keycode KB_F18              = 307;
constexpr Keycode KB_F19              = 308;
constexpr Keycode KB_F20              = 309;
constexpr Keycode KB_F21              = 310;
constexpr Keycode KB_F22              = 311;
constexpr Keycode KB_F23              = 312;
constexpr Keycode KB_F24              = 313;
constexpr Keycode KB_F25              = 314;

constexpr Keycode KB_NP_0             = 320;
constexpr Keycode KB_NP_1             = 321;
constexpr Keycode KB_NP_2             = 322;
constexpr Keycode KB_NP_3             = 323;
constexpr Keycode KB_NP_4             = 324;
constexpr Keycode KB_NP_5             = 325;
constexpr Keycode KB_NP_6             = 326;
constexpr Keycode KB_NP_7             = 327;
constexpr Keycode KB_NP_8             = 328;
constexpr Keycode KB_NP_9             = 329;
constexpr Keycode KB_NP_DECIMAL       = 330;
constexpr Keycode KB_NP_DIVIDE        = 331;
constexpr Keycode KB_NP_MULTIPLY      = 332;
constexpr Keycode KB_NP_SUBTRACT      = 333;
constexpr Keycode KB_NP_ADD           = 334;
constexpr Keycode KB_NP_ENTER         = 335;
constexpr Keycode KB_NP_EQUAL         = 336;

constexpr Keycode KB_LSHIFT           = 340;
constexpr Keycode KB_LCTRL            = 341;
constexpr Keycode KB_LALT             = 342;
constexpr Keycode KB_LSUPER           = 343;
constexpr Keycode KB_RSHIFT           = 344;
constexpr Keycode KB_RCTRL            = 345;
constexpr Keycode KB_RALT             = 346;
constexpr Keycode KB_RSUPER           = 347;
constexpr Keycode KB_MENU             = 348;
constexpr Keycode KB_LAST             = KB_MENU + 1;

constexpr Keycode MOUSE_BUTTON_1      = 0;
constexpr Keycode MOUSE_BUTTON_2      = 1;
constexpr Keycode MOUSE_BUTTON_3      = 2;
constexpr Keycode MOUSE_BUTTON_4      = 3;
constexpr Keycode MOUSE_BUTTON_5      = 4;
constexpr Keycode MOUSE_BUTTON_6      = 5;
constexpr Keycode MOUSE_BUTTON_7      = 6;
constexpr Keycode MOUSE_BUTTON_8      = 7;
constexpr Keycode MOUSE_BUTTON_LEFT   = MOUSE_BUTTON_1;
constexpr Keycode MOUSE_BUTTON_RIGHT  = MOUSE_BUTTON_2;
constexpr Keycode MOUSE_BUTTON_MIDDLE = MOUSE_BUTTON_3;
constexpr Keycode MOUSE_BUTTON_LAST   = MOUSE_BUTTON_8 + 1;

} // namespace vkl

#endif // VKLEARNIN_SYSTEM_INPUT_KEYCODES_HPP