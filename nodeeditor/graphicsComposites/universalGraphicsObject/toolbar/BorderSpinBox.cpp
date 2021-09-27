#include "BorderSpinBox.h"

BorderSpinBox::BorderSpinBox(QWidget* parent)
: QSpinBox {parent}
{
}

void BorderSpinBox::stepBy(int steps)
{
    if (steps < 0)
        setValue(value() - singleStep());
    else if (steps > 0)
        setValue(value() + singleStep());
}
