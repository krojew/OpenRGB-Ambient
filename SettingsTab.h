//
// Created by Kamil Rojewski on 15.07.2021.
//

#ifndef OPENRGB_AMBIENT_SETTINGSTAB_H
#define OPENRGB_AMBIENT_SETTINGSTAB_H

#include <vector>

#include <QWidget>

#include <RGBController.h>

class ResourceManagerInterface;
class Settings;
class LedPreviewWidget;
class QImage;

class SettingsTab
        : public QWidget
{
    Q_OBJECT

public:
    SettingsTab(ResourceManagerInterface *resourceManager, Settings &settings, QWidget *parent = nullptr);
    ~SettingsTab() override = default;

public slots:
    void updatePreview(const QImage &image) const;
    void updateLedColors(const QString &location, const std::vector<RGBColor> &colors) const;

signals:
    void controllerListChanged();
    void previewChanged(bool enabled);
    void settingsVisibilityChanged(bool visible);

protected:
    void showEvent(QShowEvent *event) override;
    void hideEvent(QHideEvent *event) override;

private:
    LedPreviewWidget *preview = nullptr;
};

#endif //OPENRGB_AMBIENT_SETTINGSTAB_H
