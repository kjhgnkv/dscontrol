#pragma once

#include <QStringList>
#include <QUuid>
#include <map>

namespace dscontrol
{
/*!
\brief class executing generation of Visual Studio and Qt Creator IDE projects
and launching them
*/
class IDEProjectCreator
{
public:
    enum IDEType
    {
        Unknown,
        QtCreator,
        VisualStudio
    };

    /*!
    \brief creates @projectName project by @type (Visual Studio/ Qt Creator) at
    target @path with @compile and @include files
    */
    static std::pair<QUuid, QString> createProject(const QString &projectName, QStringList &compile, QStringList &include, const QString &path, int type);

    /*!
    \brief creates @name project by @type (Visual Studio/ Qt Creator) with all
    (cpp/hpp/h) files at target @folder
    */
    static std::pair<QUuid, QString> createProjectFromExistingCode(const QString &name, const QString &folder, int type);

    /*!
    \brief launch @projects by @type (Visual Studio/ Qt Creator) with new
    appropriate IDE instance
    */
    static void launchProject(const QStringList &projects, int type);

    /*!
    \brief creates @projectName Visual Studio project  at
    target @path with @compile and @include files
    */
    static std::pair<QUuid, QString> createVSProject(const QString &projectName, const QStringList &compile, const QStringList &include, const QString &path);

    /*!
    \brief creates @name Visual Studio solution with @projects at
    target @path
    */
    static QString createVSSolution(std::map<QUuid, QString> projects, const QString &path, const QString &name);

    /*!
    \brief creates @projectName Qt Creator project at
    target @path with @compile and @include files
    */
    static std::pair<QUuid, QString> createQtProject(const QString &projectName, const QStringList &compile, const QStringList &include, const QString &path);

private:
};
} // namespace dscontrol
