#pragma once

#include <QTabBar>

namespace dscontrol
{
namespace CustomEventFilters
{
/*!
\brief Event Filter class for repository tab widget.

 Opens appropriate tab when drag components over
 */
class RepoTabEventFilter : public QObject
{
public:
    RepoTabEventFilter(QTabBar* bar);

protected:
    bool eventFilter(QObject* obj, QEvent* event);

private:
    QTabBar* bar_;
};

} // namespace CustomEventFilters
} // namespace dscontrol
