//
// Created by Kamil Rojewski on 18.07.2021.
//

#ifndef OPENRGB_AMBIENT_HDRHORIZONTALREGIONPROCESSOR_H
#define OPENRGB_AMBIENT_HDRHORIZONTALREGIONPROCESSOR_H

#include <array>

#include <QtGlobal>

#include <RGBController.h>

class HdrHorizontalRegionProcessor final
{
public:
    HdrHorizontalRegionProcessor(int samples, std::array<float, 3> colorFactors);

    void processRegion(RGBColor *result, const std::uint32_t *data, int width, int height) const;

private:
    int samples = 0;
    std::array<float, 3> colorFactors;
};

#endif //OPENRGB_AMBIENT_HDRHORIZONTALREGIONPROCESSOR_H
