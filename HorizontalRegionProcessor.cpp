//
// Created by Kamil Rojewski on 16.07.2021.
//

#include "ColorConversion.h"

#include "HorizontalRegionProcessor.h"

HorizontalRegionProcessor::HorizontalRegionProcessor(int samples)
    : samples{samples}
{
}

void HorizontalRegionProcessor::processRegion(RGBColor *result, const uchar *data, int width, int height) const
{
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
                // bgr
                red += data[4 * (y * width + x) + 2];
                green += data[4 * (y * width + x) + 1];
                blue += data[4 * (y * width + x)];
            }
        }

        result[samples - sample - 1] = ToRGBColor(red / samplePixels, green / samplePixels, static_cast<uchar>(blue * blueScale / samplePixels));
    }
}
