#ifndef INCLUDE_GAME_FONTS_HPP
#define INCLUDE_GAME_FONTS_HPP

#include <SDL2/SDL_ttf.h>


namespace game
{
    enum FontTarget
    {
        FT_DEBUG,
        FT_DEFAULT
    };

    TTF_Font* get_font(FontTarget = FT_DEFAULT,
                       int size = 18);
}

#endif // INCLUDE_GAME_FONTS_HPP
