#include "game/track.hpp"
#include "utils/fs.hpp"
#include "utils/textures.hpp"
#include "utils/pi.hpp"
#include <fstream>
#include <iostream>

using std::cerr;
using std::cout;
using std::endl;


#define PATH_BASE "assets/tracks/"

static constexpr std::array<const char*, game::TRACK_LAST> _textures
{
    PATH_BASE "01_track_circle.png",
    PATH_BASE "02_track_long.png",
};

static constexpr std::array<const char*, game::TRACK_LAST> _paths
{
    PATH_BASE "01_track_circle.path",
    PATH_BASE "02_track_long.path",
};

static constexpr std::array<size_t, game::TRACK_LAST> _prices
{
    0,
    500,
    // 500, // TODO balance
};

#undef PATH_BASE

static std::array<game::Track, game::TRACK_LAST> _tracks;


static void parse_path(game::Track&, fs::path);
static void calculate_lengths(game::Track&);


game::Track& game::track(TrackEnum index)
{
    if(_tracks[index].tex != nullptr)
        return _tracks[index];


    _tracks[index].tex = utils::load_texture(_textures[index]);
    _tracks[index].price = _prices[index];

    parse_path(_tracks[index], _paths[index]);
    calculate_lengths(_tracks[index]);


    cout << "Track " << index << " image: " << _textures[index] << endl;
    cout << "Track " << index << " points: " << _tracks[index].path.size() << endl;
    cout << "Track " << index <<  " width: " << _tracks[index].path_width << endl;
    cout << "Track " << index <<  " length: " << _tracks[index].lap_len << endl;

    return _tracks[index];
}


void parse_path(game::Track& track, fs::path path)
{
    enum TrackSection {
        TS_NONE,
        TS_PATH,
        TS_ENTRANCE,
    } section = TS_NONE;

    vector<FPoint> path_points;
    vector<FPoint> entrance_points;


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


        if(type == "TRACK_WIDTH") {
            float width;
            line >> width;
            track.path_width = width;
            continue;
        }


        if(type == "PATH_BEGIN") {
            if(section != TS_NONE)
                cerr << "Error in " << path << ": PATH_BEGIN in wrong section!" << endl;

            section = TS_PATH;
            continue;
        }

        if(type == "PATH_END") {
            if(section != TS_PATH)
                cerr << "Error in " << path << ": PATH_END but not in path section!" << endl;

            section = TS_NONE;
            continue;
        }

        if(type == "ENTRANCE_BEGIN") {
            if(section != TS_NONE)
                cerr << "Error in " << path << ": ENTRANCE_BEGIN in wrong section!" << endl;

            section = TS_ENTRANCE;
            continue;
        }

        if(type == "ENTRANCE_END") {
            if(section != TS_ENTRANCE)
                cerr << "Error in " << path << ": ENTRANCE_END but not in entrance section!" << endl;
            section = TS_NONE;
            continue;
        }


        if(type == "ENTRANCE_PATH_OVERLAP") {
            float num;
            line >> num;
            track.entrance_path_overlap = num;
            continue;
        }

        if(type == "MAX_CAR_COUNT") {
            size_t num;
            line >> num;
            track.max_cars = num;
            continue;
        }


        if(type == "POINT") {
            if(section != TS_PATH && section != TS_ENTRANCE)
                cerr << "Error in " << path << ": POINT not in path or entrance section!" << endl;

            float x, y;
            line >> x >> y;
            switch(section) {
                case TS_NONE:
                    continue;

                case TS_PATH:
                    path_points.push_back({x, y});
                    continue;

                case TS_ENTRANCE:
                    entrance_points.push_back({x, y});
                    continue;
            }
            continue;
        }

        if(type == "ARC") {
            if(section != TS_PATH && section != TS_ENTRANCE)
                cerr << "Error in " << path << ": ARC not in path or entrance section!" << endl;

            FPoint center, start;
            float degrees;
            line >> center.x >> center.y >> start.x >> start.y >> degrees;
            float radians = degrees * pi_f() / 180;

            FPoint d = start - center;
            auto radius = std::sqrt(d.x * d.x + d.y * d.y);
            auto a1 = atan2(d.y, d.x);

            size_t count = radius * abs(radians) / 5 + 2;

            for(size_t i = 0; i <= count; i++) {
                float angle = a1 + radians * i / count;
                FPoint delta {
                    cos(angle) * radius,
                    sin(angle) * radius
                };
                FPoint pos = center + delta;

                switch(section) {
                    case TS_NONE:
                        continue;

                    case TS_PATH:
                        path_points.push_back(pos);
                        continue;

                    case TS_ENTRANCE:
                        entrance_points.push_back(pos);
                        continue;
                }
            }

            continue;
        }

        cout << "Unknown path instruction: " << _line << endl;
    }

    track.path = std::move(path_points);
    track.entrance = std::move(entrance_points);
}


static std::pair<float, vector<float>> get_length(vector<FPoint> const& path, bool include_last)
{
    float len = 0;
    vector<float> lens;

    for(size_t i = 0; i < path.size() - 1 + include_last; i++) {
        FPoint p1 = path[i % path.size()];
        FPoint p2 = path[(i + 1) % path.size()];

        FPoint d = p1 - p2;
        len += std::sqrt(d.x * d.x + d.y * d.y);

        lens.push_back(len);
    }

    return {len, lens};
}

void calculate_lengths(game::Track& track)
{
    auto len = get_length(track.path, true);
    track.lap_len = len.first;
    track.path_lens = std::move(len.second);

    len = get_length(track.entrance, false);
    track.entrance_len = len.first;
    track.entrance_lens = std::move(len.second);
}
