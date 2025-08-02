#include "game/data.hpp"
#include "game/render.hpp"
#include <array>

using namespace game;
using std::array;


TrackEnum game::current_track = TRACK_01;
TrackEnum game::unlocked_end = static_cast<TrackEnum>(current_track + 1);

vector<Car> game::cars;


size_t game::tires = 0;

array<u32, car_types_per_track> game::new_car_timeouts {0, 0, 0, 0};
