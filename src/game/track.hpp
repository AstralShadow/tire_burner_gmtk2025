#include "utils/types.hpp"
#include "utils/point.hpp"
#include <string>
#include <array>
#include <vector>

using std::string;
using std::array;
using std::vector;

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
        vector<FPoint> path;
        float lap_len;
        vector<float> path_lens;
    };


    Track& track(TrackEnum);
}
