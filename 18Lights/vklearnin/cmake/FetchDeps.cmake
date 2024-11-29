include(FetchContent)

# ------------------------------------------------------------------------------
# spdlog -----------------------------------------------------------------------
FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog
    GIT_TAG 7e635fca68d014934b4af8a1cf874f63989352b7 # 2023-07-08
    # FIND_PACKAGE_ARGS 1.12.0
)

set(SPDLOG_USE_STD_FORMAT "ON" CACHE BOOL "" FORCE)

# ------------------------------------------------------------------------------
# glm --------------------------------------------------------------------------
FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm/
    GIT_TAG bf71a834948186f4097caa076cd2663c69a10e1e # 2020-04-13
    # FIND_PACKAGE_ARGS 0.9.9.8
)


# ------------------------------------------------------------------------------
# SDL2 -------------------------------------------------------------------------
FetchContent_Declare(
    SDL2
    GIT_REPOSITORY https://github.com/libsdl-org/SDL
    GIT_TAG 15ead9a40d09a1eb9972215cceac2bf29c9b77f6 # 2023-11-02
    # FIND_PACKAGE_ARGS 2.6.5
)

set(SDL2_DISABLE_INSTALL   "ON" CACHE BOOL "" FORCE)
set(SDL2_DISABLE_UNINSTALL "ON" CACHE BOOL "" FORCE)
set(SDL2_DISABLE_SDL2MAIN  "ON" CACHE BOOL "" FORCE)

set(SDL_SHARED "OFF" CACHE BOOL "" FORCE)
set(SDL_STATIC "ON"  CACHE BOOL "" FORCE)
set(SDL_TEST   "OFF" CACHE BOOL "" FORCE)

set(SDL_DBUS                 "OFF" CACHE BOOL "" FORCE)
set(SDL_DISKAUDIO            "OFF" CACHE BOOL "" FORCE)
set(SDL_DUMMYAUDIO           "OFF" CACHE BOOL "" FORCE)
set(SDL_DIRECTFB             "OFF" CACHE BOOL "" FORCE)
set(SDL_DIRECTFB_SHARED      "OFF" CACHE BOOL "" FORCE)
set(SDL_DUMMYVIDEO           "OFF" CACHE BOOL "" FORCE)
set(SDL_IBUS                 "OFF" CACHE BOOL "" FORCE)
set(SDL_SYSTEM_ICONV         "OFF" CACHE BOOL "" FORCE)
set(SDL_OPENGL               "OFF" CACHE BOOL "" FORCE)
set(SDL_OPENGLES             "OFF" CACHE BOOL "" FORCE)
set(SDL_OSS                  "OFF" CACHE BOOL "" FORCE)
set(SDL_ALSA                 "OFF" CACHE BOOL "" FORCE)
set(SDL_ALSA_SHARED          "OFF" CACHE BOOL "" FORCE)
set(SDL_JACK                 "OFF" CACHE BOOL "" FORCE)
set(SDL_JACK_SHARED          "OFF" CACHE BOOL "" FORCE)
set(SDL_ESD                  "OFF" CACHE BOOL "" FORCE)
set(SDL_ESD_SHARED           "OFF" CACHE BOOL "" FORCE)
set(SDL_PIPEWIRE             "OFF" CACHE BOOL "" FORCE)
set(SDL_PIPEWIRE_SHARED      "OFF" CACHE BOOL "" FORCE)
set(SDL_PULSEAUDIO           "OFF" CACHE BOOL "" FORCE)
set(SDL_PULSEAUDIO_SHARED    "OFF" CACHE BOOL "" FORCE)
set(SDL_ARTS                 "OFF" CACHE BOOL "" FORCE)
set(SDL_ARTS_SHARED          "OFF" CACHE BOOL "" FORCE)
set(SDL_NAS                  "OFF" CACHE BOOL "" FORCE)
set(SDL_NAS_SHARED           "OFF" CACHE BOOL "" FORCE)
set(SDL_SNDIO                "OFF" CACHE BOOL "" FORCE)
set(SDL_SNDIO_SHARED         "OFF" CACHE BOOL "" FORCE)
set(SDL_FUSIONSOUND          "OFF" CACHE BOOL "" FORCE)
set(SDL_FUSIONSOUND_SHARED   "OFF" CACHE BOOL "" FORCE)
set(SDL_LIBSAMPLERATE        "OFF" CACHE BOOL "" FORCE)
set(SDL_LIBSAMPLERATE_SHARED "OFF" CACHE BOOL "" FORCE)
set(SDL_RPATH                "OFF" CACHE BOOL "" FORCE)
set(SDL_CLOCK_GETTIME        "OFF" CACHE BOOL "" FORCE)

set(SDL_WAYLAND                 "OFF" CACHE BOOL "" FORCE)
set(SDL_WAYLAND_SHARED          "OFF" CACHE BOOL "" FORCE)
set(SDL_WAYLAND_LIBDECOR        "OFF" CACHE BOOL "" FORCE)
set(SDL_WAYLAND_LIBDECOR_SHARED "OFF" CACHE BOOL "" FORCE)
set(SDL_WAYLAND_QT_TOUCH        "OFF" CACHE BOOL "" FORCE)

set(SDL_XINPUT       "OFF" CACHE BOOL "" FORCE)
set(SDL_WASAPI       "OFF" CACHE BOOL "" FORCE)
set(SDL_RENDER_D3D   "OFF" CACHE BOOL "" FORCE)
set(SDL_RENDER_METAL "OFF" CACHE BOOL "" FORCE)
set(SDL_VIVANTE      "OFF" CACHE BOOL "" FORCE)

set(SDL_KMSDRM              "OFF" CACHE BOOL "" FORCE)
set(SDL_KMSDRM_SHARED       "OFF" CACHE BOOL "" FORCE)
set(SDL_OFFSCREEN           "OFF" CACHE BOOL "" FORCE)
set(SDL_HIDAPI              "OFF" CACHE BOOL "" FORCE)
set(SDL_HIDAPI_LIBUSB       "OFF" CACHE BOOL "" FORCE)
set(SDL_HIDAPI_JOYSTICK     "OFF" CACHE BOOL "" FORCE)
set(SDL_VIRTUAL_JOYSTICK    "OFF" CACHE BOOL "" FORCE)
set(SDL_ASAN                "OFF" CACHE BOOL "" FORCE)

set(SDL_AUDIO      "OFF" CACHE BOOL "" FORCE)
set(SDL_FILESYSTEM "OFF" CACHE BOOL "" FORCE)
set(SDL_HAPTIC     "OFF" CACHE BOOL "" FORCE)
set(SDL_JOYSTICK   "OFF" CACHE BOOL "" FORCE)
set(SDL_HIDAPI     "OFF" CACHE BOOL "" FORCE)
set(SDL_POWER      "OFF" CACHE BOOL "" FORCE)
set(SDL_LOCALE     "OFF" CACHE BOOL "" FORCE)
set(SDL_TIMERS     "OFF" CACHE BOOL "" FORCE)
set(SDL_SENSOR     "OFF" CACHE BOOL "" FORCE)