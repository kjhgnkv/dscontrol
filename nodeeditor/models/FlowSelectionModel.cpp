// FlowSelectionModel.cpp

#include "FlowSelectionModel.hpp"
#include "ModelAdapter.hpp"

namespace QtNodes
{
FlowSelectionModel::FlowSelectionModel(ModelAdapter* model, QObject* parent)
: QItemSelectionModel {model, parent}
{
}
} // namespace QtNodes
