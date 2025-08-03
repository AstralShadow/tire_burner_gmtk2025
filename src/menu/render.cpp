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
    // Background
    SDL_RenderCopy(rnd, bg_frames()[current_bg_frame % bg_frames_count], nullptr, nullptr);

    /* Animated car glass */ {
        size_t frame1 = current_glass_frame % glass_frames_count;
        size_t frame2 = (frame1 + 1) % glass_frames_count;
        auto tex1 = glass_frames()[frame1];
        auto tex2 = glass_frames()[frame2];
        float progress = glass_frame_transition_progress;

        SDL_SetTextureAlphaMod(tex1, 255);
        SDL_SetTextureAlphaMod(tex2, 255 * progress);

        auto pos = glass_pos();
        auto size = glass_size();

        SDL_Rect area {
            pos.x, pos.y,
            size.x, size.y
        };

        SDL_RenderCopy(rnd, tex1, nullptr, &area);
        SDL_RenderCopy(rnd, tex2, nullptr, &area);
    }

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


    if(!black_scene_transition_done) {
        SDL_SetRenderDrawBlendMode(rnd, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(rnd, 0, 0, 0, 255 * (1 - black_scene_transition));
        SDL_RenderFillRect(rnd, nullptr);
        SDL_SetRenderDrawBlendMode(rnd, SDL_BLENDMODE_NONE);
    }

    SDL_RenderPresent(rnd);
}
