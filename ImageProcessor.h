//
// Created by Kamil Rojewski on 16.07.2021.
//

#ifndef OPENRGB_AMBIENT_IMAGEPROCESSOR_H
#define OPENRGB_AMBIENT_IMAGEPROCESSOR_H

#include <string>
#include <vector>

#include <QtGlobal>

#include <RGBController.h>

#include "SdrHorizontalRegionProcessor.h"
#include "SdrVerticalRegionProcessor.h"
#include "HdrHorizontalRegionProcessor.h"
#include "HdrVerticalRegionProcessor.h"
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
                   float blueCompensation,
                   QObject *eventReceiver);

    void processSdrImage(const uchar *data, int width, int height);
    void processHdrImage(const uint *data, int width, int height);

private:
    static const int resolutionHeightDivisor = 10;
    static const int resolutionWidthDivisor = 12;

    std::string controllerLocation;
    QObject *eventReceiver;

    LedRange topRange;
    LedRange bottomRange;
    LedRange leftRange;
    LedRange rightRange;

    SdrHorizontalRegionProcessor topSdrProcessor;
    SdrHorizontalRegionProcessor bottomSdrProcessor;
    SdrVerticalRegionProcessor leftSdrProcessor;
    SdrVerticalRegionProcessor rightSdrProcessor;

    HdrHorizontalRegionProcessor topHdrProcessor;
    HdrHorizontalRegionProcessor bottomHdrProcessor;
    HdrVerticalRegionProcessor leftHdrProcessor;
    HdrVerticalRegionProcessor rightHdrProcessor;

    std::vector<RGBColor> colors;
};

#endif //OPENRGB_AMBIENT_IMAGEPROCESSOR_H
