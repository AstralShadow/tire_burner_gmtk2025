#include "menu/menu.hpp"
#include "core/core.hpp"
#include "menu/logo.hpp"
#include <SDL2/SDL_events.h>


void menu::keyup(SDL_KeyboardEvent&, scene_uid)
{

}

void menu::keydown(SDL_KeyboardEvent& ev, scene_uid)
{
    auto const& scancode = ev.keysym.scancode;
#ifndef __EMSCRIPTEN__
    if(scancode == SDL_SCANCODE_Q)
        core::stop();
#endif // __EMSCRIPTEN__

    if(scancode == SDL_SCANCODE_RETURN) {
        static size_t count = 0;
        count++;
        if(count > 3) {
            core::set_scene(game_scene);
            count = 0;
        }
    }
}

void menu::mouseup(SDL_MouseButtonEvent& ev, scene_uid)
{
    if(point_in_play_btn({ev.x, ev.y}))
        core::set_scene(game_scene);
}

void menu::mousedown(SDL_MouseButtonEvent&, scene_uid)
{

}

void menu::mouse_motion(SDL_MouseMotionEvent& ev, scene_uid)
{
    enlarge_play_btn = point_in_play_btn({ev.x, ev.y});
}
