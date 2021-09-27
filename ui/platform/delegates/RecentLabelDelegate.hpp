#pragma once

#include <QItemDelegate>
#include <QObject>

namespace dscontrol
{
/*!
\brief Delegate class for recent projects list
 */
class RecentLabelDelegate : public QItemDelegate
{
Q_OBJECT
public:
    explicit RecentLabelDelegate(QObject* parent = nullptr);

protected:
    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void setEditorData(QWidget* editor, const QModelIndex &index) const override;
    void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex &index) const override;

    // QAbstractItemDelegate interface
public:
    void paint(QPainter* painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

    // QAbstractItemDelegate interface
public:
    void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};
} // namespace dscontrol
