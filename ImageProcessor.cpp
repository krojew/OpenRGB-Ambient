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
                               QObject *eventReceiver)
        : controllerLocation{std::move(controllerLocation)}
        , eventReceiver{eventReceiver}
        , rightRange{rightRange}
        , leftRange{leftRange}
        , bottomRange{bottomRange}
        , topRange{topRange}
        , topProcessor{topRange.getLength()}
        , bottomProcessor{bottomRange.getLength()}
        , leftProcessor{leftRange.getLength()}
        , rightProcessor{rightRange.getLength()}
        , colors(topRange.getLength() + bottomRange.getLength() + leftRange.getLength() + rightRange.getLength())
{}

void ImageProcessor::processImage(const uchar *data, int width, int height)
{
    const auto sampleHeight = height / resolutionDivisor;
    const auto sampleWidth = width / resolutionDivisor;

    const auto realTop = std::min(topRange.from, topRange.to);
    const auto realBottom = std::min(bottomRange.from, bottomRange.to);
    const auto realRight = std::min(rightRange.from, rightRange.to);
    const auto realLeft = std::min(leftRange.from, leftRange.to);

    topProcessor.processRegion(colors.data() + realTop, data, width, sampleHeight);
    bottomProcessor.processRegion(colors.data() + realBottom, data + 4 * width * (height - sampleHeight), width, sampleHeight);
    leftProcessor.processRegion(colors.data() + realLeft, data, sampleWidth, height, 0, width);
    rightProcessor.processRegion(colors.data() + realRight, data, sampleWidth, height, width - sampleWidth, width);

    QCoreApplication::postEvent(eventReceiver, new LedUpdateEvent{controllerLocation, colors});
}
