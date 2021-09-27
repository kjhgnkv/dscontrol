#include "ComputerItemImpl.h"

namespace QtNodes
{
ComputerItemImpl::ComputerItemImpl(const QtNodes::Item::DeploymentType &deploymentType, const QString &caption)
: UniversalGraphicsItemImpl {deploymentType, caption}
{
    type_ = QtNodes::Item::NodeType::ComputerNode;
    minSize_ = {350, 350};
    curMinSize_ = minSize_;
    size_ = minSize_;
}
} // namespace QtNodes
