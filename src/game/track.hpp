#include "utils/types.hpp"
#include <string>
#include <array>

using std::string;
using std::array;

struct SDL_Texture;


namespace game
{
    enum TrackEnum
    {
        TRACK_01,

        TRACK_LAST
    };


    struct Track
    {
        SDL_Texture* tex = nullptr;
    };


    Track& track(TrackEnum);
}
