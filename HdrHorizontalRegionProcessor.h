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
    HdrHorizontalRegionProcessor(int samples, float blueCompensation);

    void processRegion(RGBColor *result, const std::uint32_t *data, int width, int height) const;

private:
    int samples = 0;
    float blueCompensation = 1;
};

#endif //OPENRGB_AMBIENT_HDRHORIZONTALREGIONPROCESSOR_H
