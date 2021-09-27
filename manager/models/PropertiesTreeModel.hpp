#pragma once

#include <QAbstractItemModel>
#include <QColor>
#include <QModelIndex>
#include <QUuid>
#include <QVariant>

namespace QtNodes
{
class DataFlowModel;
class NodesScene;
class UniversalGraphicsItemImpl;
class UniversalGraphicsItem;
} // namespace QtNodes

namespace dscontrol
{
class PropertiesTreeItem;

/*!
\brief Model class for object properties widgets tree view
 */
class PropertiesTreeModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    explicit PropertiesTreeModel(QObject *parent = nullptr);
    ~PropertiesTreeModel();

    QVariant data(const QModelIndex &index, int role) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

    /*!
    \brief Setup model with id of chosen object
     */
    void setup(const QUuid &id);

    /*!
    \brief Updates model. Call when object properties changed
     */
    void update();

    /*!
    \brief update captions in dependency on language
     */
    void translateCaptions();

    QtNodes::UniversalGraphicsItem *graphicsItem() const;

private:
    QUuid uid_;

    void setupModelData(PropertiesTreeItem *parent);

    PropertiesTreeItem *rootItem_;

    QtNodes::DataFlowModel *nodesModel_;
    QtNodes::NodesScene *sceneModel_;
    QtNodes::UniversalGraphicsItemImpl *itemImpl_;
    QtNodes::UniversalGraphicsItem *item_;
};
} // namespace dscontrol
