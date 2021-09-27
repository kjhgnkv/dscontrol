#pragma once

#include <QTabBar>

namespace dscontrol
{
class DSTabBar : public QTabBar
{
public:
    explicit DSTabBar(QWidget* parent = nullptr);

    // QWidget interface
protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

private:
    QRect tabRect_;
    int delta_;
    int currentTab_;
};

} // namespace dscontrol
