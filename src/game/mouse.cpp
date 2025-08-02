#include "game/game.hpp"
#include "game/data.hpp"
#include "game/render.hpp"
#include <SDL2/SDL_events.h>
#include <algorithm>


void game::mouseup(SDL_MouseButtonEvent&, scene_uid)
{ }


void game::mousedown(SDL_MouseButtonEvent& ev, scene_uid)
{
    Point pos { ev.x, ev.y };

    for(size_t i = 0; i < car_types_per_track; i++) {
        if(!SDL_PointInRect(&pos, &new_car_buttons[i]))
            continue;

        if(new_car_timeouts[i] > 0)
            return;


        auto const& track = game::track(current_track);
        size_t car_count = 0, max_cars = track.max_cars;
        array<size_t, car_types_per_track> type_count {};
        for(auto const& car : cars)
            if(car.track == current_track) {
                car_count++;
                type_count[car.type % car_types_per_track]++;
            }

        bool has_to_scrap = car_count >= max_cars;

        if(has_to_scrap && type_count[i] == 0)
            return;

        if(has_to_scrap) {
            auto itr = std::find_if(
                cars.begin(),
                cars.end(),
                [i] (auto const& car) {
                    return car.track == current_track && (car.type % car_types_per_track == i);
                }
            );
            cars.erase(itr);

        } else {
            auto car_type = static_cast<CarEnum>(i + current_track * car_types_per_track);
            if(!(cars.empty() && i == 0) && tires >= game::car_type(car_type).price) {
                tires -= game::car_type(car_type).price;
            } else if(!(cars.empty() && i == 0)) {
                return;
            }

            cars.push_back(Car {
                .track = current_track,
                .type = static_cast<CarEnum>(i + current_track * car_types_per_track)
            });
        }

        new_car_timeouts[i] = new_car_timeout;
    }
}

