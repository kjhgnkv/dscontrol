// NodeImp.hpp

#pragma once

#include "nodeeditor/nodeeditor_export.h"
#include "common/nodeeditor_globals.hpp"
#include "Serializable.hpp"

#include <QObject>
#include <QPixmap>
#include <QString>
#include <set>

namespace QtNodes
{
class Node;
class DataFlowModel;

class NODEEDITOR_EXPORT NodeImp : public QObject, public Serializable
{
Q_OBJECT

    friend DataFlowModel;

public:
    NodeImp(const QString &caption = tr("Node"));
    ~NodeImp() = default;

    enum
    {
        None,
        Frame,
        UserType = 100
    };

    /**\return type of NodeImp. If you want to set you own type - use
     * UserType. By default return None
     */
    virtual int type() const;

    virtual QtNodes::Item::DeploymentType deploymentType() const = 0;
    virtual void setDeploymentType(const QtNodes::Item::DeploymentType &) = 0;

    /**\brief this is type identifier of NodeImp - this have to be unique,
     * and can not change dynamicly. Uses in factory
     */

    /**\return by default empty QPixmap, if you want to set an icon, please
     *  the method
     */
    virtual QPixmap icon() const;

    /**\return if caption is not set, return name(). By default name()
     */
    QString caption() const;
    void setCaption(const QString &caption);

    virtual QWidget* embeddedWidget();

    /**\return validation message. By default ""
     */
    virtual QString validationMessage() const;

    virtual bool isInstall() const;
    virtual void setInstall(const bool &isInstall);

    /**\brief by default save name and caption of node
     */
    QJsonObject save() const;

    /**\brief by defautl restore caption of node. Uses setCaption method
     */
    void restore(const QJsonObject &obj);

    /**\return capacity of ports with input type
     */
    unsigned int nPorts(int type) const;

    /**\return all ports indexes
     */
    std::list<int> ports(int type) const;

    /**\return caption of port
     */
    QString portCaption(int type, int index) const;

    bool portCaptionVisibility(int type, int index) const;
signals:
    /**\brief emit the signal manually if you need update port information by
     * connection
     */
    void dataUpdated(int index);

    /**\brief emit the signal if you need update node
     */
    void nodeUpdated();

    /**\brief this is private signal - you can connect to this, but you can not
     * emit this
     */
    void portAdded(int type, int index);

    /**\brief this is private signal - you can connect to this, but you can not
     * emit this
     */
    void portRemoved(int type, int index);

private:
    QString caption_;
    bool isInstall_;
};
} // namespace QtNodes
