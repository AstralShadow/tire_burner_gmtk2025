#include "game/game.hpp"
#include "game/data.hpp"
#include "game/render.hpp"
#include "core/core.hpp"
#include "utils/pi.hpp"
#include "utils/fonts.hpp"
#include "utils/textures.hpp"
#include "menu/menu.hpp"
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
static constexpr bool always_display_car_stats = false;
#else
static constexpr bool render_debug_lines = true;
static constexpr bool always_display_car_stats = true;
#endif

static constexpr bool render_track_path = render_debug_lines;
static constexpr bool render_button_area = render_debug_lines;

bool game::render_help = false;


static auto& rnd = core::renderer;

namespace game
{
    static void render_track();
    static void render_cars();

    static void render_track_overlay();
    static void render_stats();
    static void render_car_buttons();
    static void render_scrap_button();
    static void render_track_buttons();
    static void render_price(SDL_Rect area, string const& price, SDL_Color color, size_t font_size = 24);

    static void render_help_overlay();

    static void render_profit_particles();

    static void render_scene_transition();
}

void game::render(scene_uid)
{
    SDL_RenderClear(rnd);

    render_track();
    render_cars();

    render_track_overlay();
    render_stats();
    render_car_buttons();
    render_scrap_button();
    render_track_buttons();

    render_help_overlay();

    render_profit_particles();

    if(!start_transition_done) // the other scene's render has a RenderPresent call
        render_scene_transition();
    else 
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
        if(car.track != current_track)
            continue;

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

    double mileage = deleted_mileage;
    double laps = deleted_laps;
    static std::map<TrackEnum, size_t> cars_on_tracks;
    cars_on_tracks.clear();

    for(auto& car : game::cars) {
        auto itr = cars_on_tracks.find(car.track);
        if(itr == cars_on_tracks.end())
            cars_on_tracks[car.track] = 1;
        else
            itr->second += 1;

        auto const& track = game::track(car.track);
        mileage += track.lap_len * px_to_meter * car.laps + car.pos * px_to_meter;
        mileage += car.stashed_mileage * px_to_meter;
        if(!car.on_entrance)
            laps += car.laps + car.pos / track.lap_len;
    }

    string cars = "";
    for(int i = TRACK_LAST - 1; i >= TRACK_01; i--) {
        size_t count = cars_on_tracks[static_cast<TrackEnum>(i)];
        if(!cars.empty())
            cars = "+" + cars;
        if(count > 0 || !cars.empty())
            cars = std::to_string(count) + cars;
    }
    if(cars.empty())
        cars = "0";

    string text = "Cars: " + cars + "\n";
    if(mileage > 0.0)
        text += "Mileage: " + format_number(mileage, false) + "m\n";
    if(laps > 1.0)
        text += "Loops: " + format_number(laps) + "\n";
    if(discovered_tires || tires > 0) {
        text += "Burned: " + format_number(tires, false) + " tires\n";
        discovered_tires = true;
    }


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
    static std::map<size_t, SDL_Texture*> white_textures;
    static std::map<size_t, Point> white_texture_sizes;

    // Gray version for ones from another track (hidden ones)
    static std::map<size_t, SDL_Texture*> gray_textures;
    static std::map<size_t, Point> gray_texture_sizes;

    for(auto p : tire_profit_particles) {
        bool same_track = p.track == current_track;
        auto& textures = same_track ? white_textures : gray_textures;
        auto& texture_sizes = same_track ? white_texture_sizes : white_texture_sizes;

        auto itr = textures.find(p.tires);
        auto size_itr = texture_sizes.find(p.tires);
        if(itr == textures.end()) {
            auto font = get_font(FT_DEFAULT, 24);
            auto text = std::format("{} tires!", p.tires);
            auto color = same_track ? profit_particle_color : profit_particle_color_hidden;
            auto surf = TTF_RenderUTF8_Blended(font, text.c_str(), color);
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


void game::render_price(SDL_Rect area, string const& price, SDL_Color color, size_t font_size)
{
    auto font = get_font(FT_DEFAULT, font_size);
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

    bool has_to_scrap = car_count >= max_cars || (scrap_mode && cars.size() > 1);

    for(size_t i = 0; i < car_types_per_track; i++) {
        auto new_car_button = new_car_buttons[i];

        auto expense = spent_tires_at_purchase[current_track];
        auto enum_type = static_cast<CarEnum>(i + car_types_per_track * current_track);
        auto const& type = car_type(enum_type);
        if(type.price > total_tires - expense && enum_type != CAR_01)
            continue;

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
        

        SDL_Rect area {
            new_car_button.x + new_car_button_icon.x + (new_car_button_icon.w - type.size.x) / 2,
            new_car_button.y + new_car_button_icon.y + (new_car_button_icon.h - type.size.y) / 2,
            type.size.x, type.size.y
        };
        SDL_RenderCopyEx(rnd, type.tex, nullptr, &area, 90, nullptr, SDL_FLIP_NONE);

        string price_tag = format_number(type.price, false) + (discovered_tires ? " tires" : " things");

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


        // Display info
        if(total_tires < car_type(CAR_03).price && !always_display_car_stats)
            continue;

        Point mouse;
        SDL_GetMouseState(&mouse.x, &mouse.y);
        if(SDL_PointInRect(&mouse, &new_car_button)) {
            static auto frame = utils::load_texture(text_box_path);
            constexpr Point size { 225, 107 };

            SDL_Rect dest {
                new_car_button.x - size.x - 10,
                new_car_button.y + (new_car_button.h - size.y) / 2,
                size.x, size.y
            };

            SDL_RenderCopy(rnd, frame, nullptr, &dest);

            Point info_size = type.info_size;
            SDL_Rect dest2 {
                dest.x + dest.w / 2 - info_size.x / 2,
                dest.y + dest.h / 2 - info_size.y / 2,
                info_size.x, info_size.y
            };

            SDL_RenderCopy(rnd, type.info, nullptr, &dest2);
        }
    }
}

void game::render_scrap_button()
{
    if(!discovered_car_limit)
        return;

    if(render_button_area) {
        SDL_SetRenderDrawColor(rnd, 0, 0, 0, 255);
        SDL_RenderDrawRect(rnd, &scrap_toggle_button);
    }

    size_t count = 0;
    for(auto const& c : cars)
        if(c.track == current_track)
            count++;

    size_t cap = track(current_track).max_cars;

    if(discovered_car_limit)
        render_price(scrap_toggle_top_label, std::format("{}/{} cars", count, cap), car_limit_color);

    if(discovered_scrap_option) {
        auto color = scrap_mode ? scrap_enabled_color : scrap_disabled_color;
        auto text = scrap_mode ? "SCRAP!" : "scrap?";
        if(cars.size() == 1) {
            text = "no scrap!";
            color = scrap_disabled_color;
        }
        render_price(scrap_toggle_button, text, color);
    }
}

void game::render_track_buttons()
{
    for(size_t i = 0; i < map_buttons.size(); i++) {
        if(i == 0 && current_track == 0)
            continue;
        if(i == 1 && current_track == TRACK_LAST - 1)
            continue;


        auto const& track = game::track(static_cast<TrackEnum>(current_track + (i ? 1 : -1)));
        if(i == 1 && track.price > total_tires)
            continue;

        auto const* area_ptr = &map_buttons[i];

        if(render_button_area) {
            SDL_SetRenderDrawColor(rnd, 0, 0, 0, 255);
            SDL_RenderDrawRect(rnd, area_ptr);
        }

        auto icon = track.icon;
        SDL_Rect icon_area {
            area_ptr->x + map_button_icon.x,
            area_ptr->y + map_button_icon.y,
            map_button_icon.w, map_button_icon.h
        };

        SDL_RenderCopy(rnd, icon, nullptr, &icon_area);


        string tag = (i == 0 ? "Prev track" : "Next track");

        auto color = prev_map_tag_color;

        if(i == 1 && current_track + 1 == unlocked_tracks_end) {
            tag = format_number(track.price, false) + " tires";

            color = track.price <= tires ? price_color : price_color_too_expensive;
        }

        render_price(*area_ptr, tag, color);
    }


    // No more maps notice
    if(current_track == TRACK_LAST - 1) {
        auto area = map_buttons[1];

        if(render_button_area) {
            SDL_SetRenderDrawColor(rnd, 0, 0, 0, 255);
            SDL_RenderDrawRect(rnd, &area);
        }

        // In reverse order
        render_price(area, "Have fun!", last_track_notice_color, 24);
        area.h -= 28;
        render_price(area, "click & photo", last_track_notice_color, 18);
        area.h -= 22;
        render_price(area, "min max", last_track_notice_color, 18);
        area.h -= 22;
        render_price(area, "no more tHraX", last_track_notice_color, 18);
        area.h += 22 + 22 + 28;


        // Extra stats
        Point mouse;
        SDL_GetMouseState(&mouse.x, &mouse.y);
        if(!SDL_PointInRect(&mouse, &area) && !show_extra_stats)
            return;


        static auto frame = utils::load_texture(text_box_path); // TODO don't load twice?
        constexpr Point size { 225, 107 };


        SDL_Rect dest {
            area.x - size.x - 10,
            area.y + (area.h - size.y) / 2,
            size.x, size.y
        };

        SDL_RenderCopy(rnd, frame, nullptr, &dest);


        string final_stats = std::format(
            "Total tires: {:.1e}\nTime played: {:.0f}s\nScrapped cars: {}\nThanks for playing!",
            total_tires * 1.0,
            (SDL_GetTicks() - start_time) / 1000.0,
            deleted_cars
        );

        auto font = get_font(FT_DEFAULT, 18);
        auto surface = TTF_RenderUTF8_Blended_Wrapped(font, final_stats.c_str(), car_info_color, 0);
        if(!surface) {
            cout << "Failed to render text" << endl;
            cout << TTF_GetError() << endl;
            return;
        }
        auto final_stats_texture = utils::create_texture(surface);
        Point info_size { surface->w, surface->h };
        SDL_FreeSurface(surface);


        SDL_Rect dest2 {
            dest.x + dest.w / 2 - info_size.x / 2,
            dest.y + dest.h / 2 - info_size.y / 2,
            info_size.x, info_size.y
        };

        SDL_RenderCopy(rnd, final_stats_texture, nullptr, &dest2);
    }
}


void game::render_help_overlay()
{
    static bool new_help = false;

    auto help_text_bg = new_help ? game::help_text_bg_new : game::help_text_bg;
    auto help_color = new_help ? game::help_color_new : game::help_color;

    SDL_SetRenderDrawBlendMode(rnd, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(rnd, help_text_bg.r, help_text_bg.g, help_text_bg.b, help_text_bg.a);
    SDL_RenderFillRect(rnd, &help_area);
    SDL_SetRenderDrawBlendMode(rnd, SDL_BLENDMODE_NONE);
    SDL_RenderDrawRect(rnd, &help_area);

    auto area = help_area;
    area.h -= 4;
    render_price(area, "Need help?", help_color, 18);

    string info =
        "This idle game is about wearing down tires.\n\n"
        "Buy cars from the right and wait for them\n"
        "to wear down the tires. Then buy more cars.\n\n"
        "Your progress is marked at the top left.\n"
        "As you progress you'll unlock more stuff.\n"
        "You may check this help section again then.";

    if(discovered_tires) {
        info += "\n\n"
        "Tires. Burned tires. Your currency.";
    }

    if(discovered_car_limit) {
        info += "\n\n"
        "Tracks can only contain so much cars.";
        if(discovered_scrap_option)
            info += "\nYou can scrap some of the cars.\n"
                "You won't get your tires back.\n"
                "You only get space for better cars.";
    }

    static string last_info;

    if(!render_help) {
        if(info != last_info)
            new_help = true;
        return;
    }
    new_help = false;

    static SDL_Texture* texture = nullptr;
    static Point size;


    if(info != last_info || texture == nullptr) {
        auto font = get_font(FT_DEFAULT, 24);
        auto surface = TTF_RenderUTF8_Blended_Wrapped(font, info.c_str(), help_color, 0);
        if(!surface) {
            cout << "Failed to render text" << endl;
            cout << TTF_GetError() << endl;
            return;
        }
        texture = utils::create_texture(surface);
        size = Point{ surface->w, surface->h };
        SDL_FreeSurface(surface);

        last_info = info;
    }

    SDL_Rect dest2 {
        512 + - size.x / 2,
        360 - size.y / 2,
        size.x, size.y
    };

    SDL_SetRenderDrawBlendMode(rnd, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(rnd, help_text_bg.r, help_text_bg.g, help_text_bg.b, help_text_bg.a);
    SDL_RenderFillRect(rnd, &dest2);
    SDL_SetRenderDrawBlendMode(rnd, SDL_BLENDMODE_NONE);
    SDL_RenderDrawRect(rnd, &dest2);

    SDL_RenderCopy(rnd, texture, nullptr, &dest2);
}


void game::render_scene_transition()
{
    SDL_Rect area {
        0, static_cast<int>(-720 * start_transition),
        1024, 720
    };
    SDL_RenderSetViewport(rnd, &area);

    menu::render();

    SDL_RenderSetViewport(rnd, nullptr);
}
