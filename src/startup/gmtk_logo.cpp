#include "startup/logo.hpp"
#include "utils/textures.hpp"

using std::string;
using utils::load_surface;
using utils::create_texture;


static const string _path = "assets/gmtk/gmtk-game-jam-2025-logo-white_025.png";

static Point _size {1, 1};


static SDL_Texture* load_gmtk_logo()
{
    auto surface = load_surface(_path);
    if(!surface)
        return nullptr;

    _size.x = surface->w / 2;
    _size.y = surface->h / 2;

    auto texture = create_texture(surface);
    SDL_FreeSurface(surface);

    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);

    return texture;
}


SDL_Texture* startup::gmtk_logo()
{
    static SDL_Texture* texture = load_gmtk_logo();

    return texture;
}

Point startup::gmtk_logo_size()
{
    return _size;
}

