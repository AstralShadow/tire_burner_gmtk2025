#include "game/data.hpp"

using namespace game;


TrackEnum game::current_track = TRACK_01;
TrackEnum game::unlocked_end = static_cast<TrackEnum>(current_track + 1);

vector<Car> game::cars;


size_t game::tires = 0;
