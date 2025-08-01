#include "game/game.hpp"
#include "game/data.hpp"
#include "core/core.hpp"
#include <SDL2/SDL_render.h>

static auto& rnd = core::renderer;


void game::render(scene_uid)
{
    SDL_RenderClear(rnd);

    auto& track = game::track(current_track);
    SDL_RenderCopy(rnd, track.tex, nullptr, nullptr);


    SDL_RenderPresent(rnd);
}
