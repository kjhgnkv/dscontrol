#include "PropertiesTreeModel.hpp"

#include "graphicsComposites/universalGraphicsObject/UniversalGraphicsItem.h"
#include "nodes/UniversalGraphicsItemImpl.h"
#include "models/NodesScene.hpp"
#include "PropertiesTreeItem.hpp"
#include "nodes/UniversalGraphicsItemImpl.h"
#include "singletones/Facade.hpp"

#include <QDialog>
#include <QStringList>

namespace dscontrol
{
PropertiesTreeModel::PropertiesTreeModel(QObject* parent)
: QAbstractItemModel {parent}
, itemImpl_ {nullptr}
{
    rootItem_ = new PropertiesTreeItem({tr("Property"), tr("Value")}, PropertiesTreeItem::NoEdit);
    nodesModel_ = QtNodes::Facade::Instance().model();
    sceneModel_ = QtNodes::Facade::Instance().sceneModel();
    setupModelData(rootItem_);
}

PropertiesTreeModel::~PropertiesTreeModel()
{
    delete rootItem_;
}

int PropertiesTreeModel::columnCount(const QModelIndex &parent) const
{
    return parent.isValid() ? static_cast<PropertiesTreeItem*>(parent.internalPointer())->columnCount() : rootItem_->columnCount();
}

QVariant PropertiesTreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || !itemImpl_ || !item_)
    {
        return {};
    }

    PropertiesTreeItem* item = static_cast<PropertiesTreeItem*>(index.internalPointer());

    switch (role)
    {
        case Qt::DisplayRole:
        {
            switch (item->type())
            {
                case PropertiesTreeItem::NameTextEdit:
                {
                    return (index.column() == 1) ? itemImpl_->caption() : item->data(index.column());
                }
                case PropertiesTreeItem::IdNoEdit:
                {
                    return (index.column() == 1) ? uid_.toString() : item->data(index.column());
                }
                case PropertiesTreeItem::XTextEdit:
                {
                    return (index.column() == 1) ? int(itemImpl_->getPos().rx()) : item->data(index.column());
                }
                case PropertiesTreeItem::YTextEdit:
                {
                    return (index.column() == 1) ? int(itemImpl_->getPos().ry()) : item->data(index.column());
                }
                case PropertiesTreeItem::WidthTextEdit:
                {
                    return (index.column() == 1) ? int(itemImpl_->getSize().width()) : item->data(index.column());
                }
                case PropertiesTreeItem::HeightTextEdit:
                {
                    return (index.column() == 1) ? int(itemImpl_->getSize().height()) : item->data(index.column());
                }
                case PropertiesTreeItem::FontFamilyCombo:
                {
                    return (index.column() == 1) ? itemImpl_->getFontFamily() : item->data(index.column());
                }
                case PropertiesTreeItem::FontSizeCombo:
                {
                    return (index.column() == 1) ? itemImpl_->getFontSize() : item->data(index.column());
                }
                case PropertiesTreeItem::FontColorDialog:
                {
                    return (index.column() == 1) ? itemImpl_->getColor().name() : item->data(index.column());
                }
                case PropertiesTreeItem::BorderColorDialog:
                {
                    return (index.column() == 1) ? itemImpl_->getBorderColor().name() : item->data(index.column());
                }
                case PropertiesTreeItem::BackgroundColorDialog:
                {
                    return (index.column() == 1) ? itemImpl_->getBackColor().name() : item->data(index.column());
                }
                case PropertiesTreeItem::BackgroundImageDialog:
                {
                    if (index.column() == 1)
                    {
                        return (itemImpl_->viewMode() == QtNodes::UniversalGraphicsItemImpl::Picture) ? itemImpl_->picturePath() : tr(
                        "Default");
                    }
                    else
                    {
                        return item->data(index.column());
                    }
                }
                case PropertiesTreeItem::GuiXml:
                {
                    if (item_->isGui())
                    {
                        return (index.column() == 1) ? item_->guiXml() : item->data(index.column());
                    }
                    break;
                }
                default:
                {
                    return item->data(index.column());
                }
            }
        }
        case Qt::CheckStateRole:
        {
            if (index.column() == 1)
            {
                if (item->type() == PropertiesTreeItem::LockCheckbox)
                {
                    return item_->isLocked() ? Qt::Checked : Qt::Unchecked;
                }
                if (item->type() == PropertiesTreeItem::UnderlineCheckBox)
                {
                    return itemImpl_->getFont().underline() ? Qt::Checked : Qt::Unchecked;
                }
                if (item->type() == PropertiesTreeItem::GuiCheckBox)
                {
                    return item_->isGui() ? Qt::Checked : Qt::Unchecked;
                }
            }
            return {};
        }
        case PropertiesTreeItem::GetFontRole:
        {
            return itemImpl_->getFont();
        }
        case PropertiesTreeItem::GetMinSizeRole:
        {
            return itemImpl_->getMinSize();
        }
        default:
        {
            return {};
        }
    }
}

