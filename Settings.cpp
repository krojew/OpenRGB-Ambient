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
QString Settings::ZONE_MAPPINGS_KEY   = "ZoneMappings"; // NOLINT(cert-err58-cpp)
QString Settings::DISABLED_ZONES_KEY         = "DisabledZones";        // NOLINT(cert-err58-cpp)
QString Settings::ZONE_MAPPING_LOCATIONS_KEY = "ZoneMappingLocations"; // NOLINT(cert-err58-cpp)
QString Settings::MONITOR_ADAPTER_KEY = "MonitorAdapter"; // NOLINT(cert-err58-cpp)
QString Settings::MONITOR_OUTPUT_KEY  = "MonitorOutput"; // NOLINT(cert-err58-cpp)

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

    fillZoneParts(zoneParts, settings.value(ZONE_MAPPINGS_KEY).toHash());

    const auto disabledList = settings.value(DISABLED_ZONES_KEY).toStringList();
    for (const auto &key : disabledList)
        disabledZones.insert(key.toStdString());

    const auto zoneModeList = settings.value(ZONE_MAPPING_LOCATIONS_KEY).toStringList();
    for (const auto &loc : zoneModeList)
        zoneMappingLocations.insert(loc.toStdString());

    coolWhiteCompensation = settings.value(COOL_WHITE_COMPENSATION_KEY, true).toBool();
    colorTemperatureIndex = std::clamp(
            settings.value(COLOR_TEMPERATURE_KEY, colorTemperatureIndex).toInt(),
            0,
            static_cast<int>(std::extent_v<decltype(colorTemperatureFactors)>)
    );

    smoothing = settings.value(SMOOTHING_KEY, false).toBool();
    smoothingWeight = settings.value(SMOOTHING_WEIGHT_KEY, 0.5).toFloat();

    monitorAdapterIndex = settings.value(MONITOR_ADAPTER_KEY, 0).toInt();
    monitorOutputIndex  = settings.value(MONITOR_OUTPUT_KEY,  0).toInt();
}

bool Settings::isControllerSelected(const std::string &location) const {
    return selectedControllers.find(location) != std::end(selectedControllers);
}

bool Settings::isZoneEnabled(const std::string &location, const std::string &zoneName) const
{
    return disabledZones.find(location + "|" + zoneName) == std::end(disabledZones);
}

void Settings::setZoneEnabled(const std::string &location, const std::string &zoneName, bool enabled)
{
    const auto key = location + "|" + zoneName;
    if (enabled)
        disabledZones.erase(key);
    else
        disabledZones.insert(key);
    syncDisabledZones();
}

MappingMode Settings::getMappingMode(const std::string &location) const
{
    return zoneMappingLocations.count(location) ? MappingMode::Zone : MappingMode::Standard;
}

void Settings::setMappingMode(const std::string &location, MappingMode mode)
{
    if (mode == MappingMode::Zone)
        zoneMappingLocations.insert(location);
    else
        zoneMappingLocations.erase(location);
    syncMappingModes();
}

void Settings::syncMappingModes()
{
    QStringList value;
    for (const auto &loc : zoneMappingLocations)
        value.append(QString::fromStdString(loc));
    settings.setValue(ZONE_MAPPING_LOCATIONS_KEY, value);
    emit settingsChanged();
}

void Settings::syncDisabledZones()
{
    QStringList value;
    for (const auto &key : disabledZones)
        value.append(QString::fromStdString(key));
    settings.setValue(DISABLED_ZONES_KEY, value);
    emit settingsChanged();
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

int Settings::monitorAdapter() const noexcept { return monitorAdapterIndex; }
int Settings::monitorOutput()  const noexcept { return monitorOutputIndex; }

void Settings::setMonitorAdapter(int index)
{
    monitorAdapterIndex = index;
    settings.setValue(MONITOR_ADAPTER_KEY, monitorAdapterIndex);
    emit settingsChanged();
}

void Settings::setMonitorOutput(int index)
{
    monitorOutputIndex = index;
    settings.setValue(MONITOR_OUTPUT_KEY, monitorOutputIndex);
    emit settingsChanged();
}

std::vector<ZonePart> Settings::getZoneParts(const std::string &location, const std::string &zoneName) const
{
    const auto key = location + "|" + zoneName;
    const auto it = zoneParts.find(key);
    return (it != std::end(zoneParts)) ? it->second : std::vector<ZonePart>{};
}

void Settings::setZoneParts(const std::string &location, const std::string &zoneName, std::vector<ZonePart> parts)
{
    zoneParts[location + "|" + zoneName] = std::move(parts);
    syncZoneMappings();
}

void Settings::syncZoneMappings()
{
    QVariantHash data;
    for (const auto &entry : zoneParts)
    {
        QVariantList partsList;
        for (const auto &p : entry.second)
            partsList.append(QVariant{QVariantList{p.from, p.to, static_cast<int>(p.region), p.reversed}});
        data[QString::fromStdString(entry.first)] = partsList;
    }

    settings.setValue(ZONE_MAPPINGS_KEY, data);

    emit settingsChanged();
}

void Settings::fillZoneParts(ZonePartsMap &map, const QVariantHash &data)
{
    QHashIterator i{data};
    while (i.hasNext())
    {
        i.next();
        std::vector<ZonePart> parts;
        for (const auto &item : i.value().toList())
        {
            const auto entry = item.toList();
            if (entry.size() < 4)
                continue;
            ZonePart p;
            p.from     = entry[0].toInt();
            p.to       = entry[1].toInt();
            p.region   = static_cast<ScreenRegion>(entry[2].toInt());
            p.reversed = entry[3].toBool();
            parts.push_back(p);
        }
        map[i.key().toStdString()] = std::move(parts);
    }
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
