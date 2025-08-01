#pragma once
#include "utils/types.hpp"
#include <string>
#include <array>

using std::string;
using std::array;

struct SDL_Texture;


namespace game
{
    enum CarEnum
    {
        CAR_01,

        CAR_LAST
    };

    struct CarType
    {
        SDL_Texture* tex = nullptr;
    };


    CarType& car_type(CarEnum);
}
