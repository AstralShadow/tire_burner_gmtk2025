#pragma once

#include <SDL2/SDL_render.h>
#include "utils/point.hpp"
#include "utils/types.hpp"
#include <string>
#include <array>

using std::array;

namespace menu
{
    constexpr size_t bg_frames_count = 4;

    array<SDL_Texture*, bg_frames_count> const& bg_frames();
    constexpr Point bg_size() { return { 1024, 720 }; }

    constexpr float bg_framerate = 12.0f;
    extern size_t current_bg_frame;


    constexpr size_t title_texture_count = 2;
    array<SDL_Texture*, title_texture_count> const& title_textures();
    array<Point, title_texture_count> const& title_size();
    array<Point, title_texture_count> title_pos();


    SDL_Texture* play_button_texture();
    Point play_button_size();
    Point play_button_pos();
    bool point_in_play_btn(Point);
    extern bool enlarge_play_btn;


    constexpr auto game_scene = "game";

    bool init_next(); // return if there's more to init
}
