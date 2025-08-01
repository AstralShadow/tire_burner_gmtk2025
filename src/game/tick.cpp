#include "game/game.hpp"
#include "game/data.hpp"


void game::tick(u32 ms, scene_uid)
{
    for(auto& car : cars) {
        auto const& type = car_type(car.type);
        auto const& track = game::track(car.track);
        car.pos += type.speed * ms / 1000.0f;
        if(car.pos > track.lap_len) {
            car.pos -= track.lap_len;
            car.laps++;
        }
    }
}
