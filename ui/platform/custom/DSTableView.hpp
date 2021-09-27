#pragma once

#include <QTableView>

namespace dscontrol
{
/*!
\brief Custom table view for dscontrol widgets
 */
class DSTableView : public QTableView
{
public:
    explicit DSTableView(QWidget* parent = nullptr);
    ~DSTableView() = default;

    // QWidget interface
protected:
    void mouseReleaseEvent(QMouseEvent* event);

    // QAbstractItemView interface
protected slots:
    void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected);

private:
    bool canDeselect_;
    QModelIndex lastIdx_;
};

} // namespace dscontrol
