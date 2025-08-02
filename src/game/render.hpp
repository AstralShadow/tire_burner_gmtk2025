#pragma once

#include <array>
#include <vector>

using std::array;
using std::vector;


namespace game
{
    enum TrackEnum : u8;

    constexpr array<SDL_Rect, 4> new_car_buttons { SDL_Rect
        {855, 36, 140, 84},
        {855, 130, 140, 84},
        {855, 224, 140, 84},
        {855, 318, 140, 84},
    };
    extern array<u32, 4> new_car_timeouts;
    constexpr u32 new_car_timeout = 1500; // ms

    constexpr SDL_Rect new_car_button_icon {
        0, 0, 140, 50
    };


    constexpr array<SDL_Rect, 2> map_buttons { SDL_Rect
        {855, 486, 140, 94},
        {855, 590, 140, 94}
    };


    struct TireProfitParticle
    {
        TrackEnum track;
        FPoint pos;
        float lifetime = 1.5f; // seconds

        FPoint motion;
        size_t tires;

        static constexpr float speed = 60.0f; // px/s
    };
    extern vector<TireProfitParticle> tire_profit_particles;
    void spawn_profit_particle(FPoint pos, size_t tires);


    constexpr SDL_Color stats_color {255, 255, 255, 255};
    constexpr SDL_Color price_color {0, 255, 0, 255};
    constexpr SDL_Color price_color_too_expensive {255, 0, 0, 255};
    constexpr SDL_Color price_color_no_space {32, 32, 32, 255};
    constexpr SDL_Color scrap_color {255, 255, 0, 255};
    constexpr SDL_Color profit_particle_color {255, 255, 255, 255};

    constexpr SDL_Color prev_map_tag_color {255, 255, 255, 255};

    constexpr auto menu_track_overlay = "assets/menu/track_overlay.png";
}
