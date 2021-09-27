// MimeMenu.hpp

#pragma once

#include <QObject>

class QToolBar;

namespace dscontrol
{
/*!
\brief event filter for Design menu
 */
class MimeMenu : public QObject
{
public:
    /*!
    \param toolbar toolbar when set MimeButtons. It is needed, because
      MimeButton - just event filter, which set for widget (widgetForAction).
      In menu we can not access to the widgets, so we need set here toolbar for
      get access
     */
    MimeMenu(QToolBar* toolbar, QObject* parent = nullptr);
    bool eventFilter(QObject* obj, QEvent* event) override;

private:
    QToolBar* toolbar_;
};
} // namespace dscontrol
