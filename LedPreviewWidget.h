//

#ifndef OPENRGB_AMBIENT_LEDPREVIEWWIDGET_H
#define OPENRGB_AMBIENT_LEDPREVIEWWIDGET_H

#include <string>
#include <vector>
#include <unordered_map>

#include <QImage>
#include <QWidget>

#include <RGBController.h>

#include "ZoneMapping.h"

class ResourceManagerInterface;
class Settings;
class QPaintEvent;

class LedPreviewWidget : public QWidget
{
    Q_OBJECT

public:
    LedPreviewWidget(ResourceManagerInterface *resourceManager, Settings &settings, QWidget *parent = nullptr);
    ~LedPreviewWidget() override = default;

public slots:
    void updateFrame(const QImage &image);
    void updateLedColors(const QString &location, const std::vector<RGBColor> &colors);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    static constexpr int EDGE = 14; // px for each LED strip border

    ResourceManagerInterface *resourceManager;
    Settings &settings;

    QImage currentFrame;
    std::unordered_map<std::string, std::vector<RGBColor>> deviceColors;

    [[nodiscard]] std::vector<QColor> collectRegionColors(ScreenRegion region) const;
};

#endif //OPENRGB_AMBIENT_LEDPREVIEWWIDGET_H
