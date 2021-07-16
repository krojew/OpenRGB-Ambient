//
// Created by Kamil Rojewski on 15.07.2021.
//

#ifndef OPENRGB_AMBIENT_REGIONSWIDGET_H
#define OPENRGB_AMBIENT_REGIONSWIDGET_H

#include <string>

#include <QWidget>

class ResourceManager;
class RegionWidget;
class Settings;

class RegionsWidget
        : public QWidget
{
    Q_OBJECT

public:
    RegionsWidget(ResourceManager *resourceManager, Settings &settings, QWidget *parent = nullptr);
    ~RegionsWidget() override = default;

public slots:
    void selectController(const QString &location);

private:
    ResourceManager *resourceManager = nullptr;
    Settings &settings;

    RegionWidget *top = nullptr;
    RegionWidget *bottom = nullptr;
    RegionWidget *right = nullptr;
    RegionWidget *left = nullptr;

    std::string currentLocation;

    void showCurrentLeds(int from, int to);
};

#endif //OPENRGB_AMBIENT_REGIONSWIDGET_H
