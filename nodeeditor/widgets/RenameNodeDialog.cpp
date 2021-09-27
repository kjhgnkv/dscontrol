#include "widgets/RenameNodeDialog.h"
#include "singletones/Facade.hpp"
#include "singletones/NameController.hpp"

#include <QLineEdit>
#include <QDebug>

RenameNodeDialog::RenameNodeDialog(const QtNodes::NodeIndex &nodeIndex, QWidget* parent)
: RenameDialog {nodeCaption(nodeIndex), parent}
, nodeIndex_ {nodeIndex}
{
}

bool RenameNodeDialog::hasNameAlreadyTaken(const QString &name) const
{
    return QtNodes::NameController::Instance()->containsCaption(name);
}

QString RenameNodeDialog::nodeCaption(const QtNodes::NodeIndex &nodeIndex) const
{
    const auto* dataNodesModel = QtNodes::Facade::Instance().model();
    const bool isValid = dataNodesModel != nullptr && nodeIndex.isValid();
    return isValid ? dataNodesModel->nodeCaption(nodeIndex) : QString {};
}
