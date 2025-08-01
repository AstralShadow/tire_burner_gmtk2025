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

#undef PATH_BASE

static std::array<game::CarType, game::CAR_LAST> _cars;


game::CarType& game::car_type(CarEnum index)
{
    if(_cars[index].tex != nullptr)
        return _cars[index];

    cout << "Loading car image: " << _textures[index];

    _cars[index].tex = utils::load_texture(_textures[index]);

    return _cars[index];
}
