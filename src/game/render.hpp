#pragma once

#include <array>

using std::array;


namespace game
{
    constexpr array<SDL_Rect, 4> new_car_buttons { SDL_Rect
        {855, 36, 140, 94},
        {855, 140, 140, 94},
        {855, 244, 140, 94},
        {855, 348, 140, 94},
    };
    extern array<u32, 4> new_car_timeouts;
    constexpr u32 new_car_timeout = 1500; // ms

    constexpr SDL_Rect new_car_button_icon {
        0, 0, 140, 50
    };


    constexpr SDL_Color stats_color {255, 255, 255, 255};
    constexpr SDL_Color price_color {0, 255, 0, 255};
    constexpr SDL_Color price_color_too_expensive {255, 0, 0, 255};
    constexpr SDL_Color price_color_no_space {32, 32, 32, 255};
    constexpr SDL_Color scrap_color {255, 255, 0, 255};

    constexpr auto menu_track_overlay = "assets/menu/track_overlay.png";
}
