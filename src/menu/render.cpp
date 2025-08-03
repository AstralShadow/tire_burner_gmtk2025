#include "menu/menu.hpp"
#include "menu/logo.hpp"
#include "core/core.hpp"
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_mouse.h>
#include <string>
#include <iostream>

using std::cout;
using std::endl;
using std::string;

static auto& rnd = core::renderer;


void menu::render(scene_uid)
{
    // SDL_SetRenderDrawColor(rnd, 0, 0, 0, 255);
    // SDL_RenderClear(rnd);

    SDL_RenderCopy(rnd, bg_frames()[current_bg_frame % bg_frames_count], nullptr, nullptr);


    for(size_t i = 0; i < title_texture_count; i++) {
        auto tex = title_textures()[i];
        auto pos = title_pos()[i];
        auto size = title_size()[i];

        SDL_Rect area {
            pos.x, pos.y,
            size.x, size.y
        };

        SDL_RenderCopy(rnd, tex, nullptr, &area);
    }

    /* Play button */ {
        auto tex = play_button_texture();
        auto pos = play_button_pos();
        auto size = play_button_size();

        SDL_Rect area {
            pos.x, pos.y,
            size.x, size.y
        };

        SDL_RenderCopy(rnd, tex, nullptr, &area);
    }

    SDL_RenderPresent(rnd);
}
