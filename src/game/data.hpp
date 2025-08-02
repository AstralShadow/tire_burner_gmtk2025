#pragma once

#include "game/track.hpp"
#include "game/car.hpp"
#include <vector>

using std::vector;


namespace game
{
    constexpr float px_to_meter = 0.06;


    extern TrackEnum current_track;
    extern TrackEnum unlocked_end;


    struct Car
    {
        TrackEnum track;
        CarEnum type;

        float pos = 0;
        size_t laps = 0;
        bool on_entrance = true;

        float offset = 0;
        float target_offset = 0;

        float stopped = 0.0f; // 1 - fully stopped
        float stashed_mileage = 0; // in pixels
    };

    extern vector<Car> cars;


    extern size_t tires;
};
