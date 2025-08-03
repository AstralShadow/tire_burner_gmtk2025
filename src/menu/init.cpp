#include "menu/menu.hpp"
#include "menu/logo.hpp"


void menu::init(int, char**, scene_uid)
{
    // Init textures
    while(init_next()) { }
}

void menu::deinit(scene_uid)
{

}
