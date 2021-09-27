#include "lineedit.hpp"

LineEdit::LineEdit(QWidget* parent)
: QLineEdit(parent)
, c_(nullptr)
{
}

LineEdit::~LineEdit()
{
}

void LineEdit::setCompleter(QCompleter* completer)
{
    if (c_)
        QObject::disconnect(connection_);

    c_ = completer;

    if (!c_)
        return;

    c_->setWidget(this);
    c_->setCompletionMode(QCompleter::PopupCompletion);
    c_->setCaseSensitivity(Qt::CaseInsensitive);

    connection_ = QObject::connect(c_, QOverload<const QString &>::of(&QCompleter::activated), this,
                                   &LineEdit::insertCompletion);
}

QCompleter* LineEdit::completer() const
{
    return c_;
}

void LineEdit::insertCompletion(const QString &completion)
{
    Q_UNUSED(completion);
    if (c_->widget() != this)
        return;
}
