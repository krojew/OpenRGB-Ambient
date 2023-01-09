//
// Created by Kamil Rojewski on 18.07.2021.
//

#ifndef OPENRGB_AMBIENT_HDRVERTICALREGIONPROCESSOR_H
#define OPENRGB_AMBIENT_HDRVERTICALREGIONPROCESSOR_H

#include <array>

#include <QtGlobal>

#include <RGBController.h>

class HdrVerticalRegionProcessor final
{
public:
    HdrVerticalRegionProcessor(int samples, std::array<float, 3> colorFactors);

    void processRegion(RGBColor *result, const std::uint32_t *data, int width, int height, int startX, int realWidth) const;

private:
    int samples = 0;
    std::array<float, 3> colorFactors;
};

#endif //OPENRGB_AMBIENT_HDRVERTICALREGIONPROCESSOR_H
