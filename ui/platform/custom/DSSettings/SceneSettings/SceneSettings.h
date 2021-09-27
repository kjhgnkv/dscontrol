#pragma once

#include <QGroupBox>

namespace Ui
{
class SceneSettings;
}

class SceneSettings : public QGroupBox
{
Q_OBJECT

public:
    explicit SceneSettings(QWidget* parent = nullptr);
    ~SceneSettings();

    void setValues(int minZoom, int maxZoom, int step, int hideZoom, const QColor &backColor);

    int minZoom();
    int maxZoom();
    int hideTextZoom();
    int step();
    QColor backgroundColor();

private slots:
    void backColorButtonClicked();
    void stepChanged(int curIndex);

private:
    const QColor selectColor(const QColor &initColor);
    void setColor(const QColor &color);
    void adjustZoomLimits();

    int step_;
    QString qss_;
    QColor backgroundColor_;

    Ui::SceneSettings* ui_;
};

