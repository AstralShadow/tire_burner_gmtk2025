#ifdef USE_SDL2_IMAGE

#include "utils/textures.hpp"
#include "core/core.hpp"
#include <SDL2/SDL_image.h>
#include <string>
#include <iostream>

using std::string;
using std::cout;
using std::endl;

static auto& rnd = core::renderer;


SDL_Surface* utils::load_surface(string path)
{
    auto surface = IMG_Load(path.c_str());
    if(!surface) {
        cout << "Could not load asset: ";
        cout << path << endl;
        cout << "IMG_Load error: " << endl;
        cout << IMG_GetError() << endl;
    }

    return surface;
}

SDL_Texture* utils::create_texture(SDL_Surface* src)
{
    auto texture = SDL_CreateTextureFromSurface
        (rnd, src);

    if(!texture) {
        cout << "Failed to convert surface to texture.";
        cout << " SDL error:" << endl;
        cout << SDL_GetError() << endl;
    }

    return texture;
}


SDL_Texture* utils::load_texture(std::string path)
{
    auto src = load_surface(path);

    if(!src)
        return nullptr;

    auto texture = SDL_CreateTextureFromSurface
        (rnd, src);

    SDL_FreeSurface(src);

    if(!texture) {
        cout << "Failed to convert surface to texture.";
        cout << " SDL error:" << endl;
        cout << SDL_GetError() << endl;
    }

    return texture;
}

#endif // USE_SDL2_IMAGE
