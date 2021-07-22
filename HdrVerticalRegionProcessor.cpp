//
// Created by Kamil Rojewski on 18.07.2021.
//

#include "ColorConversion.h"

#include "HdrVerticalRegionProcessor.h"

HdrVerticalRegionProcessor::HdrVerticalRegionProcessor(int samples)
        : samples{samples}
{
}

void HdrVerticalRegionProcessor::processRegion(RGBColor *result, const std::uint32_t *data, int width, int height, int startX, int realWidth) const
{
    const auto sampleHeight = height / samples;
    const auto samplePixels = sampleHeight * width;

    for (auto sample = 0; sample < samples; ++sample)
    {
        uint red = 0;
        uint green = 0;
        uint blue = 0;

        const auto currentHeight = (sample + 1) * sampleHeight;
        for (auto y = sample * sampleHeight; y < currentHeight; ++y)
        {
            const auto endX = startX + width;
            for (auto x = startX; x < endX; ++x)
            {
                const auto colors = data[y * realWidth + x];
                const ushort curRed = colors & 0x3ff;
                const ushort curGreen = (colors >> 10) & 0x3ff;
                const ushort curBlue = (colors >> 20) & 0x3ff;

                red += curRed;
                green += curGreen;
                blue += curBlue;
            }
        }

        result[samples - sample - 1] = ToRGBColor(
                red / samplePixels / 4,
                green / samplePixels / 4,
                static_cast<uchar>(blue * blueScale / samplePixels / 4)
        );
    }
}
