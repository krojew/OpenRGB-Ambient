//
// Created by Kamil Rojewski on 15.07.2021.
//

#include <QHBoxLayout>
#include <QCheckBox>
#include <QComboBox>
#include <QSlider>
#include <QLabel>

#include "ColorConversion.h"
#include "RegionsWidget.h"
#include "DeviceList.h"
#include "Settings.h"

#include "SettingsTab.h"

SettingsTab::SettingsTab(ResourceManagerInterface *resourceManager, Settings &settings, QWidget *parent)
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

    const auto smoothTransitionsBtn = new QCheckBox{"Smooth transitions"};
    smoothTransitionsBtn->setChecked(settings.smoothTransitions());
    colorCorrectionLayout->addWidget(smoothTransitionsBtn);

    const auto smoothTransitionsWeightSlider = new QSlider{Qt::Horizontal, this};
    smoothTransitionsWeightSlider->setEnabled(settings.smoothTransitions());
    smoothTransitionsWeightSlider->setRange(1, 99);
    smoothTransitionsWeightSlider->setValue(static_cast<int>(settings.smoothTransitionsWeight() * 100));
    connect(smoothTransitionsWeightSlider, &QSlider::valueChanged, this, [&, smoothTransitionsWeightSlider](auto to) {
        settings.setSmoothTransitionsWeight(static_cast<float>(smoothTransitionsWeightSlider->value()) * 0.01f);
    });

    colorCorrectionLayout->addWidget(smoothTransitionsWeightSlider);

    connect(smoothTransitionsBtn, &QCheckBox::stateChanged, this, [&, smoothTransitionsWeightSlider](auto state) {
        settings.setSmoothTransitions(state == Qt::Checked);
        smoothTransitionsWeightSlider->setEnabled(settings.smoothTransitions());
    });

    mainLayout->addLayout(colorCorrectionLayout);

    const auto deviceList = new DeviceList{resourceManager, settings};
    connect(this, &SettingsTab::controllerListChanged, deviceList, &DeviceList::fillControllerList);
    connect(deviceList, &DeviceList::controllerSelected, regionsWidget, &RegionsWidget::selectController);

    mainLayout->addWidget(deviceList);
}

void SettingsTab::updatePreview(const QImage &image) const
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
