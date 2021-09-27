#include "ApplicationItemImpl.h"

#include "common/nodeeditor_globals.hpp"

namespace QtNodes
{
ApplicationItemImpl::ApplicationItemImpl(QtNodes::Item::DeploymentType deploymentType, const QString &caption)
: UniversalGraphicsItemImpl {deploymentType, caption}
{
    type_ = QtNodes::Item::NodeType::ApplicationNode;

    minSize_ = {200, 200};
    curMinSize_ = minSize_;
    size_ = minSize_;
}
} // namespace QtNodes
