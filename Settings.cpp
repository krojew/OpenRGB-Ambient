//
// Created by Kamil Rojewski on 15.07.2021.
//

#include "Settings.h"

QString Settings::SELECTED_CONTROLLERS_KEY = "SelectedControllers"; // NOLINT(cert-err58-cpp)
QString Settings::CONTROLLER_REGIONS_KEY = "ControllerRegions"; // NOLINT(cert-err58-cpp)

QString Settings::TOP_SUFFIX = "_Top"; // NOLINT(cert-err58-cpp)
QString Settings::BOTTOM_SUFFIX = "_Bottom"; // NOLINT(cert-err58-cpp)
QString Settings::LEFT_SUFFIX = "_Left"; // NOLINT(cert-err58-cpp)
QString Settings::RIGHT_SUFFIX = "_Right"; // NOLINT(cert-err58-cpp)

Settings::Settings(const QString &file)
    : settings{file, QSettings::IniFormat}
{
    const auto selectedList = settings.value(SELECTED_CONTROLLERS_KEY).toStringList();
    for (const auto &selected : selectedList)
        selectedControllers.insert(selected.toStdString());

    fillRegions(topRegions, settings.value(CONTROLLER_REGIONS_KEY + TOP_SUFFIX).toHash());
    fillRegions(bottomRegions, settings.value(CONTROLLER_REGIONS_KEY + BOTTOM_SUFFIX).toHash());
    fillRegions(leftRegions, settings.value(CONTROLLER_REGIONS_KEY + LEFT_SUFFIX).toHash());
    fillRegions(rightRegions, settings.value(CONTROLLER_REGIONS_KEY + RIGHT_SUFFIX).toHash());
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
}

void Settings::syncRegions(const RegionMap &map, const QString &key)
{
    QVariantHash data;
    for (const auto &region : map)
        data[QString::fromStdString(region.first)] = QVariantList{region.second.from, region.second.to};

    settings.setValue(key, data);
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
