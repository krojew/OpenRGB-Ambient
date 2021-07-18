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
    explicit HdrVerticalRegionProcessor(int samples);

    void processRegion(RGBColor *result, const uint *data, int width, int height, int startX, int realWidth) const;

private:
    int samples = 0;
};

#endif //OPENRGB_AMBIENT_HDRVERTICALREGIONPROCESSOR_H
