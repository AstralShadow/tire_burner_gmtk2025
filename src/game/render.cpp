#include "game/game.hpp"
#include "game/data.hpp"
#include "game/render.hpp"
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


#ifdef __EMSCRIPTEN__
static constexpr bool render_debug_lines = false;
#else
static constexpr bool render_debug_lines = true;
#endif

static constexpr bool render_track_path = render_debug_lines;
static constexpr bool render_button_area = render_debug_lines;

static constexpr SDL_Color stats_color {0, 0, 0, 255};
static constexpr SDL_Color price_color {0, 255, 0, 255};
static constexpr SDL_Color price_color_too_expensive {255, 0, 0, 255};


static auto& rnd = core::renderer;

namespace game
{
    static void render_track();
    static void render_cars();
    static void render_stats();
    static void render_buttons();
    static size_t render_price(SDL_Rect area, string price, SDL_Color color);
}

void game::render(scene_uid)
{
    SDL_RenderClear(rnd);

    render_track();
    render_cars();

    render_stats();
    render_buttons();

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

    if(render_track_path && !track.entrance.empty()) {
        SDL_SetRenderDrawColor(rnd, 255, 0, 0, 255);
        SDL_RenderDrawLinesF(rnd, track.entrance.data(), track.entrance.size());
        SDL_RenderDrawLineF(rnd,
            track.entrance.rbegin()->x,
            track.entrance.rbegin()->y,
            track.path.begin()->x,
            track.path.begin()->y
        );
    }
}

void game::render_cars()
{
    for(auto& car : cars) {
        auto const& track = game::track(car.track);
        auto const& type = car_type(car.type);
        auto pos = car.pos;

        auto const& path = car.on_entrance ? track.entrance : track.path;
        auto const& lens = car.on_entrance ? track.entrance_lens : track.path_lens;

        auto itr = std::lower_bound(lens.begin(), lens.end(), pos);
        auto i1 = std::distance(lens.begin(), itr);
        auto i2 = (i1 + 1) % path.size();

        FPoint p1 = path[i1];
        FPoint p2 = path[i2];
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
        return std::format("{:.1f}K", number / 1000);
    else if (number < 1000 * 1000 * 1000)
        return std::format("{:.1f}M", number / 1000000);
    else
        return std::format("{:.3e}", number);
}

void game::render_stats()
{
    constexpr float px_to_meter = 0.06;
    constexpr Point pos {21, 27};

    double mileage = 0;
    double laps = 0;
    for(auto& car : cars) {
        auto const& track = game::track(car.track);
        mileage += track.lap_len * px_to_meter * car.laps + car.pos * px_to_meter;
        laps += car.laps + car.pos / track.lap_len;
    }

    string text = "Cars: " + format_number(cars.size(), false) + "\n";
    text += "Mileage: " + format_number(mileage, false) + "m\n";
    text += "Loops: " + format_number(laps) + "\n";
    if(tires > 0)
        text += "\nTested: " + format_number(tires, false) + " tires\n";


    auto font = get_font(FT_DEFAULT, 24);
    auto surf = TTF_RenderUTF8_Blended_Wrapped(font, text.c_str(), stats_color, 0);
    if(!surf) {
        cout << "Failed to render text" << endl;
        cout << TTF_GetError() << endl;
        return;
    }

    auto texture = utils::create_texture(surf);
    Point size { surf->w, surf->h };
    SDL_FreeSurface(surf);

    SDL_Rect dest { pos.x, pos.y, size.x, size.y };

    SDL_RenderCopy(rnd, texture, nullptr, &dest);

    SDL_DestroyTexture(texture);
}


size_t game::render_price(SDL_Rect area, string price, SDL_Color color)
{
    auto font = get_font(FT_DEFAULT, 24);
    auto surf = TTF_RenderUTF8_Blended_Wrapped(font, price.c_str(), color, 0);
    if(!surf) {
        cout << "Failed to render text" << endl;
        cout << TTF_GetError() << endl;
        return area.h + area.y + 10;
    }
    
    auto texture = utils::create_texture(surf);
    Point size { surf->w, surf->h };
    SDL_FreeSurface(surf);

    SDL_Rect dest {
        area.x + (area.w - size.x) / 2,
        area.y + area.h - size.y - 10,
        size.x, size.y
    };

    SDL_RenderCopy(rnd, texture, nullptr, &dest);

    SDL_DestroyTexture(texture);

    return dest.y + dest.y + 10;
}

void game::render_buttons()
{
    if(render_button_area) {
        SDL_SetRenderDrawColor(rnd, 0, 0, 0, 255);
        SDL_RenderDrawRect(rnd, &new_car_button);
    }

    auto type = car_type(CAR_01);
    SDL_Rect area {
        new_car_button_icon.x + (new_car_button_icon.w - type.size.x) / 2,
        new_car_button_icon.y + (new_car_button_icon.h - type.size.y) / 2,
        type.size.x, type.size.y
    };
    SDL_RenderCopyEx(rnd, type.tex, nullptr, &area, 90, nullptr, SDL_FLIP_NONE);

    string price_tag = format_number(type.price, false) + " tires";
    auto color = type.price > tires ? price_color_too_expensive : price_color;

    area.y = render_price(new_car_button, price_tag, color);
}
