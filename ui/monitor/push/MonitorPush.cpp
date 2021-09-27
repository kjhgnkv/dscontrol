#include "MonitorPush.hpp"

#include "ui_MonitorPush.h"

#include <QApplication>
#include <QDebug>
#include <QRect>
#include <QResizeEvent>
#include <QScreen>
#include <QTimer>

MonitorPush::MonitorPush(QWidget* parent)
: QDialog(parent)
, ui_(new Ui::MonitorPush)
{
    ui_->setupUi(this);
    timer_ = new QTimer(this);
    timer_->setInterval(3000);
    timer_->setSingleShot(true);
    connect(timer_, &QTimer::timeout, [=]()
    {
        close();
        timer_->deleteLater();
    });

    timer_->start();
}

MonitorPush::~MonitorPush()
{
    delete timer_;
    delete ui_;
}

void MonitorPush::setData(const QString &error, const QString &message)
{
    ui_->labelError->setText(error);
    ui_->labelMessage->setText(message);
}

void MonitorPush::resizeEvent(QResizeEvent* event)
{
    QScreen* screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    int height = screenGeometry.height();
    int width = screenGeometry.width();

    QSize size = event->size();

    move(width - size.width(), height - size.height());
}
