#include "DSListView.hpp"

namespace dscontrol
{
DSListView::DSListView(QWidget* parent)
{
    setParent(parent);
}

DSListView::~DSListView()
{
}

QSize DSListView::sizeHint() const
{
    if (!model())
    {
        return QListView::sizeHint();
    }

    if (model()->rowCount() == 0)
    {
        return QSize(width(), 0);
    }

    int rowSize = 0;

    for (int i {0}; i < model()->rowCount(); ++i)
    {
        if (auto sz = sizeHintForRow(i); sz > rowSize)
        {
            rowSize = sz;
        }
    }

    return QSize(width(), rowSize);
}

} // namespace dscontrol
