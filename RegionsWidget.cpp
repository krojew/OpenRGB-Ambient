//
// Created by Kamil Rojewski on 15.07.2021.
//

#include <algorithm>

#include <QFormLayout>

#include <ResourceManager.h>

#include "RegionWidget.h"
#include "Settings.h"

#include "RegionsWidget.h"

RegionsWidget::RegionsWidget(ResourceManager *resourceManager, Settings &settings, QWidget *parent)
    : QWidget{parent}
    , resourceManager{resourceManager}
    , settings{settings}
{
    const auto layout = new QFormLayout{this};

    top = new RegionWidget{};
    connect(top, &RegionWidget::regionChanged, this, [=](auto from, auto to) {
        if (!currentLocation.empty())
        {
            this->settings.setTopRegion(currentLocation, {from, to});
            showCurrentLeds(from, to);
        }
    });

    layout->addRow("Top", top);

    bottom = new RegionWidget{};
    connect(bottom, &RegionWidget::regionChanged, this, [=](auto from, auto to) {
        if (!currentLocation.empty())
        {
            this->settings.setBottomRegion(currentLocation, {from, to});
            showCurrentLeds(from, to);
        }
    });

    layout->addRow("Bottom", bottom);

    right = new RegionWidget{};
    connect(right, &RegionWidget::regionChanged, this, [=](auto from, auto to) {
        if (!currentLocation.empty())
        {
            this->settings.setRightRegion(currentLocation, {from, to});
            showCurrentLeds(from, to);
        }
    });

    layout->addRow("Right", right);

    left = new RegionWidget{};
    connect(left, &RegionWidget::regionChanged, this, [=](auto from, auto to) {
        if (!currentLocation.empty())
        {
            this->settings.setLeftRegion(currentLocation, {from, to});
            showCurrentLeds(from, to);
        }
    });

    layout->addRow("Left", left);
}

void RegionsWidget::selectController(const QString &location)
{
    currentLocation = location.toStdString();

    const auto &controllers = resourceManager->GetRGBControllers();
    const auto controller = std::find_if(std::begin(controllers), std::end(controllers), [&](auto controller) {
        return controller->location == currentLocation;
    });

    if (controller == std::end(controllers))
    {
        top->setConfiguration(0, 0, 0);
        bottom->setConfiguration(0, 0, 0);
        right->setConfiguration(0, 0, 0);
        left->setConfiguration(0, 0, 0);

        return;
    }

    const auto topRange = settings.getTopRegion(currentLocation);
    const auto bottomRange = settings.getBottomRegion(currentLocation);
    const auto rightRange = settings.getRightRegion(currentLocation);
    const auto leftRange = settings.getLeftRegion(currentLocation);

    const auto max = static_cast<int>((*controller)->leds.size()) + 1;

    top->setConfiguration(max, topRange.from, topRange.to);
    bottom->setConfiguration(max, bottomRange.from, bottomRange.to);
    right->setConfiguration(max, rightRange.from, rightRange.to);
    left->setConfiguration(max, leftRange.from, leftRange.to);
}

void RegionsWidget::showCurrentLeds(int from, int to)
{
    const auto &controllers = resourceManager->GetRGBControllers();
    const auto controller = std::find_if(std::begin(controllers), std::end(controllers), [&](auto controller) {
        return controller->location == currentLocation;
    });

    if (controller == std::end(controllers))
        return;

    const auto realFrom = std::min(from, to);
    const auto realTo = std::max(from, to);

    const auto len = (*controller)->leds.size();

    for (auto i = 0; i < realFrom; ++i)
        (*controller)->SetLED(i, 0);

    for (auto i = realFrom; i < realTo; ++i)
        (*controller)->SetLED(i, ToRGBColor(255, 255, 255));

    for (auto i = realTo; i < len; ++i)
        (*controller)->SetLED(i, 0);

    (*controller)->UpdateLEDs();
}
