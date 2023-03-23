#ifndef  VKLEARNIN_SYSTEM_INPUT_XCB_TO_VKL_KEYS_HPP
#define  VKLEARNIN_SYSTEM_INPUT_XCB_TO_VKL_KEYS_HPP

#ifdef VKL_UNUSED

#include "vklearnin/system/pch.hpp"
#include "vklearnin/system/input/Keycodes.hpp"

namespace vkl {

static std::unordered_map<xcb_keysym_t, Keycode> const keycode_table {
        { XK_A               , KB_A             },
        { XK_B               , KB_B             },
        { XK_C               , KB_C             },
        { XK_D               , KB_D             },
        { XK_E               , KB_E             },
        { XK_F               , KB_F             },
        { XK_G               , KB_G             },
        { XK_H               , KB_H             },
        { XK_I               , KB_I             },
        { XK_J               , KB_J             },
        { XK_K               , KB_K             },
        { XK_L               , KB_L             },
        { XK_M               , KB_M             },
        { XK_N               , KB_N             },
        { XK_O               , KB_O             },
        { XK_P               , KB_P             },
        { XK_Q               , KB_Q             },
        { XK_R               , KB_R             },
        { XK_S               , KB_S             },
        { XK_T               , KB_T             },
        { XK_U               , KB_U             },
        { XK_V               , KB_V             },
        { XK_W               , KB_W             },
        { XK_X               , KB_X             },
        { XK_Y               , KB_Y             },
        { XK_Z               , KB_Z             },
        { XK_a               , KB_A             },
        { XK_b               , KB_B             },
        { XK_c               , KB_C             },
        { XK_d               , KB_D             },
        { XK_e               , KB_E             },
        { XK_f               , KB_F             },
        { XK_g               , KB_G             },
        { XK_h               , KB_H             },
        { XK_i               , KB_I             },
        { XK_j               , KB_J             },
        { XK_k               , KB_K             },
        { XK_l               , KB_L             },
        { XK_m               , KB_M             },
        { XK_n               , KB_N             },
        { XK_o               , KB_O             },
        { XK_p               , KB_P             },
        { XK_q               , KB_Q             },
        { XK_r               , KB_R             },
        { XK_s               , KB_S             },
        { XK_t               , KB_T             },
        { XK_u               , KB_U             },
        { XK_v               , KB_V             },
        { XK_w               , KB_W             },
        { XK_x               , KB_X             },
        { XK_y               , KB_Y             },
        { XK_z               , KB_Z             },
        { XK_Escape          , KB_ESCAPE        },
        { XK_F1              , KB_F1            },
        { XK_F2              , KB_F2            },
        { XK_F3              , KB_F3            },
        { XK_F4              , KB_F4            },
        { XK_F5              , KB_F5            },
        { XK_F6              , KB_F6            },
        { XK_F7              , KB_F7            },
        { XK_F8              , KB_F8            },
        { XK_F9              , KB_F9            },
        { XK_F10             , KB_F10           },
        { XK_F11             , KB_F11           },
        { XK_F12             , KB_F12           },
        { XK_Insert          , KB_INSERT        },
        { XK_Delete          , KB_DELETE        },
        { XK_Home            , KB_HOME          },
        { XK_End             , KB_END           },
        { XK_Page_Up         , KB_PAGE_UP       },
        { XK_Page_Down       , KB_PAGE_DOWN     },
        { XK_Up              , KB_UP            },
        { XK_Down            , KB_DOWN          },
        { XK_Left            , KB_LEFT          },
        { XK_Right           , KB_RIGHT         },
        { XK_grave           , KB_GRAVE         },
        { XK_0               , KB_0             },
        { XK_1               , KB_1             },
        { XK_2               , KB_2             },
        { XK_3               , KB_3             },
        { XK_4               , KB_4             },
        { XK_5               , KB_5             },
        { XK_6               , KB_6             },
        { XK_7               , KB_7             },
        { XK_8               , KB_8             },
        { XK_9               , KB_9             },
        { XK_minus           , KB_MINUS         },
        { XK_equal           , KB_EQUAL         },
        { XK_BackSpace       , KB_BACKSPACE     },
        { XK_bracketleft     , KB_LEFT_BRACKET  },
        { XK_bracketright    , KB_RIGHT_BRACKET },
        { XK_backslash       , KB_BACK_SLASH    },
        { XK_semicolon       , KB_SEMICOLON     },
        { XK_apostrophe      , KB_APOSTROPHE    },
        { XK_Return          , KB_ENTER         },
        { XK_comma           , KB_COMMA         },
        { XK_period          , KB_PERIOD        },
        { XK_slash           , KB_FRONT_SLASH   },
        { XK_space           , KB_SPACE         },
        { XK_Tab             , KB_TAB           },
        { XK_Shift_L         , KB_LEFT_SHIFT    },
        { XK_Shift_R         , KB_RIGHT_SHIFT   },
        { XK_Control_L       , KB_LEFT_CTRL     },
        { XK_Control_R       , KB_RIGHT_CTRL    },
        { XK_Alt_L           , KB_LEFT_ALT      },
        // I'd expect the following to be XK_Alt_R / 0xffea, but it's not
        { XK_ISO_Level3_Shift, KB_RIGHT_ALT     },
        { XK_Menu            , KB_MENU          },
};

static std::unordered_map<xcb_button_t, Keycode> const mouse_button_table {
    { ::XCB_BUTTON_INDEX_1, MOUSE_BUTTON_LEFT    },
    { ::XCB_BUTTON_INDEX_2, MOUSE_BUTTON_MIDDLE  },
    { ::XCB_BUTTON_INDEX_3, MOUSE_BUTTON_RIGHT   },
    { ::XCB_BUTTON_INDEX_4, MOUSE_WHEEL_UP       },
    { ::XCB_BUTTON_INDEX_5, MOUSE_WHEEL_DOWN     },
    // Alas that XCB doesn't have indices for these. But it also hasn't decided
    // if the above wheel indices are up or down. This works on my machine.
    { 0x0006              , MOUSE_WHEEL_LEFT     },
    { 0x0007              , MOUSE_WHEEL_RIGHT    },
    { 0x0008              , MOUSE_BUTTON_FORWARD },
    { 0x0009              , MOUSE_BUTTON_BACK    },
};

inline Keycode xcb_to_vkl(xcb_keysym_t const key) {
    auto const &code = keycode_table.find(key);
    if(code != keycode_table.end()) {
        return code->second;
    }
    else {
        return KB_UNKNOWN;
    }
}

inline Keycode xcb_to_vkl(xcb_button_t const button) {
    auto const &code = mouse_button_table.find(button);
    if(code != mouse_button_table.end()) {
        return code->second;
    }
    else {
        return KB_UNKNOWN;
    }
}

} // namespace vkl

#endif // VKL_UNUSED

#endif //  VKLEARNIN_SYSTEM_INPUT_WIN32TOVKLKEYS_HPP
