#include "game/game.hpp"
#include "game/data.hpp"
#include "core/core.hpp"
#include <SDL2/SDL_render.h>


static constexpr bool render_track_path = true;


static auto& rnd = core::renderer;

namespace game
{
    static void render_cars();
    static void render_track();
}

void game::render(scene_uid)
{
    SDL_RenderClear(rnd);

    render_track();

    render_cars();

    SDL_RenderPresent(rnd);
}


void game::render_cars()
{

}

void game::render_track()
{
    auto& track = game::track(current_track);
    SDL_RenderCopy(rnd, track.tex, nullptr, nullptr);

    if(render_track_path && !track.path.empty()) {
        SDL_SetRenderDrawColor(rnd, 255, 0, 0, 255);
        SDL_RenderDrawLinesF(rnd, track.path.data(), track.path.size());
        SDL_RenderDrawLineF(rnd,
            track.path.begin()->x,
            track.path.begin()->y,
            track.path.rbegin()->x,
            track.path.rbegin()->y
        );
    }
}
