//
// Created by Kamil Rojewski on 16.07.2021.
//

#ifndef OPENRGB_AMBIENT_SDRVERTICALREGIONPROCESSOR_H
#define OPENRGB_AMBIENT_SDRVERTICALREGIONPROCESSOR_H

#include <QtGlobal>

#include <RGBController.h>

class SdrVerticalRegionProcessor final
{
public:
    SdrVerticalRegionProcessor(int samples, float blueCompensation);

    void processRegion(RGBColor *result, const uchar *data, int width, int height, int startX, int realWidth) const;

private:
    int samples = 0;
    float blueCompensation = 1;
};

#endif //OPENRGB_AMBIENT_SDRVERTICALREGIONPROCESSOR_H
