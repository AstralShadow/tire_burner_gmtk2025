#pragma once

#include "utils/types.hpp"

struct SDL_KeyboardEvent;
struct SDL_MouseButtonEvent;


namespace startup
{
    struct scene_uid_t;
    typedef struct scene_uid_t* scene_uid;

    void init(int, char**, scene_uid = 0);
    void deinit(scene_uid = 0);
    void tick(u32 ms, scene_uid = 0);
    void render(scene_uid = 0);

    void keydown(SDL_KeyboardEvent&, scene_uid = 0);
    void keyup(SDL_KeyboardEvent&, scene_uid = 0);

    void mousedown(SDL_MouseButtonEvent&, scene_uid = 0);
    void mouseup(SDL_MouseButtonEvent&, scene_uid = 0);
}
