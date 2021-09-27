#include "PropertiesItemDelegate.hpp"
#include "PropertiesTreeItem.hpp"

#include "common/globals.h"
#include "PropertiesTreeModel.hpp"
#include "graphicsComposites/universalGraphicsObject/imageSelector/ImageSelector.hpp"
#include "ui/platform/TextBrowserDialog/TextBrowserDialog.h"

#include <QAbstractItemDelegate>
#include <QColorDialog>
#include <QComboBox>
#include <QEvent>
#include <QFile>
#include <QFontDatabase>
#include <QLabel>
#include <QLineEdit>
#include <QPainter>
#include <QRegularExpressionValidator>
#include <QSortFilterProxyModel>

#define COLOR_BORDER_WIDTH 2

namespace dscontrol
{
PropertiesDelegate::PropertiesDelegate(QObject* parent)
: QStyledItemDelegate(parent)
{
}

QWidget* PropertiesDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem &, const QModelIndex &index) const
{
    if (index.column() != 1)
    {
        return nullptr;
    }

    auto model = static_cast<const QAbstractProxyModel*>(index.model());
    auto sourceIndex = model->mapToSource(index);

    auto item = static_cast<PropertiesTreeItem*>(sourceIndex.internalPointer());
    auto itemType = item->type();

    switch (itemType)
    {
        case PropertiesTreeItem::NameTextEdit:
        {
            return new QLineEdit {parent};
        }
        case PropertiesTreeItem::XTextEdit:
        case PropertiesTreeItem::YTextEdit:
        {
            auto editor = new QLineEdit {parent};
            editor->setValidator(new QIntValidator(0, 9999));

            return editor;
        }
        case PropertiesTreeItem::WidthTextEdit:
        case PropertiesTreeItem::HeightTextEdit:
        {
            auto editor = new QLineEdit {parent};
            QSizeF size = index.model()->data(index, PropertiesTreeItem::GetMinSizeRole).value<QSizeF>();
            auto bottomValue = (itemType == PropertiesTreeItem::WidthTextEdit) ? size.width() : size.height();
            editor->setValidator(new QIntValidator(bottomValue, 9999));

            return editor;
        }
        case PropertiesTreeItem::FontSizeCombo:
        {
            auto editor = new QComboBox {parent};
            QFontDatabase fonts;
            QFont font = index.model()->data(index, PropertiesTreeItem::GetFontRole).value<QFont>();
            for (auto i : fonts.pointSizes(font.family()))
            {
                editor->addItem(QString::number(i));
            }
            return editor;
        }
        case PropertiesTreeItem::FontFamilyCombo:
        {
            auto editor = new QComboBox {parent};
            QFontDatabase fonts;
            editor->addItems(fonts.families());

            return editor;
        }
        case PropertiesTreeItem::GuiXml:
        {
            return new QLabel {parent};
        }
        default:
        {
            return nullptr;
        }
    }
}

void PropertiesDelegate::setEditorData(QWidget* editor, const QModelIndex &index) const
{
    auto model = static_cast<const QAbstractProxyModel*>(index.model());
    auto sourceModel = model->sourceModel();
    auto sourceIndex = model->mapToSource(index);

    auto item = static_cast<PropertiesTreeItem*>(sourceIndex.internalPointer());

    switch (item->type())
    {
        case PropertiesTreeItem::NameTextEdit:
        case PropertiesTreeItem::XTextEdit:
        case PropertiesTreeItem::YTextEdit:
        case PropertiesTreeItem::WidthTextEdit:
        case PropertiesTreeItem::HeightTextEdit:
        {
            QString value = index.model()->data(index, Qt::DisplayRole).toString();
            QLineEdit* edit = static_cast<QLineEdit*>(editor);
            edit->setText(value);
            break;
        }
        case PropertiesTreeItem::FontSizeCombo:
        case PropertiesTreeItem::FontFamilyCombo:
        {
            auto value = index.model()->data(index, Qt::DisplayRole).toString();
            QComboBox* edit = static_cast<QComboBox*>(editor);
            int i = edit->findText(value);
            if (i != -1)
            {
                edit->setCurrentIndex(i);
            }
            connect(edit, static_cast<void (QComboBox::*)(const QString &)>(&QComboBox::currentIndexChanged),
                    [=](const QString &text)
                    {
                        sourceModel->setData(sourceIndex, text, Qt::EditRole);
                    });
            edit->showPopup();
            break;
        }
        case PropertiesTreeItem::GuiXml:
        {
            QString value = index.model()->data(index, Qt::DisplayRole).toString();
            TextBrowserDialog dialog {value};
            dialog.exec();
            break;
        }
        default:
        {
            break;
        }
    }
}

void PropertiesDelegate::setModelData(QWidget* editor, QAbstractItemModel*, const QModelIndex &index) const
{
    auto proxyModel = static_cast<const QAbstractProxyModel*>(index.model());
    auto sourceIndex = proxyModel->mapToSource(index);
    auto sourceModel = proxyModel->sourceModel();

    auto item = static_cast<PropertiesTreeItem*>(sourceIndex.internalPointer());

    switch (item->type())
    {
        case PropertiesTreeItem::NameTextEdit:
        case PropertiesTreeItem::XTextEdit:
        case PropertiesTreeItem::YTextEdit:
        case PropertiesTreeItem::WidthTextEdit:
        case PropertiesTreeItem::HeightTextEdit:
        {
            QLineEdit* edit = static_cast<QLineEdit*>(editor);
            QString value = edit->text();
            sourceModel->setData(sourceIndex, value, Qt::EditRole);
            break;
        }
        default:
        {
            break;
        }
    }
}

void PropertiesDelegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem &option,
                                              const QModelIndex &) const
{
    editor->setGeometry(option.rect);
}

bool PropertiesDelegate::editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem &option,
                                     const QModelIndex &index)
{
    auto proxyModel = static_cast<const QAbstractProxyModel*>(index.model());
    auto sourceIndex = proxyModel->mapToSource(index);
    auto sourceModel = proxyModel->sourceModel();

    auto item = static_cast<PropertiesTreeItem*>(sourceIndex.internalPointer());
    switch (item->type())
    {
        case PropertiesTreeItem::BackgroundImageDialog:
        {
            if (event->type() == QEvent::MouseButtonPress)
            {
                if (index.column() == 1)
                {
                    ImageSelector imageSelector(sourceModel->data(sourceIndex).toString());
                    imageSelector.exec();

                    if (imageSelector.result() == QDialog::Rejected)
                    {
                        break;
                    }

                    QString picturePath = imageSelector.picturePath();
                    if (QFile::exists(picturePath))
                    {
                        sourceModel->setData(sourceIndex, picturePath, Qt::EditRole);
                    }
                    else
                    {
                        sourceModel->setData(sourceIndex, "", Qt::EditRole);
                    }
                }
                else if (index.column() == 2)
                {
                    sourceModel->setData(sourceIndex, "", Qt::EditRole);
                }
            }
            break;
        }
        case PropertiesTreeItem::FontColorDialog:
        case PropertiesTreeItem::BackgroundColorDialog:
        case PropertiesTreeItem::BorderColorDialog:
        {
            if (event->type() == QEvent::MouseButtonPress)
            {
                if (index.column() == 1)
                {
                    QColorDialog dlg;
                    dlg.setStyleSheet(_DEF_STYLESHEET);
                    dlg.exec();
                    if (dlg.result() == QDialog::Accepted)
                    {
                        auto color = dlg.currentColor();
                        sourceModel->setData(sourceIndex, color, Qt::EditRole);
                    }
                }
            }
            break;
        }
        default:
        {
            break;
        }
    }
    return QStyledItemDelegate::editorEvent(event, model, option, index);
}

QSize PropertiesDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QSize size = QStyledItemDelegate::sizeHint(option, index);

    auto proxyModel = static_cast<const QAbstractProxyModel*>(index.model());
    auto sourceIndex = proxyModel->mapToSource(index);

    auto item = static_cast<PropertiesTreeItem*>(sourceIndex.internalPointer());

    if (item->type() == PropertiesTreeItem::GroupItem || item->type() == PropertiesTreeItem::GuiXml)
    {
        size.setHeight(24);
    }
    return size;
}

void PropertiesDelegate::paint(QPainter* painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    auto value = index.model()->data(index, Qt::DisplayRole).toString();
    auto model = static_cast<const QAbstractProxyModel*>(index.model());
    auto sourceIndex = model->mapToSource(index);

    auto item = static_cast<PropertiesTreeItem*>(sourceIndex.internalPointer());
    switch (item->type())
    {
        case PropertiesTreeItem::GuiXml:
        {
            if (index.column() == 1)
            {
                auto lines = sourceIndex.data(Qt::DisplayRole).toString().split("\n");
                if (lines.size() != 0)
                {
                    painter->save();
                    painter->setPen("#a0a0a0");
                    painter->drawText(option.rect.translated(8, 0),
                                      qvariant_cast<int>(sourceIndex.data(Qt::TextAlignmentRole)), lines.first());
                    painter->restore();
                }
            }
            break;
        }
        case PropertiesTreeItem::FontColorDialog:
        case PropertiesTreeItem::BorderColorDialog:
        case PropertiesTreeItem::BackgroundColorDialog:
        {
            if (index.column() == 1)
            {
                painter->save();

                QPointF topLeftPadding {9, 3};
                QSizeF rectSize {12, 12};
                QRectF colorRect = {option.rect.topLeft() + topLeftPadding, rectSize};

                painter->setRenderHint(QPainter::Antialiasing);
                painter->setPen("#AAAAC5");
                painter->setBrush(QColor(value));
                painter->drawRoundedRect(colorRect, 4, 4);

                painter->restore();
            }
            else
            {
                QStyledItemDelegate::paint(painter, option, index);
            }
            break;
        }
        case PropertiesTreeItem::GroupItem:
        {
            painter->save();

            auto font = painter->font();
            font.setPixelSize(14);
            font.setWeight(QFont::DemiBold);
            painter->setFont(font);
            painter->setPen(QColor {"#7b59ff"});

            painter->drawText(option.rect.translated(8, 4), qvariant_cast<int>(sourceIndex.data(Qt::TextAlignmentRole)),
                              sourceIndex.data(Qt::DisplayRole).toString());

            painter->setPen(QColor {"#3D3D46"});
            painter->drawLine(option.rect.topLeft(), option.rect.topRight());

            painter->restore();

            break;
        }
        default:
        {
            QStyledItemDelegate::paint(painter, option, index);
            break;
        }
    }
    if (index.column() == 0)
    {
        painter->save();
        painter->setPen(QColor {"#3d3d46"});
        painter->drawLine(option.rect.topRight(), option.rect.bottomRight());
        painter->restore();
    }
}
} // namespace dscontrol
