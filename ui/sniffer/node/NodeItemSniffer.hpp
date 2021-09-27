#pragma once

#include "models/AbstractNodeItem.hpp"
#include <QMetaType>

class NodeItemSniffer : public AbstractNodeItem
{
public:
    explicit NodeItemSniffer();
    explicit NodeItemSniffer(const QVariant &variant);
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
};
