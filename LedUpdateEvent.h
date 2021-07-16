//
// Created by Kamil Rojewski on 16.07.2021.
//

#ifndef OPENRGB_AMBIENT_LEDUPDATEEVENT_H
#define OPENRGB_AMBIENT_LEDUPDATEEVENT_H

#include <string>
#include <vector>

#include <QEvent>

#include <RGBController.h>

class LedUpdateEvent
        : public QEvent
{
public:
    static const int eventType;

    LedUpdateEvent(std::string controllerLocation, std::vector<RGBColor> colors);
    ~LedUpdateEvent() override = default;

    [[nodiscard]] const std::string &getControllerLocation() const;
    [[nodiscard]] const std::vector<RGBColor> &getColors() const;

private:
    std::string controllerLocation;
    std::vector<RGBColor> colors;
};

#endif //OPENRGB_AMBIENT_LEDUPDATEEVENT_H
