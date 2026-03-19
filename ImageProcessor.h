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
#include "ZoneMapping.h"

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
                   int totalLeds,
                   LedRange topRange,
                   LedRange bottomRange,
                   LedRange rightRange,
                   LedRange leftRange,
                   const std::vector<ZoneLedRange> &zoneMappings,
                   std::array<float, 3> colorFactors,
                   CPP colorPostProcessor,
                   bool useZoneMapping,
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
            , colors(totalLeds)
            , useZoneMapping{useZoneMapping}
    {
        for (const auto &z : zoneMappings)
        {
            const auto len = z.range.getLength();
            switch (z.region)
            {
                case ScreenRegion::Top:
                    topZoneEntries.push_back({z.range, z.reversed});
                    topSdrZoneProcs.emplace_back(len, colorFactors, colorPostProcessor);
                    break;
                case ScreenRegion::Bottom:
                    bottomZoneEntries.push_back({z.range, z.reversed});
                    bottomSdrZoneProcs.emplace_back(len, colorFactors, colorPostProcessor);
                    break;
                case ScreenRegion::Left:
                    leftZoneEntries.push_back({z.range, z.reversed});
                    leftSdrZoneProcs.emplace_back(len, colorFactors, colorPostProcessor);
                    break;
                case ScreenRegion::Right:
                    rightZoneEntries.push_back({z.range, z.reversed});
                    rightSdrZoneProcs.emplace_back(len, colorFactors, colorPostProcessor);
                    break;
                default:
                    break;
            }
        }
    }

    void processSdrImage(const uchar *data, int width, int height, int stridePixels) override
    {
        const auto sampleHeight = height / resolutionHeightDivisor;
        const auto sampleWidth = width / resolutionWidthDivisor;

        const auto realTop = std::min(topRange.from, topRange.to);
        const auto realBottom = std::min(bottomRange.from, bottomRange.to);
        const auto realRight = std::min(rightRange.from, rightRange.to);
        const auto realLeft = std::min(leftRange.from, leftRange.to);

        if (!useZoneMapping)
        {
            topSdrProcessor.processRegion(colors.data() + realTop, data, width, sampleHeight, stridePixels);
            bottomSdrProcessor.processRegion(colors.data() + realBottom, data + 4 * stridePixels * (height - sampleHeight), width, sampleHeight, stridePixels);
            leftSdrProcessor.processRegion(colors.data() + realLeft, data, sampleWidth, height, 0, stridePixels);
            rightSdrProcessor.processRegion(colors.data() + realRight, data, sampleWidth, height, width - sampleWidth, stridePixels);
        }
        else
        {
            for (auto i = 0u; i < topZoneEntries.size(); ++i)
            {
                const auto start = std::min(topZoneEntries[i].range.from, topZoneEntries[i].range.to);
                const auto len = topZoneEntries[i].range.getLength();
                topSdrZoneProcs[i].processRegion(colors.data() + start, data, width, sampleHeight, stridePixels);
                if (topZoneEntries[i].reversed)
                    std::reverse(colors.data() + start, colors.data() + start + len);
            }
            for (auto i = 0u; i < bottomZoneEntries.size(); ++i)
            {
                const auto start = std::min(bottomZoneEntries[i].range.from, bottomZoneEntries[i].range.to);
                const auto len = bottomZoneEntries[i].range.getLength();
                bottomSdrZoneProcs[i].processRegion(colors.data() + start, data + 4 * stridePixels * (height - sampleHeight), width, sampleHeight, stridePixels);
                if (bottomZoneEntries[i].reversed)
                    std::reverse(colors.data() + start, colors.data() + start + len);
            }
            for (auto i = 0u; i < leftZoneEntries.size(); ++i)
            {
                const auto start = std::min(leftZoneEntries[i].range.from, leftZoneEntries[i].range.to);
                const auto len = leftZoneEntries[i].range.getLength();
                leftSdrZoneProcs[i].processRegion(colors.data() + start, data, sampleWidth, height, 0, stridePixels);
                if (leftZoneEntries[i].reversed)
                    std::reverse(colors.data() + start, colors.data() + start + len);
            }
            for (auto i = 0u; i < rightZoneEntries.size(); ++i)
            {
                const auto start = std::min(rightZoneEntries[i].range.from, rightZoneEntries[i].range.to);
                const auto len = rightZoneEntries[i].range.getLength();
                rightSdrZoneProcs[i].processRegion(colors.data() + start, data, sampleWidth, height, width - sampleWidth, stridePixels);
                if (rightZoneEntries[i].reversed)
                    std::reverse(colors.data() + start, colors.data() + start + len);
            }
        }

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

        if (!useZoneMapping)
        {
            topHdrProcessor.processRegion(colors.data() + realTop, data, width, sampleHeight, stridePixels);
            bottomHdrProcessor.processRegion(colors.data() + realBottom, data + stridePixels * (height - sampleHeight), width, sampleHeight, stridePixels);
            leftHdrProcessor.processRegion(colors.data() + realLeft, data, sampleWidth, height, 0, stridePixels);
            rightHdrProcessor.processRegion(colors.data() + realRight, data, sampleWidth, height, width - sampleWidth, stridePixels);
        }
        else
        {
            if (!topZoneEntries.empty())
            {
                hdrTopCache.resize(static_cast<size_t>(4 * width * sampleHeight));
                for (int y = 0; y < sampleHeight; ++y)
                    for (int x = 0; x < width; ++x)
                    {
                        const auto p = data[y * stridePixels + x];
                        const auto idx = 4 * (y * width + x);
                        hdrTopCache[idx]     = static_cast<uchar>(((p >> 20) & 0x3ffu) >> 2);
                        hdrTopCache[idx + 1] = static_cast<uchar>(((p >> 10) & 0x3ffu) >> 2);
                        hdrTopCache[idx + 2] = static_cast<uchar>((p & 0x3ffu) >> 2);
                        hdrTopCache[idx + 3] = 0;
                    }
                for (auto i = 0u; i < topZoneEntries.size(); ++i)
                {
                    const auto start = std::min(topZoneEntries[i].range.from, topZoneEntries[i].range.to);
                    const auto len = topZoneEntries[i].range.getLength();
                    topSdrZoneProcs[i].processRegion(colors.data() + start, hdrTopCache.data(), width, sampleHeight, width);
                    if (topZoneEntries[i].reversed)
                        std::reverse(colors.data() + start, colors.data() + start + len);
                }
            }
            if (!bottomZoneEntries.empty())
            {
                hdrBottomCache.resize(static_cast<size_t>(4 * width * sampleHeight));
                const auto bottomStartRow = height - sampleHeight;
                for (int y = 0; y < sampleHeight; ++y)
                    for (int x = 0; x < width; ++x)
                    {
                        const auto p = data[(bottomStartRow + y) * stridePixels + x];
                        const auto idx = 4 * (y * width + x);
                        hdrBottomCache[idx]     = static_cast<uchar>(((p >> 20) & 0x3ffu) >> 2);
                        hdrBottomCache[idx + 1] = static_cast<uchar>(((p >> 10) & 0x3ffu) >> 2);
                        hdrBottomCache[idx + 2] = static_cast<uchar>((p & 0x3ffu) >> 2);
                        hdrBottomCache[idx + 3] = 0;
                    }
                for (auto i = 0u; i < bottomZoneEntries.size(); ++i)
                {
                    const auto start = std::min(bottomZoneEntries[i].range.from, bottomZoneEntries[i].range.to);
                    const auto len = bottomZoneEntries[i].range.getLength();
                    bottomSdrZoneProcs[i].processRegion(colors.data() + start, hdrBottomCache.data(), width, sampleHeight, width);
                    if (bottomZoneEntries[i].reversed)
                        std::reverse(colors.data() + start, colors.data() + start + len);
                }
            }
            if (!leftZoneEntries.empty())
            {
                hdrLeftCache.resize(static_cast<size_t>(4 * sampleWidth * height));
                for (int y = 0; y < height; ++y)
                    for (int x = 0; x < sampleWidth; ++x)
                    {
                        const auto p = data[y * stridePixels + x];
                        const auto idx = 4 * (y * sampleWidth + x);
                        hdrLeftCache[idx]     = static_cast<uchar>(((p >> 20) & 0x3ffu) >> 2);
                        hdrLeftCache[idx + 1] = static_cast<uchar>(((p >> 10) & 0x3ffu) >> 2);
                        hdrLeftCache[idx + 2] = static_cast<uchar>((p & 0x3ffu) >> 2);
                        hdrLeftCache[idx + 3] = 0;
                    }
                for (auto i = 0u; i < leftZoneEntries.size(); ++i)
                {
                    const auto start = std::min(leftZoneEntries[i].range.from, leftZoneEntries[i].range.to);
                    const auto len = leftZoneEntries[i].range.getLength();
                    leftSdrZoneProcs[i].processRegion(colors.data() + start, hdrLeftCache.data(), sampleWidth, height, 0, sampleWidth);
                    if (leftZoneEntries[i].reversed)
                        std::reverse(colors.data() + start, colors.data() + start + len);
                }
            }
            if (!rightZoneEntries.empty())
            {
                hdrRightCache.resize(static_cast<size_t>(4 * sampleWidth * height));
                const auto rightStartX = width - sampleWidth;
                for (int y = 0; y < height; ++y)
                    for (int x = 0; x < sampleWidth; ++x)
                    {
                        const auto p = data[y * stridePixels + (rightStartX + x)];
                        const auto idx = 4 * (y * sampleWidth + x);
                        hdrRightCache[idx]     = static_cast<uchar>(((p >> 20) & 0x3ffu) >> 2);
                        hdrRightCache[idx + 1] = static_cast<uchar>(((p >> 10) & 0x3ffu) >> 2);
                        hdrRightCache[idx + 2] = static_cast<uchar>((p & 0x3ffu) >> 2);
                        hdrRightCache[idx + 3] = 0;
                    }
                for (auto i = 0u; i < rightZoneEntries.size(); ++i)
                {
                    const auto start = std::min(rightZoneEntries[i].range.from, rightZoneEntries[i].range.to);
                    const auto len = rightZoneEntries[i].range.getLength();
                    rightSdrZoneProcs[i].processRegion(colors.data() + start, hdrRightCache.data(), sampleWidth, height, 0, sampleWidth);
                    if (rightZoneEntries[i].reversed)
                        std::reverse(colors.data() + start, colors.data() + start + len);
                }
            }
        }

        QCoreApplication::postEvent(eventReceiver, new LedUpdateEvent{controllerLocation, colors});
    }

private:
    static const int resolutionHeightDivisor = 10;
    static const int resolutionWidthDivisor = 12;

    std::string controllerLocation;
    QObject *eventReceiver;
    bool useZoneMapping = false;

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

    struct ZoneEntry { LedRange range; bool reversed; };

    std::vector<ZoneEntry> topZoneEntries;
    std::vector<ZoneEntry> bottomZoneEntries;
    std::vector<ZoneEntry> leftZoneEntries;
    std::vector<ZoneEntry> rightZoneEntries;

    std::vector<SdrHorizontalRegionProcessor<CPP>> topSdrZoneProcs;
    std::vector<SdrHorizontalRegionProcessor<CPP>> bottomSdrZoneProcs;
    std::vector<SdrVerticalRegionProcessor<CPP>>   leftSdrZoneProcs;
    std::vector<SdrVerticalRegionProcessor<CPP>>   rightSdrZoneProcs;

    std::vector<uchar> hdrTopCache;
    std::vector<uchar> hdrBottomCache;
    std::vector<uchar> hdrLeftCache;
    std::vector<uchar> hdrRightCache;

    std::vector<RGBColor> colors;
};

#endif //OPENRGB_AMBIENT_IMAGEPROCESSOR_H
