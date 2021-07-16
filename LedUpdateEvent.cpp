//
// Created by Kamil Rojewski on 16.07.2021.
//

#include <string>
#include "LedUpdateEvent.h"

const int LedUpdateEvent::eventType = QEvent::registerEventType();

LedUpdateEvent::LedUpdateEvent(std::string controllerLocation, std::vector<RGBColor> colors)
    : QEvent{static_cast<Type>(eventType)}
    , controllerLocation{std::move(controllerLocation)}
    , colors{std::move(colors)}
{
}

const std::string &LedUpdateEvent::getControllerLocation() const
{
    return controllerLocation;
}

const std::vector<RGBColor> &LedUpdateEvent::getColors() const
{
    return colors;
}
