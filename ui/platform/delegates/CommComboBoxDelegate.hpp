#pragma once

#include <QItemDelegate>

namespace dscontrol
{
/*!
\brief custom delegate for communications dialogs table view combobox columns
 */
class CommComboBoxDelegate : public QItemDelegate
{
Q_OBJECT
public:
    /*!
    \param validValues list of values, which we can add by combobox
     */
    explicit CommComboBoxDelegate(const QStringList &validValues = {}, QObject* parent = nullptr);

    void setValidValue(const QStringList &validValues);

protected:
    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void setEditorData(QWidget* editor, const QModelIndex &index) const override;
    void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex &index) const override;

private:
    QStringList validValues_;

    // QAbstractItemDelegate interface
public:
    void paint(QPainter* painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};
} // namespace dscontrol
