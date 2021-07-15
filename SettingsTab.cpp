//
// Created by Kamil Rojewski on 15.07.2021.
//

#include <QVBoxLayout>
#include <QCheckBox>
#include <QPixmap>
#include <QLabel>

#include "DeviceList.h"

#include "SettingsTab.h"

SettingsTab::SettingsTab(ResourceManager *resourceManager, Settings &settings, QWidget *parent)
    : QWidget{parent}
{
    const auto layout = new QVBoxLayout{this};

    const auto previewBtn = new QCheckBox{"Preview"};
    connect(previewBtn, &QCheckBox::stateChanged, this, [=](auto state) {
        emit previewChanged(state == Qt::Checked);
    });

    layout->addWidget(previewBtn);

    preview = new QLabel{};
    preview->setMaximumSize(200, 100);
    preview->setScaledContents(true);

    layout->addWidget(preview);

    const auto deviceList = new DeviceList{resourceManager, settings};
    connect(this, &SettingsTab::controllerListChanged, deviceList, &DeviceList::fillControllerList);

    layout->addWidget(deviceList);
}

void SettingsTab::updatePreview(const QImage &image)
{
    preview->setPixmap(QPixmap::fromImage(image));
}
