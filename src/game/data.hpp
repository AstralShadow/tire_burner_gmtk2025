#pragma once

#include "game/track.hpp"
#include "game/car.hpp"
#include <vector>

using std::vector;


namespace game
{
    extern TrackEnum current_track;
    extern TrackEnum unlocked_end;


    struct Car
    {
        TrackEnum track;
        CarEnum type;
        float pos = 0;
    };

    extern vector<Car> cars;
};
