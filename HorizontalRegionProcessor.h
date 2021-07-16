//
// Created by Kamil Rojewski on 16.07.2021.
//

#ifndef OPENRGB_AMBIENT_HORIZONTALREGIONPROCESSOR_H
#define OPENRGB_AMBIENT_HORIZONTALREGIONPROCESSOR_H

#include <QtGlobal>

#include <RGBController.h>

class HorizontalRegionProcessor final
{
public:
    explicit HorizontalRegionProcessor(int samples);

    void processRegion(RGBColor *result, const uchar *data, int width, int height) const;

private:
    int samples = 0;
};

#endif //OPENRGB_AMBIENT_HORIZONTALREGIONPROCESSOR_H
