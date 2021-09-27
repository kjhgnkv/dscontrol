#pragma once

#include <QListView>

namespace dscontrol
{
/*!
\brief Custom list view for DSControl usage
*/
class DSListView : public QListView
{
public:
    explicit DSListView(QWidget* parent = nullptr);
    ~DSListView();

    // QWidget interface
public:
    QSize sizeHint() const;
};
} // namespace dscontrol
