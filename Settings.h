//
// Created by Kamil Rojewski on 15.07.2021.
//

#ifndef OPENRGB_AMBIENT_SETTINGS_H
#define OPENRGB_AMBIENT_SETTINGS_H

#include <unordered_set>
#include <string>

#include <QSettings>

class ResourceManager;
class QString;

class Settings final
{
public:
    explicit Settings(const QString &file);

    [[nodiscard]] bool isControllerSelected(const std::string &location) const;
    void selectController(const std::string &location);
    void unselectController(const std::string &location);

private:
    static QString SELECTED_CONTROLLERS_KEY;

    QSettings settings;

    std::unordered_set<std::string> selectedControllers;

    void syncSettings();
};

#endif //OPENRGB_AMBIENT_SETTINGS_H
