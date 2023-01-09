//
// Created by Kamil Rojewski on 18.07.2021.
//
#include "HdrVerticalRegionProcessor.h"

HdrVerticalRegionProcessor::HdrVerticalRegionProcessor(int samples, std::array<float, 3> colorFactors)
        : samples{samples}
        , colorFactors(colorFactors)
{
}

void HdrVerticalRegionProcessor::processRegion(RGBColor *result, const std::uint32_t *data, int width, int height, int startX, int realWidth) const
{
    if (samples <= 0) {
        return;
    }

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
                static_cast<uchar>(red * colorFactors[0] / samplePixels / 4),
                static_cast<uchar>(green * colorFactors[1] / samplePixels / 4),
                static_cast<uchar>(blue * colorFactors[2] / samplePixels / 4)
        );
    }
}
