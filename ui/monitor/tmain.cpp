#include "MonitorWidget.hpp"
#include <QApplication>

#include "testJson.hpp"

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);

    MonitorWidget* monitor = MonitorWidget::init();
    monitor->show();

    monitor->updateData(IP, test_1);
    monitor->updateData(IP, test_2);
    monitor->updateData(IP, test_3);

    return QApplication::exec();
}
