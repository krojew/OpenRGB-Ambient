//
// Created by Kamil Rojewski on 18.07.2021.
//
#include "HdrHorizontalRegionProcessor.h"

HdrHorizontalRegionProcessor::HdrHorizontalRegionProcessor(int samples, std::array<float, 3> colorFactors)
        : samples{samples}
        , colorFactors(colorFactors)
{
}

void HdrHorizontalRegionProcessor::processRegion(RGBColor *result, const std::uint32_t *data, int width, int height) const
{
    if (samples <= 0) {
        return;
    }

    const auto sampleWidth = width / samples;
    const auto samplePixels = sampleWidth * height;

    for (auto sample = 0; sample < samples; ++sample)
    {
        uint red = 0;
        uint green = 0;
        uint blue = 0;

        for (auto y = 0; y < height; ++y)
        {
            const auto currentWidth = (sample + 1) * sampleWidth;
            for (auto x = sample * sampleWidth; x < currentWidth; ++x)
            {
                const auto colors = data[y * width + x];
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
