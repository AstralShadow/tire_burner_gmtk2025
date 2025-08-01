#include "game/game.hpp"
#include "game/data.hpp"
#include "core/core.hpp"
#include "utils/pi.hpp"
#include <SDL2/SDL_render.h>
#include <algorithm>
#include <iostream>

using std::cout;
using std::endl;


static constexpr bool render_track_path = true;


static auto& rnd = core::renderer;

namespace game
{
    static void render_track();
    static void render_cars();
}

void game::render(scene_uid)
{
    SDL_RenderClear(rnd);

    render_track();

    render_cars();

    SDL_RenderPresent(rnd);
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

void game::render_cars()
{
    for(auto& car : cars) {
        auto const& track = game::track(car.track);
        auto const& type = car_type(car.type);
        auto pos = car.pos;

        auto itr = std::lower_bound(track.path_lens.begin(), track.path_lens.end(), pos);
        auto i1 = std::distance(track.path_lens.begin(), itr);
        auto i2 = (i1 + 1) % track.path_lens.size();

        FPoint p1 = track.path[i1];
        FPoint p2 = track.path[i2];
        FPoint delta = p2 - p1;

        float rotation = atan2(delta.y, delta.x) + pi_f() / 2;
        float degrees = rotation * 180 / pi_f();

        float len = *itr;
        float past = i1 == 0 ? 0 : *(itr - 1);
        float progress = (pos - past) / (len - past);

        delta.x *= progress;
        delta.y *= progress;

        FPoint center = p1 + delta;

        SDL_Rect dest {
            static_cast<int>(center.x) - type.size.x / 2,
            static_cast<int>(center.y) - type.size.y / 2,
            type.size.x, type.size.y
        };


        SDL_RenderCopyEx(rnd, type.tex, nullptr, &dest, degrees, nullptr, SDL_FLIP_NONE);
    }
}
