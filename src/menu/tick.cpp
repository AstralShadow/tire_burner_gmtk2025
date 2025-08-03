#include "menu/menu.hpp"
#include "menu/logo.hpp"
#include "core/scene.hpp"


size_t menu::current_bg_frame = 0;

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


void menu::tick(u32 ms, scene_uid)
{
    static float bg_frame_cache = 0;
    bg_frame_cache += ms * bg_framerate / 1000.0f;
    if(bg_frame_cache > 1) {
        bg_frame_cache -= 1.0f;
        current_bg_frame = (current_bg_frame + 1) % bg_frames_count;
    }

}
