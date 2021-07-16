//
// Created by Kamil Rojewski on 15.07.2021.
//

#ifndef OPENRGB_AMBIENT_SETTINGSTAB_H
#define OPENRGB_AMBIENT_SETTINGSTAB_H

#include <QWidget>

class ResourceManager;
class Settings;
class QLabel;
class QImage;

class SettingsTab
        : public QWidget
{
    Q_OBJECT

public:
    SettingsTab(ResourceManager *resourceManager, Settings &settings, QWidget *parent);
    ~SettingsTab() override = default;

public slots:
    void updatePreview(const QImage &image);

signals:
    void controllerListChanged();
    void previewChanged(bool enabled);
    void settingsVisibilityChanged(bool visible);

protected:
    void showEvent(QShowEvent *event) override;
    void hideEvent(QHideEvent *event) override;

private:
    QLabel *preview = nullptr;
};

#endif //OPENRGB_AMBIENT_SETTINGSTAB_H
