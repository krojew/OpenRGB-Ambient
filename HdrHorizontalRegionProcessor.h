//
// Created by Kamil Rojewski on 18.07.2021.
//

#ifndef OPENRGB_AMBIENT_HDRHORIZONTALREGIONPROCESSOR_H
#define OPENRGB_AMBIENT_HDRHORIZONTALREGIONPROCESSOR_H

#include <QtGlobal>

#include <RGBController.h>

class HdrHorizontalRegionProcessor final
{
public:
    explicit HdrHorizontalRegionProcessor(int samples);

    void processRegion(RGBColor *result, const uint *data, int width, int height) const;

private:
    int samples = 0;
};

#endif //OPENRGB_AMBIENT_HDRHORIZONTALREGIONPROCESSOR_H
