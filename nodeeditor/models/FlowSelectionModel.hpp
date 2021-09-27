// FlowSelectionModel.hpp

#pragma once

#include "nodeeditor/nodeeditor_export.h"
#include <QItemSelectionModel>

namespace QtNodes
{
class DataFlowModel;
class ModelAdapter;
class NodeIndex;

class NODEEDITOR_EXPORT FlowSelectionModel final : public QItemSelectionModel
{
Q_OBJECT
public:
    FlowSelectionModel(ModelAdapter* model, QObject* parent = nullptr);
};
} // namespace QtNodes