Qt::ItemFlags PropertiesTreeModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
    {
        return Qt::NoItemFlags;
    }

    PropertiesTreeItem* item = static_cast<PropertiesTreeItem*>(index.internalPointer());

    switch (item->type())
    {
        case PropertiesTreeItem::NoEdit:
        {
            return Qt::NoItemFlags;
        }
        case PropertiesTreeItem::NameTextEdit:
        case PropertiesTreeItem::FontSizeCombo:
        case PropertiesTreeItem::FontFamilyCombo:
        case PropertiesTreeItem::FontColorDialog:
        case PropertiesTreeItem::XTextEdit:
        case PropertiesTreeItem::YTextEdit:
        case PropertiesTreeItem::WidthTextEdit:
        case PropertiesTreeItem::HeightTextEdit:
        case PropertiesTreeItem::BorderColorDialog:
        case PropertiesTreeItem::BackgroundColorDialog:
        case PropertiesTreeItem::GuiXml:
        {
            if (index.column() == 1)
            {
                return Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled;
            }
            break;
        }
        case PropertiesTreeItem::BackgroundImageDialog:
        {
            if (index.column() != 0)
            {
                return Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled;
            }
            break;
        }
        case PropertiesTreeItem::LockCheckbox:
        case PropertiesTreeItem::UnderlineCheckBox:
        case PropertiesTreeItem::GuiCheckBox:
        {
            return Qt::ItemIsUserCheckable | Qt::ItemIsEnabled;
        }
        default:
        {
            return QAbstractItemModel::flags(index);
        }
    }
    return QAbstractItemModel::flags(index);
}

QVariant PropertiesTreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    return (orientation == Qt::Horizontal && role == Qt::DisplayRole) ? rootItem_->data(section) : QVariant {};
}

QModelIndex PropertiesTreeModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
    {
        return {};
    }

    auto parentItem = !parent.isValid() ? rootItem_ : static_cast<PropertiesTreeItem*>(parent.internalPointer());

    auto childItem = parentItem->child(row);
    return childItem ? createIndex(row, column, childItem) : QModelIndex {};
}

QModelIndex PropertiesTreeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
    {
        return {};
    }

    auto childItem = static_cast<PropertiesTreeItem*>(index.internalPointer());
    auto parentItem = childItem->parentItem();

    return (parentItem == rootItem_) ? QModelIndex {} : createIndex(parentItem->row(), 0, parentItem);
}

int PropertiesTreeModel::rowCount(const QModelIndex &parent) const
{
    if (itemImpl_)
    {
        PropertiesTreeItem* parentItem;
        if (parent.column() > 0)
        {
            return 0;
        }

        parentItem = !parent.isValid() ? rootItem_ : static_cast<PropertiesTreeItem*>(parent.internalPointer());
        return parentItem->childCount();
    }
    return 0;
}

bool PropertiesTreeModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (itemImpl_ && item_)
    {
        auto item = static_cast<PropertiesTreeItem*>(index.internalPointer());
        switch (role)
        {
            case Qt::EditRole:
            {
                switch (item->type())
                {
                    case PropertiesTreeItem::NameTextEdit:
                    {
                        auto idx = nodesModel_->nodeIndex(uid_);
                        nodesModel_->nodeChangeCaption(idx, idx.nodeCaption());
                        break;
                    }
                    case PropertiesTreeItem::XTextEdit:
                    {
                        item_->setPos({value.toDouble(), itemImpl_->getPos().y()});
                        break;
                    }
                    case PropertiesTreeItem::YTextEdit:
                    {
                        item_->setPos({itemImpl_->getPos().x(), value.toDouble()});
                        break;
                    }
                    case PropertiesTreeItem::WidthTextEdit:
                    {
                        item_->resize({value.toDouble(), itemImpl_->getSize().height()});
                        break;
                    }
                    case PropertiesTreeItem::HeightTextEdit:
                    {
                        item_->resize({itemImpl_->getSize().width(), value.toDouble()});
                        break;
                    }
                    case PropertiesTreeItem::FontSizeCombo:
                    {
                        item_->setFontSize(value.toInt());
                        break;
                    }
                    case PropertiesTreeItem::FontFamilyCombo:
                    {
                        item_->setFontFamily(value.toString());
                        break;
                    }
                    case PropertiesTreeItem::FontColorDialog:
                    {
                        item_->setColor(value.toString());
                        break;
                    }
                    case PropertiesTreeItem::BorderColorDialog:
                    {
                        item_->setBorderColor(value.toString());
                        break;
                    }
                    case PropertiesTreeItem::BackgroundColorDialog:
                    {
                        item_->setBackColor(value.toString());
                        break;
                    }
                    case PropertiesTreeItem::BackgroundImageDialog:
                    {
                        item_->setPicture(value.toString());
                        break;
                    }
                    case PropertiesTreeItem::GuiXml:
                    {
                        item_->setGuiXml(value.toString());
                        break;
                    }
                    default:
                    {
                        break;
                    }
                }
                break;
            }
            case Qt::CheckStateRole:
            {
                switch (item->type())
                {
                    case PropertiesTreeItem::UnderlineCheckBox:
                    {
                        item_->setFontUnderline(value.toBool());
                        break;
                    }
                    case PropertiesTreeItem::LockCheckbox:
                    {
                        item_->setLocked(value.toBool());
                        break;
                    }
                    case PropertiesTreeItem::GuiCheckBox:
                    {
                        item_->setAsGui(value.toBool());
                        break;
                    }
                    default:
                    {
                        break;
                    }
                }
                break;
            }
        }
    }
    return true;
}

