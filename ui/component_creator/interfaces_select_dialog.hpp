// interfaces_select_dialog.hpp

#pragma once

#include <QDialog>

namespace Ui
{
class interfaces_select_dialog;
}

class QSortFilterProxyModel;
class QModelIndex;

class interfaces_select_dialog : public QDialog
{
Q_OBJECT
public:
    interfaces_select_dialog(QSortFilterProxyModel* model, ::QWidget* parent = nullptr);
    ~interfaces_select_dialog();

signals:
    /**\param index current highlithed index*/
    void selected_index(const QModelIndex &index);

private:
    Ui::interfaces_select_dialog* ui;

    // QWidget interface
protected:
    void changeEvent(QEvent* event);
};
