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
        float lap_len; // TODO rename to path_len
        vector<float> path_lens;

        float path_width = 0;


        vector<FPoint> entrance;
        float entrance_len;
        vector<float> entrance_lens;

        static constexpr float entrance_width = 0;
        float entrance_path_overlap = 0; // for traffic control


        size_t loops_per_tire_change;
    };


    Track& track(TrackEnum);
}
