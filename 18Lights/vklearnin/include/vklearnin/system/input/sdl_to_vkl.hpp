#ifndef  VKLEARNIN_SYSTEM_INPUT_SDL_TO_VLK_HPP
#define  VKLEARNIN_SYSTEM_INPUT_SDL_TO_VLK_HPP

#include "vklearnin/system/pch.hpp"
#include "vklearnin/system/input/Keycodes.hpp"

namespace vkl {

static std::unordered_map<::SDL_Scancode, Keycode> const keycode_table {
        { ::SDL_SCANCODE_A,            KB_A             },
        { ::SDL_SCANCODE_B,            KB_B             },
        { ::SDL_SCANCODE_C,            KB_C             },
        { ::SDL_SCANCODE_D,            KB_D             },
        { ::SDL_SCANCODE_E,            KB_E             },
        { ::SDL_SCANCODE_F,            KB_F             },
        { ::SDL_SCANCODE_G,            KB_G             },
        { ::SDL_SCANCODE_H,            KB_H             },
        { ::SDL_SCANCODE_I,            KB_I             },
        { ::SDL_SCANCODE_J,            KB_J             },
        { ::SDL_SCANCODE_K,            KB_K             },
        { ::SDL_SCANCODE_L,            KB_L             },
        { ::SDL_SCANCODE_M,            KB_M             },
        { ::SDL_SCANCODE_N,            KB_N             },
        { ::SDL_SCANCODE_O,            KB_O             },
        { ::SDL_SCANCODE_P,            KB_P             },
        { ::SDL_SCANCODE_Q,            KB_Q             },
        { ::SDL_SCANCODE_R,            KB_R             },
        { ::SDL_SCANCODE_S,            KB_S             },
        { ::SDL_SCANCODE_T,            KB_T             },
        { ::SDL_SCANCODE_U,            KB_U             },
        { ::SDL_SCANCODE_V,            KB_V             },
        { ::SDL_SCANCODE_W,            KB_W             },
        { ::SDL_SCANCODE_X,            KB_X             },
        { ::SDL_SCANCODE_Y,            KB_Y             },
        { ::SDL_SCANCODE_Z,            KB_Z             },
        { ::SDL_SCANCODE_ESCAPE,       KB_ESCAPE        },
        { ::SDL_SCANCODE_F1,           KB_F1            },
        { ::SDL_SCANCODE_F2,           KB_F2            },
        { ::SDL_SCANCODE_F3,           KB_F3            },
        { ::SDL_SCANCODE_F4,           KB_F4            },
        { ::SDL_SCANCODE_F5,           KB_F5            },
        { ::SDL_SCANCODE_F6,           KB_F6            },
        { ::SDL_SCANCODE_F7,           KB_F7            },
        { ::SDL_SCANCODE_F8,           KB_F8            },
        { ::SDL_SCANCODE_F9,           KB_F9            },
        { ::SDL_SCANCODE_F10,          KB_F10           },
        { ::SDL_SCANCODE_F11,          KB_F11           },
        { ::SDL_SCANCODE_F12,          KB_F12           },
        { ::SDL_SCANCODE_INSERT,       KB_INSERT        },
        { ::SDL_SCANCODE_DELETE,       KB_DELETE        },
        { ::SDL_SCANCODE_HOME,         KB_HOME          },
        { ::SDL_SCANCODE_END,          KB_END           },
        { ::SDL_SCANCODE_PAGEUP,       KB_PAGE_UP       },
        { ::SDL_SCANCODE_PAGEDOWN,     KB_PAGE_DOWN     },
        { ::SDL_SCANCODE_UP,           KB_UP            },
        { ::SDL_SCANCODE_DOWN,         KB_DOWN          },
        { ::SDL_SCANCODE_LEFT,         KB_LEFT          },
        { ::SDL_SCANCODE_RIGHT,        KB_RIGHT         },
        { ::SDL_SCANCODE_GRAVE,        KB_GRAVE         },
        { ::SDL_SCANCODE_1,            KB_0             },
        { ::SDL_SCANCODE_2,            KB_1             },
        { ::SDL_SCANCODE_3,            KB_2             },
        { ::SDL_SCANCODE_4,            KB_3             },
        { ::SDL_SCANCODE_5,            KB_4             },
        { ::SDL_SCANCODE_6,            KB_5             },
        { ::SDL_SCANCODE_7,            KB_6             },
        { ::SDL_SCANCODE_8,            KB_7             },
        { ::SDL_SCANCODE_9,            KB_8             },
        { ::SDL_SCANCODE_0,            KB_9             },
        { ::SDL_SCANCODE_MINUS,        KB_MINUS         },
        { ::SDL_SCANCODE_EQUALS,       KB_EQUAL         },
        { ::SDL_SCANCODE_BACKSPACE,    KB_BACKSPACE     },
        { ::SDL_SCANCODE_LEFTBRACKET,  KB_LEFT_BRACKET  },
        { ::SDL_SCANCODE_RIGHTBRACKET, KB_RIGHT_BRACKET },
        { ::SDL_SCANCODE_BACKSLASH,    KB_BACK_SLASH    },
        { ::SDL_SCANCODE_SEMICOLON,    KB_SEMICOLON     },
        { ::SDL_SCANCODE_APOSTROPHE,   KB_APOSTROPHE    },
        { ::SDL_SCANCODE_RETURN,       KB_ENTER         },
        { ::SDL_SCANCODE_COMMA,        KB_COMMA         },
        { ::SDL_SCANCODE_PERIOD,       KB_PERIOD        },
        { ::SDL_SCANCODE_SLASH,        KB_FRONT_SLASH   },
        { ::SDL_SCANCODE_SPACE,        KB_SPACE         },
        { ::SDL_SCANCODE_TAB,          KB_TAB           },
        { ::SDL_SCANCODE_LSHIFT,       KB_LEFT_SHIFT    },
        { ::SDL_SCANCODE_RSHIFT,       KB_RIGHT_SHIFT   },
        { ::SDL_SCANCODE_LCTRL,        KB_LEFT_CTRL     },
        { ::SDL_SCANCODE_RCTRL,        KB_RIGHT_CTRL    },
        { ::SDL_SCANCODE_LALT,         KB_LEFT_ALT      },
        { ::SDL_SCANCODE_RALT,         KB_RIGHT_ALT     },
        { ::SDL_SCANCODE_MENU,         KB_MENU          },
};

static std::unordered_map<int, Keycode> const mouse_button_table {
    { SDL_BUTTON_LEFT,   MOUSE_BUTTON_LEFT    },
    { SDL_BUTTON_RIGHT,  MOUSE_BUTTON_RIGHT   },
    { SDL_BUTTON_MIDDLE, MOUSE_BUTTON_MIDDLE  },
    { SDL_BUTTON_X1,     MOUSE_BUTTON_BACK    },
    { SDL_BUTTON_X2,     MOUSE_BUTTON_FORWARD },
};

inline Keycode sdl_to_vkl(::SDL_Scancode const key) {
    auto const &code = keycode_table.find(key);
    if(code != keycode_table.end()) {
        return code->second;
    }
    else {
        return KB_UNKNOWN;
    }
}

inline Keycode sdl_to_vkl(int const button) {
    auto const &code = mouse_button_table.find(button);
    if(code != mouse_button_table.end()) {
        return code->second;
    }
    else {
        return KB_UNKNOWN;
    }
}

} // namespace vkl

#endif //  VKLEARNIN_SYSTEM_INPUT_SDL_TO_VLK_HPP
