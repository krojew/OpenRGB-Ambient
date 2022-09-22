//
// Created by Kamil Rojewski on 16.07.2021.
//

#ifndef OPENRGB_AMBIENT_SDRHORIZONTALREGIONPROCESSOR_H
#define OPENRGB_AMBIENT_SDRHORIZONTALREGIONPROCESSOR_H

#include <QtGlobal>

#include <RGBController.h>

class SdrHorizontalRegionProcessor final
{
public:
    SdrHorizontalRegionProcessor(int samples, float blueCompensation);

    void processRegion(RGBColor *result, const uchar *data, int width, int height) const;

private:
    int samples = 0;
    float blueCompensation = 1;
};

#endif //OPENRGB_AMBIENT_SDRHORIZONTALREGIONPROCESSOR_H
