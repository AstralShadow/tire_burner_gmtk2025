#include "game/game.hpp"
#include "game/data.hpp"
#include "utils/trandom.hpp"


void game::tick(u32 ms, scene_uid)
{
    for(auto& car : cars) {
        auto const& type = car_type(car.type);
        auto const& track = game::track(car.track);
        car.pos += type.speed * ms / 1000.0f;
        if(car.pos > track.lap_len) {
            car.pos -= track.lap_len;
            car.laps++;
            if(car.laps % track.loops_per_tire_change == 0)
                tires += type.tires;
        }


        if(std::abs(car.target_offset - car.offset) < 5) {
            struct CarOffsetRNG;
            float leeway = (track.path_width - type.size.x - 5);
            car.target_offset = leeway * (randomf<CarOffsetRNG>() - 0.5);
        }

        float off_delta = car.target_offset - car.offset;
        if(off_delta > 0)
            car.offset += ms * type.speed / (75 * 1000.0f);
        if(off_delta < 0)
            car.offset -= ms * type.speed / (75 * 1000.0f);
    }
}
