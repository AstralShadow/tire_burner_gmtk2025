#include "game/game.hpp"
#include "game/data.hpp"
#include "game/render.hpp"
#include "utils/trandom.hpp"
#include <algorithm>
#include <format>
#include <iostream>

using std::cout;
using std::endl;


#ifdef __EMSCRIPTEN__
static constexpr bool debug_log = false;
#else
static constexpr bool debug_log = false;
#endif


namespace game
{
    static void move_cars(u32 ms);
    static void prevent_crashes(u32 ms);
    static void decrease_timeouts(u32 ms);
    static void move_particles(u32 ms);
}


void game::tick(u32 ms, scene_uid)
{
    move_cars(ms);
    prevent_crashes(ms);
    decrease_timeouts(ms);
    move_particles(ms);
}


void game::move_cars(u32 ms)
{
    for(auto& car : cars) {
        auto const& type = car_type(car.type);
        auto const& track = game::track(car.track);

        car.pos += (1 - car.stopped) * (car.burning_tires ? 0.85 : 1) * type.speed * ms / 1000.0f;

        if(car.on_entrance && car.pos > track.entrance_len) {
            car.stashed_mileage = track.entrance_len;
            car.pos -= track.entrance_len;
            car.on_entrance = false;
        } else if(!car.on_entrance && car.pos > track.lap_len) {
            car.pos -= track.lap_len;
            car.laps++;
        }

        double mileage = car.stashed_mileage + car.laps * track.lap_len + car.pos;
        double delta = mileage * px_to_meter - car.converted_meters_into_tires;
        if(type.meters_per_tire_change < delta) {
            tires += type.tires;
            car.converted_meters_into_tires += type.meters_per_tire_change;
            spawn_profit_particle(car_pos(car), type.tires);

            car.burning_tires = false;
        } else if(type.meters_per_tire_change * 0.9 < delta) {
            car.burning_tires = true;
        }


        if(std::abs(car.target_offset - car.offset) < 5) {
            struct CarOffsetRNG;
            auto path_width = car.on_entrance ? track.entrance_width : track.path_width;
            float leeway = (path_width - type.size.x - 5);
            car.target_offset = leeway * (randomf<CarOffsetRNG>() - 0.5);
        }

        float off_delta = car.target_offset - car.offset;
        if(off_delta > 0)
            car.offset += (1 - car.stopped) * ms * type.speed / (75 * 1000.0f);
        if(off_delta < 0)
            car.offset -= (1 - car.stopped) * ms * type.speed / (75 * 1000.0f);

        float next_off_delta = car.target_offset - car.offset;
        if((off_delta < 0 && next_off_delta > 0) || (off_delta > 0 && next_off_delta < 0))
            car.offset = car.target_offset;
    }
}

void game::prevent_crashes(u32 ms)
{
    for(size_t track_id = current_track; track_id < unlocked_tracks_end; track_id++) {
        vector<float> pos;
        vector<float> e_pos;

        for(auto& car : cars) {
            if(car.track != track_id)
                continue;

            auto const& track = game::track(car.track);
            auto const& type = game::car_type(car.type);

            if(car.on_entrance) {
                // Position on entrance
                float relative = car.pos - type.size.y / 2;
                if(relative < 0)
                    relative += track.lap_len;
                e_pos.push_back(relative);

                // Position on track
                relative = car.pos - track.entrance_len;
                float overlap = track.entrance_path_overlap + type.size.y * 1.5;
                if(-relative < overlap) {
                    relative += track.lap_len;
                    pos.push_back(relative);
                }
            } else {
                // Position on track
                float relative = car.pos - type.size.y / 2;
                if(relative < 0)
                    relative += track.lap_len;
                pos.push_back(relative);

                // Position on entrance
                relative = car.pos - type.size.y / 2 + track.entrance_len;
                auto overlap = track.entrance_path_overlap + type.size.y;
                if(car.pos - type.size.y / 2 > track.lap_len - overlap)
                    relative = track.entrance_len - track.entrance_path_overlap;
                e_pos.push_back(relative);
            }
        }

        std::sort(pos.begin(), pos.end());
        std::sort(e_pos.begin(), e_pos.end());

        if(debug_log) {
            cout << endl << "track: " << track_id << endl;

            cout << "pos: ";
            for(auto p : pos)
                cout << std::format("{:5.1f}", p) << ' ';
            cout << endl;

            cout << "e_pos: ";
            for(auto p : e_pos)
                cout << std::format("{:5.1f}", p) << ' ';
            cout << endl;
        }


        for(auto& car : cars) {
            if(car.track != track_id)
                continue;

            auto const& track = game::track(car.track);
            auto const& type = game::car_type(car.type);

            float back = car.pos - type.size.y / 2 + 1; // +1 to skip self
            float nose = car.pos + type.size.y / 2;
            if(debug_log)
                cout << std::format("nose: {:5.1f} back: {:5.1f}", nose, back) << endl;

            auto const& close_by = car.on_entrance ? e_pos : pos;
            auto next = std::lower_bound(close_by.begin(), close_by.end(), back);

            float car_len = type.size.y;
            float target_distance = car_len * 1.5;

            bool should_wait = false;
            bool hard_stop = false;

            if(next != close_by.end()) {
                if(*next < nose + type.speed / 10) {
                    should_wait = true;
                    if(*next < nose + 10)
                        hard_stop = true;
                }

                if(*next - nose < target_distance)
                    should_wait = true;

                if(car.on_entrance) {
                    float relative = car.pos - track.entrance_len;
                    float overlap = track.entrance_path_overlap + type.size.y * 1.5;
                    if(-relative < overlap) {
                        should_wait = false;
                        hard_stop = false;
                    }
                }
            }

            if(!car.on_entrance) {
                back -= track.lap_len;
                nose -= track.lap_len;
                next = std::lower_bound(close_by.begin(), close_by.end(), back);

                if(next != close_by.end()) {
                    if(*next < nose + type.speed / 10) {
                        should_wait = true;
                        if(*next < nose + 10)
                            hard_stop = true;
                    }

                    if(*next - nose < target_distance)
                        should_wait = true;
                }
            }

            if(hard_stop)
                if(car.stopped < 0.9)
                    car.stopped = 0.9;

            if(should_wait)
                car.stopped += type.speed * ms / (1000.0f * 100);
            else
                car.stopped -= type.speed * ms / (1000.0f * 200);

            if(car.stopped < 0)
                car.stopped = 0;

            if(car.stopped > 1)
                car.stopped = 1;
        }
    }
}

void game::decrease_timeouts(u32 ms)
{
    for(auto& timeout : new_car_timeouts) {
        if(timeout <= ms)
            timeout = 0;
        else
            timeout -= ms;
    }
}

void game::move_particles(u32 ms)
{
    for(auto& particle : tire_profit_particles) {
        particle.lifetime -= ms / 1000.0f;
        particle.pos.x += particle.motion.x * particle.speed * ms / 1000.0f;
        particle.pos.y += particle.motion.y * particle.speed * ms / 1000.0f;
    }

    for(size_t i = 0; i < tire_profit_particles.size(); i++) {
        if(tire_profit_particles[i].lifetime < 0.0f) {
            std::swap(tire_profit_particles[i], tire_profit_particles.back());
            tire_profit_particles.pop_back();
        }
    }
}
