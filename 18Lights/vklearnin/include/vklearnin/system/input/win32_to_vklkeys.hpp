#ifndef  VKLEARNIN_SYSTEM_INPUT_WIN32TOVKLKEYS_HPP
#define  VKLEARNIN_SYSTEM_INPUT_WIN32TOVKLKEYS_HPP

#ifdef VKL_WINDOWS

#include "vklearnin/system/pch.hpp"
#include "vklearnin/system/input/Keycodes.hpp"

namespace vkl {

inline Keycode win32_to_vkl(::WPARAM vkey) {
    Keycode code = KB_UNKNOWN;

    switch(vkey) {
        case   8: code = KB_BACKSPACE;     break; // Backspace
        case   9: code = KB_TAB;           break; // Tab
        case  13: code = KB_ENTER;         break; // Enter
        case  19: code = KB_PAUSE;         break; // Pause
        case  20: code = KB_CAPS_LOCK;     break; // Caps Lock
        case  27: code = KB_ESCAPE;        break; // Esc
        case  32: code = KB_SPACE;         break; // Space
        case  33: code = KB_PAGE_UP;       break; // Page Up
        case  34: code = KB_PAGE_DOWN;     break; // Page Down
        case  35: code = KB_END;           break; // End
        case  36: code = KB_HOME;          break; // Home
        case  37: code = KB_LEFT;          break; // Left
        case  38: code = KB_UP;            break; // Up
        case  39: code = KB_RIGHT;         break; // Right
        case  40: code = KB_DOWN;          break; // Down
        case  44: code = KB_PRINT_SCREEN;  break; // Prnt Scrn
        case  45: code = KB_INSERT;        break; // Insert
        case  46: code = KB_DEL;           break; // Delete
        case  48: code = KB_ZERO;          break; // 0
        case  49: code = KB_ONE;           break; // 1
        case  50: code = KB_TWO;           break; // 2
        case  51: code = KB_THREE;         break; // 3
        case  52: code = KB_FOUR;          break; // 4
        case  53: code = KB_FIVE;          break; // 5
        case  54: code = KB_SIX;           break; // 6
        case  55: code = KB_SEVEN;         break; // 7
        case  56: code = KB_EIGHT;         break; // 8
        case  57: code = KB_NINE;          break; // 9
        case  91: code = KB_LSUPER;        break; // Left Windows
        case  93: code = KB_MENU;          break; // Application
        case  96: code = KB_NP_0;          break; // Num 0
        case  97: code = KB_NP_1;          break; // Num 1
        case  98: code = KB_NP_2;          break; // Num 2
        case  99: code = KB_NP_3;          break; // Num 3
        case 100: code = KB_NP_4;          break; // Num 4
        case 101: code = KB_NP_5;          break; // Num 5
        case 102: code = KB_NP_6;          break; // Num 6
        case 103: code = KB_NP_7;          break; // Num 7
        case 104: code = KB_NP_8;          break; // Num 8
        case 105: code = KB_NP_9;          break; // Num 9
        case 106: code = KB_NP_MULTIPLY;   break; // Num *
        case 107: code = KB_NP_ADD;        break; // Num +
        case 109: code = KB_NP_SUBTRACT;   break; // Num -
        case 111: code = KB_NP_DIVIDE;     break; // Num /
        case 112: code = KB_F1;            break; // F1
        case 113: code = KB_F2;            break; // F2
        case 114: code = KB_F3;            break; // F3
        case 115: code = KB_F4;            break; // F4
        case 116: code = KB_F5;            break; // F5
        case 117: code = KB_F6;            break; // F6
        case 118: code = KB_F7;            break; // F7
        case 119: code = KB_F8;            break; // F8
        case 120: code = KB_F9;            break; // F9
        case 121: code = KB_F10;           break; // F10
        case 122: code = KB_F11;           break; // F11
        case 144: code = KB_NUM_LOCK;      break; // Num Lock
        case 145: code = KB_SCROLL_LOCK;   break; // Scroll Lock
        case 160: code = KB_LSHIFT;        break; // Shift
        case 161: code = KB_RSHIFT;        break; // Right Shift
        case 186: code = KB_SEMICOLON;     break; // ;
        case 187: code = KB_EQUAL;         break; // =
        case 189: code = KB_MINUS;         break; // -
        case 190: code = KB_PERIOD;        break; // .
        case 191: code = KB_SLASH;         break; // Frontslash
        case 192: code = KB_GRAVE_ACCENT;  break; // `
        case 219: code = KB_LEFT_BRACKET;  break; // [
        case 221: code = KB_RIGHT_BRACKET; break; // ]
        case 220: code = KB_BACKSLASH;     break; // Backslash
        case 222: code = KB_APOSTROPHE;    break; // '
        case 330: code = KB_NP_DECIMAL;    break; // Num Del
        case 335: code = KB_NP_ENTER;      break; // Num Enter
        case 341: code = KB_LCTRL;         break; // Ctrl
        case 342: code = KB_LALT;          break; // Alt
        case 345: code = KB_RCTRL;         break; // Right Ctrl
        case 346: code = KB_RALT;          break; // Right Alt

        default: code = static_cast<uint32_t>(vkey); break;
    }

    return code;
}

} // namespace vkl

#endif // VKL_WINDOWS

#endif //  VKLEARNIN_SYSTEM_INPUT_WIN32TOVKLKEYS_HPP
