//

#include <algorithm>

#include <QPainter>

#include <ResourceManager.h>
#include <RGBController.h>

#include "Settings.h"
#include "LedPreviewWidget.h"

#include <OpenRGBPluginInterface.h>

LedPreviewWidget::LedPreviewWidget(OpenRGBPluginAPIInterface *pluginInterface, Settings &settings, QWidget *parent)
    : QWidget{parent}
    , pluginInterface{pluginInterface}
    , settings{settings}
{
    setFixedWidth(320);
    setFixedHeight(180); // updated on first frame to match display proportions
}

void LedPreviewWidget::updateFrame(const QImage &image)
{
    if (!image.isNull() && image.width() > 0)
    {
        const int h = 320 * image.height() / image.width();
        if (h != height())
            setFixedHeight(h);
    }
    currentFrame = image;
    update();
}

void LedPreviewWidget::updateLedColors(const QString &location, const std::vector<RGBColor> &colors)
{
    deviceColors[location.toStdString()] = colors;
    update();
}

std::vector<QColor> LedPreviewWidget::collectRegionColors(ScreenRegion region) const
{
    std::vector<QColor> result;

    for (const auto controller : pluginInterface->GetRGBControllers())
    {
        const auto location = controller->GetLocation();
        if (!settings.isControllerSelected(location))
            continue;

        const auto it = deviceColors.find(location);
        if (it == deviceColors.end())
            continue;

        const auto &colors = it->second;

        if (settings.getMappingMode(location) == MappingMode::Standard)
        {
            LedRange range;
            switch (region)
            {
                case ScreenRegion::Top:    range = settings.getTopRegion(location);    break;
                case ScreenRegion::Bottom: range = settings.getBottomRegion(location); break;
                case ScreenRegion::Left:   range = settings.getLeftRegion(location);   break;
                case ScreenRegion::Right:  range = settings.getRightRegion(location);  break;
                default: continue;
            }

            const int lo = std::min(range.from, range.to);
            const int hi = std::min(std::max(range.from, range.to), static_cast<int>(colors.size()));
            for (int i = hi - 1; i >= lo; --i)
            {
                const RGBColor c = colors[i];
                result.emplace_back(
                    static_cast<int>(c & 0xFF),
                    static_cast<int>((c >> 8) & 0xFF),
                    static_cast<int>((c >> 16) & 0xFF)
                );
            }
        }
        else
        {
            const auto zoneCount = controller->GetZoneCount();
            for (auto i = 0u; i < zoneCount; ++i)
            {
                const auto zone = controller->GetZone(i);
                if (!settings.isZoneEnabled(location, zone.name))
                    continue;

                const auto parts = settings.getZoneParts(location, zone.name);
                for (const auto &part : parts)
                {
                    if (part.region != region)
                        continue;

                    const int absFrom = static_cast<int>(zone.start_idx) + part.from;
                    const int absTo   = static_cast<int>(zone.start_idx) + part.to;
                    const int lo      = std::min(absFrom, absTo);
                    const int hi      = std::max(absFrom, absTo);

                    const int segStart = static_cast<int>(result.size());
                    for (int i = lo; i < hi && i < static_cast<int>(colors.size()); ++i)
                    {
                        const RGBColor c = colors[i];
                        result.emplace_back(
                            static_cast<int>(c & 0xFF),
                            static_cast<int>((c >> 8) & 0xFF),
                            static_cast<int>((c >> 16) & 0xFF)
                        );
                    }
                    if (!part.reversed)
                        std::reverse(result.begin() + segStart, result.end());
                }
            }
        }
    }

    return result;
}

void LedPreviewWidget::paintEvent(QPaintEvent *)
{
    QPainter painter{this};

    const int w = width();
    const int h = height();
    const int e = EDGE;

    painter.fillRect(rect(), Qt::black);

    // Draw captured frame in the inner area
    if (!currentFrame.isNull())
    {
        const QRect frameRect{e, e, w - 2 * e, h - 2 * e};
        painter.drawImage(frameRect, currentFrame.scaled(frameRect.size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    }

    // Draw horizontal LED strip (top or bottom edge)
    auto drawHStrip = [&](const std::vector<QColor> &colors, int x, int y, int stripW, int stripH) {
        if (colors.empty()) return;
        const int n = static_cast<int>(colors.size());
        for (int i = 0; i < n; ++i)
        {
            const int rx = x + i * stripW / n;
            const int rw = (i + 1) * stripW / n - i * stripW / n;
            painter.fillRect(rx, y, rw, stripH, colors[i]);
        }
    };

    // Draw vertical LED strip (left or right edge)
    auto drawVStrip = [&](const std::vector<QColor> &colors, int x, int y, int stripW, int stripH) {
        if (colors.empty()) return;
        const int n = static_cast<int>(colors.size());
        for (int i = 0; i < n; ++i)
        {
            const int ry = y + i * stripH / n;
            const int rh = (i + 1) * stripH / n - i * stripH / n;
            painter.fillRect(x, ry, stripW, rh, colors[i]);
        }
    };

    drawHStrip(collectRegionColors(ScreenRegion::Top),    e,     0,     w - 2 * e, e);
    drawHStrip(collectRegionColors(ScreenRegion::Bottom), e,     h - e, w - 2 * e, e);
    drawVStrip(collectRegionColors(ScreenRegion::Left),   0,     e,     e, h - 2 * e);
    drawVStrip(collectRegionColors(ScreenRegion::Right),  w - e, e,     e, h - 2 * e);
}
