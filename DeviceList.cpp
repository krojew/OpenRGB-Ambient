//
// Created by Kamil Rojewski on 15.07.2021.
//
#include <algorithm>

#include <QHBoxLayout>
#include <QListWidget>

#include <ResourceManager.h>

#include "Settings.h"

#include "DeviceList.h"

#include <RGBController.h>

DeviceList::DeviceList(ResourceManagerInterface *resourceManager, Settings &settings, QWidget *parent)
    : QWidget{parent}
    , resourceManager{resourceManager}
    , settings{settings}
{
    const auto layout = new QHBoxLayout{this};

    deviceList = new QListWidget{};
    connect(deviceList, &QListWidget::itemChanged, this, &DeviceList::saveCheckState);
    connect(deviceList, &QListWidget::currentItemChanged, this, [this](auto current, auto previous) {
        if (current != nullptr)
            emit controllerSelected(current->data(Qt::UserRole).toString());
    });

    layout->addWidget(deviceList);
}

void DeviceList::fillControllerList() const
{
    deviceList->clear();

    const auto &controllers = resourceManager->GetRGBControllers();
    for (const auto controller : controllers)
    {
        // if (std::ranges::none_of(controller->modes, [](const auto &mode) {
        //     return mode.name == "Direct";
        // }))
        // {
        //     continue;
        // }

        const auto item = new QListWidgetItem{QString::fromStdString(controller->name)};
        item->setCheckState(settings.isControllerSelected(controller->location) ? Qt::Checked : Qt::Unchecked);
        item->setData(Qt::UserRole, QString::fromStdString(controller->location));

        deviceList->addItem(item);
    }
}

void DeviceList::saveCheckState(QListWidgetItem *item) const
{
    const auto location = item->data(Qt::UserRole).toString().toStdString();
    if (item->checkState() == Qt::Checked)
        settings.selectController(location);
    else
        settings.unselectController(location);
}
