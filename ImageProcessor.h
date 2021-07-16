//
// Created by Kamil Rojewski on 16.07.2021.
//

#ifndef OPENRGB_AMBIENT_IMAGEPROCESSOR_H
#define OPENRGB_AMBIENT_IMAGEPROCESSOR_H

#include <string>
#include <vector>

#include <QtGlobal>

#include <RGBController.h>

#include "HorizontalRegionProcessor.h"
#include "VerticalRegionProcessor.h"
#include "LedRange.h"

class QObject;

class ImageProcessor final
{
public:
    ImageProcessor(std::string controllerLocation,
                   LedRange topRange,
                   LedRange bottomRange,
                   LedRange rightRange,
                   LedRange leftRange,
                   QObject *eventReceiver);

    void processImage(const uchar *data, int width, int height);

private:
    static const int resolutionDivisor = 5;

    std::string controllerLocation;
    QObject *eventReceiver;

    LedRange topRange;
    LedRange bottomRange;
    LedRange leftRange;
    LedRange rightRange;

    HorizontalRegionProcessor topProcessor;
    HorizontalRegionProcessor bottomProcessor;
    VerticalRegionProcessor leftProcessor;
    VerticalRegionProcessor rightProcessor;

    std::vector<RGBColor> colors;
};

#endif //OPENRGB_AMBIENT_IMAGEPROCESSOR_H
