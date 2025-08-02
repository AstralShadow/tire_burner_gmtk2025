#include "game/car.hpp"
#include "utils/fs.hpp"
#include "utils/textures.hpp"
#include <iostream>

using std::cout;
using std::endl;


#define PATH_BASE "assets/cars/"

static constexpr std::array<std::pair<const char*, const char*>, game::CAR_LAST> _textures
{   std::pair
    {PATH_BASE "car1_1.png", PATH_BASE "car1_1_fire.png"},
    {PATH_BASE "car1_2.png", PATH_BASE "car1_2_fire.png"},
    {PATH_BASE "car1_3.png", PATH_BASE "car1_3_fire.png"},
    {PATH_BASE "car2_3.png", PATH_BASE "car2_3_fire.png"},

    {PATH_BASE "car1_4.png", PATH_BASE "car1_4_fire.png"},
    {PATH_BASE "car2_2.png", PATH_BASE "car2_2_fire.png"},
    {PATH_BASE "car2_1.png", PATH_BASE "car2_1_fire.png"},
    {PATH_BASE "car3_1.png", PATH_BASE "car3_1_fire.png"},

    {PATH_BASE "car2_4.png", PATH_BASE "car2_4_fire.png"},
    {PATH_BASE "car3_2.png", PATH_BASE "car3_2_fire.png"},
    {PATH_BASE "car3_3.png", PATH_BASE "car3_3_fire.png"},
    {PATH_BASE "car3_4.png", PATH_BASE "car3_4_fire.png"},

    {PATH_BASE "car4_1.png", PATH_BASE "car4_1_fire.png"},
    {PATH_BASE "car4_2.png", PATH_BASE "car4_2_fire.png"},
    {PATH_BASE "car4_3.png", PATH_BASE "car4_3_fire.png"},
    {PATH_BASE "car4_4.png", PATH_BASE "car4_4_fire.png"},
};

static constexpr std::array<float, game::CAR_LAST> _speed // px/s
{
    230, 280, 310, 430,
    330, 390, 430, 510, // TODO balance

    470, 530, 550, 590,
    280, 430, 480, 525,
};

static constexpr std::array<size_t, game::CAR_LAST> _price // px/s
{
    2, 6, 20, 64,
    25, 39, 65, 120,
    75, 130, 145, 170,
    300, 400, 500, 750
};

static constexpr std::array<size_t, game::CAR_LAST> _tires
{
    4, 4, 4, 4,
    4, 4, 4, 4,
    4, 4, 4, 4,
    6, 6, 6, 6
};

static constexpr std::array<size_t, game::CAR_LAST> _meters_per_tire_change
{
    230, 200, 180, 125,
    150, 125, 105, 70, // TODO balance
    100, 75, 64, 60,
    90, 80, 70, 60
};

#undef PATH_BASE

static std::array<game::CarType, game::CAR_LAST> _cars;


game::CarType& game::car_type(CarEnum index)
{
    if(_cars[index].tex != nullptr)
        return _cars[index];


    cout << "Car " << index
        << " textures: " << _textures[index].first
        << " " << _textures[index].second << endl;

    _cars[index].speed = _speed[index];
    _cars[index].price = _price[index];
    _cars[index].tires = _tires[index];
    _cars[index].meters_per_tire_change = _meters_per_tire_change[index];


    auto surface = utils::load_surface(_textures[index].second);
    if(!surface) {
        cout << "Failed to load surface: " << endl;
        cout << _textures[index].first << endl;
        return _cars[index];
    }
    _cars[index].tex2 = utils::create_texture(surface);
    SDL_FreeSurface(surface);


    surface = utils::load_surface(_textures[index].first);
    if(!surface) {
        cout << "Failed to load surface: " << endl;
        cout << _textures[index].first << endl;
        return _cars[index];
    }

    _cars[index].tex = utils::create_texture(surface);
    _cars[index].size = Point {surface->w, surface->h};
    SDL_FreeSurface(surface);


    return _cars[index];
}
