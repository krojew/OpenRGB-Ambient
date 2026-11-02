#ifndef OPENRGB_AMBIENT_DEVICELIST_H
#define OPENRGB_AMBIENT_DEVICELIST_H

#include <QWidget>

class OpenRGBPluginAPIInterface;
class QListWidgetItem;
class QListWidget;
class Settings;

class DeviceList
        : public QWidget
{
    Q_OBJECT

public:
    DeviceList(OpenRGBPluginAPIInterface *pluginInterface, Settings &settings, QWidget *parent = nullptr);
    ~DeviceList() override = default;

signals:
    void controllerSelected(const QString &location);

public slots:
    void fillControllerList() const;

private slots:
    void onItemChanged(QListWidgetItem *item) const;

private:
    OpenRGBPluginAPIInterface *pluginInterface;
    Settings &settings;
    QListWidget *deviceList;
};

#endif //OPENRGB_AMBIENT_DEVICELIST_H