void PropertiesTreeModel::setup(const QUuid &id)
{
    uid_ = id;
    beginResetModel();
    if (!id.isNull() && nodesModel_ && sceneModel_)
    {
        itemImpl_ = reinterpret_cast<QtNodes::UniversalGraphicsItemImpl*>(nodesModel_->nodeImplementation(
        nodesModel_->nodeIndex(id)));

        item_ = nullptr;
        for (auto scene : sceneModel_->allScenes())
        {
            item_ = dynamic_cast<QtNodes::UniversalGraphicsItem*>(scene->graphicsItem(id));
            if (item_)
            {
                break;
            }
        }
    }
    else
    {
        itemImpl_ = nullptr;
        item_ = nullptr;
    }
    endResetModel();
}

void PropertiesTreeModel::update()
{
    beginResetModel();
    endResetModel();
}

void PropertiesTreeModel::translateCaptions()
{
    rootItem_->setCaption({tr("Property"), tr("Value")});
}

void PropertiesTreeModel::setupModelData(PropertiesTreeItem* root)
{
    QVector<PropertiesTreeItem*> parents;

    root->appendChild(new PropertiesTreeItem({tr("Object"), ""}, PropertiesTreeItem::GroupItem, root));
    parents << root->child(root->childCount() - 1);
    parents.last()->appendChild(
    new PropertiesTreeItem({tr("Name"), ""}, PropertiesTreeItem::NameTextEdit, parents.last()));
    parents.last()->appendChild(new PropertiesTreeItem({tr("ID")}, PropertiesTreeItem::IdNoEdit, parents.last()));
    parents.last()->appendChild(
    new PropertiesTreeItem({tr("Lock"), ""}, PropertiesTreeItem::LockCheckbox, parents.last()));

    root->appendChild(new PropertiesTreeItem({tr("Geometry"), ""}, PropertiesTreeItem::GroupItem, root));
    parents << root->child(root->childCount() - 1);
    parents.last()->appendChild(new PropertiesTreeItem({tr("X"), ""}, PropertiesTreeItem::XTextEdit, parents.last()));
    parents.last()->appendChild(new PropertiesTreeItem({tr("Y"), ""}, PropertiesTreeItem::YTextEdit, parents.last()));
    parents.last()->appendChild(
    new PropertiesTreeItem({tr("Width"), ""}, PropertiesTreeItem::WidthTextEdit, parents.last()));
    parents.last()->appendChild(
    new PropertiesTreeItem({tr("Height"), ""}, PropertiesTreeItem::HeightTextEdit, parents.last()));

    root->appendChild(new PropertiesTreeItem({tr("Font"), ""}, PropertiesTreeItem::GroupItem, root));
    parents << root->child(root->childCount() - 1);
    parents.last()->appendChild(
    new PropertiesTreeItem({tr("Style"), ""}, PropertiesTreeItem::FontFamilyCombo, parents.last()));
    parents.last()->appendChild(
    new PropertiesTreeItem({tr("Size"), ""}, PropertiesTreeItem::FontSizeCombo, parents.last()));
    parents.last()->appendChild(
    new PropertiesTreeItem({tr("Underline"), ""}, PropertiesTreeItem::UnderlineCheckBox, parents.last()));
    parents.last()->appendChild(
    new PropertiesTreeItem({tr("Color"), ""}, PropertiesTreeItem::FontColorDialog, parents.last()));

    root->appendChild(new PropertiesTreeItem({tr("Line border"), ""}, PropertiesTreeItem::GroupItem, root));
    parents << root->child(root->childCount() - 1);
    parents.last()->appendChild(
    new PropertiesTreeItem({tr("Color"), ""}, PropertiesTreeItem::BorderColorDialog, parents.last()));

    root->appendChild(new PropertiesTreeItem({tr("Background"), ""}, PropertiesTreeItem::GroupItem, root));
    parents << root->child(root->childCount() - 1);
    parents.last()->appendChild(
    new PropertiesTreeItem({tr("Color"), ""}, PropertiesTreeItem::BackgroundColorDialog, parents.last()));
    parents.last()->appendChild(
    new PropertiesTreeItem({tr("Image"), "", "<<"}, PropertiesTreeItem::BackgroundImageDialog, parents.last()));

    root->appendChild(new PropertiesTreeItem({tr("Working Type"), ""}, PropertiesTreeItem::GroupItem, root));
    parents << root->child(root->childCount() - 1);
    parents.last()->appendChild(
    new PropertiesTreeItem({tr("GUI"), ""}, PropertiesTreeItem::GuiCheckBox, parents.last()));
    parents.last()->appendChild(new PropertiesTreeItem({tr("XML"), ""}, PropertiesTreeItem::GuiXml, parents.last()));
}
} // namespace dscontrol
