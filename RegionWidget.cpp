//
// Created by Kamil Rojewski on 16.07.2021.
//

#include <QHBoxLayout>
#include <QSlider>

#include "RegionWidget.h"

RegionWidget::RegionWidget(QWidget *parent)
    : QWidget{parent}
{
    const auto layout = new QHBoxLayout{this};

    fromSlider = new QSlider{Qt::Horizontal};
    connect(fromSlider, &QSlider::valueChanged, this, [=](auto from) {
        emit regionChanged(from, toSlider->value());
    });
    connect(fromSlider, &QSlider::sliderReleased, this, [=]() {
        emit adjustmentFinished();
    });

    layout->addWidget(fromSlider);

    toSlider = new QSlider{Qt::Horizontal};
    connect(toSlider, &QSlider::valueChanged, this, [=](auto to) {
        emit regionChanged(fromSlider->value(), to);
    });
    connect(toSlider, &QSlider::sliderReleased, this, [=]() {
        emit adjustmentFinished();
    });

    layout->addWidget(toSlider);
}

void RegionWidget::setConfiguration(int max, int from, int to)
{
    fromSlider->setMaximum(max);
    toSlider->setMaximum(max);

    fromSlider->setValue(from);
    toSlider->setValue(to);
}
