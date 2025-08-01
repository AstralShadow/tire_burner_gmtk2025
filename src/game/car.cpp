#include "game/car.hpp"
#include "utils/fs.hpp"
#include "utils/textures.hpp"
#include <iostream>

using std::cout;
using std::endl;


#define PATH_BASE "assets/cars/"

static constexpr std::array<const char*, game::CAR_LAST> _textures
{
    PATH_BASE "car1.png",
};

static constexpr std::array<float, game::CAR_LAST> _speed // px/s
{
    250,
};

static constexpr std::array<size_t, game::CAR_LAST> _price // px/s
{
    10,
};

#undef PATH_BASE

static std::array<game::CarType, game::CAR_LAST> _cars;


game::CarType& game::car_type(CarEnum index)
{
    if(_cars[index].tex != nullptr)
        return _cars[index];


    cout << "Loading car image: " << _textures[index];
    auto surface = utils::load_surface(_textures[index]);
    _cars[index].tex = utils::create_texture(surface);
    _cars[index].size = {surface->w, surface->h};
    SDL_FreeSurface(surface);

    _cars[index].speed = _speed[index];
    _cars[index].price = _price[index];

    return _cars[index];
}
