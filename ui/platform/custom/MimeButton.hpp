// MimeButton.hpp

#pragma once

#include <QToolButton>
#include <functional>

namespace dscontrol
{
/*!
\brief Event filter class for toolbar mime-generative buttons (i.e. system,
  subsystem etc.)
 */
class MimeButton : public QObject
{
public:
    /*!
    \param callback callback function for generate mime data
     */
    MimeButton(std::function<void(QObject*)> callback, QObject* parent = nullptr);

    bool eventFilter(QObject* obj, QEvent* event) override;

private:
    std::function<void(QObject*)> callback_;
};
} // namespace dscontrol
