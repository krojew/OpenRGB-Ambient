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
    SdrHorizontalRegionProcessor(int samples, std::array<float, 3> colorFactors, CPP colorPostProcessor)
            : samples{samples}
            , colorFactors(colorFactors)
            , colorPostProcessor{colorPostProcessor}
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

            result[samples - sample - 1] = colorPostProcessor.process(
                    static_cast<uchar>(red * colorFactors[0] / samplePixels),
                    static_cast<uchar>(green  * colorFactors[1]/ samplePixels),
                    static_cast<uchar>(blue * colorFactors[2] / samplePixels),
                    result[samples - sample - 1]
            );
        }
    }

private:
    int samples = 0;
    std::array<float, 3> colorFactors;
    CPP colorPostProcessor;
};

#endif //OPENRGB_AMBIENT_SDRHORIZONTALREGIONPROCESSOR_H
