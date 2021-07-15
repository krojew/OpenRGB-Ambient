//
// Created by Kamil Rojewski on 15.07.2021.
//

#include "Settings.h"

QString Settings::SELECTED_CONTROLLERS_KEY = "SelectedControllers"; // NOLINT(cert-err58-cpp)

Settings::Settings(const QString &file)
    : settings{file, QSettings::IniFormat}
{
    const auto selectedList = settings.value(SELECTED_CONTROLLERS_KEY).toStringList();
    for (const auto &selected : selectedList)
        selectedControllers.insert(selected.toStdString());
}

bool Settings::isControllerSelected(const std::string &location) const {
    return selectedControllers.find(location) != std::end(selectedControllers);
}

void Settings::selectController(const std::string &location) {
    selectedControllers.insert(location);
    syncSettings();
}

void Settings::unselectController(const std::string &location) {
    selectedControllers.erase(location);
    syncSettings();
}

void Settings::syncSettings() {
    QStringList value;
    for (const auto &selected : selectedControllers)
        value.append(QString::fromStdString(selected));

    settings.setValue(SELECTED_CONTROLLERS_KEY, value);
}
