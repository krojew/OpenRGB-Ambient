#ifndef OPENRGB_AMBIENT_REGIONSWIDGET_H
#define OPENRGB_AMBIENT_REGIONSWIDGET_H

#include <string>

#include <QWidget>

class OpenRGBPluginAPIInterface;
class QFormLayout;
class QRadioButton;
class RegionWidget;
class Settings;

class RegionsWidget
        : public QWidget
{
    Q_OBJECT

public:
    RegionsWidget(OpenRGBPluginAPIInterface *pluginInterface, Settings &settings, QWidget *parent = nullptr);
    ~RegionsWidget() override = default;

public slots:
    void selectController(const QString &location);
    void setPreview(bool enabled);

private:
    OpenRGBPluginAPIInterface *pluginInterface = nullptr;
    Settings &settings;

    RegionWidget *top = nullptr;
    RegionWidget *bottom = nullptr;
    RegionWidget *right = nullptr;
    RegionWidget *left = nullptr;

    std::string currentLocation;
    bool preview = false;

    QRadioButton *standardRadio     = nullptr;
    QRadioButton *zoneRadio         = nullptr;
    QWidget     *standardContainer = nullptr;
    QWidget     *zoneContainer     = nullptr;
    QWidget     *zonesContainer    = nullptr;
    QFormLayout *zonesLayout       = nullptr;

    void showCurrentLeds(int from, int to);
    void clearCurrentLeds();
    void rebuildZoneRows();
};

#endif //OPENRGB_AMBIENT_REGIONSWIDGET_H
