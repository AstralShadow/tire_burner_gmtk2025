#include "utils/number.hpp"
#include <sstream>
#include <algorithm>
#include <iostream>

using std::stringstream;
using std::cout;
using std::endl;


void Number::populate(i64 num)
{
    if(num < 0) {
        negative = true;
        num = -num;
    }

    while(num > 0) {
        u8 byte = (num % 10) + ((num / 10) % 10) * 16;

        _bcd.push_back(byte);
        num = num / 100;
    }
}

void Number::populate(i64 num, u64 mant)
{
    populate(num);

    mant++; // 0 means one digit

    size_t log10 = this->log10();
    auto size = log10 / 2 + (log10 % 2);
    auto target = mant / 2 + (mant % 2);
    if(size < target) {
        size_t bytes = _bcd.size();
        _bcd.resize(target, 0);

        auto delta = mant - size;

        /*
        if(delta % 2 == 0)
            std::move_backward(_bcd.begin(), _bcd.begin() + bytes, _bcd.begin() + (target - bytes));
        else
            cout << "not impl" << endl;
        */
        // TODO implement
    }
}


string Number::to_dec() const
{
    if(_bcd.empty())
        return "0";

    stringstream output;
    if(negative)
        output << '-';

    for(size_t i = _bcd.size(); i > 0; i--) {
        u8 byte = _bcd[i - 1];
        char big = '0' + (byte / 16);
        char small = '0' + (byte & 0x0f);
        if(big != '0')
            output << big;
        output << small;
    }

    return output.str();
}


size_t Number::log10() const
{
    u8 last = _bcd.back();
    size_t count = 1;
    if(last / 16 > 0)
        count = 2;

    count += (_bcd.size() - 1) * 2;

    return count;
}
