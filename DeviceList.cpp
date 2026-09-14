#include <algorithm>

#include <QHBoxLayout>
#include <QListWidget>

#include <ResourceManager.h>

#include "Settings.h"

#include "DeviceList.h"

#include <OpenRGBPluginInterface.h>

static constexpr int LOC_ROLE = Qt::UserRole;

DeviceList::DeviceList(OpenRGBPluginAPIInterface *pluginInterface, Settings &settings, QWidget *parent)
    : QWidget{parent}
    , pluginInterface{pluginInterface}
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

        const auto &controllers = pluginInterface->GetRGBControllers();
        for (const auto controller : controllers)
        {
            auto hasDirect = false;

            const auto modeCount = controller->GetModeCount();
            for (auto i = 0u; i < modeCount; ++i)
            {
                if (controller->GetModeName(i) == "Direct")
                {
                    hasDirect = true;
                    break;
                }
            }

            if (!hasDirect)
                continue;

            const auto location = controller->GetLocation();

            const auto item = new QListWidgetItem{QString::fromStdString(controller->GetName())};
            item->setData(LOC_ROLE, QString::fromStdString(location));
            item->setCheckState(settings.isControllerSelected(location) ? Qt::Checked : Qt::Unchecked);
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
