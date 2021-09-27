#include "RecentLabelDelegate.hpp"

#include <QLabel>
#include <QPainter>
#include <QRegExpValidator>

namespace dscontrol
{
RecentLabelDelegate::RecentLabelDelegate(QObject* parent)
: QItemDelegate(parent)
{
}

QWidget* RecentLabelDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem &option,
                                           const QModelIndex &index) const
{
    Q_UNUSED(option);
    Q_UNUSED(index);
    QLabel* editor = new QLabel(parent);
    editor->setMouseTracking(true);

    return editor;
}

void RecentLabelDelegate::setEditorData(QWidget* editor, const QModelIndex &index) const
{
    QString value = index.data(Qt::DisplayRole).toString();
    if (QLabel* line = qobject_cast<QLabel*>(editor))
    {
        line->setText(value);
    }
}

void RecentLabelDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex &index) const
{
    if (QLabel* line = qobject_cast<QLabel*>(editor))
    {
        QString value = line->text();
        model->setData(index, value, Qt::EditRole);
    }
}

void RecentLabelDelegate::paint(QPainter* painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItem itemOpt(option);
    itemOpt.features |= QStyleOptionViewItemV2::HasDisplay;

    auto text = index.data(Qt::DisplayRole).toString();
    itemOpt.text = painter->fontMetrics().elidedText(text, Qt::ElideRight, itemOpt.rect.width());

    itemOpt.palette.setColor(QPalette::Text, QColor(Qt::white));

    painter->fillRect(option.rect, QColor(Qt::transparent));

    QFont font;

    if (option.state & QStyle::State_MouseOver)
    {
        font.setUnderline(true);
    }

    painter->setFont(font);
    painter->drawText(itemOpt.rect, qvariant_cast<int>(index.data(Qt::TextAlignmentRole)), itemOpt.text);

    painter->restore();
}

void RecentLabelDelegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem &option,
                                               const QModelIndex &index) const
{
    editor->setGeometry(option.rect);

    Q_UNUSED(index)
}

} // namespace dscontrol
