#pragma once
#include "utils/point.hpp"
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
        float speed;
        Point size;
        size_t tires = 4;
    };


    CarType& car_type(CarEnum);
}
