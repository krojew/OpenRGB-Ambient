//
// Created by Kamil Rojewski on 18.07.2021.
//

#ifndef OPENRGB_AMBIENT_HDRVERTICALREGIONPROCESSOR_H
#define OPENRGB_AMBIENT_HDRVERTICALREGIONPROCESSOR_H

#include <QtGlobal>

#include <RGBController.h>

class HdrVerticalRegionProcessor final
{
public:
    HdrVerticalRegionProcessor(int samples, float blueCompensation);

    void processRegion(RGBColor *result, const std::uint32_t *data, int width, int height, int startX, int realWidth) const;

private:
    int samples = 0;
    float blueCompensation = 1;
};

#endif //OPENRGB_AMBIENT_HDRVERTICALREGIONPROCESSOR_H
