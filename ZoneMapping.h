//

#ifndef OPENRGB_AMBIENT_ZONEMAPPING_H
#define OPENRGB_AMBIENT_ZONEMAPPING_H

#include "LedRange.h"

enum class ScreenRegion : int
{
    None   = 0,
    Top    = 1,
    Bottom = 2,
    Left   = 3,
    Right  = 4
};

// One LED sub-range within a zone mapped to a screen region.
// from/to are LED indices relative to the zone's start (0-based).
struct ZonePart
{
    int          from     = 0;
    int          to       = 0;
    ScreenRegion region   = ScreenRegion::None;
    bool         reversed = false;
};

// Computed at processor-build time; passed to ImageProcessor.
// range holds absolute device LED indices.
struct ZoneLedRange
{
    LedRange     range    = {0, 0};
    ScreenRegion region   = ScreenRegion::None;
    bool         reversed = false;
};

#endif //OPENRGB_AMBIENT_ZONEMAPPING_H
