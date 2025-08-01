#include "game/track.hpp"
#include "utils/fs.hpp"
#include "utils/textures.hpp"
#include "utils/pi.hpp"
#include <fstream>
#include <iostream>

using std::cout;
using std::endl;


#define PATH_BASE "assets/tracks/"

static constexpr std::array<const char*, game::TRACK_LAST> _textures
{
    PATH_BASE "01_track_circle.png",
};

static constexpr std::array<const char*, game::TRACK_LAST> _paths
{
    PATH_BASE "01_track_circle.path",
};

#undef PATH_BASE

static std::array<game::Track, game::TRACK_LAST> _tracks;


static vector<FPoint> parse_path(fs::path);
static std::pair<float, vector<float>> get_length(vector<FPoint> const&);


game::Track& game::track(TrackEnum index)
{
    if(_tracks[index].tex != nullptr)
        return _tracks[index];


    _tracks[index].tex = utils::load_texture(_textures[index]);
    _tracks[index].path = parse_path(_paths[index]);
    auto len = get_length(_tracks[index].path);
    _tracks[index].lap_len = len.first;
    _tracks[index].path_lens= std::move(len.second);

    cout << "Track " << index << " image: " << _textures[index] << endl;
    cout << "Track " << index << " poitns: " << _tracks[index].path.size() << endl;
    cout << "Track " << index <<  " length: " << _tracks[index].lap_len << endl;

    return _tracks[index];
}


vector<FPoint> parse_path(fs::path path)
{
    vector<FPoint> points;

    std::ifstream file(path);

    string _line;
    while (std::getline(file, _line)) {
        if(_line.empty())
            continue;

        std::istringstream line(_line);

        string type;
        line >> type;
        if(type.empty() || type[0] == '#')
            continue;

        if(type == "POINT") {
            float x, y;
            line >> x >> y;
            points.push_back({x, y});
            continue;
        }

        if(type == "ARC") {
            FPoint center, start;
            float degrees;
            line >> center.x >> center.y >> start.x >> start.y >> degrees;
            float radians = degrees * pi_f() / 180;

            FPoint d = start - center;
            auto radius = std::sqrt(d.x * d.x + d.y * d.y);
            auto a1 = atan2(d.y, d.x);

            size_t count = radius * radians / 5 + 2;

            for(size_t i = 0; i <= count; i++) {
                float angle = a1 + radians * i / count;
                FPoint delta {
                    cos(angle) * radius,
                    sin(angle) * radius
                };
                FPoint pos = center + delta;

                points.push_back(pos);
            }

            continue;
        }

        cout << "Unknown path instruction: " << _line << endl;
    }

    return points;
}

std::pair<float, vector<float>> get_length(vector<FPoint> const& path)
{
    float len = 0;
    vector<float> lens;

    for(size_t i = 0; i < path.size(); i++) {
        FPoint p1 = path[i % path.size()];
        FPoint p2 = path[(i + 1) % path.size()];

        FPoint d = p1 - p2;
        len += std::sqrt(d.x * d.x + d.y * d.y);

        lens.push_back(len);
    }

    return {len, lens};
}
