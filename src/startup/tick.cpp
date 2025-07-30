#include "startup/startup.hpp"
#include "startup/logo.hpp"
#include "utils/screen.hpp"
#include "core/scene.hpp"

SDL_Rect startup::gmtk_logo_pos {0, 0, 1, 1};
float startup::gmtk_logo_opacity = 0;
u32 startup::time = 0;
int startup::text_offset = 0;

SDL_Color startup::bg {0x05, 0x12, 0x1a, 255};

static float time2 = 0;

void startup::tick(u32 ms, scene_uid)
{
    update_screen_size();
    time += ms;


    auto screen = screen_size();
    auto logo_size = gmtk_logo_size();

    float rate1 = 1.0f * (time - enter_start)
                       / (enter_end - enter_start);
    float rate2 = 1.0f * (time - exit_start)
                       / (exit_end - exit_start);

    if(rate1 < 0 || time < enter_start)
        rate1 = 0;
    if(rate1 > 1)
        rate1 = 1;

    if(rate2 < 0 || time < exit_start)
        rate2 = 0;
    if(rate2 > 1)
        rate2 = 1;

    // Tip from Trevor, mentor in Linux Game Jam
    rate1 = (3 - 2 * rate1) * rate1 * rate1;
    rate2 = (3 - 2 * rate2) * rate2 * rate2;

    time2 += (0.3 + rate1) * ms / 3.5;

    //auto target_x = (screen.x - logo_size.x) / 2;
    auto target_y = (screen.y - logo_size.y) / 2;

    gmtk_logo_pos = {
        static_cast<int>(time2 - logo_size.x),
        target_y,
        logo_size.x,
        logo_size.y
    };


    if(rate2 == 0)
        gmtk_logo_opacity = rate1;
    else
        gmtk_logo_opacity = 1 - rate2;


    text_offset = ((time - next_scene_t / 2.0f) * 2 / next_scene_t) * 100;


    if(background_hide_start < time) {
        bg = {0x05, 0x12, 0x1a, 255};
        bg.r *= 1 - (time - background_hide_start) * 1.0f / (next_scene_t - background_hide_start);
        bg.g *= 1 - (time - background_hide_start) * 1.0f / (next_scene_t - background_hide_start);
        bg.b *= 1 - (time - background_hide_start) * 1.0f / (next_scene_t - background_hide_start);
    }



    if(time >= next_scene_t)
        core::set_scene(next_scene);
}
