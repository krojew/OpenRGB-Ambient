//
// Created by Kamil Rojewski on 15.07.2021.
//

#include <QHBoxLayout>
#include <QCheckBox>
#include <QLabel>

#include "RegionsWidget.h"
#include "DeviceList.h"
#include "Settings.h"

#include "SettingsTab.h"

SettingsTab::SettingsTab(ResourceManager *resourceManager, Settings &settings, QWidget *parent)
    : QWidget{parent}
{
    const auto mainLayout = new QVBoxLayout{this};
    const auto topLayout = new QHBoxLayout{};
    const auto previewLayout = new QVBoxLayout{};

    const auto previewBtn = new QCheckBox{"Preview (not supported for HDR)"};
    connect(previewBtn, &QCheckBox::stateChanged, this, [=](auto state) {
        emit previewChanged(state == Qt::Checked);
    });

    previewLayout->addWidget(previewBtn);

    preview = new QLabel{};
    preview->setMaximumSize(200, 100);
    preview->setScaledContents(true);

    previewLayout->addWidget(preview);

    topLayout->addLayout(previewLayout);

    const auto regionsWidget = new RegionsWidget{resourceManager, settings};
    topLayout->addWidget(regionsWidget);

    mainLayout->addLayout(topLayout);

    const auto coolWhiteCompensationBtn = new QCheckBox{"Cool white LED compensation", this};
    coolWhiteCompensationBtn->setChecked(settings.compensateCoolWhite());
    connect(coolWhiteCompensationBtn, &QCheckBox::stateChanged, this, [&](auto state) {
        settings.setCoolWhiteCompensation(state == Qt::Checked);
    });
    mainLayout->addWidget(coolWhiteCompensationBtn);

    const auto deviceList = new DeviceList{resourceManager, settings};
    connect(this, &SettingsTab::controllerListChanged, deviceList, &DeviceList::fillControllerList);
    connect(deviceList, &DeviceList::controllerSelected, regionsWidget, &RegionsWidget::selectController);

    mainLayout->addWidget(deviceList);
}

void SettingsTab::updatePreview(const QImage &image)
{
    preview->setPixmap(QPixmap::fromImage(image));
}

void SettingsTab::showEvent(QShowEvent *event)
{
    emit settingsVisibilityChanged(true);

    QWidget::showEvent(event);
}

void SettingsTab::hideEvent(QHideEvent *event)
{
    emit settingsVisibilityChanged(false);

    QWidget::hideEvent(event);
}
