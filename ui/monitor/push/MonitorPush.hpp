#pragma once

#include <QDialog>

namespace Ui
{
class MonitorPush;
}

class MonitorPush : public QDialog
{
public:
    explicit MonitorPush(QWidget* parent = nullptr);
    ~MonitorPush();
    void setData(const QString &error, const QString &message);
    virtual void resizeEvent(QResizeEvent* event);

private:
    QTimer* timer_;
    Ui::MonitorPush* ui_;
};
