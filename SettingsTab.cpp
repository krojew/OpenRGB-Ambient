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
#include "ScreenCapture.h"
#include "LedPreviewWidget.h"

#include "SettingsTab.h"

SettingsTab::SettingsTab(ResourceManagerInterface *resourceManager, Settings &settings, QWidget *parent)
    : QWidget{parent}
{
    const auto mainLayout = new QVBoxLayout{this};

    // Monitor selection
    const auto monitorLayout = new QHBoxLayout{};
    monitorLayout->addWidget(new QLabel{"Monitor:", this});

    const auto monitorCombo = new QComboBox{this};
    const auto monitors = ScreenCapture::enumerateMonitors();
    int savedMonitorIdx = 0;
    for (int i = 0; i < static_cast<int>(monitors.size()); ++i)
    {
        const auto &m = monitors[i];
        monitorCombo->addItem(QString::fromStdString(m.displayName));
        if (m.adapterIndex == static_cast<UINT>(settings.monitorAdapter()) &&
            m.outputIndex  == settings.monitorOutput())
            savedMonitorIdx = i;
    }
    monitorCombo->setCurrentIndex(savedMonitorIdx);
    connect(monitorCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [&, monitors](int index) {
        if (index < 0 || index >= static_cast<int>(monitors.size()))
            return;
        settings.setMonitorAdapter(static_cast<int>(monitors[index].adapterIndex));
        settings.setMonitorOutput(monitors[index].outputIndex);
    });
    monitorLayout->addWidget(monitorCombo);
    monitorLayout->addStretch();
    mainLayout->addLayout(monitorLayout);

    const auto topLayout = new QHBoxLayout{};
    const auto previewLayout = new QVBoxLayout{};

    const auto previewBtn = new QCheckBox{"Preview"};
    connect(previewBtn, &QCheckBox::stateChanged, this, [=](auto state) {
        emit previewChanged(state == Qt::Checked);
    });

    previewLayout->addWidget(previewBtn);

    preview = new LedPreviewWidget{resourceManager, settings};

    previewLayout->addWidget(preview);

    topLayout->addLayout(previewLayout);

    const auto regionsWidget = new RegionsWidget{resourceManager, settings};
    connect(this, &SettingsTab::previewChanged, regionsWidget, &RegionsWidget::setPreview);
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
    preview->updateFrame(image);
}

void SettingsTab::updateLedColors(const QString &location, const std::vector<RGBColor> &colors) const
{
    preview->updateLedColors(location, colors);
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
