//
// Created by Kamil Rojewski on 16.07.2021.
//

#include <QCoreApplication>

#include "LedUpdateEvent.h"

#include "ImageProcessor.h"

ImageProcessor::ImageProcessor(std::string controllerLocation,
                               LedRange topRange,
                               LedRange bottomRange,
                               LedRange rightRange,
                               LedRange leftRange,
                               float blueCompensation,
                               QObject *eventReceiver)
        : controllerLocation{std::move(controllerLocation)}
        , eventReceiver{eventReceiver}
        , rightRange{rightRange}
        , leftRange{leftRange}
        , bottomRange{bottomRange}
        , topRange{topRange}
        , topSdrProcessor{topRange.getLength(), blueCompensation}
        , bottomSdrProcessor{bottomRange.getLength(), blueCompensation}
        , leftSdrProcessor{leftRange.getLength(), blueCompensation}
        , rightSdrProcessor{rightRange.getLength(), blueCompensation}
        , topHdrProcessor{topRange.getLength(), blueCompensation}
        , bottomHdrProcessor{bottomRange.getLength(), blueCompensation}
        , leftHdrProcessor{leftRange.getLength(), blueCompensation}
        , rightHdrProcessor{rightRange.getLength(), blueCompensation}
        , colors(topRange.getLength() + bottomRange.getLength() + leftRange.getLength() + rightRange.getLength())
{}

void ImageProcessor::processSdrImage(const uchar *data, int width, int height)
{
    const auto sampleHeight = height / resolutionHeightDivisor;
    const auto sampleWidth = width / resolutionWidthDivisor;

    const auto realTop = std::min(topRange.from, topRange.to);
    const auto realBottom = std::min(bottomRange.from, bottomRange.to);
    const auto realRight = std::min(rightRange.from, rightRange.to);
    const auto realLeft = std::min(leftRange.from, leftRange.to);

    topSdrProcessor.processRegion(colors.data() + realTop, data, width, sampleHeight);
    bottomSdrProcessor.processRegion(colors.data() + realBottom, data + 4 * width * (height - sampleHeight), width, sampleHeight);
    leftSdrProcessor.processRegion(colors.data() + realLeft, data, sampleWidth, height, 0, width);
    rightSdrProcessor.processRegion(colors.data() + realRight, data, sampleWidth, height, width - sampleWidth, width);

    QCoreApplication::postEvent(eventReceiver, new LedUpdateEvent{controllerLocation, colors});
}

void ImageProcessor::processHdrImage(const uint *data, int width, int height)
{
    const auto sampleHeight = height / resolutionHeightDivisor;
    const auto sampleWidth = width / resolutionHeightDivisor;

    const auto realTop = std::min(topRange.from, topRange.to);
    const auto realBottom = std::min(bottomRange.from, bottomRange.to);
    const auto realRight = std::min(rightRange.from, rightRange.to);
    const auto realLeft = std::min(leftRange.from, leftRange.to);

    topHdrProcessor.processRegion(colors.data() + realTop, data, width, sampleHeight);
    bottomHdrProcessor.processRegion(colors.data() + realBottom, data + width * (height - sampleHeight), width, sampleHeight);
    leftHdrProcessor.processRegion(colors.data() + realLeft, data, sampleWidth, height, 0, width);
    rightHdrProcessor.processRegion(colors.data() + realRight, data, sampleWidth, height, width - sampleWidth, width);

    QCoreApplication::postEvent(eventReceiver, new LedUpdateEvent{controllerLocation, colors});
}
