#include "game/game.hpp"
#include "game/data.hpp"
#include <iostream>

using std::cout;
using std::endl;


void game::init(int, char**, scene_uid)
{
    // Preload static resources
    for(size_t i = 0; i < CAR_LAST; i++)
        car_type(static_cast<CarEnum>(i));

    for(size_t i = 0; i < TRACK_LAST; i++)
        track(static_cast<TrackEnum>(i));

#ifndef __EMSCRIPTEN__
    tires = 20;
#endif
}

void game::deinit(scene_uid)
{

}
