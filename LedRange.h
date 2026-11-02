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

    // Saved ranges can exceed the current LED count, e.g. after a zone resize.
    inline LedRange clampedTo(int ledCount) const noexcept
    {
        const auto clamp = [ledCount](int value) { const auto v = value > ledCount ? ledCount : value; return v < 0 ? 0 : v; };
        return {clamp(from), clamp(to)};
    }
};

#endif //OPENRGB_AMBIENT_LEDRANGE_H
