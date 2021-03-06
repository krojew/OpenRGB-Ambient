//
// Created by Kamil Rojewski on 15.07.2021.
//

#ifndef OPENRGB_AMBIENT_DEVICELIST_H
#define OPENRGB_AMBIENT_DEVICELIST_H

#include <QWidget>

class ResourceManager;
class QListWidgetItem;
class QListWidget;
class Settings;

class DeviceList
        : public QWidget
{
    Q_OBJECT

public:
    DeviceList(ResourceManager *resourceManager, Settings &settings, QWidget *parent = nullptr);
    ~DeviceList() override = default;

signals:
    void controllerSelected(const QString &location);

public slots:
    void fillControllerList();
    void saveCheckState(QListWidgetItem *item);

private:
    ResourceManager *resourceManager;
    Settings &settings;
    QListWidget *deviceList;
};

#endif //OPENRGB_AMBIENT_DEVICELIST_H
