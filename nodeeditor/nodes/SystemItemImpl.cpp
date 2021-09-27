#include "SystemItemImpl.h"

namespace QtNodes
{
SystemItemImpl::SystemItemImpl(const QtNodes::Item::DeploymentType &deploymentType, const QString &caption)
: UniversalGraphicsItemImpl {deploymentType, caption}
{
    type_ = QtNodes::Item::NodeType::SystemNode;
    minSize_ = {800, 600};
    curMinSize_ = minSize_;
    size_ = minSize_;
}
} // namespace QtNodes
