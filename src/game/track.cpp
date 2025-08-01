#include "game/track.hpp"
#include "utils/fs.hpp"
#include "utils/textures.hpp"
#include <iostream>

using std::cout;
using std::endl;


#define PATH_BASE "assets/tracks/"

static constexpr std::array<const char*, game::TRACK_LAST> _textures
{
    PATH_BASE "01_track_circle.png",
};

#undef PATH_BASE

static std::array<game::Track, game::TRACK_LAST> _tracks;


game::Track& game::track(TrackEnum index)
{
    if(_tracks[index].tex != nullptr)
        return _tracks[index];

    cout << "Loading track image: " << _textures[index];

    _tracks[index].tex = utils::load_texture(_textures[index]);

    return _tracks[index];
}
