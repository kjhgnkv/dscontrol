#include "SubSystemItemImpl.h"

namespace QtNodes
{
SubSystemItemImpl::SubSystemItemImpl(const QtNodes::Item::DeploymentType &deploymentType, const QString &caption)
: UniversalGraphicsItemImpl {deploymentType, caption}
{
    type_ = QtNodes::Item::NodeType::SubSystemNode;
    minSize_ = {500, 500};
    curMinSize_ = minSize_;
    size_ = minSize_;
}
} // namespace QtNodes
