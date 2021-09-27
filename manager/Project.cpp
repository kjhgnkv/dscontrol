// Project.cpp

#include "Project.hpp"
#include "scenes/FlowScene.hpp"
#include "singletones/Facade.hpp"

#include <QFileInfo>

using namespace dscontrol;

#ifdef WIN32
const char *defaultCreator{"USERNAME"};
#else
const char* defaultCreator {"USER"};
#endif

const QString qDateTimeStringPattern {"dd.MM.yyyy hh:mm:ss"};

Project::Project(const QString &fileName)
: fileName_ {fileName}
, name_ {QFileInfo {fileName_}.baseName()}
, creator_ {qgetenv(defaultCreator)}
, creationTime_ {QDateTime::currentDateTime()}
, lastModified_ {creationTime_}
, uuid_ {generateUuidFromCreationTime()}
{

}

QString Project::fileName() const
{
    return fileName_;
}

QString Project::name() const
{
    return name_;
}

QString Project::creator() const
{
    return creator_;
}

QDateTime Project::creationTime() const
{
    return creationTime_;
}

QString Project::creationTimeAsString() const
{
    return creationTime_.toString(qDateTimeStringPattern);
}

QDateTime Project::lastModified() const
{
    return lastModified_;
}

QString Project::lastModifiedAsString() const
{
    return lastModified_.toString(qDateTimeStringPattern);
}

QUuid Project::uuid() const
{
    return uuid_;
}

QDir Project::directory() const
{
    QFileInfo info {fileName_};
    return info.isFile() ? info.absoluteDir() : QDir {};
}

void Project::copyFrom(const Project &another)
{
    setName(another.name());
    setCreator(another.creator());
    setLastModifiedToCurrent();
}

void Project::setUuid(const QUuid &newId)
{
    if (newId != uuid_)
    {
        uuid_ = newId;
    }
}

QUuid Project::generateUuidFromCreationTime() const
{
    QUuid uuid = QUuid::createUuid();
    uuid.data1 = creationTime().toTime_t();
    return uuid;
}

void Project::setCreator(const QString &newCreator)
{
    creator_ = newCreator;
}

void Project::setCreationTime(const QDateTime &newCreationTime)
{
    creationTime_ = newCreationTime;
}

void Project::setLastModified(const QDateTime &newLastModified) const
{
    lastModified_ = newLastModified;
}

void Project::setLastModifiedToCurrent() const
{
    setLastModified(QDateTime::currentDateTime());
}

void Project::setName(const QString &newProjectName)
{
    if (newProjectName != name_)
    {
        const QString &previousName = name_;
        name_ = newProjectName;
        emit nameHasChanged(previousName);
    }
}

QJsonObject Project::saveHelper() const
{
    QJsonObject retval;

    setLastModifiedToCurrent();

    retval["Name"] = name_;
    retval["Creator"] = creator_;
    retval["Creation time"] = creationTimeAsString();
    retval["Last modified"] = lastModifiedAsString();
    retval["Uuid"] = uuid_.toString();

    return retval;
}

QJsonObject Project::save() const
{
    QJsonObject retval = saveHelper();
    retval[uuid_.toString()] = QtNodes::Facade::Instance().save();
    return retval;
}

QJsonObject Project::unsafeSave() const
{
    QJsonObject retval = saveHelper();
    retval[uuid_.toString()] = QtNodes::Facade::Instance().unsafeSave();
    return retval;
}

bool Project::load(const QJsonObject &obj)
{
    const QString &loadingName = obj["Name"].toString();
    const QString &loadingCreator = obj["Creator"].toString();
    const QDateTime &loadingCreationTime = QDateTime::fromString(obj["Creation time"].toString(),
                                                                 qDateTimeStringPattern);
    const QDateTime &loadingLastModified = QDateTime::fromString(obj["Last modified"].toString(),
                                                                 qDateTimeStringPattern);
    const QString &loadingIdAsString = obj["Uuid"].toString();
    const QUuid &loadingId = QUuid::fromString(loadingIdAsString);

    setName(loadingName);
    setCreator(loadingCreator);
    setCreationTime(loadingCreationTime);
    setLastModified(loadingLastModified);
    setUuid(loadingId);

    QtNodes::Facade::Instance().restore(obj[loadingIdAsString].toObject());
    return true;
}
