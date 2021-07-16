//
// Created by Kamil Rojewski on 16.07.2021.
//

#ifndef OPENRGB_AMBIENT_REGIONWIDGET_H
#define OPENRGB_AMBIENT_REGIONWIDGET_H

#include <QWidget>

class QSlider;

class RegionWidget
        : public QWidget
{
    Q_OBJECT

public:
    explicit RegionWidget(QWidget *parent = nullptr);
    ~RegionWidget() override = default;

    void setConfiguration(int max, int from, int to);

signals:
    void regionChanged(int from, int to);

private:
    QSlider *fromSlider = nullptr;
    QSlider *toSlider = nullptr;
};

#endif //OPENRGB_AMBIENT_REGIONWIDGET_H
