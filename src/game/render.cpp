#include "game/game.hpp"
#include "game/data.hpp"
#include "game/render.hpp"
#include "core/core.hpp"
#include "utils/pi.hpp"
#include "utils/fonts.hpp"
#include "utils/textures.hpp"
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_ttf.h>
#include <map>
#include <algorithm>
#include <format>
#include <iostream>

using std::map;
using std::cout;
using std::endl;


#ifdef __EMSCRIPTEN__
static constexpr bool render_debug_lines = false;
#else
static constexpr bool render_debug_lines = true;
#endif

static constexpr bool render_track_path = render_debug_lines;
static constexpr bool render_button_area = render_debug_lines;



static auto& rnd = core::renderer;

namespace game
{
    static void render_track();
    static void render_cars();

    static void render_track_overlay();
    static void render_stats();
    static void render_car_buttons();
    static void render_track_buttons();
    static void render_price(SDL_Rect area, string const& price, SDL_Color color);

    static void render_profit_particles();
}

void game::render(scene_uid)
{
    SDL_RenderClear(rnd);

    render_track();
    render_cars();

    render_track_overlay();
    render_stats();
    render_car_buttons();
    render_track_buttons();

    render_profit_particles();

    SDL_RenderPresent(rnd);
}


void game::render_track()
{
    auto const& track = game::track(current_track);
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

FPoint game::car_pos(Car const& car, float* _rotation)
{
    auto const& track = game::track(car.track);

    float pos = car.pos;

    auto const& path = car.on_entrance ? track.entrance : track.path;
    auto const& lens = car.on_entrance ? track.entrance_lens : track.path_lens;

    auto itr = std::lower_bound(lens.begin(), lens.end(), pos);
    auto i1 = std::distance(lens.begin(), itr);
    auto i2 = (i1 + 1) % path.size();

    FPoint p1 = path[i1];
    FPoint p2 = path[i2];
    FPoint delta = p2 - p1;

    float rotation = atan2(delta.y, delta.x) + pi_f() / 2;
    if(_rotation)
        *_rotation = rotation;

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

    return center;
}

void game::render_cars()
{
    for(auto& car : cars) {
        auto const& type = car_type(car.type);

        float rotation;
        FPoint center = car_pos(car, &rotation);

        float degrees = rotation * 180 / pi_f();

        SDL_Rect dest {
            static_cast<int>(center.x) - type.size.x / 2,
            static_cast<int>(center.y) - type.size.y / 2,
            type.size.x, type.size.y
        };

        bool burning = car.burning_tires && car.stopped < 0.6;
        SDL_RenderCopyEx(rnd, burning ? type.tex2 : type.tex, nullptr, &dest, degrees, nullptr, SDL_FLIP_NONE);
    }
}


void game::render_track_overlay()
{
    static auto texture = utils::load_texture(menu_track_overlay);
    SDL_RenderCopy(rnd, texture, nullptr, nullptr);
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
    constexpr Point pos {21, 27};

    double mileage = 0;
    double laps = 0;
    for(auto& car : cars) {
        auto const& track = game::track(car.track);
        mileage += track.lap_len * px_to_meter * car.laps + car.pos * px_to_meter;
        mileage += car.stashed_mileage * px_to_meter;
        if(!car.on_entrance)
            laps += car.laps + car.pos / track.lap_len;
    }

    string text = "Cars: " + format_number(cars.size(), false) + "\n";
    text += "Mileage: " + format_number(mileage, false) + "m\n";
    text += "Loops: " + format_number(laps) + "\n";
    if(tires > 0)
        text += "Burned: " + format_number(tires, false) + " tires\n";


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


void game::render_profit_particles()
{
    // Cached by tires count
    static std::map<size_t, SDL_Texture*> textures;
    static std::map<size_t, Point> texture_sizes;

    for(auto p : tire_profit_particles) {
        if(p.track != current_track)
            continue;

        auto itr = textures.find(p.tires);
        auto size_itr = texture_sizes.find(p.tires);
        if(itr == textures.end()) {
            auto font = get_font(FT_DEFAULT, 24);
            auto text = std::format("{} tires!", p.tires);
            auto surf = TTF_RenderUTF8_Blended(font, text.c_str(), profit_particle_color);
            if(!surf) {
                cout << "Failed to render text" << endl;
                cout << TTF_GetError() << endl;
                return;
            }

            itr = textures.emplace(p.tires, std::move(utils::create_texture(surf))).first;
            size_itr = texture_sizes.emplace(p.tires, Point { surf->w, surf->h }).first;
            SDL_FreeSurface(surf);
        }

        SDL_Rect area {
            static_cast<int>(p.pos.x - size_itr->second.x / 2),
            static_cast<int>(p.pos.y - size_itr->second.y / 2),
            size_itr->second.x, size_itr->second.y
        };

        SDL_RenderCopy(rnd, itr->second, nullptr, &area);
    }
}


void game::render_price(SDL_Rect area, string const& price, SDL_Color color)
{
    auto font = get_font(FT_DEFAULT, 24);
    auto surf = TTF_RenderUTF8_Blended(font, price.c_str(), color);
    if(!surf) {
        cout << "Failed to render text" << endl;
        cout << TTF_GetError() << endl;
        return;
    }
    
    auto texture = utils::create_texture(surf);
    Point size { surf->w, surf->h };
    SDL_FreeSurface(surf);

    SDL_Rect dest {
        area.x + (area.w - size.x) / 2,
        area.y + area.h - size.y,
        size.x, size.y
    };

    SDL_RenderCopy(rnd, texture, nullptr, &dest);

    SDL_DestroyTexture(texture);
}

void game::render_car_buttons()
{
    auto const& track = game::track(current_track);
    size_t car_count = 0, max_cars = track.max_cars;
    array<size_t, car_types_per_track> type_count {};
    for(auto const& car : cars)
        if(car.track == current_track) {
            car_count++;
            type_count[car.type % car_types_per_track]++;
        }

    bool has_to_scrap = car_count >= max_cars;

    for(size_t i = 0; i < car_types_per_track; i++) {
        auto new_car_button = new_car_buttons[i];
        if(new_car_timeouts[i] > 0) {
            SDL_SetRenderDrawColor(rnd, 64, 64, 64, 255);
            auto area = new_car_button;
            area.y += area.h;
            area.h = (area.h * new_car_timeouts[i] * 1.0f / new_car_timeout);
            area.y -= area.h;
            SDL_RenderFillRect(rnd, &area);
        }

        if(render_button_area) {
            SDL_SetRenderDrawColor(rnd, 0, 0, 0, 255);
            SDL_RenderDrawRect(rnd, &new_car_button);
        }

        auto const& type = car_type(static_cast<CarEnum>(i + car_types_per_track * current_track));
        SDL_Rect area {
            new_car_button.x + new_car_button_icon.x + (new_car_button_icon.w - type.size.x) / 2,
            new_car_button.y + new_car_button_icon.y + (new_car_button_icon.h - type.size.y) / 2,
            type.size.x, type.size.y
        };
        SDL_RenderCopyEx(rnd, type.tex, nullptr, &area, 90, nullptr, SDL_FLIP_NONE);

        string price_tag = format_number(type.price, false) + " tires";

        bool free = cars.empty() && i == 0;

        if(free) {
            render_price(new_car_button, "Free", price_color);
        } else if(!has_to_scrap) {
            auto color = type.price > tires ? price_color_too_expensive : price_color;

            render_price(new_car_button, price_tag, color);
        } else {
            static const string scrap_tag = "Scrap";

            if(type_count[i] > 0)
                render_price(new_car_button, scrap_tag, scrap_color);
            else
                render_price(new_car_button, price_tag, price_color_no_space);
        }
    }
}

void game::render_track_buttons()
{
    for(size_t i = 0; i < map_buttons.size(); i++) {
        if(i == 0 && current_track == 0)
            continue;
        if(i == 1 && current_track == TRACK_LAST - 1)
            continue;

        auto const* area_ptr = &map_buttons[i];

        if(render_button_area) {
            SDL_SetRenderDrawColor(rnd, 0, 0, 0, 255);
            SDL_RenderDrawRect(rnd, area_ptr);
        }

        // TODO render icon

        string tag = (i == 0 ? "Prev track" : "Next track");

        auto color = prev_map_tag_color;

        if(i == 1 && current_track + 1 == unlocked_tracks_end) {
            auto const& track = game::track(static_cast<TrackEnum>(current_track + 1));
            tag = format_number(track.price, false) + " tires";

            color = track.price <= tires ? price_color : price_color_too_expensive;
        }

        render_price(*area_ptr, tag, color);
    }
}
