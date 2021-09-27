#pragma once

#include <QPixmap>

namespace monitor
{
class Icon
{
public:
    Icon();
    virtual ~Icon();
public:
    enum class IconColor
    {
        Red = 0,
        Green = 1
    };

public:
    void setIconColor(IconColor value);
    QPixmap* px() const;

private:
    void iconGreen();
    void iconRed();

private:
    QPixmap* px_;
};
}