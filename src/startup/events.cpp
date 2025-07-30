#include "startup/startup.hpp"
#include "core/core.hpp"
#include "startup/logo.hpp"
#include <SDL2/SDL_events.h>


void startup::keydown(SDL_KeyboardEvent& ev, scene_uid)
{
    auto const& scancode = ev.keysym.scancode;
#ifndef __EMSCRIPTEN__
    if(scancode == SDL_SCANCODE_Q)
        core::stop();
#endif // __EMSCRIPTEN__

    if(scancode == SDL_SCANCODE_ESCAPE)
        core::set_scene(next_scene);

    if(scancode == SDL_SCANCODE_SPACE)
        core::set_scene(next_scene);

    if(scancode == SDL_SCANCODE_RETURN)
        core::set_scene(next_scene);
}
