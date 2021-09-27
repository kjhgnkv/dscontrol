// FlowItemMimeData.hpp

#pragma once

#include <QMimeData>
#include "nodeeditor/nodeeditor_export.h"

namespace QtNodes
{
class NODEEDITOR_EXPORT FlowItemMimeData final : public QMimeData
{
public:
    /**\brief create invalid mimedata
     */
    FlowItemMimeData();

    /**\return type name of the mime data
     */
    static QString mimeType();

    /**\param impName name of implementation, provided by DataImpRegistry
     * \param info information for method NodeImp::restore
     * \brief create mime data, which can be accepted by FlowScene
     */
    FlowItemMimeData(const QString &impName, const QJsonObject &info);
    ~FlowItemMimeData() = default;

    /**\return name of implementation type name, provided DataImpRegistry
     */
    const QString &impName() const;

    /**\return information for method NodeImp::restore
     */
    QJsonObject info() const;

private:
    QString impName_;
};
} // namespace QtNodes
