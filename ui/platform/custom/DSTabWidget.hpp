#pragma once

#include <QTabWidget>

namespace dscontrol
{
class DSTabBar;

class DSTabWidget : public QTabWidget
{
public:
    explicit DSTabWidget(QWidget* parent = nullptr);

private:
    DSTabBar* tabBar_;
};
} // namespace dscontrol
