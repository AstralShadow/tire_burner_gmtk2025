#include "core/scene.hpp"

#include "startup/startup.hpp"
#include "menu/menu.hpp"
#include "game/game.hpp"


void core::register_scenes()
{
    core::scene<startup::scene_uid>("startup");
    core::scene<menu::scene_uid>("menu");
    core::scene<game::scene_uid>("game");

    core::set_scene("startup");
}
