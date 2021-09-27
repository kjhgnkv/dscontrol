#include "DSTabWidget.hpp"

#include "DSTabBar.hpp"

namespace dscontrol
{
DSTabWidget::DSTabWidget(QWidget* parent)
: QTabWidget(parent)
{
    tabBar_ = new DSTabBar(this);

    tabBar_->setDrawBase(false);
    setTabBar(tabBar_);
}

} // namespace dscontrol
