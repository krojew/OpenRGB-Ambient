//
// Created by Kamil Rojewski on 16.07.2021.
//

#ifndef OPENRGB_AMBIENT_SDRVERTICALREGIONPROCESSOR_H
#define OPENRGB_AMBIENT_SDRVERTICALREGIONPROCESSOR_H

#include <array>

#include <QtGlobal>

#include <RGBController.h>

#include "ColorPostProcessor.h"

template<ColorPostProcessor CPP>
class SdrVerticalRegionProcessor final
{
public:
    SdrVerticalRegionProcessor(int samples, std::array<float, 3> colorFactors, CPP colorPostProcessor)
            : samples{samples}
            , colorFactors(colorFactors)
            , colorPostProcessor{colorPostProcessor}
    {
    }

    void processRegion(RGBColor *result, const uchar *data, int width, int height, int startX, int realWidth) const
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
                    // bgr
                    red += data[4 * (y * realWidth + x) + 2];
                    green += data[4 * (y * realWidth + x) + 1];
                    blue += data[4 * (y * realWidth + x)];
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

#endif //OPENRGB_AMBIENT_SDRVERTICALREGIONPROCESSOR_H
