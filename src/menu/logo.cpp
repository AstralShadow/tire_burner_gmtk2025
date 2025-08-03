#include "menu/logo.hpp"
#include "utils/textures.hpp"
#include <iostream>

using std::string;
using std::cout;
using std::endl;

using utils::load_surface;
using utils::create_texture;

using namespace menu;

namespace menu
{
    static array<SDL_Texture*, bg_frames_count> _bg_frames;
    static array<SDL_Texture*, title_texture_count> _title_textures;
    static array<Point, title_texture_count> _title_size;
    static SDL_Texture* _play_btn_texture;
    static Point _play_btn_size;

#define BASE "assets/menu/"
    constexpr std::array<const char*, bg_frames_count> bg_paths {
        BASE "frame_1.png",
        BASE "frame_2.png",
        BASE "frame_3.png",
        BASE "frame_4.png"
    };
    constexpr std::array<const char*, title_texture_count> title_paths {
        BASE "title1.png",
        BASE "title2.png"
    };
    constexpr auto play_btn_path = BASE "play_btn.png";

    constexpr Point _enlarged_play_btn_size { 254 + 254 / 10 , 220 + 220 / 10 };
#undef BASE
}


static std::pair<SDL_Texture*, Point> load_texture(string const& path, bool blend)
{
    auto surface = load_surface(path);
    if(!surface)
        return {nullptr, {0, 0}};

    Point size { surface->w, surface->h};

    auto texture = create_texture(surface);
    SDL_FreeSurface(surface);

    SDL_SetTextureBlendMode(texture, blend ? SDL_BLENDMODE_BLEND : SDL_BLENDMODE_NONE);

    return {texture, size};
}


array<SDL_Texture*, bg_frames_count> const& menu::bg_frames()
{
    while(init_next()) { }

    return _bg_frames;
}

array<SDL_Texture*, title_texture_count> const& menu::title_textures()
{
    while(init_next()) { }

    return _title_textures;
}

array<Point, title_texture_count> const& menu::title_size()
{
    while(init_next()) { }

    return _title_size;
}

SDL_Texture* menu::play_button_texture()
{
    while(init_next()) { }

    return _play_btn_texture;
}

Point menu::play_button_size()
{
    while(init_next()) { }

    return enlarge_play_btn ? _enlarged_play_btn_size : _play_btn_size;
}


bool menu::init_next()
{
    static size_t _step = 0;

    if(_step == bg_frames_count + title_texture_count + 1)
        return false;

    auto step = _step++;

    if(step < bg_frames_count) {
        auto data = load_texture(bg_paths[step], false);
        _bg_frames[step] = data.first;
        if(data.second != bg_size()) {
            cout << "Background frame doesn't match expected size!" << endl;
        }

        return true;
    }

    if(step < bg_frames_count + title_texture_count) {
        step -= bg_frames_count;

        auto data = load_texture(title_paths[step], true);
        _title_textures[step] = data.first;
        _title_size[step] = data.second;

        return true;
    }

    if(step == bg_frames_count + title_texture_count) {
        auto data = load_texture(play_btn_path, true);
        _play_btn_texture = data.first;
        _play_btn_size = data.second;

        return false;
    }

    cout << "Huh, menu::init_next overshooted?" << endl;
    return false;
}
