#include "CustomDesignerPluginGenerator.h"
#include "IDEProjectCreator.hpp"

#include <QString>
#include <QFileInfo>
#include <QDir>

namespace dscontrol
{
static const QString &proSuffix {".pro"};
static const QString &headerSuffix {".h"};
static const QString &sourceSuffix(".cpp");
static const QString &uiSuffix {".ui"};
static const QString &pluginSuffix {"Plugin"};

static const QString &exampleClassName {"MyWidget"};
static const QString &exampleFieldName {"myWidget"};

static const QString &headerExamplePath {":/manager/HeaderExample.h"};
static const QString &srcExamplePath {":/manager/SourceExample.cpp"};
static const QString &proExamplePath {":/manager/ProExample.pro"};
}

using namespace dscontrol;

enum class CustomDesignerPluginGenerator::FileType
{
    Header,
    Source,
    Pro,
    UI
};

void CustomDesignerPluginGenerator::generate(const QString &widgetName, const QString &widgetUiPath)
{
    if (widgetName.isEmpty() || widgetUiPath.isEmpty())
    {
        return;
    }
    const QFileInfo &widgetUiFileInfo {widgetUiPath};
    if (!widgetUiFileInfo.isFile())
    {
        return;
    }

    const QString &widgetProFileName = generateFileName(widgetName, FileType::Pro);
    const QDir &widgetUiFileDir = widgetUiFileInfo.dir();
    const QString &widgetProFilePath = generatePath(widgetUiFileDir, widgetProFileName);

    if (!QFileInfo::exists(widgetProFilePath))
    {
        generateProjectFiles(widgetProFilePath, widgetName, widgetUiFileDir);
    }

    const QStringList &projectList {widgetProFilePath};
    IDEProjectCreator::launchProject(projectList, IDEProjectCreator::IDEType::QtCreator);
}

void CustomDesignerPluginGenerator::generateFile(const QString &filePath,
                                                 CustomDesignerPluginGenerator::FileType fileType,
                                                 const QString &widgetName)
{
    QString examplePath;
    switch (fileType)
    {
        case FileType::Header:
        {
            examplePath = headerExamplePath;
            break;
        }
        case FileType::Source:
        {
            examplePath = srcExamplePath;
            break;
        }
        case FileType::Pro:
        {
            examplePath = proExamplePath;
            break;
        }
        default:
        {
            return;
        }
    }

    QFile generatingFile {filePath};
    if (generatingFile.exists())
    {
        generatingFile.remove();
    }

    if (!QFile::copy(examplePath, filePath))
    {
        return;
    }

    generatingFile.setPermissions(QFile::WriteUser | QFile::ReadUser);

    changeFileSubstring(generatingFile, exampleClassName, widgetName);

    const QString &widgetFieldName = transformStringToLowerFirstLetter(widgetName);
    changeFileSubstring(generatingFile, exampleFieldName, widgetFieldName);
}

void CustomDesignerPluginGenerator::changeFileSubstring(QFile &file, const QString &previousSubstring,
                                                        const QString &newSubstring)
{
    if (!file.open(QIODevice::ReadWrite))
    {
        return;
    }
    QString text(file.readAll());
    text.replace(previousSubstring, newSubstring);
    file.seek(0);
    file.write(text.toUtf8());
    file.close();
}

void CustomDesignerPluginGenerator::generateProjectFiles(const QString &widgetProFilePath, const QString &widgetName,
                                                         const QDir &widgetUiFileDir)
{
    const QString &widgetHeaderFileName = generateFileName(widgetName, FileType::Header);
    const QString &widgetSourceFileName = generateFileName(widgetName, FileType::Source);
    const QString &widgetHeaderFilePath = generatePath(widgetUiFileDir, widgetHeaderFileName);
    const QString &widgetSourceFilePath = generatePath(widgetUiFileDir, widgetSourceFileName);

    generateFile(widgetProFilePath, FileType::Pro, widgetName);
    generateFile(widgetHeaderFilePath, FileType::Header, widgetName);
    generateFile(widgetSourceFilePath, FileType::Source, widgetName);
}

QString CustomDesignerPluginGenerator::generateFileName(const QString &fileName,
                                                        CustomDesignerPluginGenerator::FileType fileType)
{
    QString suffix;
    switch (fileType)
    {
        case FileType::Header:
        {
            suffix = headerSuffix;
            break;
        }
        case FileType::Source:
        {
            suffix = sourceSuffix;
            break;
        }
        case FileType::Pro:
        {
            suffix = proSuffix;
            break;
        }
        case FileType::UI:
        {
            suffix = uiSuffix;
            break;
        }
        default:
        {
            return fileName;
        }
    }
    return fileName + pluginSuffix + suffix;
}

QString CustomDesignerPluginGenerator::generatePath(const QDir &dir, const QString &filename)
{
    return dir.path() + "/" + filename;
}

QString CustomDesignerPluginGenerator::transformStringToLowerFirstLetter(const QString &str)
{
    if (str.isEmpty() || str.begin()->isLower())
    {
        return str;
    }

    QString ret = str;
    QCharRef firstLetter = ret.front();
    firstLetter = firstLetter.toLower();
    return ret;
}
