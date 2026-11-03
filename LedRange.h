//
// Created by Kamil Rojewski on 16.07.2021.
//

#ifndef OPENRGB_AMBIENT_LEDRANGE_H
#define OPENRGB_AMBIENT_LEDRANGE_H

#include <algorithm>
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
        const auto max = std::max(ledCount, 0);
        return {std::clamp(from, 0, max), std::clamp(to, 0, max)};
    }
};

#endif //OPENRGB_AMBIENT_LEDRANGE_H
