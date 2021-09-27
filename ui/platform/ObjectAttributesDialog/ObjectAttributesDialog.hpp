#pragma once

#include "nodeeditor/nodes/NodeIndex.hpp"
#include <QDialog>
#include <QStandardItemModel>
#include <QUuid>

namespace Ui
{
class ObjectAttributesDialog;
}

namespace QtNodes
{
class DataFlowModel;
class MessageList;
} // namespace QtNodes

namespace dscontrol
{
class MessageModel;
class ObjectConnectionModel;

/*!
\brief Object attributes dialog class
 */
class ObjectAttributesDialog : public QDialog
{
Q_OBJECT

public:
    /*!
    \param id Node Uuid
     */
    explicit ObjectAttributesDialog(const QUuid &id, QWidget* parent = nullptr);
    ~ObjectAttributesDialog();

    void keyPressEvent(QKeyEvent* event);

private:
    Ui::ObjectAttributesDialog* ui_;

    QUuid sourceId_;
    const QtNodes::DataFlowModel* model_;
    QString currentComponent_;
    QtNodes::NodeIndex sourceIdx_;
    ObjectConnectionModel* sendModel_;
    ObjectConnectionModel* recvModel_;
    QStandardItemModel* componentsModel_;
    std::function<bool(const QModelIndex &sourceIndex)> filter_;
};

} // namespace dscontrol
