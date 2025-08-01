#pragma once

#include "utils/types.hpp"
#include <vector>
#include <string>

using std::vector;
using std::string;


class Number
{
public:
    Number() : Number(0) {}
    Number(i64 num) { populate(num); }
    Number(i64 num, u64 mant) { populate(num, mant); }
    Number(Number const&) = default;
    ~Number() = default;


    Number& operator +=(Number const& other);
    inline Number operator +(Number const& other);


    size_t log10() const;

    string to_dec() const;
    string to_sci() const;

private:
    vector<u8> _bcd; // lowest digit first
    bool negative = false;

    void populate(i64 num);
    void populate(i64 num, u64 mant); // num e mant (ex. 10e4)
};


Number Number::operator +(Number const& other)
{
    Number copy(*this);
    copy += other;
    return copy;
}
