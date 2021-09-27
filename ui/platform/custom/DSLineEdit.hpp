#pragma once

#include <QLineEdit>

class QToolButton;

namespace dscontrol
{
/*!
\brief Custom line edit for DSControl usage
*/
class DSLineEdit : public QLineEdit
{
Q_OBJECT

public:
    DSLineEdit(QWidget* parent = 0);

protected:
    void resizeEvent(QResizeEvent*);

signals:
    void toolClicked();

private:
    QToolButton* toolButton_;
};
} // namespace dscontrol
