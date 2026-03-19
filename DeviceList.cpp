//
// Created by Kamil Rojewski on 15.07.2021.
//
#include <algorithm>

#include <QHBoxLayout>
#include <QListWidget>

#include <ResourceManager.h>
#include <RGBController.h>

#include "Settings.h"

#include "DeviceList.h"

static constexpr int LOC_ROLE = Qt::UserRole;

DeviceList::DeviceList(ResourceManagerInterface *resourceManager, Settings &settings, QWidget *parent)
    : QWidget{parent}
    , resourceManager{resourceManager}
    , settings{settings}
{
    const auto layout = new QHBoxLayout{this};

    deviceList = new QListWidget{};
    connect(deviceList, &QListWidget::itemChanged, this, &DeviceList::onItemChanged);
    connect(deviceList, &QListWidget::currentItemChanged, this, [this](QListWidgetItem *current, QListWidgetItem *) {
        if (current != nullptr)
            emit controllerSelected(current->data(LOC_ROLE).toString());
    });

    layout->addWidget(deviceList);
}

void DeviceList::fillControllerList() const
{
    {
        const QSignalBlocker blocker{deviceList};
        deviceList->clear();

        const auto &controllers = resourceManager->GetRGBControllers();
        for (const auto controller : controllers)
        {
            if (std::ranges::none_of(controller->modes, [](const auto &mode) {
                return mode.name == "Direct";
            }))
                continue;

            const auto item = new QListWidgetItem{QString::fromStdString(controller->name)};
            item->setData(LOC_ROLE, QString::fromStdString(controller->location));
            item->setCheckState(settings.isControllerSelected(controller->location) ? Qt::Checked : Qt::Unchecked);
            deviceList->addItem(item);
        }
    }

    if (deviceList->count() > 0)
        deviceList->setCurrentRow(0);
}

void DeviceList::onItemChanged(QListWidgetItem *item) const
{
    const auto location = item->data(LOC_ROLE).toString().toStdString();
    if (item->checkState() == Qt::Checked)
        settings.selectController(location);
    else
        settings.unselectController(location);
}
