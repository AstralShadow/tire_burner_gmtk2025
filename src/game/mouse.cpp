#include "game/game.hpp"
#include "game/data.hpp"
#include "game/render.hpp"
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_timer.h>
#include <algorithm>
#include <iostream>

using std::cout;
using std::endl;


namespace game
{
    static bool new_car_button_click_hdl(Point);
    static bool scrap_button_click_hdl(Point);
    static bool track_button_click_hdl(Point);
    static bool help_button_click_hdl(Point);
}


void game::mouseup(SDL_MouseButtonEvent&, scene_uid)
{ }


void game::mousedown(SDL_MouseButtonEvent& ev, scene_uid)
{
    Point pos { ev.x, ev.y };
    if(new_car_button_click_hdl(pos))
        return;
    if(track_button_click_hdl(pos))
        return;
    if(scrap_button_click_hdl(pos))
        return;
    if(help_button_click_hdl(pos))
        return;
}

bool game::new_car_button_click_hdl(Point pos)
{
    for(size_t i = 0; i < car_types_per_track; i++) {
        if(!SDL_PointInRect(&pos, &new_car_buttons[i]))
            continue;

        if(new_car_timeouts[i] > 0)
            return true;


        auto const& track = game::track(current_track);
        size_t car_count = 0, max_cars = track.max_cars;
        array<size_t, car_types_per_track> type_count {};
        for(auto const& car : cars)
            if(car.track == current_track) {
                car_count++;
                type_count[car.type % car_types_per_track]++;
            }

        bool has_to_scrap = (scrap_mode && !cars.empty()) || car_count >= max_cars;

        if(has_to_scrap && type_count[i] == 0)
            return true;

        if(has_to_scrap) {
            auto itr = std::find_if(
                cars.begin(),
                cars.end(),
                [i] (auto const& car) {
                    return car.track == current_track && (car.type % car_types_per_track == i);
                }
            );
            if(itr != cars.end()) {
                auto track = game::track(itr->track);
                deleted_cars++;
                deleted_mileage = itr->laps * track.lap_len + itr->pos + itr->stashed_mileage;
                deleted_laps = itr->laps;

                cars.erase(itr);

                discovered_car_limit = true;
                discovered_scrap_option = true;

                if(cars.size() == 1)
                    scrap_mode = false;
            }
        } else {
            auto car_type = static_cast<CarEnum>(i + current_track * car_types_per_track);
            if(!(cars.empty() && i == 0) && tires >= game::car_type(car_type).price) {
                tires -= game::car_type(car_type).price;
            } else if(!(cars.empty() && i == 0)) {
                return true;
            }

            if(cars.empty() && i == 0)
                start_time = SDL_GetTicks();


            cars.push_back(Car {
                .track = current_track,
                .type = static_cast<CarEnum>(i + current_track * car_types_per_track)
            });

            if(car_count == max_cars - 2)
                discovered_car_limit = true;
        }

        new_car_timeouts[i] = new_car_timeout;
        return true;
    }

    return false;
}

bool game::scrap_button_click_hdl(Point pos)
{
    if(!SDL_PointInRect(&pos, &scrap_toggle_button))
        return false;

    if(discovered_scrap_option)
        scrap_mode = !scrap_mode;

    if(cars.size() == 1 && scrap_mode)
        scrap_mode = false;

    return true;
}

bool game::track_button_click_hdl(Point pos)
{
    if(SDL_PointInRect(&pos, &map_buttons[0])) {
        if(current_track == 0)
            return true;
        current_track = static_cast<TrackEnum>(current_track - 1);
        return true;
    }

    if(SDL_PointInRect(&pos, &map_buttons[1])) {
        if(current_track >= TRACK_LAST - 1) {
            show_extra_stats = !show_extra_stats;
            return true;
        }

        auto next = static_cast<TrackEnum>(current_track + 1);

        if(next < unlocked_tracks_end) {
            current_track = next;
            return true;
        }

        auto price = track(next).price;
        if(price > tires)
            return true;

        tires -= price;

        current_track = unlocked_tracks_end;
        unlocked_tracks_end = static_cast<TrackEnum>(unlocked_tracks_end + 1);

        spent_tires_at_purchase[current_track] = total_tires - tires;

        return true;
    }

    return false;
}

bool game::help_button_click_hdl(Point pos)
{
    if(!SDL_PointInRect(&pos, &help_area))
        return false;

    render_help = !render_help;

    return true;
}
