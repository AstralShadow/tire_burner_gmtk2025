#include "startup/startup.hpp"
#include "startup/logo.hpp"
#include "utils/screen.hpp"
#include "core/scene.hpp"

SDL_Rect startup::gmtk_logo_pos {0, 0, 1, 1};
u32 startup::time = 0;


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


    auto target_x = (screen.x - logo_size.x) / 2;
    auto target_y = (screen.y - logo_size.y) / 2;

    if(rate2 == 0) {
        gmtk_logo_pos = {
            target_x,
            static_cast<int>
                (screen.y * (1 - rate1)
                    + target_y * rate1),
            logo_size.x,
            logo_size.y
        };
    } else {
        gmtk_logo_pos = {
            target_x,
            static_cast<int>
                (screen.y * rate2
                    + target_y * (1 - rate2)),
            logo_size.x,
            logo_size.y
        };
    }

    if(time >= next_scene_t)
        core::set_scene(next_scene);

}
