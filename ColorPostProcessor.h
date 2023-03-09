//
// Created by Kamil Rojewski on 09.03.2023.
//

#ifndef OPENRGB_AMBIENT_COLORPOSTPROCESSOR_H
#define OPENRGB_AMBIENT_COLORPOSTPROCESSOR_H

#include <concepts>

#include <QtGlobal>

#include <RGBController.h>

template<class T>
concept ColorPostProcessor = requires(T cpp) {
    { cpp.process(uchar(), uchar(), uchar(), RGBColor()) } noexcept -> std::convertible_to<RGBColor>;
};

struct IdentityColorPostProcessor
{
#pragma clang diagnostic push
#pragma ide diagnostic ignored "readability-convert-member-functions-to-static"
    [[nodiscard]] inline RGBColor process(uchar red, uchar green, uchar blue, RGBColor previous) const noexcept {
        return ToRGBColor(red, green, blue);
    }
#pragma clang diagnostic pop
};

struct SmoothingColorPostProcessor
{
    float weight;

    [[nodiscard]] inline RGBColor process(uchar red, uchar green, uchar blue, RGBColor previous) const noexcept {
        const auto previousWeight = 1 - weight;
        return ToRGBColor(
                static_cast<uchar>(red * weight + RGBGetRValue(previous) * previousWeight),
                static_cast<uchar>(green * weight + RGBGetGValue(previous) * previousWeight),
                static_cast<uchar>(blue * weight + RGBGetBValue(previous) * previousWeight)
        );
    }
};

#endif //OPENRGB_AMBIENT_COLORPOSTPROCESSOR_H
