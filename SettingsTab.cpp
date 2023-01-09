//
// Created by Kamil Rojewski on 15.07.2021.
//

#include <QHBoxLayout>
#include <QCheckBox>
#include <QComboBox>
#include <QLabel>

#include "ColorConversion.h"
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

    const auto colorCorrectionLayout = new QHBoxLayout{};
    colorCorrectionLayout->addWidget(new QLabel{"Display color temperature:", this});

    const auto colorTemperatureCombo = new QComboBox{this};

    for (auto i = 0; i < std::extent_v<decltype(colorTemperatureFactors)>; ++i)
    {
        colorTemperatureCombo->addItem(QString::number(1000 + i * 100), i);
    }

    colorTemperatureCombo->setCurrentIndex(settings.colorTemperatureFactorIndex());
    connect(colorTemperatureCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [&](auto index) {
        settings.setColorTemperatureFactorIndex(index);
    });
    colorCorrectionLayout->addWidget(colorTemperatureCombo);

    const auto coolWhiteCompensationBtn = new QCheckBox{"Cool white LED compensation", this};
    coolWhiteCompensationBtn->setChecked(settings.compensateCoolWhite());
    connect(coolWhiteCompensationBtn, &QCheckBox::stateChanged, this, [&](auto state) {
        settings.setCoolWhiteCompensation(state == Qt::Checked);
    });
    colorCorrectionLayout->addWidget(coolWhiteCompensationBtn);
    colorCorrectionLayout->addStretch();

    mainLayout->addLayout(colorCorrectionLayout);

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
