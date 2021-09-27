#include "CommComboBoxDelegate.hpp"

#include <QComboBox>
#include <QLineEdit>
#include <QPainter>
#include <QRegExpValidator>
#include <QStyleOptionViewItem>

namespace dscontrol
{
CommComboBoxDelegate::CommComboBoxDelegate(const QStringList &validValues, QObject* parent)
: QItemDelegate(parent)
, validValues_ {validValues}
{
}

void CommComboBoxDelegate::setValidValue(const QStringList &validValues)
{
    validValues_ = validValues;
}

QWidget* CommComboBoxDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem &option,
                                            const QModelIndex &index) const
{
    Q_UNUSED(index);
    Q_UNUSED(option);

    QComboBox* methods = new QComboBox(parent);
    methods->addItems(validValues_);

    methods->setEditable(false);
    return methods;
}

void CommComboBoxDelegate::setEditorData(QWidget* editor, const QModelIndex &index) const
{
    QString value = index.data(Qt::DisplayRole).toString();
    if (QComboBox* methods = qobject_cast<QComboBox*>(editor))
    {
        int i = methods->findText(value);
        if (i != -1)
            methods->setCurrentIndex(i);
        methods->showPopup();
    }
}

void CommComboBoxDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex &index) const
{
    if (QComboBox* methods = qobject_cast<QComboBox*>(editor))
    {
        QString value = methods->currentText();
        model->setData(index, value, Qt::EditRole);
        methods->close();
    }
}

void CommComboBoxDelegate::paint(QPainter* painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItem itemOpt(option);
    itemOpt.features |= QStyleOptionViewItemV2::HasDisplay;
    itemOpt.text = index.data(Qt::DisplayRole).toString();

    itemOpt.palette.setColor(QPalette::Text, QColor(Qt::white));

    if (option.state & QStyle::State_Enabled)
    {
        painter->fillRect(option.rect, QColor("#605F6E"));
    }
    if (option.state & QStyle::State_MouseOver)
    {
        painter->fillRect(option.rect, QColor("#595866"));
    }
    if (option.state & QStyle::State_Selected)
    {
        painter->fillRect(option.rect, QColor("#484853"));
    }
    painter->setPen(Qt::white);
    painter->setBrush(itemOpt.palette.highlightedText());
    painter->drawText(itemOpt.rect, qvariant_cast<int>(index.data(Qt::TextAlignmentRole)), itemOpt.text);
    painter->restore();
}
} // namespace dscontrol
