#pragma once

#include <QString>
#include <QDir>
#include <Poco/Types.h>

#define SHARED_MEMORY_MEMORY_SIZE 40000

#ifdef WIN32
    #define SHARED_MEMORY_LIB_PATH "SharedMemory.dll"
#else
    #define SHARED_MEMORY_LIB_PATH "../lib/libSharedMemory.so"
#endif
#define SHARED_MEMORY_CHANEL "DSControl"

namespace SettingsManager
{
const QString homeDir = QDir::currentPath();
const QString localization = "./languages/localization_";
const QString pathZip = "/Samples_.zip";
const QString pathComponents = "/Samples/components";

// для передачи Samples.zip
static const Poco::UInt64 blocksCountSamplesZip = 30000;
// для передачи Бандлов
static const Poco::UInt64 blocksCountComponent = 5000;
}
