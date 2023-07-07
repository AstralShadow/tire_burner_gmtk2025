#ifndef INCLUDE_UTILS_TEXTURES_HPP
#define INCLUDE_UTILS_TEXTURES_HPP

#include <SDL2/SDL_render.h>
#include <string>

namespace utils
{
    SDL_Surface* load_surface(std::string path);
    SDL_Texture* create_texture(SDL_Surface*);

    SDL_Texture* create_texture(std::string path);
}

#endif // INCLUDE_UTILS_TEXTURES_HPP
