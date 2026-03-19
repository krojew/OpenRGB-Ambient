//
// Created by Kamil Rojewski on 09.03.2023.
//

#ifndef OPENRGB_AMBIENT_COLORPOSTPROCESSOR_H
#define OPENRGB_AMBIENT_COLORPOSTPROCESSOR_H

#include <algorithm>
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

template<ColorPostProcessor Inner>
struct SaturatingColorPostProcessor
{
    float saturation;  // 1.0 = no change; >1 = more vivid; 0 = fully gray
    Inner inner;

    [[nodiscard]] inline RGBColor process(uchar red, uchar green, uchar blue, RGBColor previous) const noexcept {
        const float r = red, g = green, b = blue;
        const float gray = (r + g + b) / 3.0f;
        const auto sr = static_cast<uchar>(std::clamp(gray + (r - gray) * saturation, 0.0f, 255.0f));
        const auto sg = static_cast<uchar>(std::clamp(gray + (g - gray) * saturation, 0.0f, 255.0f));
        const auto sb = static_cast<uchar>(std::clamp(gray + (b - gray) * saturation, 0.0f, 255.0f));
        return inner.process(sr, sg, sb, previous);
    }
};

template<ColorPostProcessor Inner>
struct BrightnessColorPostProcessor
{
    float brightness;  // [0.0, 1.0]; 1.0 = full brightness, 0.0 = off
    Inner inner;

    [[nodiscard]] inline RGBColor process(uchar red, uchar green, uchar blue, RGBColor previous) const noexcept {
        const RGBColor result = inner.process(red, green, blue, previous);
        return ToRGBColor(
                static_cast<uchar>(RGBGetRValue(result) * brightness),
                static_cast<uchar>(RGBGetGValue(result) * brightness),
                static_cast<uchar>(RGBGetBValue(result) * brightness)
        );
    }
};

#endif //OPENRGB_AMBIENT_COLORPOSTPROCESSOR_H
