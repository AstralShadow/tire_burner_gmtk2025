#pragma once

#include "game/track.hpp"
#include "game/car.hpp"
#include <vector>

using std::vector;


namespace game
{
    constexpr float px_to_meter = 0.06;
    constexpr size_t car_types_per_track = 4; // define buttons in render.hpp


    extern TrackEnum current_track;
    extern TrackEnum unlocked_tracks_end;


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

        double converted_meters_into_tires = 0; // how much of the mileage was converted into tires

        bool burning_tires = false;
    };

    extern vector<Car> cars;

    FPoint car_pos(Car const&, float* rotation = nullptr);

    extern size_t total_tires;
    extern size_t tires;
    extern bool discovered_tires;
};
