//
// Created by Kamil Rojewski on 16.07.2021.
//

#ifndef OPENRGB_AMBIENT_IMAGEPROCESSOR_H
#define OPENRGB_AMBIENT_IMAGEPROCESSOR_H

#include <string>
#include <vector>
#include <array>

#include <QtGlobal>
#include <QCoreApplication>

#include <RGBController.h>

#include "SdrHorizontalRegionProcessor.h"
#include "SdrVerticalRegionProcessor.h"
#include "HdrHorizontalRegionProcessor.h"
#include "HdrVerticalRegionProcessor.h"
#include "ColorPostProcessor.h"
#include "LedUpdateEvent.h"
#include "LedRange.h"

class QObject;

class ImageProcessorBase
{
public:
    virtual void processSdrImage(const uchar *data, int width, int height, int stridePixels) = 0;
    virtual void processHdrImage(const uint *data, int width, int height, int stridePixels) = 0;
};

template<ColorPostProcessor CPP>
class ImageProcessor final
        : public ImageProcessorBase
{
public:
    ImageProcessor(std::string controllerLocation,
                   LedRange topRange,
                   LedRange bottomRange,
                   LedRange rightRange,
                   LedRange leftRange,
                   std::array<float, 3> colorFactors,
                   CPP colorPostProcessor,
                   QObject *eventReceiver)
            : controllerLocation{std::move(controllerLocation)}
            , eventReceiver{eventReceiver}
            , rightRange{rightRange}
            , leftRange{leftRange}
            , bottomRange{bottomRange}
            , topRange{topRange}
            , topSdrProcessor{topRange.getLength(), colorFactors, colorPostProcessor}
            , bottomSdrProcessor{bottomRange.getLength(), colorFactors, colorPostProcessor}
            , leftSdrProcessor{leftRange.getLength(), colorFactors, colorPostProcessor}
            , rightSdrProcessor{rightRange.getLength(), colorFactors, colorPostProcessor}
            , topHdrProcessor{topRange.getLength(), colorFactors, colorPostProcessor}
            , bottomHdrProcessor{bottomRange.getLength(), colorFactors, colorPostProcessor}
            , leftHdrProcessor{leftRange.getLength(), colorFactors, colorPostProcessor}
            , rightHdrProcessor{rightRange.getLength(), colorFactors, colorPostProcessor}
            , colors(topRange.getLength() + bottomRange.getLength() + leftRange.getLength() + rightRange.getLength())
    {
    }

    void processSdrImage(const uchar *data, int width, int height, int stridePixels) override
    {
        const auto sampleHeight = height / resolutionHeightDivisor;
        const auto sampleWidth = width / resolutionWidthDivisor;

        const auto realTop = std::min(topRange.from, topRange.to);
        const auto realBottom = std::min(bottomRange.from, bottomRange.to);
        const auto realRight = std::min(rightRange.from, rightRange.to);
        const auto realLeft = std::min(leftRange.from, leftRange.to);

        topSdrProcessor.processRegion(colors.data() + realTop, data, width, sampleHeight, stridePixels);
        bottomSdrProcessor.processRegion(colors.data() + realBottom, data + 4 * stridePixels * (height - sampleHeight), width, sampleHeight, stridePixels);
        leftSdrProcessor.processRegion(colors.data() + realLeft, data, sampleWidth, height, 0, stridePixels);
        rightSdrProcessor.processRegion(colors.data() + realRight, data, sampleWidth, height, width - sampleWidth, stridePixels);

        QCoreApplication::postEvent(eventReceiver, new LedUpdateEvent{controllerLocation, colors});
    }

    void processHdrImage(const uint *data, int width, int height, int stridePixels) override
    {
        const auto sampleHeight = height / resolutionHeightDivisor;
        const auto sampleWidth = width / resolutionWidthDivisor;

        const auto realTop = std::min(topRange.from, topRange.to);
        const auto realBottom = std::min(bottomRange.from, bottomRange.to);
        const auto realRight = std::min(rightRange.from, rightRange.to);
        const auto realLeft = std::min(leftRange.from, leftRange.to);

        topHdrProcessor.processRegion(colors.data() + realTop, data, width, sampleHeight, stridePixels);
        bottomHdrProcessor.processRegion(colors.data() + realBottom, data + stridePixels * (height - sampleHeight), width, sampleHeight, stridePixels);
        leftHdrProcessor.processRegion(colors.data() + realLeft, data, sampleWidth, height, 0, stridePixels);
        rightHdrProcessor.processRegion(colors.data() + realRight, data, sampleWidth, height, width - sampleWidth, stridePixels);

        QCoreApplication::postEvent(eventReceiver, new LedUpdateEvent{controllerLocation, colors});
    }

private:
    static const int resolutionHeightDivisor = 10;
    static const int resolutionWidthDivisor = 12;

    std::string controllerLocation;
    QObject *eventReceiver;

    LedRange topRange;
    LedRange bottomRange;
    LedRange leftRange;
    LedRange rightRange;

    SdrHorizontalRegionProcessor<CPP> topSdrProcessor;
    SdrHorizontalRegionProcessor<CPP> bottomSdrProcessor;
    SdrVerticalRegionProcessor<CPP> leftSdrProcessor;
    SdrVerticalRegionProcessor<CPP> rightSdrProcessor;

    HdrHorizontalRegionProcessor<CPP> topHdrProcessor;
    HdrHorizontalRegionProcessor<CPP> bottomHdrProcessor;
    HdrVerticalRegionProcessor<CPP> leftHdrProcessor;
    HdrVerticalRegionProcessor<CPP> rightHdrProcessor;

    std::vector<RGBColor> colors;
};

#endif //OPENRGB_AMBIENT_IMAGEPROCESSOR_H
