#include "game/game.hpp"
#include "game/data.hpp"
#include "core/core.hpp"
#include <SDL2/SDL_events.h>


void game::mousedown(SDL_MouseButtonEvent& ev, scene_uid)
{
    static auto car = CAR_01;

    cars.push_back(Car {
        .track = TRACK_01,
        .type = car,
    });

    car = static_cast<CarEnum>(car + 1);
    if(car == CAR_LAST)
        car = CAR_01;
}

