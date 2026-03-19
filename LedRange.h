//
// Created by Kamil Rojewski on 16.07.2021.
//

#ifndef OPENRGB_AMBIENT_LEDRANGE_H
#define OPENRGB_AMBIENT_LEDRANGE_H

#include <cmath>

struct LedRange
{
    int from = 0;
    int to   = 0;

    inline int getLength() const noexcept
    {
        return std::abs(to - from);
    }
};

#endif //OPENRGB_AMBIENT_LEDRANGE_H
