//
// Created by Kamil Rojewski on 15.07.2021.
//

#ifndef OPENRGB_AMBIENT_SETTINGS_H
#define OPENRGB_AMBIENT_SETTINGS_H

#include <unordered_map>
#include <unordered_set>
#include <string>

#include <QSettings>
#include <QObject>

#include "LedRange.h"

class ResourceManager;
class QString;

class Settings final
        : public QObject
{
    Q_OBJECT

public:
    explicit Settings(const QString &file, QObject *parent = nullptr);

    [[nodiscard]] bool isControllerSelected(const std::string &location) const;
    void selectController(const std::string &location);
    void unselectController(const std::string &location);

    [[nodiscard]] LedRange getTopRegion(const std::string &location) const;
    [[nodiscard]] LedRange getBottomRegion(const std::string &location) const;
    [[nodiscard]] LedRange getRightRegion(const std::string &location) const;
    [[nodiscard]] LedRange getLeftRegion(const std::string &location) const;

    void setTopRegion(const std::string &location, LedRange range);
    void setBottomRegion(const std::string &location, LedRange range);
    void setRightRegion(const std::string &location, LedRange range);
    void setLeftRegion(const std::string &location, LedRange range);

    bool compensateCoolWhite() const noexcept;
    void setCoolWhiteCompensation(bool value);

signals:
    void settingsChanged();

private:
    static QString SELECTED_CONTROLLERS_KEY;
    static QString CONTROLLER_REGIONS_KEY;
    static QString COOL_WHITE_COMPENSATION_KEY;

    static QString TOP_SUFFIX;
    static QString BOTTOM_SUFFIX;
    static QString LEFT_SUFFIX;
    static QString RIGHT_SUFFIX;

    using RegionMap = std::unordered_map<std::string, LedRange>;

    QSettings settings;

    std::unordered_set<std::string> selectedControllers;

    RegionMap topRegions;
    RegionMap bottomRegions;
    RegionMap rightRegions;
    RegionMap leftRegions;

    bool coolWhiteCompensation = true;

    void syncSelectedControllers();
    void syncRegions(const RegionMap &map, const QString &key);

    static void fillRegions(RegionMap &map, const QVariantHash &data);
    [[nodiscard]] static LedRange findRange(const RegionMap &map, const std::string &location);
};

#endif //OPENRGB_AMBIENT_SETTINGS_H
