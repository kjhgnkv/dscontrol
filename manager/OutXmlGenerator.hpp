#pragma once

#include <QDomDocument>
#include <QJsonObject>
#include <QString>
#include <QStringList>
#include <QUuid>
#include <list>
#include <map>
#include <set>
#include <vector>

namespace dscontrol
{
struct DeployObject
{
    inline friend bool operator==(const DeployObject &lhs, const DeployObject &rhs)
    {
        return lhs.appName == rhs.appName && lhs.xml.toByteArray() == rhs.xml.toByteArray();
    }

    inline friend bool operator<(const DeployObject &lhs, const DeployObject &rhs)
    {
        return lhs.appName < rhs.appName || lhs.xml.toByteArray() < rhs.xml.toByteArray();
    }

    QString appName;
    QDomDocument xml;
};

struct UploadObject
{
    inline friend bool operator==(const UploadObject &lhs, const UploadObject &rhs)
    {
        return lhs.appName == rhs.appName && lhs.componentName == rhs.componentName;
    }

    inline friend bool operator<(const UploadObject &lhs, const UploadObject &rhs)
    {
        return lhs.componentName < rhs.componentName || lhs.appName < rhs.appName;
    }

    QString filePath;
    QString componentName;
    QString appName;
    QString xml;

};

struct ProgressDataObject
{
    struct Component
    {
        QString filePath;
        unsigned int size_ = 0;

        inline friend bool operator<(const Component &lhs, const Component &rhs)
        {
            return lhs.filePath < rhs.filePath || lhs.size_ < rhs.size_;
        }
    };

    QString computer_;
    std::map<QString, std::list<Component>> applications_;
    QString ipAddress_;

    inline friend bool operator<(const ProgressDataObject &lhs, const ProgressDataObject &rhs)
    {
        return lhs.computer_ < rhs.computer_ || lhs.applications_ < rhs.applications_ || lhs.ipAddress_ < rhs.ipAddress_;
    }
};

class OutXmlGenerator
{
public:
    /**\return xml file for dshost by application name and ip. Map key - device
     * name, MapVal - List of all applications on device { First - app name,
     * Second - ip, third - list of all components in application, four - xml }
     */
    //appName, xml
    static std::set<DeployObject> generateApplicationXmls(const QJsonObject &project);

    //<nodeName, nodeLevel, checked>
    static std::vector<std::tuple<int, QString, QString>> generateProjectHierarchy(const QJsonObject &project);

    // <path, xml>
    static std::set<UploadObject> generateUploadXmls(const QJsonObject &project);

    //<computer, <application>>
    static std::set<ProgressDataObject> generateComputersMap(const QJsonObject &project);

    static std::set<QByteArray> generateStopXmls(const QStringList &apps);

    static QByteArray generateApplicationTemplateXmls(const QString &filePath);

    static QByteArray generateSetServerData(const QString &data);
    static QByteArray generateGetServerData();

private:
    static int getComponentSize(const QString &componentFilePath);
    static QString getComponentFilePath(const QString &componentType, const QString &componentDir);
    static int getComponentBlocksCount(int componentSize);
    static std::vector<std::tuple<int, QString, QString>> generateLevelHierarchy(const QJsonObject &top, const QJsonArray &comms);
    static QString getNodeIP(const QJsonObject& node, const QJsonArray& comms);
};

} // namespace dscontrol
