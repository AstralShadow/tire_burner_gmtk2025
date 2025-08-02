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
        CAR_02,
        CAR_03,
        CAR_04,

        CAR_11,
        CAR_12,
        CAR_13,
        CAR_14,

        CAR_21,
        CAR_22,
        CAR_23,
        CAR_24,

        CAR_LAST
    };

    struct CarType
    {
        SDL_Texture* tex = nullptr;
        SDL_Texture* tex2 = nullptr; // upgraded
        float speed;
        Point size;
        size_t tires;

        size_t price;
        size_t meters_per_tire_change;
    };


    CarType& car_type(CarEnum);
}
