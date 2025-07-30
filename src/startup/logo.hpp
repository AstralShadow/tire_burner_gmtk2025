#ifndef INCLUDE_STARTUP_LOGO_HPP
#define INCLUDE_STARTUP_LOGO_HPP

#include <SDL2/SDL_render.h>
#include "utils/point.hpp"
#include "utils/types.hpp"
#include <string>

namespace startup
{
    SDL_Texture* gmtk_logo();
    Point gmtk_logo_size();

    extern SDL_Rect gmtk_logo_pos;
    extern float gmtk_logo_opacity;
    extern u32 time;
    extern int text_offset;


    /* Header */
    constexpr u32 text_enter_start = 250;
    constexpr u32 text_enter_end = 1250;

    constexpr u32 text_exit_start = 5750;
    constexpr u32 text_exit_end = 6500;


    /* GMTK Logo */
    constexpr u32 logo_distance = 64;
    constexpr u32 enter_start = 1500;
    constexpr u32 enter_end = 3250;

    constexpr u32 exit_start = 5250;
    constexpr u32 exit_end = 6500;


    /* Credits */
    constexpr u32 creds_enter_start = 2000;
    constexpr u32 creds_enter_end = 2500;

    constexpr u32 creds_exit_start = 5500;
    constexpr u32 creds_exit_end = 6500;


    /* Background */
    extern SDL_Color bg;
    constexpr u32 background_hide_start = 6500;


    constexpr u32 next_scene_t = 7000;
    const std::string next_scene = "menu";
}

#endif // INCLUDE_STARTUP_LOGO_HPP
