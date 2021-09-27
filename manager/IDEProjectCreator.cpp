#include "IDEProjectCreator.hpp"
#include "ui/platform/DSMessageBox/DSMessageBox.hpp"
#include "common/globals.h"
#include "manager/DSConfiguration.h"
#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QProcess>
#include <QTextStream>

namespace dscontrol
{
std::pair<QUuid, QString> IDEProjectCreator::createProject(const QString &projectName, QStringList &compile,
                                                           QStringList &include, const QString &path, int type)
{
    QDir source {path};
    if (!source.exists())
    {
        DSMessageBox::critical(nullptr, QObject::tr("Error"), QObject::tr("Source folder does not exists!"));
        return {};
    }

    for (auto &file : compile)
    {
        file.remove(path);
        if (file.at(0) == "/")
        {
            file.remove(0, 1);
        }
    }

    for (auto &file : include)
    {
        file.remove(path);
        if (file.at(0) == "/")
        {
            file.remove(0, 1);
        }
    }

    switch (type)
    {
        case VisualStudio:
            return createVSProject(projectName, compile, include, path);
            break;
        case QtCreator:
            return createQtProject(projectName, compile, include, path);
            break;
        default:
            return {};
            break;
    }
}

std::pair<QUuid, QString> IDEProjectCreator::createProjectFromExistingCode(const QString &name, const QString &folder,
                                                                           int type)
{
    QDir sourceFolder {folder};
    if (!sourceFolder.exists())
    {
        DSMessageBox::critical(nullptr, QObject::tr("Error"), QObject::tr("Source folder does not exists!"));
        return {};
    }

    QDirIterator it(folder, QStringList() << "*.h" << "*.hpp", QDir::Files, QDirIterator::Subdirectories);
    QStringList include;
    while (it.hasNext())
    {
        include.append(it.next());
    }

    QDirIterator itcomp(folder, QStringList() << "*.cpp", QDir::Files, QDirIterator::Subdirectories);
    QStringList compile;
    while (itcomp.hasNext())
    {
        compile.append(itcomp.next());
    }

    return createProject(name, compile, include, folder, type);
}

std::pair<QUuid, QString> IDEProjectCreator::createQtProject(const QString &projectName, const QStringList &compile,
                                                             const QStringList &include, const QString &path)
{
    QDir source {path};
    if (!source.exists())
    {
        DSMessageBox::critical(nullptr, QObject::tr("Error"), QObject::tr("Source folder does not exists!"));
        return {};
    }
    QStringList file;

    file.append("QT       -= core gui");
    file.append("TARGET = " + projectName);
    file.append("DEFINES += " + projectName.toUpper() + "_LIBRARY");

    file.append("SOURCES += " + compile.join(" \\\n"));
    file.append("HEADERS += " + include.join(" \\\n"));
    file.append("unix {target.path = /usr/lib");
    file.append("    INSTALLS += target}");

    auto text = file.join("\n");

    QString fullpath = path + "/" + projectName + ".pro";

    QFile outputFile(fullpath);
    outputFile.open(QIODevice::WriteOnly);

    QTextStream outStream(&outputFile);

    outStream << text;

    outputFile.close();

    return std::make_pair(QUuid::createUuid(), fullpath);
}

std::pair<QUuid, QString> IDEProjectCreator::createVSProject(const QString &projectName, const QStringList &compile,
                                                             const QStringList &include, const QString &path)
{
    QDir source {path};
    if (!source.exists())
    {
        DSMessageBox::critical(nullptr, QObject::tr("Error"), QObject::tr("Source folder does not exists!"));
        return {};
    }
    std::map<QUuid, QString> projects;

    if (QFile::exists(":/other/resources/other/template.vcxproj"))
    {
        QFile f(":/other/resources/other/template.vcxproj");
        if (!f.open(QFile::ReadOnly | QFile::Text))
            return {};
        QTextStream in(&f);
        QString s;
        s.append(in.readAll());

        f.close();

        QUuid uid = QUuid::createUuid();
        s.replace("_PROJECT_GUID_", uid.toString());
        QStringList compileGroup;
        QStringList includeGroup;
        for (const auto &file : compile)
        {
            compileGroup.append("<ClCompile Include=\"" + file + "\" />");
        }
        s.replace("_COMPILE_GROUP_", compileGroup.join("\n"));
        for (const auto &file : include)
        {
            includeGroup.append("<ClInclude Include=\"" + file + "\" />");
        }
        s.replace("_INCLUDE_GROUP_", includeGroup.join("\n"));

        QString fullpath = path + "/" + projectName + ".vcxproj";

        QFile outputFile(fullpath);
        outputFile.open(QIODevice::WriteOnly);

        QTextStream outStream(&outputFile);

        outStream << s;

        outputFile.close();

        return std::make_pair(uid, fullpath);
    }
    return {};
}

QString IDEProjectCreator::createVSSolution(std::map<QUuid, QString> projects, const QString &path, const QString &name)
{
    QDir source {path};
    if (!source.exists())
    {
        DSMessageBox::critical(nullptr, QObject::tr("Error"), QObject::tr("Source folder does not exists!"));
        return {};
    }
    QStringList file;

    file.append("Microsoft Visual Studio Solution File, Format Version 12.00");
    file.append("# Visual Studio 15");
    // TODO test VS versions
    file.append("VisualStudioVersion = 15.0.28307.572");
    file.append("MinimumVisualStudioVersion = 10.0.40219.1");
    auto someuid = QUuid::createUuid();
    for (auto project : projects)
    {
        auto proj = project.second;
        QFileInfo newFileInfo {proj};
        proj.remove(path);
        if (proj.at(0) == "/")
        {
            proj.remove(0, 1);
        }
        file.append(
        "Project(\"" + someuid.toString() + "\") = \"" + newFileInfo.fileName() + "\", \"" + proj + "\"" + ", \"" + project.first.toString() + "\"");
        file.append("EndProject");
    }
    file.append("Global");
    file.append("   GlobalSection(SolutionConfigurationPlatforms) = preSolution");
    file.append("       Debug|x64 = Debug|x64");
    file.append("       Debug|x86 = Debug|x86");
    file.append("       Release|x64 = Release|x64");
    file.append("       Release|x86 = Release|x86");
    file.append("   EndGlobalSection");
    file.append("   GlobalSection(ProjectConfigurationPlatforms) = postSolution");
    for (auto project : projects)
    {
        file.append("\t\t" + project.first.toString() + ".Debug|x64.ActiveCfg = Debug|x64");
        file.append("\t\t" + project.first.toString() + ".Debug|x64.Build.0 = Debug|x64");
        file.append("\t\t" + project.first.toString() + ".Debug|x86.ActiveCfg = Debug|Win32");
        file.append("\t\t" + project.first.toString() + ".Debug|x86.Build.0 = Debug|Win32");
        file.append("\t\t" + project.first.toString() + ".Release|x64.ActiveCfg = Release|x64");
        file.append("\t\t" + project.first.toString() + ".Release|x64.Build.0 = Release|x64");
        file.append("\t\t" + project.first.toString() + ".Release|x86.ActiveCfg = Release|Win32");
        file.append("\t\t" + project.first.toString() + ".Release|x86.Build.0 = Release|Win32");
    }
    file.append("   EndGlobalSection");
    file.append("   GlobalSection(SolutionProperties) = preSolution");
    file.append("       HideSolutionNode = FALSE");
    file.append("   EndGlobalSection");
    file.append("   GlobalSection(ExtensibilityGlobals) = postSolution");
    file.append("       SolutionGuid = " + QUuid::createUuid().toString());
    file.append("   EndGlobalSection");
    file.append("EndGlobal");
    auto text = file.join("\n");

    QString fullpath = path + "/" + name + ".sln";

    QFile outputFile(fullpath);
    outputFile.open(QIODevice::WriteOnly);

    QTextStream outStream(&outputFile);

    outStream << text;

    outputFile.close();

    return fullpath;
}

void IDEProjectCreator::launchProject(const QStringList &projects, int type)
{
    //TODO remove commented code
    //QSettings settings;
    switch (type)
    {
        case VisualStudio:
        {
            /*settings.beginGroup(DS_SETTINGS_EXTRACT_SECTION);
            auto vsPath = settings.value(DS_SETTINGS_VS_PATH).toString();
            settings.endGroup();*/
            auto vsPath = DSConfiguration::Instance().vsPath();
            if (vsPath.isEmpty())
            {
                QString bat {QDir::currentPath() + "/run.bat"};
                QFile batFile(bat);
                batFile.open(QIODevice::WriteOnly);

                QTextStream outStream(&batFile);
                outStream << "START devenv.exe " + projects.at(0);
                batFile.close();

                QProcess::execute(bat);

                QFile::remove(bat);
            }
            else
            {
                QProcess::startDetached(vsPath, projects);
            }
        }
            break;
        case QtCreator:
        {
            /*settings.beginGroup(DS_SETTINGS_EXTRACT_SECTION);
            auto qtPath = settings.value(DS_SETTINGS_QT_PATH).toString();
            settings.endGroup();*/
            auto qtPath = DSConfiguration::Instance().qtPath();
            if (!qtPath.isEmpty())
            {
                QProcess::startDetached(qtPath, projects);
            }
            else
            {
                DSMessageBox::critical(nullptr, QObject::tr("Error"),
                                       QObject::tr("Path to Qt Creator executable is not set!"));
            }
        }
            break;
        default:
            break;
    }
}
} // namespace dscontrol
