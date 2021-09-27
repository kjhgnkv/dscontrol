#pragma once

#include <QSpinBox>

class BorderSpinBox : public QSpinBox
{
public:
    BorderSpinBox(QWidget* parent = nullptr);

protected:
    void stepBy(int steps) override;
};
