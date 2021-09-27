// Project.hpp

#pragma once

#include <QDateTime>
#include <QDir>
#include <QString>
#include <QUuid>

namespace dscontrol
{
/*!
\brief Class that represents a Project main information
 */
class Project final : public QObject
{
    Q_OBJECT

public:
    Project(const QString &fileName);

public:
    QString fileName() const;
    QString name() const;
    QString creator() const;
    QDateTime creationTime() const;
    QString creationTimeAsString() const;
    QDateTime lastModified() const;
    QString lastModifiedAsString() const;
    QUuid uuid() const;
    QDir directory() const;

    void copyFrom(const Project &another);
    void setName(const QString &newProjectName);
    QJsonObject save() const;
    QJsonObject unsafeSave() const;
    bool load(const QJsonObject &obj);

signals:
    void nameHasChanged(const QString previousName);

private:
    void setUuid(const QUuid &newUuid);
    QUuid generateUuidFromCreationTime() const;
    void setCreator(const QString &newCreator);
    void setCreationTime(const QDateTime &newCreationTime);
    void setLastModified(const QDateTime &newLastModified) const;
    void setLastModifiedToCurrent() const;
    QJsonObject saveHelper() const;

private:
    const QString fileName_;
    QString name_;
    QString creator_;
    QDateTime creationTime_;
    mutable QDateTime lastModified_;
    QUuid uuid_;
};
} // namespace dscontrol
