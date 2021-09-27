#include "manager/LinuxSettings.h"
#include <QDir>

using namespace dscontrol;

LinuxSettings::LinuxSettings()
{
     baseFolder_ = QDir::currentPath();
}
