//
// Created by Kamil Rojewski on 15.07.2021.
//

#include "ColorConversion.h"

#include "Settings.h"

QString Settings::SELECTED_CONTROLLERS_KEY = "SelectedControllers"; // NOLINT(cert-err58-cpp)
QString Settings::CONTROLLER_REGIONS_KEY = "ControllerRegions"; // NOLINT(cert-err58-cpp)
QString Settings::COOL_WHITE_COMPENSATION_KEY = "CoolWhiteCompensation"; // NOLINT(cert-err58-cpp)
QString Settings::COLOR_TEMPERATURE_KEY = "ColorTemperature"; // NOLINT(cert-err58-cpp)
QString Settings::SMOOTHING_KEY = "Smoothing"; // NOLINT(cert-err58-cpp)
QString Settings::SMOOTHING_WEIGHT_KEY = "SmoothingWeight"; // NOLINT(cert-err58-cpp)

QString Settings::TOP_SUFFIX = "_Top"; // NOLINT(cert-err58-cpp)
QString Settings::BOTTOM_SUFFIX = "_Bottom"; // NOLINT(cert-err58-cpp)
QString Settings::LEFT_SUFFIX = "_Left"; // NOLINT(cert-err58-cpp)
QString Settings::RIGHT_SUFFIX = "_Right"; // NOLINT(cert-err58-cpp)

Settings::Settings(const QString &file, QObject *parent)
    : QObject{parent}
    , settings{file, QSettings::IniFormat}
{
    const auto selectedList = settings.value(SELECTED_CONTROLLERS_KEY).toStringList();
    for (const auto &selected : selectedList)
        selectedControllers.insert(selected.toStdString());

    fillRegions(topRegions, settings.value(CONTROLLER_REGIONS_KEY + TOP_SUFFIX).toHash());
    fillRegions(bottomRegions, settings.value(CONTROLLER_REGIONS_KEY + BOTTOM_SUFFIX).toHash());
    fillRegions(leftRegions, settings.value(CONTROLLER_REGIONS_KEY + LEFT_SUFFIX).toHash());
    fillRegions(rightRegions, settings.value(CONTROLLER_REGIONS_KEY + RIGHT_SUFFIX).toHash());

    coolWhiteCompensation = settings.value(COOL_WHITE_COMPENSATION_KEY, true).toBool();
    colorTemperatureIndex = std::clamp(
            settings.value(COLOR_TEMPERATURE_KEY, colorTemperatureIndex).toInt(),
            0,
            static_cast<int>(std::extent_v<decltype(colorTemperatureFactors)>)
    );

    smoothing = settings.value(SMOOTHING_KEY, false).toBool();
    smoothingWeight = settings.value(SMOOTHING_WEIGHT_KEY, 0.5).toFloat();
}

bool Settings::isControllerSelected(const std::string &location) const {
    return selectedControllers.find(location) != std::end(selectedControllers);
}

void Settings::selectController(const std::string &location) {
    selectedControllers.insert(location);
    syncSelectedControllers();
}

void Settings::unselectController(const std::string &location) {
    selectedControllers.erase(location);
    syncSelectedControllers();
}

LedRange Settings::getTopRegion(const std::string &location) const
{
    return findRange(topRegions, location);
}

LedRange Settings::getBottomRegion(const std::string &location) const
{
    return findRange(bottomRegions, location);
}

LedRange Settings::getRightRegion(const std::string &location) const
{
    return findRange(rightRegions, location);
}

LedRange Settings::getLeftRegion(const std::string &location) const
{
    return findRange(leftRegions, location);
}

void Settings::setTopRegion(const std::string &location, LedRange range)
{
    topRegions[location] = range;
    syncRegions(topRegions, CONTROLLER_REGIONS_KEY + TOP_SUFFIX);
}

void Settings::setBottomRegion(const std::string &location, LedRange range)
{
    bottomRegions[location] = range;
    syncRegions(bottomRegions, CONTROLLER_REGIONS_KEY + BOTTOM_SUFFIX);
}

void Settings::setRightRegion(const std::string &location, LedRange range)
{
    rightRegions[location] = range;
    syncRegions(rightRegions, CONTROLLER_REGIONS_KEY + RIGHT_SUFFIX);
}

void Settings::setLeftRegion(const std::string &location, LedRange range)
{
    leftRegions[location] = range;
    syncRegions(leftRegions, CONTROLLER_REGIONS_KEY + LEFT_SUFFIX);
}

void Settings::syncSelectedControllers()
{
    QStringList value;
    for (const auto &selected : selectedControllers)
        value.append(QString::fromStdString(selected));

    settings.setValue(SELECTED_CONTROLLERS_KEY, value);

    emit settingsChanged();
}

void Settings::syncRegions(const RegionMap &map, const QString &key)
{
    QVariantHash data;
    for (const auto &region : map)
        data[QString::fromStdString(region.first)] = QVariantList{region.second.from, region.second.to};

    settings.setValue(key, data);

    emit settingsChanged();
}

void Settings::fillRegions(RegionMap &map, const QVariantHash &data)
{
    QHashIterator i{data};
    while (i.hasNext())
    {
        i.next();

        const auto list = i.value().toList();
        map[i.key().toStdString()] = LedRange{list[0].toInt(), list[1].toInt()};
    }
}

LedRange Settings::findRange(const RegionMap &map, const std::string &location)
{
    const auto entry = map.find(location);
    return (entry != std::end(map)) ? (entry->second) : (LedRange{});
}

bool Settings::compensateCoolWhite() const noexcept
{
    return coolWhiteCompensation;
}

void Settings::setCoolWhiteCompensation(bool value)
{
    coolWhiteCompensation = value;
    settings.setValue(COOL_WHITE_COMPENSATION_KEY, coolWhiteCompensation);

    emit settingsChanged();
}

int Settings::colorTemperatureFactorIndex() const noexcept
{
    return colorTemperatureIndex;
}

void Settings::setColorTemperatureFactorIndex(int index)
{
    colorTemperatureIndex = index;
    settings.setValue(COLOR_TEMPERATURE_KEY, colorTemperatureIndex);

    emit settingsChanged();
}

bool Settings::smoothTransitions() const noexcept
{
    return smoothing;
}

void Settings::setSmoothTransitions(bool value)
{
    smoothing = value;
    settings.setValue(SMOOTHING_KEY, smoothing);

    emit settingsChanged();
}

float Settings::smoothTransitionsWeight() const noexcept
{
    return smoothingWeight;
}

void Settings::setSmoothTransitionsWeight(float value)
{
    smoothingWeight = value;
    settings.setValue(SMOOTHING_WEIGHT_KEY, smoothingWeight);

    emit settingsChanged();
}
