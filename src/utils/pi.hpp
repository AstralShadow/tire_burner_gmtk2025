#ifndef INCLUDE_PI_HPP
#define INCLUDE_PI_HPP

#include <cmath>

inline double pi()
{
    return std::atan2(0.0, -1.0);
}

inline float pi_f()
{
    return std::atan2(0.0f, -1.0f);
}

#endif // INCLUDE_PI_HPP
