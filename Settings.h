//
// Created by Kamil Rojewski on 15.07.2021.
//

#ifndef OPENRGB_AMBIENT_SETTINGS_H
#define OPENRGB_AMBIENT_SETTINGS_H

#include <unordered_map>
#include <unordered_set>
#include <string>
#include <vector>

#include <QSettings>
#include <QObject>

#include "LedRange.h"
#include "ZoneMapping.h"

class ResourceManager;
class QString;

enum class MappingMode { Standard, Zone };

class Settings final
        : public QObject
{
    Q_OBJECT

public:
    explicit Settings(const QString &file, QObject *parent = nullptr);

    [[nodiscard]] bool isControllerSelected(const std::string &location) const;
    void selectController(const std::string &location);
    void unselectController(const std::string &location);

    [[nodiscard]] bool isZoneEnabled(const std::string &location, const std::string &zoneName) const;
    void setZoneEnabled(const std::string &location, const std::string &zoneName, bool enabled);

    [[nodiscard]] MappingMode getMappingMode(const std::string &location) const;
    void setMappingMode(const std::string &location, MappingMode mode);

    [[nodiscard]] LedRange getTopRegion(const std::string &location) const;
    [[nodiscard]] LedRange getBottomRegion(const std::string &location) const;
    [[nodiscard]] LedRange getRightRegion(const std::string &location) const;
    [[nodiscard]] LedRange getLeftRegion(const std::string &location) const;

    void setTopRegion(const std::string &location, LedRange range);
    void setBottomRegion(const std::string &location, LedRange range);
    void setRightRegion(const std::string &location, LedRange range);
    void setLeftRegion(const std::string &location, LedRange range);

    [[nodiscard]] std::vector<ZonePart> getZoneParts(const std::string &location, const std::string &zoneName) const;
    void setZoneParts(const std::string &location, const std::string &zoneName, std::vector<ZonePart> parts);

    [[nodiscard]] int monitorAdapter() const noexcept;
    [[nodiscard]] int monitorOutput() const noexcept;
    void setMonitorAdapter(int index);
    void setMonitorOutput(int index);

    [[nodiscard]] bool compensateCoolWhite() const noexcept;
    void setCoolWhiteCompensation(bool value);

    [[nodiscard]] int colorTemperatureFactorIndex() const noexcept;
    void setColorTemperatureFactorIndex(int index);

    [[nodiscard]] bool smoothTransitions() const noexcept;
    void setSmoothTransitions(bool value);

    [[nodiscard]] float smoothTransitionsWeight() const noexcept;
    void setSmoothTransitionsWeight(float value);

signals:
    void settingsChanged();

private:
    static QString SELECTED_CONTROLLERS_KEY;
    static QString CONTROLLER_REGIONS_KEY;
    static QString COOL_WHITE_COMPENSATION_KEY;
    static QString COLOR_TEMPERATURE_KEY;
    static QString SMOOTHING_KEY;
    static QString SMOOTHING_WEIGHT_KEY;

    static QString TOP_SUFFIX;
    static QString BOTTOM_SUFFIX;
    static QString LEFT_SUFFIX;
    static QString RIGHT_SUFFIX;

    static QString ZONE_MAPPINGS_KEY;
    static QString DISABLED_ZONES_KEY;
    static QString ZONE_MAPPING_LOCATIONS_KEY;
    static QString MONITOR_ADAPTER_KEY;
    static QString MONITOR_OUTPUT_KEY;

    using RegionMap     = std::unordered_map<std::string, LedRange>;
    using ZonePartsMap  = std::unordered_map<std::string, std::vector<ZonePart>>; // key: "location|zoneName"

    QSettings settings;

    std::unordered_set<std::string> selectedControllers;
    std::unordered_set<std::string> disabledZones;         // key: "location|zoneName"
    std::unordered_set<std::string> zoneMappingLocations;  // locations using zone mapping mode

    RegionMap topRegions;
    RegionMap bottomRegions;
    RegionMap rightRegions;
    RegionMap leftRegions;

    ZonePartsMap zoneParts;

    int monitorAdapterIndex = 0;
    int monitorOutputIndex  = 0;

    bool coolWhiteCompensation = true;

    int colorTemperatureIndex = 55; // 6500K

    bool smoothing = false;
    float smoothingWeight = 0.5;

    void syncSelectedControllers();
    void syncDisabledZones();
    void syncMappingModes();
    void syncRegions(const RegionMap &map, const QString &key);
    void syncZoneMappings();

    static void fillRegions(RegionMap &map, const QVariantHash &data);
    [[nodiscard]] static LedRange findRange(const RegionMap &map, const std::string &location);
    static void fillZoneParts(ZonePartsMap &map, const QVariantHash &data);
};

#endif //OPENRGB_AMBIENT_SETTINGS_H
