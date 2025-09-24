//
// Created by Kamil Rojewski on 15.07.2021.
//

#ifndef OPENRGB_AMBIENT_REGIONSWIDGET_H
#define OPENRGB_AMBIENT_REGIONSWIDGET_H

#include <string>

#include <QWidget>

class ResourceManagerInterface;
class RegionWidget;
class Settings;

class RegionsWidget
        : public QWidget
{
    Q_OBJECT

public:
    RegionsWidget(ResourceManagerInterface *resourceManager, Settings &settings, QWidget *parent = nullptr);
    ~RegionsWidget() override = default;

public slots:
    void selectController(const QString &location);
    void setPreview(bool enabled);

private:
    ResourceManagerInterface *resourceManager = nullptr;
    Settings &settings;

    RegionWidget *top = nullptr;
    RegionWidget *bottom = nullptr;
    RegionWidget *right = nullptr;
    RegionWidget *left = nullptr;

    std::string currentLocation;
    bool preview = false;

    void showCurrentLeds(int from, int to);
    void clearCurrentLeds();
};

#endif //OPENRGB_AMBIENT_REGIONSWIDGET_H
