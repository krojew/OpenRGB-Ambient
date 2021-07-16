//
// Created by Kamil Rojewski on 16.07.2021.
//

#include "ColorConversion.h"

#include "VerticalRegionProcessor.h"

VerticalRegionProcessor::VerticalRegionProcessor(int samples)
    : samples{samples}
{
}

void VerticalRegionProcessor::processRegion(RGBColor *result, const uchar *data, int width, int height, int startX, int realWidth) const
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
                // bgr
                red += data[4 * (y * realWidth + x) + 2];
                green += data[4 * (y * realWidth + x) + 1];
                blue += data[4 * (y * realWidth + x)];
            }
        }

        result[samples - sample - 1] = ToRGBColor(red / samplePixels, green / samplePixels, static_cast<uchar>(blue * blueScale / samplePixels));
    }
}
