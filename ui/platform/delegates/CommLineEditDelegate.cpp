#include "CommLineEditDelegate.hpp"

#include <QLineEdit>
#include <QPainter>
#include <QRegExpValidator>

namespace dscontrol
{
CommLineEditDelegate::CommLineEditDelegate(const QRegExp &regex, QObject* parent)
: QItemDelegate(parent)
, regex_ {regex}
{
}

QWidget* CommLineEditDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem &option,
                                            const QModelIndex &index) const
{
    Q_UNUSED(option);
    Q_UNUSED(index);
    QLineEdit* editor = new QLineEdit(parent);
    QRegExpValidator* indexDataValidator = new QRegExpValidator(regex_, parent);
    editor->setValidator(indexDataValidator);
    connect(editor, &QLineEdit::cursorPositionChanged, editor, [editor](int oldPos, int newPos)
    {
        if (/*(index.column() == CommunicationModel::SenderParam ||
             index.column() == CommunicationModel::RecvParam)
            &&*/
        newPos < editor->text().size())
        {
            editor->setCursorPosition(oldPos);
        }
    });
    return editor;
}

void CommLineEditDelegate::setEditorData(QWidget* editor, const QModelIndex &index) const
{
    QString value = index.data(Qt::DisplayRole).toString();
    if (QLineEdit* line = qobject_cast<QLineEdit*>(editor))
    {
        line->setText(value);
    }
}

void CommLineEditDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex &index) const
{
    if (QLineEdit* line = qobject_cast<QLineEdit*>(editor))
    {
        QString value = line->text();
        model->setData(index, value, Qt::EditRole);
    }
}

void CommLineEditDelegate::paint(QPainter* painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
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
