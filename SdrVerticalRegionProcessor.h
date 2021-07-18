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
    explicit SdrVerticalRegionProcessor(int samples);

    void processRegion(RGBColor *result, const uchar *data, int width, int height, int startX, int realWidth) const;

private:
    int samples = 0;
};

#endif //OPENRGB_AMBIENT_SDRVERTICALREGIONPROCESSOR_H
