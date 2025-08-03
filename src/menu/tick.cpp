#include "menu/menu.hpp"
#include "menu/logo.hpp"
#include "core/scene.hpp"
#include <iostream>


size_t menu::current_bg_frame = 0;
size_t menu::current_glass_frame = 0;
float menu::glass_frame_transition_progress = 0;

using std::cout;
using std::endl;

namespace menu
{
    constexpr array<Point, title_texture_count> _title_pos { Point
        { 128, 63 },
        { 108, 151 }
    };
    constexpr Point _play_btn_pos { 243, 438 };
    constexpr Point _play_btn_big_pos { _play_btn_pos.x - 254 / 20, _play_btn_pos.y - 220 / 20 };
}

array<Point, menu::title_texture_count> menu::title_pos() { return _title_pos; }
Point menu::play_button_pos() {
    return enlarge_play_btn ? _play_btn_big_pos : _play_btn_pos ;
}


bool menu::black_scene_transition_done = false;
float menu::black_scene_transition = 0;


void menu::tick(u32 ms, scene_uid)
{
    static float bg_frame_cache = 0;
    bg_frame_cache += ms * bg_framerate / 1000.0f;
    if(bg_frame_cache > 1) {
        bg_frame_cache -= 1.0f;
        current_bg_frame = (current_bg_frame + 1) % bg_frames_count;
    }


    static bool glass_waiting = true;
    static float glass_frame_cache = 0;
    glass_frame_cache += ms * glass_framerate / 1000.0f;

    if(glass_waiting && glass_frame_cache > glass_delay * glass_framerate / 1000.0f) {
        glass_frame_cache -= glass_delay * glass_framerate / 1000.0f;
        glass_waiting = false;
    } else if(!glass_waiting && glass_frame_cache > 1) {
        glass_frame_cache -= 1.0f;
        current_glass_frame = (current_glass_frame + 1) % glass_frames_count;
        if(current_glass_frame == 0)
            glass_waiting = true;
    }

    glass_frame_transition_progress = glass_waiting ? 0.0f : glass_frame_cache;


    if(!black_scene_transition_done) {
        static size_t time = 0;
        time += ms;
        black_scene_transition = time * 1.0f / black_scene_transition_time;
        if(time > black_scene_transition_time) {
            black_scene_transition_done = true;
            black_scene_transition = 1.0f;
        }
    }
}
