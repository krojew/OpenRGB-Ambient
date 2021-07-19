//
// Created by Kamil Rojewski on 18.07.2021.
//

#include "ColorConversion.h"

#include "HdrHorizontalRegionProcessor.h"

HdrHorizontalRegionProcessor::HdrHorizontalRegionProcessor(int samples)
        : samples{samples}
{
}

void HdrHorizontalRegionProcessor::processRegion(RGBColor *result, const std::uint32_t *data, int width, int height) const
{
    const auto sampleWidth = width / samples;
    const auto samplePixels = sampleWidth * height;

#ifdef NORMALIZED_HDR

    // Converting HDR to SDR is non-trivial and requires tone mapping with information about maximum luminance in given
    // frame. Here a workaround simply tries to avoid all that and preserve relative color distribution to at least
    // look similar.

    for (auto sample = 0; sample < samples; ++sample)
    {
        uint red = 0;
        uint green = 0;
        uint blue = 0;

        ushort maxColor = 255;

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

                maxColor = std::max({maxColor, curRed, curBlue, curGreen});
            }
        }

        result[samples - sample - 1] = ToRGBColor(
                red / samplePixels * 255 / maxColor,
                green / samplePixels * 255 / maxColor,
                static_cast<uchar>(blue * blueScale / samplePixels * 255 / maxColor)
        );
    }

#else

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
                red / samplePixels / 4,
                green / samplePixels / 4,
                static_cast<uchar>(blue * blueScale / samplePixels / 4)
        );
    }

#endif
}
