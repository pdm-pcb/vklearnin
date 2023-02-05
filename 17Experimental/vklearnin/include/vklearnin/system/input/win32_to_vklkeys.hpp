#ifndef  VKLEARNIN_SYSTEM_INPUT_WIN32TOVKLKEYS_HPP
#define  VKLEARNIN_SYSTEM_INPUT_WIN32TOVKLKEYS_HPP

#include "vklearnin/system/input/Keycodes.hpp"

namespace vkl {

inline Keycode win32_to_vkl(uint32_t vkey) {
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

        default: code = vkey; break;
    }

    return code;
}

inline Keycode vkl_to_win32(Keycode vkl_key) {
    Keycode vkey = KB_UNKNOWN;

    switch(vkl_key) {
        case KB_BACKSPACE:     vkey = 8;   break; // Backspace
        case KB_TAB:           vkey = 9;   break; // Tab
        case KB_ENTER:         vkey = 13;  break; // Enter
        case KB_PAUSE:         vkey = 19;  break; // Pause
        case KB_CAPS_LOCK:     vkey = 20;  break; // Caps Lock
        case KB_ESCAPE:        vkey = 27;  break; // Esc
        case KB_SPACE:         vkey = 32;  break; // Space
        case KB_PAGE_UP:       vkey = 33;  break; // Page Up
        case KB_PAGE_DOWN:     vkey = 34;  break; // Page Down
        case KB_HOME:          vkey = 35;  break; // End
        case KB_END:           vkey = 36;  break; // Home
        case KB_LEFT:          vkey = 37;  break; // Left
        case KB_UP:            vkey = 38;  break; // Up
        case KB_RIGHT:         vkey = 39;  break; // Right
        case KB_DOWN:          vkey = 40;  break; // Down
        case KB_PRINT_SCREEN:  vkey = 44;  break; // Prnt Scrn
        case KB_INSERT:        vkey = 45;  break; // Insert
        case KB_DEL:           vkey = 46;  break; // Delete
        case KB_ZERO:          vkey = 48;  break; // 0
        case KB_ONE:           vkey = 49;  break; // 1
        case KB_TWO:           vkey = 50;  break; // 2
        case KB_THREE:         vkey = 51;  break; // 3
        case KB_FOUR:          vkey = 52;  break; // 4
        case KB_FIVE:          vkey = 53;  break; // 5
        case KB_SIX:           vkey = 54;  break; // 6
        case KB_SEVEN:         vkey = 55;  break; // 7
        case KB_EIGHT:         vkey = 56;  break; // 8
        case KB_NINE:          vkey = 57;  break; // 9
        case KB_LSUPER:        vkey = 91;  break; // Left Windows
        case KB_MENU:          vkey = 93;  break; // Application
        case KB_NP_0:          vkey = 96;  break; // Num 0
        case KB_NP_1:          vkey = 97;  break; // Num 1
        case KB_NP_2:          vkey = 98;  break; // Num 2
        case KB_NP_3:          vkey = 99;  break; // Num 3
        case KB_NP_4:          vkey = 100; break; // Num 4
        case KB_NP_5:          vkey = 101; break; // Num 5
        case KB_NP_6:          vkey = 102; break; // Num 6
        case KB_NP_7:          vkey = 103; break; // Num 7
        case KB_NP_8:          vkey = 104; break; // Num 8
        case KB_NP_9:          vkey = 105; break; // Num 9
        case KB_NP_MULTIPLY:   vkey = 106; break; // Num *
        case KB_NP_ADD:        vkey = 107; break; // Num +
        case KB_NP_SUBTRACT:   vkey = 109; break; // Num -
        case KB_NP_DIVIDE:     vkey = 111; break; // Num /
        case KB_F1:            vkey = 112; break; // F1
        case KB_F2:            vkey = 113; break; // F2
        case KB_F3:            vkey = 114; break; // F3
        case KB_F4:            vkey = 115; break; // F4
        case KB_F5:            vkey = 116; break; // F5
        case KB_F6:            vkey = 117; break; // F6
        case KB_F7:            vkey = 118; break; // F7
        case KB_F8:            vkey = 119; break; // F8
        case KB_F9:            vkey = 120; break; // F9
        case KB_F10:           vkey = 121; break; // F10
        case KB_F11:           vkey = 122; break; // F11
        case KB_NUM_LOCK:      vkey = 144; break; // Num Lock
        case KB_SCROLL_LOCK:   vkey = 145; break; // Scroll Lock
        case KB_LSHIFT:        vkey = 160; break; // Shift
        case KB_RSHIFT:        vkey = 161; break; // Right Shift
        case KB_SEMICOLON:     vkey = 186; break; // ;
        case KB_EQUAL:         vkey = 187; break; // =
        case KB_MINUS:         vkey = 189; break; // -
        case KB_PERIOD:        vkey = 190; break; // .
        case KB_SLASH:         vkey = 191; break; // Frontslash
        case KB_GRAVE_ACCENT:  vkey = 192; break; // `
        case KB_LEFT_BRACKET:  vkey = 219; break; // [
        case KB_RIGHT_BRACKET: vkey = 221; break; // ]
        case KB_BACKSLASH:     vkey = 220; break; // Backslash
        case KB_APOSTROPHE:    vkey = 222; break; // '
        case KB_NP_DECIMAL:    vkey = 330; break; // Num Del
        case KB_NP_ENTER:      vkey = 335; break; // Num Enter
        case KB_LCTRL:         vkey = 341; break; // Ctrl
        case KB_LALT:          vkey = 342; break; // Alt
        case KB_RCTRL:         vkey = 345; break; // Right Ctrl
        case KB_RALT:          vkey = 346; break; // Right Alt

        default: vkey = vkl_key; break;
    }

    return vkey;
}

} // namespace vkl

#endif //  VKLEARNIN_SYSTEM_INPUT_WIN32TOVKLKEYS_HPP