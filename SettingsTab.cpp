//
// Created by Kamil Rojewski on 15.07.2021.
//

#include <QColorDialog>
#include <QHBoxLayout>
#include <QCheckBox>
#include <QComboBox>
#include <QPushButton>
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

    // Color correction row (checkbox + saturation + wall color)
    const auto extraCorrectionLayout = new QHBoxLayout{};

    const auto colorCorrectionCheck = new QCheckBox{"Color correction", this};
    colorCorrectionCheck->setChecked(settings.colorCorrectionEnabled());
    extraCorrectionLayout->addWidget(colorCorrectionCheck);

    const auto saturationLabel = new QLabel{this};
    auto updateSatLabel = [=, &settings]() {
        saturationLabel->setText(QString("Sat: ×%1").arg(settings.saturationBoost(), 0, 'f', 2));
    };
    updateSatLabel();

    const auto saturationSlider = new QSlider{Qt::Horizontal, this};
    saturationSlider->setRange(0, 300);
    saturationSlider->setValue(static_cast<int>(settings.saturationBoost() * 100.0f));
    saturationSlider->setToolTip("100 = neutral. Increase to make colors more vivid.");
    saturationSlider->setEnabled(settings.colorCorrectionEnabled());
    connect(saturationSlider, &QSlider::valueChanged, this, [=, &settings](int v) {
        settings.setSaturationBoost(v * 0.01f);
        updateSatLabel();
    });
    extraCorrectionLayout->addWidget(saturationLabel);
    extraCorrectionLayout->addWidget(saturationSlider);

    const auto wallColorLabel = new QLabel{"Wall color:", this};
    const auto wallColorBtn = new QPushButton{this};
    wallColorBtn->setFixedWidth(90);
    wallColorBtn->setEnabled(settings.colorCorrectionEnabled());
    auto updateWallBtnColor = [=, &settings]() {
        const QColor c = settings.wallColor();
        wallColorBtn->setStyleSheet(
            QString("background-color: %1; color: %2;")
                .arg(c.name())
                .arg(c.lightness() > 128 ? "black" : "white")
        );
        wallColorBtn->setText(c.name().toUpper());
    };
    updateWallBtnColor();
    connect(wallColorBtn, &QPushButton::clicked, this, [=, &settings]() {
        const QColor chosen = QColorDialog::getColor(settings.wallColor(), this, "Select wall color");
        if (chosen.isValid())
        {
            settings.setWallColor(chosen);
            updateWallBtnColor();
        }
    });
    extraCorrectionLayout->addWidget(wallColorLabel);
    extraCorrectionLayout->addWidget(wallColorBtn);
    extraCorrectionLayout->addStretch();

    connect(colorCorrectionCheck, &QCheckBox::stateChanged, this, [=, &settings](int state) {
        const bool enabled = (state == Qt::Checked);
        settings.setColorCorrectionEnabled(enabled);
        saturationSlider->setEnabled(enabled);
        wallColorLabel->setEnabled(enabled);
        wallColorBtn->setEnabled(enabled);
    });

    mainLayout->addLayout(extraCorrectionLayout);

    // Brightness row
    const auto brightnessLayout = new QHBoxLayout{};

    const auto brightnessCheck = new QCheckBox{"Brightness", this};
    brightnessCheck->setChecked(settings.brightnessEnabled());
    brightnessLayout->addWidget(brightnessCheck);

    const auto brightnessValueLabel = new QLabel{this};
    auto updateBrightnessLabel = [=, &settings]() {
        brightnessValueLabel->setText(QString("%1").arg(settings.brightness(), 0, 'f', 2));
    };
    updateBrightnessLabel();

    const auto brightnessSlider = new QSlider{Qt::Horizontal, this};
    brightnessSlider->setRange(0, 100);
    brightnessSlider->setValue(static_cast<int>(settings.brightness() * 100.0f));
    brightnessSlider->setEnabled(settings.brightnessEnabled());
    connect(brightnessSlider, &QSlider::valueChanged, this, [=, &settings](int v) {
        settings.setBrightness(static_cast<float>(v) / 100.0f);
        updateBrightnessLabel();
    });

    brightnessLayout->addWidget(brightnessValueLabel);
    brightnessLayout->addWidget(brightnessSlider);
    brightnessLayout->addStretch();

    connect(brightnessCheck, &QCheckBox::stateChanged, this, [=, &settings](int state) {
        const bool enabled = (state == Qt::Checked);
        settings.setBrightnessEnabled(enabled);
        brightnessSlider->setEnabled(enabled);
    });

    mainLayout->addLayout(brightnessLayout);

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
