//
// Created by Kamil Rojewski on 16.07.2021.
//

#ifndef OPENRGB_AMBIENT_SDRVERTICALREGIONPROCESSOR_H
#define OPENRGB_AMBIENT_SDRVERTICALREGIONPROCESSOR_H

#include <array>

#include <QtGlobal>

#include <RGBController.h>

class SdrVerticalRegionProcessor final
{
public:
    SdrVerticalRegionProcessor(int samples, std::array<float, 3> colorFactors);

    void processRegion(RGBColor *result, const uchar *data, int width, int height, int startX, int realWidth) const;

private:
    int samples = 0;
    std::array<float, 3> colorFactors;
};

#endif //OPENRGB_AMBIENT_SDRVERTICALREGIONPROCESSOR_H
