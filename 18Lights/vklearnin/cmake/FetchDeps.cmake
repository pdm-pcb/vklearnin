include(FetchContent)

# ------------------------------------------------------------------------------
# fmtlib -----------------------------------------------------------------------
FetchContent_Declare(
    fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt
    GIT_TAG a33701196adfad74917046096bf5a2aa0ab0bb50 # 2022-08-27
    # FIND_PACKAGE_ARGS 9.1.0
)

# ------------------------------------------------------------------------------
# spdlog -----------------------------------------------------------------------
FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog
    GIT_TAG ad0e89cbfb4d0c1ce4d097e134eb7be67baebb36 # 2022-11-02
    # FIND_PACKAGE_ARGS 1.11.0
)

set(SPDLOG_FMT_EXTERNAL "ON" CACHE BOOL "" FORCE)

# ------------------------------------------------------------------------------
# SDL2 -------------------------------------------------------------------------
FetchContent_Declare(
    SDL2
    GIT_REPOSITORY https://github.com/libsdl-org/SDL
    GIT_TAG 07d0f51fa292895443f563f0cbde4cb3802d87fa # 2023-03-06
    # FIND_PACKAGE_ARGS 2.26.4
)

set(SDL2_DISABLE_INSTALL   "ON" CACHE BOOL "" FORCE)
set(SDL2_DISABLE_UNINSTALL "ON" CACHE BOOL "" FORCE)
set(SDL2_DISABLE_SDL2MAIN  "ON" CACHE BOOL "" FORCE)

set(SDL_SHARED "OFF" CACHE BOOL "" FORCE)
set(SDL_STATIC "ON"  CACHE BOOL "" FORCE)

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
set(SDL_PTHREADS             "OFF" CACHE BOOL "" FORCE)
set(SDL_PTHREADS_SEM         "OFF" CACHE BOOL "" FORCE)
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