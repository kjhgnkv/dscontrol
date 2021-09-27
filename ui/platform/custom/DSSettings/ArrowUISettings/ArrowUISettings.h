#pragma once

#include <QGroupBox>

namespace Ui
{
class ArrowUISettings;
}

class ArrowUiSettings : public QGroupBox
{
Q_OBJECT

public:
    explicit ArrowUiSettings(QWidget* parent = nullptr);
    ~ArrowUiSettings();

    void setValues(int width, int direction, const QColor &color);

    int width();
    int direction();
    QColor color();

private slots:
    void colorButtonClicked();

private:
    const QColor selectColor(const QColor &initColor);
    void setColor(const QColor &color);

    QString qss_;
    QColor currentColor_;

    Ui::ArrowUISettings* ui_;
};

