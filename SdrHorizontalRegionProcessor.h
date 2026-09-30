//
// Created by Kamil Rojewski on 16.07.2021.
//

#ifndef OPENRGB_AMBIENT_SDRHORIZONTALREGIONPROCESSOR_H
#define OPENRGB_AMBIENT_SDRHORIZONTALREGIONPROCESSOR_H

#include <array>

#include <QtGlobal>

#include <RGBController.h>

#include "ColorPostProcessor.h"

template<ColorPostProcessor CPP>
class SdrHorizontalRegionProcessor final
{
public:
    SdrHorizontalRegionProcessor(int samples, std::array<float, 3> colorFactors, CPP colorPostProcessor, bool reversed = false)
            : samples{samples}
            , colorFactors(colorFactors)
            , colorPostProcessor{colorPostProcessor}
            , reversed{reversed}
    {
    }

    void processRegion(RGBColor *result, const uchar *data, int width, int height, int stridePixels) const
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
                    // bgr
                    red += data[4 * (y * stridePixels + x) + 2];
                    green += data[4 * (y * stridePixels + x) + 1];
                    blue += data[4 * (y * stridePixels + x)];
                }
            }

            const float avgR = red   * colorFactors[0] / samplePixels;
            const float avgG = green * colorFactors[1] / samplePixels;
            const float avgB = blue  * colorFactors[2] / samplePixels;
            // result doubles as the previous frame for the post-processor, so each sample must keep a fixed LED index
            const auto index = reversed ? sample : samples - sample - 1;
            result[index] = colorPostProcessor.process(
                    static_cast<uchar>(std::clamp(avgR, 0.0f, 255.0f)),
                    static_cast<uchar>(std::clamp(avgG, 0.0f, 255.0f)),
                    static_cast<uchar>(std::clamp(avgB, 0.0f, 255.0f)),
                    result[index]
            );
        }
    }

private:
    int samples = 0;
    std::array<float, 3> colorFactors;
    CPP colorPostProcessor;
    bool reversed = false;
};

#endif //OPENRGB_AMBIENT_SDRHORIZONTALREGIONPROCESSOR_H
