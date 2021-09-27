#include "RenameDialog.h"
#include "ui_RenameDialog.h"

#include <QRegularExpressionValidator>

RenameDialog::RenameDialog(const QString &name, QWidget* parent)
: QDialog {parent}
, ui_ {new Ui::RenameDialog}
, currentName_ {name}
, takenText_ {tr("This name has already taken. Enter a different name.")}
, enterNameText_ {tr("Enter name.")}
, regExpStr_ {"[A-Za-z0-9_]{0,25}"}
{
    ui_->setupUi(this);

    auto okButton = ui_->okButton;
    if (nullptr != okButton)
    {
        connect(okButton, &QPushButton::clicked, this, &RenameDialog::okPushButtonClicked);
    }

    auto cancelButton = ui_->cancelButton;
    if (nullptr != cancelButton)
    {
        connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
    }

    auto lineEdit = ui_->lineEdit;
    if (nullptr != lineEdit)
    {
        connect(lineEdit, &QLineEdit::textEdited, this, &RenameDialog::lineEditTextEdited);
        lineEdit->setValidator(new QRegExpValidator {QRegExp {regExpStr_}, this});
        lineEdit->setText(currentName_);
        lineEdit->setFocus();
    }
}

RenameDialog::~RenameDialog()
{
    if (nullptr != ui_)
    {
        delete ui_;
        ui_ = nullptr;
    }
}

QString RenameDialog::currentName() const
{
    return currentName_;
}

bool RenameDialog::hasNameAlreadyTaken(const QString &name) const
{
    return name == currentName();
}

bool RenameDialog::setNewName(const QString &newName)
{
    if (newName != currentName_)
    {
        currentName_ = newName;
        return true;
    }
    return false;
}

void RenameDialog::lineEditTextEdited(const QString &text)
{
    const QString &newText = text.isEmpty() ? enterNameText_ : "";
    setLabelText(newText);
}

void RenameDialog::okPushButtonClicked()
{
    auto label = ui_->label;
    if (nullptr != label && !label->text().isEmpty())
    {
        shakeLabel();
        return;
    }

    const QString &newName = ui_->lineEdit->text();
    if (!hasNameAlreadyTaken(newName))
    {
        return setNewName(newName) ? emit accept() : emit reject();
    }
    setLabelText(takenText_);
}

void RenameDialog::changeEvent(QEvent* event)
{
    if (nullptr == event)
    {
        return;
    }
    switch (event->type())
    {
        case QEvent::LanguageChange:
        {
            ui_->retranslateUi(this);
            break;
        }
        default:
        {
            break;
        }
    }
    QDialog::changeEvent(event);
}

void RenameDialog::shakeLabel()
{
    auto label = ui_->label;
    if (nullptr != label && !label->text().isEmpty())
    {
        /**
         * TODO
         * Add the shaking animation for label.
        **/
    }
}

void RenameDialog::setLabelText(const QString &newText)
{
    auto label = ui_->label;
    if (label->text() == newText)
    {
        return;
    }
    label->setText(newText);
    shakeLabel();
}
