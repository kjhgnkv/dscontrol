#include "DSLineEdit.hpp"

#include <QStyle>
#include <QToolButton>

namespace dscontrol
{
DSLineEdit::DSLineEdit(QWidget* parent)
: QLineEdit(parent)
{
    toolButton_ = new QToolButton(this);

    toolButton_->setFixedSize(16, 18);

    toolButton_->setIcon(QIcon(":/images/resources/images/toolImage.png"));

    toolButton_->setCursor(Qt::ArrowCursor);

    toolButton_->setStyleSheet(R"(

                               QToolButton {
                                    border: none;
                               }

                               )");

    connect(toolButton_, &QToolButton::clicked, this, &DSLineEdit::toolClicked);
}

void DSLineEdit::resizeEvent(QResizeEvent*)
{
    if (toolButton_)
        toolButton_->move(width() - toolButton_->width() - 1, 1);
}
} // namespace dscontrol
