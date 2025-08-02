#include "game/data.hpp"
#include "game/render.hpp"
#include "utils/trandom.hpp"
#include <array>

using namespace game;
using std::array;


TrackEnum game::current_track = TRACK_01;
TrackEnum game::unlocked_tracks_end = static_cast<TrackEnum>(TRACK_01 + 1);

vector<Car> game::cars;


bool game::discovered_tires = false;
size_t game::tires = 0;
size_t game::total_tires = false;


bool game::discovered_car_limit = false;
bool game::discovered_scrap_option = false;
bool game::scrap_mode = false;


double game::deleted_mileage = 0;
double game::deleted_laps = 0;
size_t game::deleted_cars = 0;


array<size_t, TRACK_LAST> game::spent_tires_at_purchase {
    0, 0, 0, 0
};


bool game::show_extra_stats;
u32 game::start_time;


array<u32, car_types_per_track> game::new_car_timeouts {0, 0, 0, 0};


vector<TireProfitParticle> game::tire_profit_particles;

void game::spawn_profit_particle(FPoint pos, size_t tires, TrackEnum track)
{
    struct RNGToken;

    tire_profit_particles.push_back({
        .track = track,
        .pos = pos,
        .motion = { randomf<RNGToken>() - 0.5f, - randomf<RNGToken>()},
        .tires = tires
    });
}
