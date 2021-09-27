#include "manager/WindowsSettings.h"

using namespace dscontrol;

WindowsSettings::WindowsSettings()
{
    baseFolder_ = QString(getenv("APPDATA"));
    baseFolder_.replace("\\", "/");
    baseFolder_ += "/DSControl";
}
