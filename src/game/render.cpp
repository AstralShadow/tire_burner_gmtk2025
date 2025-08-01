#include "game/game.hpp"
#include "game/data.hpp"
#include "core/core.hpp"
#include "utils/pi.hpp"
#include "utils/fonts.hpp"
#include "utils/textures.hpp"
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_ttf.h>
#include <algorithm>
#include <format>
#include <iostream>

using std::cout;
using std::endl;


static constexpr bool render_track_path = true;


static auto& rnd = core::renderer;

namespace game
{
    static void render_track();
    static void render_cars();
    static void render_stats();
}

void game::render(scene_uid)
{
    SDL_RenderClear(rnd);

    render_track();
    render_cars();

    render_stats();

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

        FPoint offset_delta {
            car.offset * cos(rotation),
            car.offset * sin(rotation)
        };

        FPoint center = p1 + delta + offset_delta;

        SDL_Rect dest {
            static_cast<int>(center.x) - type.size.x / 2,
            static_cast<int>(center.y) - type.size.y / 2,
            type.size.x, type.size.y
        };


        SDL_RenderCopyEx(rnd, type.tex, nullptr, &dest, degrees, nullptr, SDL_FLIP_NONE);
    }
}


static string format_number(double number, bool floating = true)
{
    if(number < 1000 && floating)
        return std::format("{:.2f}", number);
    else if(number < 1000 && !floating)
        return std::format("{:.0f}", number);
    else if (number < 1000 * 1000)
        return std::format("{:.3f}K", number / 1000);
    else if (number < 1000 * 1000 * 1000)
        return std::format("{:.3f}M", number / 1000000);
    else
        return std::format("{:.2e}", number);
}

void game::render_stats()
{
    constexpr float px_to_meter = 0.06;

    double mileage = 0;
    double laps = 0;
    for(auto& car : cars) {
        auto const& track = game::track(car.track);
        mileage += track.lap_len * px_to_meter * car.laps + car.pos * px_to_meter;
        laps += car.laps + car.pos / track.lap_len;
    }

    string text = "Mileage: " + format_number(mileage) + "m\n";
    text += "Loops: " + format_number(laps) + " laps\n";
    if(tires > 0)
        text += "Tested tires: " + format_number(tires, false) + " tires\n";


    SDL_Color color {0, 0, 255, 255};
    auto font = get_font(FT_DEFAULT, 24);
    auto surf = TTF_RenderUTF8_Blended_Wrapped(font, text.c_str(), color, 0);
    if(!surf) {
        cout << "Failed to render text" << endl;
        cout << TTF_GetError() << endl;
        return;
    }

    auto texture = utils::create_texture(surf);
    Point size { surf->w, surf->h };
    SDL_FreeSurface(surf);

    SDL_Rect dest { 10, 10, size.x, size.y };

    SDL_RenderCopy(rnd, texture, nullptr, &dest);

    SDL_DestroyTexture(texture);
}
