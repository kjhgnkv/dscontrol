#pragma once

#include "RenameDialog.h"
#include "nodes/NodeIndex.hpp"

class RenameNodeDialog : public RenameDialog
{
Q_OBJECT

public:
    explicit RenameNodeDialog(const QtNodes::NodeIndex &nodeIndex, QWidget* parent = nullptr);

protected:
    bool hasNameAlreadyTaken(const QString &name) const override;

private:
    QString nodeCaption(const QtNodes::NodeIndex &nodeIndex) const;

private:
    const QtNodes::NodeIndex nodeIndex_;
};
