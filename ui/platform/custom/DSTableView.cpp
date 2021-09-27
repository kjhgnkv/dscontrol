#include "DSTableView.hpp"

#include <QApplication>
#include <QMouseEvent>

dscontrol::DSTableView::DSTableView(QWidget* parent)
: QTableView(parent)
{
    canDeselect_ = false;
}

void dscontrol::DSTableView::mouseReleaseEvent(QMouseEvent* event)
{
    QModelIndex idx = indexAt(event->pos());
    if (idx.isValid())
    {
        auto selModel = selectionModel();
        if (selModel->selectedIndexes().contains(idx))
        {
            if (canDeselect_ || lastIdx_ != idx)
            {
                selModel->select(idx, QItemSelectionModel::SelectionFlag::Deselect);
                canDeselect_ = false;
            }
            else
            {
                canDeselect_ = true;
            }
        }
    }

    QTableView::mouseReleaseEvent(event);
}

void dscontrol::DSTableView::selectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
    if (!selected.isEmpty())
    {
        lastIdx_ = selected.indexes().at(0);
        canDeselect_ = false;
    }
    QTableView::selectionChanged(selected, deselected);
}
