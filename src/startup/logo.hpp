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
    extern u32 time;


    /* Header */
    const u32 text_enter_start = 250;
    const u32 text_enter_end = 1250;

    const u32 text_exit_start = 5750;
    const u32 text_exit_end = 6500;


    /* GMTK Logo */
    const u32 enter_start = 1500;
    const u32 enter_end = 2750;

    const u32 exit_start = 5250;
    const u32 exit_end = 6500;


    /* Credits */
    const u32 creds_enter_start = 2000;
    const u32 creds_enter_end = 2500;

    const u32 creds_exit_start = 5500;
    const u32 creds_exit_end = 6000;


    const u32 next_scene_t = 7500;
    const std::string next_scene = "menu";
}

#endif // INCLUDE_STARTUP_LOGO_HPP
