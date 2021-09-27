#pragma once

#include <QVariant>
#include <QVector>

namespace dscontrol
{
class PropertiesTreeItem
{
public:
    enum
    {
        RootItem,
        GroupItem,
        NoEdit,
        NameTextEdit,
        IdNoEdit,
        XTextEdit,
        YTextEdit,
        WidthTextEdit,
        HeightTextEdit,
        LockCheckbox,
        UnderlineCheckBox,
        FontSizeCombo,
        FontFamilyCombo,
        FontColorDialog,
        BorderColorDialog,
        BackgroundColorDialog,
        BackgroundImageDialog,
        GuiCheckBox,
        GuiXml
    };

    enum
    {
        GetFontRole = Qt::UserRole + 1,
        GetMinSizeRole
    };

public:
    explicit PropertiesTreeItem(const QVector<QVariant> &data, int type, PropertiesTreeItem *parentItem = nullptr);
    ~PropertiesTreeItem();

    void appendChild(PropertiesTreeItem *child);
    PropertiesTreeItem *child(int row);
    int childCount() const;
    int columnCount() const;
    QVariant data(int column) const;
    int row() const;
    PropertiesTreeItem *parentItem();
    int type() const;
    void setCaption(const QVector<QVariant> &data);

private:
    int type_;
    QVector<PropertiesTreeItem *> childItems_;
    QVector<QVariant> itemData_;
    PropertiesTreeItem *parentItem_;
};
} // namespace dscontrol
