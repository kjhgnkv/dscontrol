#pragma once

#include <QItemDelegate>
#include <QObject>

namespace dscontrol
{
/*!
\brief custom delegate for communications dialogs table view line edit columns
 */
class CommLineEditDelegate : public QItemDelegate
{
Q_OBJECT
public:
    /*!
    \param regex regex, which will be used for validation
     */
    explicit CommLineEditDelegate(const QRegExp &regex, QObject* parent = nullptr);

protected:
    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void setEditorData(QWidget* editor, const QModelIndex &index) const override;
    void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex &index) const override;

private:
    QRegExp regex_;

    // QAbstractItemDelegate interface
public:
    void paint(QPainter* painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};
} // namespace dscontrol
