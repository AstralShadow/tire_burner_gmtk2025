#include "game/data.hpp"
#include "game/render.hpp"
#include "utils/trandom.hpp"
#include <array>

using namespace game;
using std::array;


TrackEnum game::current_track = TRACK_01;
TrackEnum game::unlocked_tracks_end = static_cast<TrackEnum>(TRACK_01 + 1);

vector<Car> game::cars;


size_t game::tires = 0;

array<u32, car_types_per_track> game::new_car_timeouts {0, 0, 0, 0};


vector<TireProfitParticle> game::tire_profit_particles;

void game::spawn_profit_particle(FPoint pos, size_t tires)
{
    struct RNGToken;

    tire_profit_particles.push_back({
        .track = current_track,
        .pos = pos,
        .motion = { randomf<RNGToken>() - 0.5f, - randomf<RNGToken>()},
        .tires = tires
    });
}
