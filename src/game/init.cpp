#include "game/game.hpp"
#include "game/data.hpp"
#include <iostream>

using std::cout;
using std::endl;


void game::init(int, char**, scene_uid)
{
    

#ifndef __EMSCRIPTEN__
    tires = 20;
#endif
}

void game::deinit(scene_uid)
{

}
