#pragma once

#include <QDockWidget>

namespace dscontrol
{
class DockWidget : public QDockWidget
{
Q_OBJECT

public:
    DockWidget(const QString &title, QWidget* parent = nullptr);

signals:
    void closed();

protected:
    void closeEvent(QCloseEvent* event) override;
};
} // namespace dscontrol
