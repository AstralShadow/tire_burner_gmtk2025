#include "logo.hpp"


bool menu::enlarge_play_btn = false;


bool menu::point_in_play_btn(Point pos)
{
    pos.x -= play_button_pos().x;
    pos.y -= play_button_pos().y;
    auto size = play_button_size();
    SDL_Rect rect { 0, 0, size.x, size.y };
    if(!SDL_PointInRect(&pos, &rect))
        return false;

    if(enlarge_play_btn) {
        // Convert to not-scaled coordinates
        pos.x = pos.x * 10 / 11;
        pos.y = pos.y * 10 / 11;
    }

    Point p1 { -2, 69 };
    Point p2 { 252, -3 };
    Point p3 { 53, 224 };
    Point p4 { 252, 100 };

    /* Within vertical area (p1-p2, p3-p4) */ {
        Point d12 = p2 - p1;
        float k1 = (pos.x - p1.x) * 1.0f / d12.x;
        int top_y = k1 * d12.y + p1.y;
        if(pos.y < top_y)
            return false;

        Point d34 = p4 - p3;
        float k2 = (pos.x - p3.x) * 1.0f / d34.x;
        int bot_y = k2 * d34.y + p3.y;
        if(pos.y > bot_y)
            return false;
    }

    /* Within horizontal area (p1-p3, p2-p4) */ {
        Point d13 = p3 - p1;
        float k3 = (pos.y - p1.y) * 1.0f / d13.y;
        int left_x = k3 * d13.x + p1.x;
        if(pos.x < left_x)
            return false;

        Point d24 = p4 - p2;
        float k4 = (pos.y - p2.y) * 1.0f / d24.y;
        int right_x = k4 * d24.x + p2.x;
        if(pos.x > right_x)
            return false;
    }

    return true;
}
