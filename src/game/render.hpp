#pragma once

#include "utils/types.hpp"
#include "utils/point.hpp"
#include <SDL2/SDL_pixels.h>
#include <array>
#include <vector>

using std::array;
using std::vector;


namespace game
{
    extern bool start_transition_done;
    extern float start_transition;
    constexpr size_t start_transition_time = 700;


    extern bool render_help;
    constexpr SDL_Rect help_area {
        10, 720 - 38,
        110, 28
    };


    enum TrackEnum : u8;

    constexpr array<SDL_Rect, 4> new_car_buttons { SDL_Rect
        {855, 36, 140, 84},
        {855, 130, 140, 84},
        {855, 224, 140, 84},
        {855, 318, 140, 84},
    };
    extern array<u32, 4> new_car_timeouts;
    constexpr u32 new_car_timeout = 660; // ms

    constexpr SDL_Rect new_car_button_icon {
        0, 0, 140, 50
    };


    constexpr SDL_Rect scrap_toggle_button { 855, 418, 144, 58 };
    constexpr SDL_Rect scrap_toggle_top_label { 855, 418, 144, 24 };


    constexpr array<SDL_Rect, 2> map_buttons { SDL_Rect
        {855, 486, 140, 94},
        {855, 590, 140, 94}
    };
    constexpr SDL_Rect map_button_icon {
        38, 10,
        64, 48
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
    void spawn_profit_particle(FPoint pos, size_t tires, TrackEnum track);


    constexpr SDL_Color stats_color {255, 255, 255, 255};
    constexpr SDL_Color price_color {0, 255, 0, 255};
    constexpr SDL_Color price_color_too_expensive {255, 0, 0, 255};
    constexpr SDL_Color price_color_no_space {32, 32, 32, 255};
    constexpr SDL_Color scrap_color {255, 255, 0, 255};
    constexpr SDL_Color profit_particle_color {255, 255, 255, 255};
    constexpr SDL_Color profit_particle_color_hidden {48, 48, 48, 255};

    constexpr SDL_Color prev_map_tag_color {255, 255, 255, 255};

    constexpr SDL_Color car_limit_color {255, 255, 255, 255};
    constexpr SDL_Color scrap_enabled_color {255, 255, 0, 255};
    constexpr SDL_Color scrap_disabled_color {32, 32, 32, 255};

    constexpr SDL_Color car_info_color {255, 255, 255, 255};

    constexpr SDL_Color last_track_notice_color {255, 255, 255, 255};

    constexpr SDL_Color help_color {255, 255, 255, 255};
    constexpr SDL_Color help_text_bg {64, 64, 64, 196};
    constexpr SDL_Color help_color_new {0, 0, 0, 255};
    constexpr SDL_Color help_text_bg_new {255, 64, 64, 196};


    constexpr auto menu_track_overlay = "assets/ui/track_overlay.png";
    constexpr auto text_box_path = "assets/ui/text_box.png";
}
